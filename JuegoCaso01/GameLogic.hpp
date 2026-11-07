#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
namespace game {
constexpr int Width=800, Height=600;
constexpr float PlayerSize=28.0f, PlayerSpeed=230.0f;
struct Vec2 { float x=0,y=0; };
struct Rect { float x,y,w,h; };
inline bool overlaps(Rect a,Rect b) { return a.x<b.x+b.w && a.x+a.w>b.x && a.y<b.y+b.h && a.y+a.h>b.y; }
struct Input {
    bool up=false,down=false,left=false,right=false;
    Vec2 mouse{180,104};
    void clearKeys(){ up=down=left=right=false; }
};
struct Player {
    Vec2 position{80,90}, aim{1,0};
    int health=100,maxHealth=100;
    Rect bounds() const { return {position.x,position.y,PlayerSize,PlayerSize}; }
    void setHealth(int value){ health=std::clamp(value,0,maxHealth); }
};
struct Room {
    Rect playable{32,72,736,492};
    std::vector<Rect> obstacles{{225,148,60,210},{460,105,66,170},{350,425,150,44},{610,325,90,55}};
    static Room create(int index){
        Room r;
        if(index==1) r.obstacles={{170,130,100,65},{365,135,50,230},{540,205,125,48},{180,410,145,55},{560,415,75,95}};
        return r;
    }
    bool canOccupy(Rect r) const {
        if(r.x<playable.x || r.y<playable.y || r.x+r.w>playable.x+playable.w || r.y+r.h>playable.y+playable.h) return false;
        for(const Rect& o:obstacles) if(overlaps(r,o)) return false;
        return true;
    }
};
class Game {
public:
    Player player;
    Room room;
    Input input;
    int roomIndex=0;
    void reset(){ player=Player{}; roomIndex=0; room=Room::create(0); input=Input{}; }
    void enterRoom(int index){
        roomIndex=index;
        room=Room::create(index);
        player.position=index==0 ? Vec2{Width-72.0f,286.0f} : Vec2{44,286};
    }
    void update(float dt){
        dt=std::clamp(dt,0.0f,0.05f);
        float cx=player.position.x+PlayerSize/2, cy=player.position.y+PlayerSize/2;
        float ax=input.mouse.x-cx, ay=input.mouse.y-cy;
        float length=std::sqrt(ax*ax+ay*ay);
        if(length>0.001f) player.aim={ax/length,ay/length};
        float dx=float(input.right)-float(input.left),dy=float(input.down)-float(input.up);
        if(dx!=0 && dy!=0) dy=0; // Four cardinal directions only.
        // Small substeps prevent crossing a thin obstacle after a slow frame.
        float remaining=PlayerSpeed*dt;
        while(remaining>0){
            float step=std::min(remaining,2.0f);
            remaining-=step;
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
};
}
