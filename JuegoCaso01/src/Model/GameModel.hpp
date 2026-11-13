#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace game {
constexpr int Width = 800, Height = 600;
constexpr float PlayerSize = 28.0f, EnemySize = 30.0f;
constexpr float PlayerSpeed = 230.0f;

struct Vec2 { float x = 0, y = 0; };
struct Rect { float x, y, w, h; };
inline Vec2 center(Rect r) { return {r.x + r.w / 2, r.y + r.h / 2}; }
inline float distance(Vec2 a, Vec2 b) { float x=a.x-b.x,y=a.y-b.y; return std::sqrt(x*x+y*y); }
inline Vec2 direction(Vec2 from, Vec2 to) {
    float d=distance(from,to);
    return d>0.001f ? Vec2{(to.x-from.x)/d,(to.y-from.y)/d} : Vec2{1,0};
}
inline bool overlaps(Rect a, Rect b) {
    return a.x<b.x+b.w && a.x+a.w>b.x && a.y<b.y+b.h && a.y+a.h>b.y;
}

// Segment/rectangle test: used to avoid shooting through solid walls.
inline bool segmentIntersects(Rect r, Vec2 a, Vec2 b) {
    float tmin=0, tmax=1;
    auto axis=[&](float start,float delta,float low,float high) {
        if(std::abs(delta)<0.0001f) return start>=low && start<=high;
        float t1=(low-start)/delta,t2=(high-start)/delta;
        if(t1>t2) std::swap(t1,t2);
        tmin=std::max(tmin,t1); tmax=std::min(tmax,t2);
        return tmin<=tmax;
    };
    return axis(a.x,b.x-a.x,r.x,r.x+r.w) && axis(a.y,b.y-a.y,r.y,r.y+r.h);
}

struct Input {
    bool up=false, down=false, left=false, right=false, firePressed=false;
    Vec2 mouse{180,104};
    void clearKeys(){ up=down=left=right=firePressed=false; }
};
struct Player {
    Vec2 position{80,120}, aim{1,0};
    int health=100,maxHealth=100;
    Rect bounds() const { return {position.x,position.y,PlayerSize,PlayerSize}; }
    void setHealth(int value){ health=std::clamp(value,0,maxHealth); }
};
struct Enemy {
    Vec2 position{}, facing{1,0};
    int health=3;
    float shootTimer=0.9f, hitFlash=0;
    bool alive=true;
    Rect bounds() const { return {position.x,position.y,EnemySize,EnemySize}; }
};
struct Projectile {
    Vec2 position{}, velocity{};
    float life=2.2f;
    bool hostile=false, active=true;
    Rect bounds() const { return {position.x-4,position.y-4,8,8}; }
};
struct Room {
    Rect playable{32,72,736,492};
    std::vector<Rect> obstacles{{225,148,60,210},{460,105,66,170},{350,425,150,44},{610,325,90,55}};
    static Room create(int index) {
        Room r;
        if(index==1) r.obstacles={{170,130,100,65},{365,135,50,230},{540,205,125,48},{180,410,145,55},{560,415,75,95}};
        return r;
    }
    bool canOccupy(Rect r) const {
        if(r.x<playable.x || r.y<playable.y || r.x+r.w>playable.x+playable.w || r.y+r.h>playable.y+playable.h) return false;
        for(const Rect& o:obstacles) if(overlaps(r,o)) return false;
        return true;
    }
    bool clearShot(Vec2 from,Vec2 to) const {
        for(const Rect& o:obstacles) if(segmentIntersects(o,from,to)) return false;
        return true;
    }
};

class Game {
public:
    Player player;
    Room room;
    Input input;
    int roomIndex=0;
    std::array<std::vector<Enemy>,2> roomEnemies;
    std::vector<Projectile> projectiles;

