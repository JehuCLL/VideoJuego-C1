#include "GameLogic.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main(){
    using namespace game;
    Game g;
    assert(g.room.canOccupy(g.player.bounds()));
    g.input.right=true;
    g.update(0.05f);
    assert(g.player.position.x>80 && g.player.position.y==90);
    g.input.clearKeys();
    g.player.position={32,72};
    g.input.left=true; g.input.up=true;
    g.update(0.05f);
    assert(g.player.position.x==32 && g.player.position.y==72);
    g.input.clearKeys();
    g.player.position={190,170};
    g.input.right=true;
    for(int i=0;i<10;++i) g.update(0.05f);
    assert(g.player.position.x+PlayerSize<=225);
    g.input.clearKeys();
    g.player.position={80,90};
    g.input.mouse={94,0};
    g.update(0);
    assert(std::abs(g.player.aim.x)<0.001f && g.player.aim.y<0);
    g.player.setHealth(-5); assert(g.player.health==0);
    g.player.setHealth(120); assert(g.player.health==100);
    g.player.setHealth(42);
    g.enterRoom(1); assert(g.roomIndex==1 && g.player.health==42 && g.room.canOccupy(g.player.bounds()));
    g.enterRoom(0); assert(g.roomIndex==0 && g.player.health==42 && g.room.canOccupy(g.player.bounds()));
    g.reset(); assert(g.player.health==100 && g.player.position.x==80 && g.roomIndex==0);
    std::cout << "Sprint 1 logic tests passed\n";
}
