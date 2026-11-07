#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include "GameLogic.hpp"

namespace {
game::Game state;
bool focused=true;
ULONGLONG lastTick=0;

RECT toRect(game::Rect r){ return {LONG(r.x),LONG(r.y),LONG(r.x+r.w),LONG(r.y+r.h)}; }
void fill(HDC dc,RECT r,COLORREF color){ HBRUSH b=CreateSolidBrush(color); FillRect(dc,&r,b); DeleteObject(b); }
void ellipse(HDC dc,RECT r,COLORREF color){
    HBRUSH b=CreateSolidBrush(color); HGDIOBJ oldBrush=SelectObject(dc,b);
    HPEN p=CreatePen(PS_NULL,0,color); HGDIOBJ oldPen=SelectObject(dc,p);
    Ellipse(dc,r.left,r.top,r.right,r.bottom);
    SelectObject(dc,oldPen); SelectObject(dc,oldBrush); DeleteObject(p); DeleteObject(b);
}
void label(HDC dc,int x,int y,const wchar_t* value){ TextOutW(dc,x,y,value,lstrlenW(value)); }

void draw(HDC dc){
    // Dark shell, inset playfield, and a restrained grid make the room readable.
    fill(dc,{0,0,game::Width,game::Height},RGB(13,19,30));
    fill(dc,{24,66,776,552},RGB(8,14,22));
    fill(dc,toRect(state.room.playable),RGB(29,44,53));
    HPEN grid=CreatePen(PS_SOLID,1,RGB(35,52,60)); HGDIOBJ oldGrid=SelectObject(dc,grid);
    for(int x=32;x<768;x+=32){ MoveToEx(dc,x,72,nullptr); LineTo(dc,x,564); }
    for(int y=72;y<564;y+=32){ MoveToEx(dc,32,y,nullptr); LineTo(dc,768,y); }
    SelectObject(dc,oldGrid); DeleteObject(grid);
    fill(dc,{32,72,768,75},RGB(65,91,98));
    // Open passage between the two rooms.
    if(state.roomIndex==0){
        fill(dc,{760,270,768,358},RGB(80,205,172));
        fill(dc,{748,282,760,346},RGB(44,112,105));
    } else {
        fill(dc,{32,270,40,358},RGB(80,205,172));
        fill(dc,{40,282,52,346},RGB(44,112,105));
    }
    for(const auto& wall:state.room.obstacles){
        RECT shadow=toRect(wall); OffsetRect(&shadow,4,5); fill(dc,shadow,RGB(13,27,34));
        RECT body=toRect(wall); fill(dc,body,RGB(84,106,113));
        fill(dc,{body.left,body.top,body.right,body.top+5},RGB(126,149,153));
        fill(dc,{body.left,body.bottom-5,body.right,body.bottom},RGB(52,72,81));
    }
    auto& p=state.player;
    int cx=int(p.position.x+game::PlayerSize/2),cy=int(p.position.y+game::PlayerSize/2);
    ellipse(dc,{cx-16,cy-10,cx+18,cy+23},RGB(12,30,34));
    ellipse(dc,{cx-14,cy-14,cx+14,cy+14},RGB(74,210,174));
    ellipse(dc,{cx-10,cy-10,cx+10,cy+10},RGB(31,120,108));
    ellipse(dc,{cx-5,cy-7,cx+5,cy+3},RGB(190,246,219));
    HPEN pen=CreatePen(PS_SOLID,5,RGB(255,210,102));
    HGDIOBJ oldPen=SelectObject(dc,pen);
    MoveToEx(dc,cx,cy,nullptr); LineTo(dc,cx+int(p.aim.x*25),cy+int(p.aim.y*25));
    SelectObject(dc,oldPen); DeleteObject(pen);
    int mx=int(state.input.mouse.x),my=int(state.input.mouse.y);
    pen=CreatePen(PS_SOLID,2,RGB(255,210,102)); oldPen=SelectObject(dc,pen);
    MoveToEx(dc,mx-8,my,nullptr); LineTo(dc,mx-3,my);
    MoveToEx(dc,mx+3,my,nullptr); LineTo(dc,mx+8,my);
    MoveToEx(dc,mx,my-8,nullptr); LineTo(dc,mx,my-3);
    MoveToEx(dc,mx,my+3,nullptr); LineTo(dc,mx,my+8);
    SelectObject(dc,oldPen); DeleteObject(pen);
    SetBkMode(dc,TRANSPARENT); SetTextColor(dc,RGB(245,247,250));
    HFONT font=CreateFontW(18,0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Segoe UI");
    HGDIOBJ oldFont=SelectObject(dc,font);
    label(dc,32,19,L"CASO 01   /   EXPLORACION");
    SetTextColor(dc,RGB(138,166,175)); label(dc,332,20,state.roomIndex==0 ? L"SALA 01 / 02" : L"SALA 02 / 02");
    SetTextColor(dc,RGB(245,247,250));
    wchar_t health[48]; wsprintfW(health,L"VIDA  %d / %d",p.health,p.maxHealth); label(dc,633,19,health);
    fill(dc,{633,47,760,52},RGB(59,76,81));
    fill(dc,{633,47,633+127*p.health/p.maxHealth,52},RGB(74,210,174));
    SetTextColor(dc,RGB(161,187,193));
    label(dc,32,573,L"MOVER  WASD / FLECHAS     APUNTAR  MOUSE     PASO VERDE  CAMBIAR SALA     R  REINICIAR");
    if(!focused){ fill(dc,{165,276,635,322},RGB(13,19,30)); SetTextColor(dc,RGB(245,247,250)); label(dc,225,287,L"HAZ CLIC PARA CONTINUAR"); }
    SelectObject(dc,oldFont); DeleteObject(font);
}

void setKey(WPARAM key,bool pressed){
    switch(key){
    case 'W': case VK_UP: state.input.up=pressed; break;
    case 'S': case VK_DOWN: state.input.down=pressed; break;
    case 'A': case VK_LEFT: state.input.left=pressed; break;
    case 'D': case VK_RIGHT: state.input.right=pressed; break;
    case 'R': if(pressed) state.reset(); break;
    }
}

LRESULT CALLBACK windowProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
    case WM_DESTROY: PostQuitMessage(0); return 0;
    case WM_KILLFOCUS: focused=false; state.input.clearKeys(); InvalidateRect(hwnd,nullptr,FALSE); return 0;
    case WM_SETFOCUS: focused=true; lastTick=GetTickCount64(); InvalidateRect(hwnd,nullptr,FALSE); return 0;
    case WM_KEYDOWN: setKey(wp,true); return 0;
    case WM_KEYUP: setKey(wp,false); return 0;
    case WM_MOUSEMOVE: state.input.mouse={float(GET_X_LPARAM(lp)),float(GET_Y_LPARAM(lp))}; return 0;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT:{
        PAINTSTRUCT ps; HDC target=BeginPaint(hwnd,&ps);
        HDC memory=CreateCompatibleDC(target);
        HBITMAP bitmap=CreateCompatibleBitmap(target,game::Width,game::Height);
        HGDIOBJ old=SelectObject(memory,bitmap);
        draw(memory); BitBlt(target,0,0,game::Width,game::Height,memory,0,0,SRCCOPY);
        SelectObject(memory,old); DeleteObject(bitmap); DeleteDC(memory);
        EndPaint(hwnd,&ps); return 0;
    }
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
}

int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int show){
    constexpr wchar_t className[]=L"JuegoCaso01Sprint1";
    WNDCLASSW wc{}; wc.lpfnWndProc=windowProc; wc.hInstance=instance; wc.lpszClassName=className; wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    if(!RegisterClassW(&wc)) return 1;
    RECT size{0,0,game::Width,game::Height};
    const DWORD style=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
    AdjustWindowRectEx(&size,style,FALSE,0);
    HWND hwnd=CreateWindowExW(0,className,L"Caso 1 - Exploracion 2D",style,CW_USEDEFAULT,CW_USEDEFAULT,size.right-size.left,size.bottom-size.top,nullptr,nullptr,instance,nullptr);
    if(!hwnd) return 2;
    ShowWindow(hwnd,show); lastTick=GetTickCount64();
    MSG msg{};
    while(true){
        while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){
            if(msg.message==WM_QUIT) return int(msg.wParam);
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
        ULONGLONG now=GetTickCount64();
        if(now-lastTick>=16){ float dt=float(now-lastTick)/1000.0f; lastTick=now; if(focused) state.update(dt); InvalidateRect(hwnd,nullptr,FALSE); }
        else Sleep(1);
    }
}
