#include "../src/Model/GameModel.hpp"
#include <cassert>
#include <iostream>

int main(){
    using namespace game;
    Game g;
    assert(g.room.canOccupy(g.player.bounds()));
    g.input.right=true; g.update(0.05f);
    assert(g.player.position.x>80 && g.player.position.y==120);
    g.input.clearKeys(); g.player.position={32,72};
    g.input.left=true; g.input.up=true; g.update(0.05f);
    assert(g.player.position.x==32 && g.player.position.y==72);
    g.input.clearKeys(); g.player.position={190,170}; g.input.right=true;
    for(int i=0;i<10;++i) g.update(0.05f);
    assert(g.player.position.x+PlayerSize<=225);
    g.input.clearKeys(); g.player.position={80,90}; g.input.mouse={94,0}; g.update(0);
    assert(std::abs(g.player.aim.x)<0.001f && g.player.aim.y<0);
    g.player.setHealth(-5); assert(g.player.health==0);
    g.player.setHealth(120); assert(g.player.health==100);
    g.player.setHealth(42); g.enterRoom(1);
    assert(g.roomIndex==1 && g.player.health==42 && g.room.canOccupy(g.player.bounds()));
    g.enterRoom(0); assert(g.roomIndex==0 && g.player.health==42);

    // RF-05: one valid mouse press creates one player projectile.
    g.reset(); g.room.obstacles.clear(); g.enemies().clear();
    g.player.position={80,90}; g.input.mouse={300,104}; g.input.firePressed=true; g.update(0);
    assert(g.projectiles.size()==1 && !g.projectiles[0].hostile);
    g.update(0); assert(g.projectiles.size()==1);

    // RF-06: a solid wall consumes the projectile.
    g.projectiles.clear(); g.room.obstacles.push_back({130,85,20,40});
    g.input.firePressed=true; g.update(0);
    for(int i=0;i<5;++i) g.update(0.05f);
    assert(g.projectiles.empty());

    // RF-06/07: one projectile damages one enemy; three hits remove it.
    g.room.obstacles.clear();
    g.enemies()={Enemy{{130,90}},Enemy{{130,90}}};
    for(int hit=0;hit<3;++hit){
        g.input.firePressed=true; g.update(0);
        for(int i=0;i<4;++i) g.update(0.05f);
    }
    assert(!g.enemies()[0].alive && g.enemies()[0].health==0);
    assert(g.enemies()[1].alive && g.enemies()[1].health==3);

    // RF-09/10: a live enemy moves, faces the player, and fires toward him.
    g.reset(); g.room.obstacles.clear(); g.projectiles.clear(); g.player.position={80,90};
    g.enemies()={Enemy{{220,90}}};
    g.enemies()[0].shootTimer=0;
    float oldX=g.enemies()[0].position.x;
    g.update(0.05f);
    assert(g.enemies()[0].position.x<oldX && g.enemies()[0].facing.x<0);
    assert(g.projectiles.size()==1 && g.projectiles[0].hostile && g.projectiles[0].velocity.x<0);
    g.update(0.05f); assert(g.projectiles.size()==1);

    // A wall blocks enemy line of sight and its shot.
    g.projectiles.clear(); g.room.obstacles.push_back({140,75,30,70});
    g.enemies()[0].shootTimer=0; g.update(0.05f);
    assert(g.projectiles.empty());
    g.reset(); assert(g.player.health==100 && g.roomIndex==0);
    std::cout << "Sprint 1 and selected Sprint 2 logic tests passed\n";
}
