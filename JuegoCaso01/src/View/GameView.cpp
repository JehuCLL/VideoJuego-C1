#include "GameView.hpp"
#include <string>

namespace {
constexpr Color shell{13,19,30,255}, floorColor{29,44,53,255};
constexpr Color mint{74,210,174,255}, gold{255,210,102,255};

Texture2D loadSprite(const char* path) {
    Image image = LoadImage(path);
    if (!IsImageValid(image)) return {};
    ImageAlphaCrop(&image, 0.10f);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    if (texture.id) SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    return texture;
}

void drawSprite(Texture2D texture, Rectangle destination, Color tint) {
    if (texture.id) {
        DrawTexturePro(texture, {0,0,float(texture.width),float(texture.height)},
                       destination, {0,0}, 0, tint);
    } else {
        DrawRectangleRec(destination, tint);
    }
}

void drawRoom(const game::Game& model) {
    ClearBackground(shell);
    DrawRectangle(24,66,752,486,{8,14,22,255});
    DrawRectangleRec({32,72,736,492},floorColor);
    for (int x=32;x<=768;x+=32) DrawLine(x,72,x,564,{35,52,60,255});
    for (int y=72;y<=564;y+=32) DrawLine(32,y,768,y,{35,52,60,255});
    DrawRectangle(32,72,736,3,{65,91,98,255});
    if (model.roomIndex==0) {
        DrawRectangle(760,270,8,88,mint); DrawRectangle(748,282,12,64,{44,112,105,255});
    } else {
        DrawRectangle(32,270,8,88,mint); DrawRectangle(40,282,12,64,{44,112,105,255});
    }
    for (const game::Rect& wall : model.room.obstacles) {
        DrawRectangle(int(wall.x+4),int(wall.y+5),int(wall.w),int(wall.h),{13,27,34,255});
        DrawRectangle(int(wall.x),int(wall.y),int(wall.w),int(wall.h),{84,106,113,255});
        DrawRectangle(int(wall.x),int(wall.y),int(wall.w),5,{126,149,153,255});
        DrawRectangle(int(wall.x),int(wall.y+wall.h-5),int(wall.w),5,{52,72,81,255});
    }
}

void drawActors(const game::Game& model, Texture2D explorer, Texture2D slime) {
    BeginScissorMode(32,72,736,492);
    for (const game::Enemy& enemy : model.enemies()) {
        if (!enemy.alive) continue;
        game::Vec2 c=game::center(enemy.bounds());
        DrawEllipse(int(c.x),int(c.y+17),27,8,{7,22,24,160});
        DrawLineEx({c.x,c.y},{c.x+enemy.facing.x*23,c.y+enemy.facing.y*23},2,{176,215,91,150});
        drawSprite(slime,{enemy.position.x-10,enemy.position.y-10,50,45},
                   enemy.hitFlash>0 ? WHITE : Color{220,255,220,255});
        DrawRectangle(int(enemy.position.x),int(enemy.position.y-9),30,3,{48,66,53,255});
        DrawRectangle(int(enemy.position.x),int(enemy.position.y-9),10*enemy.health,3,{188,228,73,255});
    }
    for (const game::Projectile& shot : model.projectiles) {
        Color color=shot.hostile ? Color{221,124,255,255} : gold;
        DrawCircleV({shot.position.x,shot.position.y},shot.hostile ? 6.0f : 5.0f,Fade(color,0.22f));
        DrawCircleV({shot.position.x,shot.position.y},shot.hostile ? 3.0f : 2.5f,color);
    }
    const game::Player& p=model.player;
    game::Vec2 c=game::center(p.bounds());
    DrawEllipse(int(c.x),int(c.y+17),20,7,{7,22,24,180});
    drawSprite(explorer,{p.position.x-17,p.position.y-24,62,64},WHITE);
    DrawLineEx({c.x,c.y},{c.x+p.aim.x*33,c.y+p.aim.y*33},3,gold);
    EndScissorMode();
}

void drawHud(const game::Game& model, bool focused) {
    DrawText("CASO 01  /  EXPLORACION",32,18,18,{245,247,250,255});
    DrawText(model.roomIndex==0 ? "SALA 01 / 02" : "SALA 02 / 02",326,20,16,{138,166,175,255});
    DrawText(TextFormat("VIDA  %d / %d",model.player.health,model.player.maxHealth),625,19,17,{245,247,250,255});
    DrawRectangle(625,47,135,5,{59,76,81,255});
    DrawRectangle(625,47,135*model.player.health/model.player.maxHealth,5,mint);
    int alive=0; for (const auto& e : model.enemies()) if (e.alive) ++alive;
    DrawText(TextFormat("ENEMIGOS  %d",alive),469,21,15,{155,187,179,255});
    DrawText("WASD/FLECHAS MOVER   MOUSE APUNTAR   CLIC IZQ DISPARAR   R REINICIAR",32,575,13,{161,187,193,255});
    game::Vec2 mouse=model.input.mouse;
    DrawLine(int(mouse.x)-8,int(mouse.y),int(mouse.x)-3,int(mouse.y),gold);
    DrawLine(int(mouse.x)+3,int(mouse.y),int(mouse.x)+8,int(mouse.y),gold);
    DrawLine(int(mouse.x),int(mouse.y)-8,int(mouse.x),int(mouse.y)-3,gold);
    DrawLine(int(mouse.x),int(mouse.y)+3,int(mouse.x),int(mouse.y)+8,gold);
    if (!focused) {
        DrawRectangle(190,277,420,48,{13,19,30,230});
        DrawText("HAZ CLIC PARA CONTINUAR",222,292,19,WHITE);
    }
}
}

void GameView::load() {
    const std::string assetRoot=std::string(GetApplicationDirectory())+"/assets/";
    explorer_=loadSprite((assetRoot+"explorador.png").c_str());
    slime_=loadSprite((assetRoot+"slime.png").c_str());
}

void GameView::unload() {
    if (explorer_.id) UnloadTexture(explorer_);
    if (slime_.id) UnloadTexture(slime_);
    explorer_={}; slime_={};
}

void GameView::draw(const game::Game& model, bool focused) const {
    drawRoom(model);
    drawActors(model,explorer_,slime_);
    drawHud(model,focused);
}
