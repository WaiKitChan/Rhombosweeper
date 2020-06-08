#include<windows.h>
#include<windowsx.h>
#include"main.h"
#include"system.h"
#include"board.h"

BOARD board;
const SOLID *BoardType[3]={&Hexahedron,&Dodecahedron,&Triacontahedron};
const int BoardSize[15] = {
	4,7,9,11,14,
	3,5,6,8,10,
	2,3,4,5,6
};
const int MineDensity[3]={8,6,5};

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	static PAINTSTRUCT ps;
	static PCANVAS canvas;
	static HMENU hMenu;
	static int size=1,density=6;
	switch(msg){
		case WM_PAINT:
			BeginPaint(hwnd,&ps);
			BitBlt(ps.hdc,0,0,GLOBAL_WIDTH,GLOBAL_HEIGHT,canvas->hdc,0,0,SRCCOPY);
			EndPaint(hwnd,&ps);
			break;
		case WM_MOUSEMOVE:
			if(canvas->clkstate!=CLK_RELEASED){
				POINT pt={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
				MoveCursor(canvas,&board,pt);
			}
			break;
		case WM_LBUTTONDOWN:{
			POINT pt={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
			if(PtInRect(&rcSprite,pt))canvas->clkstate=CLK_RESTART;else if(board.progress!=0){
				canvas->clkstate=CLK_REVEAL;
				UpdateSprite(canvas,SPRITE_REVEAL);
			}
			MoveCursor(canvas,&board,pt);
			break;
		}
		case WM_LBUTTONUP:
			if(canvas->clkstate==CLK_RESTART){if(board.progress>=0&&PtInRect(&rcSprite,canvas->cursor))Reinitiate(canvas,&board);}
			else if(board.progress!=0&&canvas->clkstate==CLK_REVEAL){
				RevealSquare(canvas,&board,canvas->pos);
				if(canvas->clkstate&CLK_LOSE)TerminateGame(canvas,&board,FALSE);
				else if(canvas->clkstate&CLK_WIN)TerminateGame(canvas,&board,TRUE);
				else DrawBoard(canvas,&board);
			}
			ReleaseCursor(canvas,&board);
			break;
		case WM_RBUTTONDOWN:{
			if(board.progress==0)break;
			POINT pt={GET_X_LPARAM(lParam)-FIELD_LEFT-RADIUS,GET_Y_LPARAM(lParam)-FIELD_TOP-RADIUS};
			POSITION pos=LocateCursor(&board,(float)pt.x,(float)pt.y);
			if(pos.face>=0)FlagSquare(canvas,&board,pos);
			break;
		}
		case WM_RBUTTONUP:
			ReleaseCursor(canvas,&board);
			break;
		case WM_KEYDOWN:{
			int key;
			switch(wParam){
				case VK_LEFT:case 'A':key=1;break;
				case VK_UP:case 'W':key=2;break;
				case VK_RIGHT:case 'D':key=3;break;
				case VK_DOWN:case 'S':key=4;break;
				case 'Q':key=5;break;
				case 'E':key=6;break;
				default:key=0;
			}
			if(key){
				ReleaseCursor(canvas,&board);
				RotateBoard(board,rotate[key]);
				DrawBoard(canvas,&board);
			}
			break;
		}
		case WM_TIMER:if(wParam==MAIN_TIMER)IncrementTimer(canvas);break;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case ID_6HEDRON:
				case ID_12HEDRON:
				case ID_30HEDRON:{
					int i,cmd=LOWORD(wParam)-ID_TYPE,n=BoardSize[cmd*5+size];
					for(i=0;i<3;++i)CheckMenuItem(hMenu,ID_TYPE+i,i==cmd?MF_CHECKED:MF_UNCHECKED);
					InitBoard(&board,BoardType[cmd],n,(BoardType[cmd]->type*n*n)/density);
					if(LOWORD(wParam)==ID_6HEDRON)RotateBoard(board,rotate[0]);
					InitCanvas(canvas,&board);
					break;
				}
				case ID_TINY:
				case ID_STANDARD:
				case ID_LARGE:
				case ID_MEGA:
				case ID_GIGA:{
					int i,cmd=LOWORD(wParam)-ID_SIZE;
					for(i=0;i<5;++i)CheckMenuItem(hMenu,ID_SIZE+i,i==cmd?MF_CHECKED:MF_UNCHECKED);
					size=cmd;
					break;
				}
				case ID_LOW:
				case ID_MODERATE:
				case ID_HIGH:{
					int i,cmd=LOWORD(wParam)-ID_DENSITY;
					for(i=0;i<3;++i)CheckMenuItem(hMenu,ID_DENSITY+i,i==cmd?MF_CHECKED:MF_UNCHECKED);
					density=MineDensity[cmd];
					break;
				}
				case ID_EXIT:
					PostQuitMessage(0);
					break;
			}
			break;
		case WM_CREATE:
			hMenu=GetMenu(hwnd);
			canvas=CreateCanvas(hwnd);
			InitBoard(&board,&Hexahedron,7,49);
			RotateBoard(board,rotate[0]);
			InitCanvas(canvas,&board);
			break;
		case WM_DESTROY:
			DestroyCanvas(canvas);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow) {
	WNDCLASS wc;
	HWND hwnd;
	MSG msg;

	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInst;
	wc.hIcon		 = LoadIcon(hInst,"A");
	wc.hCursor		 = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName	 = MAKEINTRESOURCE(MAIN_MENU);
	wc.lpszClassName = "WindowClass";
	if(!RegisterClass(&wc)){MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);return 0;}
	
	RECT rc={0,0,GLOBAL_WIDTH,GLOBAL_HEIGHT};
	AdjustWindowRect(&rc,WINDOW_STYLE,TRUE);
	hwnd=CreateWindow("WindowClass","Rhombosweeper",WINDOW_STYLE,CW_USEDEFAULT,CW_USEDEFAULT,rc.right-rc.left,rc.bottom-rc.top,NULL,NULL,hInst,NULL);
	if(hwnd==NULL){MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);return 0;}
	
	while(GetMessage(&msg,NULL,0,0)>0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