    Game(){ reset(); }
    std::vector<Enemy>& enemies(){ return roomEnemies[roomIndex]; }
    const std::vector<Enemy>& enemies() const { return roomEnemies[roomIndex]; }
    void reset(){
        player=Player{}; roomIndex=0; room=Room::create(0); input=Input{};
        roomEnemies[0]={Enemy{{350,160}},Enemy{{570,390}},Enemy{{620,180}}};
        roomEnemies[1]={Enemy{{310,260}},Enemy{{690,390}},Enemy{{490,485}}};
        projectiles.clear();
    }
    void enterRoom(int index){
        roomIndex=index; room=Room::create(index);
        player.position=index==0 ? Vec2{Width-72.0f,286.0f} : Vec2{44,286};
        projectiles.clear();
    }
    void firePlayer(){
        Vec2 origin=center(player.bounds());
        projectiles.push_back({{origin.x+player.aim.x*20,origin.y+player.aim.y*20},
                              {player.aim.x*440,player.aim.y*440},2.0f,false,true});
    }
    void update(float dt){
        dt=std::clamp(dt,0.0f,0.05f);
        player.aim=direction(center(player.bounds()),input.mouse);
        movePlayer(dt);
        if(input.firePressed) firePlayer();
        input.firePressed=false;
        updateEnemies(dt);
        updateProjectiles(dt);
    }

private:
    void movePlayer(float dt){
        float dx=float(input.right)-float(input.left),dy=float(input.down)-float(input.up);
        if(dx!=0 && dy!=0) dy=0;
        float remaining=PlayerSpeed*dt;
        while(remaining>0){
            float step=std::min(remaining,2.0f); remaining-=step;
            if(dx!=0){
                Rect candidate=player.bounds(); candidate.x+=dx*step;
                if(room.canOccupy(candidate)) player.position.x=candidate.x;
                else if(player.position.y>=270 && player.position.y<=330){
                    if(roomIndex==0 && dx>0 && candidate.x+candidate.w>room.playable.x+room.playable.w) enterRoom(1);
                    else if(roomIndex==1 && dx<0 && candidate.x<room.playable.x) enterRoom(0);
                }
            }
            if(dy!=0){ Rect candidate=player.bounds(); candidate.y+=dy*step; if(room.canOccupy(candidate)) player.position.y=candidate.y; }
        }
    }
    void updateEnemies(float dt){
        for(Enemy& enemy:enemies()){
            if(!enemy.alive) continue;
            enemy.hitFlash=std::max(0.0f,enemy.hitFlash-dt);
            Vec2 target=center(player.bounds()),from=center(enemy.bounds());
            enemy.facing=direction(from,target);
            float gap=distance(from,target);
            if(gap>72 && gap<360){
                float step=72*dt;
                Rect candidate=enemy.bounds(); candidate.x+=enemy.facing.x*step;
                if(room.canOccupy(candidate) && !overlaps(candidate,player.bounds())) enemy.position.x=candidate.x;
                candidate=enemy.bounds(); candidate.y+=enemy.facing.y*step;
                if(room.canOccupy(candidate) && !overlaps(candidate,player.bounds())) enemy.position.y=candidate.y;
            }
            enemy.shootTimer-=dt;
            from=center(enemy.bounds());
            if(enemy.shootTimer<=0 && gap<410 && room.clearShot(from,target)){
                enemy.facing=direction(from,target);
                projectiles.push_back({{from.x+enemy.facing.x*22,from.y+enemy.facing.y*22},
                    {enemy.facing.x*225,enemy.facing.y*225},2.5f,true,true});
                enemy.shootTimer=1.4f;
            }
        }
    }
    void updateProjectiles(float dt){
        for(Projectile& shot:projectiles){
            shot.life-=dt;
            if(shot.life<=0){ shot.active=false; continue; }
            float travel=distance({0,0},{shot.velocity.x*dt,shot.velocity.y*dt});
            int steps=std::max(1,int(std::ceil(travel/3.0f)));
            for(int i=0;i<steps && shot.active;++i){
                shot.position.x+=shot.velocity.x*dt/steps;
                shot.position.y+=shot.velocity.y*dt/steps;
                if(!room.canOccupy(shot.bounds())){ shot.active=false; break; }
                if(shot.hostile){
                    if(overlaps(shot.bounds(),player.bounds())) shot.active=false; // RF-11 damage is a later task.
                } else {
                    for(Enemy& enemy:enemies()){
                        if(enemy.alive && overlaps(shot.bounds(),enemy.bounds())){
                            shot.active=false; enemy.health-=1; enemy.hitFlash=0.18f;
                            if(enemy.health<=0) enemy.alive=false;
                            break; // Exactly one enemy can consume a shot.
                        }
                    }
                }
            }
        }
        std::erase_if(projectiles,[](const Projectile& p){ return !p.active; });
    }
};
}
