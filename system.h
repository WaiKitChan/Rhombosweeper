typedef struct pos {
	int face;	/* NEGATIVE VALUE INDICATES OUT_OF_RANGE */
	unsigned int x;
	unsigned int y;
} POSITION,*PPOSITION;
typedef struct canvas {
	int remain;
	int timer;
	int clkstate;
	POINT cursor;
	POSITION pos;
	HWND hwnd;
	HBRUSH hbr;
	HDC hdc;
	HDC hdcboard;
	HDC sprite;
	HDC digit;
	HDC number;
	HDC mine;
	HGDIOBJ hbm;
	HGDIOBJ hboard;
	HGDIOBJ hsprite;
	HGDIOBJ hdigit;
	HGDIOBJ hnumber;
	HGDIOBJ hmine;
} CANVAS,*PCANVAS;
const RECT rcField	 = {FIELD_LEFT,FIELD_TOP,FIELD_RIGHT,FIELD_BOTTOM};
const RECT rcSprite	 = {SPRITE_LEFT,SPRITE_TOP,SPRITE_RIGHT,SPRITE_BOTTOM};
const RECT rcCounter = {COUNTER_LEFT,COUNTER_TOP,COUNTER_RIGHT,COUNTER_BOTTOM};
const RECT rcTimer	 = {TIMER_LEFT,TIMER_TOP,TIMER_RIGHT,TIMER_BOTTOM};
PCANVAS CreateCanvas(HWND hwnd) {
	PCANVAS canvas=(PCANVAS)malloc(sizeof(CANVAS));
	memset(canvas,0,sizeof(CANVAS));
	canvas->pos.face=-1;
	canvas->hwnd=hwnd;
	canvas->hbr=CreateSolidBrush(0xc0c0c0);
	HDC hdc=GetDC(hwnd);
	canvas->hdc=CreateCompatibleDC(hdc);
	canvas->hdcboard=CreateCompatibleDC(hdc);
	canvas->sprite=CreateCompatibleDC(hdc);
	canvas->digit=CreateCompatibleDC(hdc);
	canvas->number=CreateCompatibleDC(hdc);
	canvas->mine=CreateCompatibleDC(hdc);
	HINSTANCE hInst=GetModuleHandle(NULL);
	HBITMAP hbm=CreateCompatibleBitmap(hdc,1,1);	/* DUMMY */
	canvas->hboard=SelectObject(canvas->hdcboard,hbm);
	hbm=LoadBitmap(hInst,MAKEINTRESOURCE(BM_BACKGROUND));
	canvas->hbm=SelectObject(canvas->hdc,hbm);
	hbm=LoadBitmap(hInst,MAKEINTRESOURCE(BM_SPRITE));
	canvas->hsprite=SelectObject(canvas->sprite,hbm);
	hbm=LoadBitmap(hInst,MAKEINTRESOURCE(BM_DIGIT));
	canvas->hdigit=SelectObject(canvas->digit,hbm);
	hbm=LoadBitmap(hInst,MAKEINTRESOURCE(BM_NUMBER));
	canvas->hnumber=SelectObject(canvas->number,hbm);
	hbm=LoadBitmap(hInst,MAKEINTRESOURCE(BM_MINE));
	canvas->hmine=SelectObject(canvas->mine,hbm);
	ReleaseDC(hwnd,hdc);
	return canvas;
}
void DestroyCanvas(PCANVAS canvas) {
	DeleteObject(canvas->hbr);
	DeleteObject(SelectObject(canvas->hdc,canvas->hbm));
	DeleteObject(SelectObject(canvas->hdcboard,canvas->hboard));
	DeleteObject(SelectObject(canvas->sprite,canvas->hsprite));
	DeleteObject(SelectObject(canvas->digit,canvas->hdigit));
	DeleteObject(SelectObject(canvas->number,canvas->hnumber));
	DeleteObject(SelectObject(canvas->mine,canvas->hmine));
	DeleteDC(canvas->hdc);
	DeleteDC(canvas->hboard);
	DeleteDC(canvas->sprite);
	DeleteDC(canvas->digit);
	DeleteDC(canvas->number);
	DeleteDC(canvas->mine);
	free(canvas);
}
void UpdateSprite(PCANVAS canvas,const int state) {
	BitBlt(canvas->hdc,SPRITE_LEFT,SPRITE_TOP,SPRITE_SIZE,SPRITE_SIZE,canvas->sprite,0,SPRITE_SIZE*state,SRCCOPY);
	InvalidateRect(canvas->hwnd,&rcSprite,FALSE);
}
void UpdateCounter(PCANVAS canvas) {
	BitBlt(canvas->hdc,COUNTER_LEFT,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->remain/100),0,SRCCOPY);
	BitBlt(canvas->hdc,COUNTER_LEFT+DIGIT_WIDTH,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->remain/10%10),0,SRCCOPY);
	BitBlt(canvas->hdc,COUNTER_LEFT+DIGIT_WIDTH*2,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->remain%10),0,SRCCOPY);
	InvalidateRect(canvas->hwnd,&rcCounter,FALSE);
}
void IncrementTimer(PCANVAS canvas) {
	if(++canvas->timer==1000){KillTimer(canvas->hwnd,MAIN_TIMER);return;}
	BitBlt(canvas->hdc,TIMER_LEFT,TIMER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->timer/100),0,SRCCOPY);
	BitBlt(canvas->hdc,TIMER_LEFT+DIGIT_WIDTH,TIMER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->timer/10%10),0,SRCCOPY);
	BitBlt(canvas->hdc,TIMER_LEFT+DIGIT_WIDTH*2,TIMER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->timer%10),0,SRCCOPY);
	InvalidateRect(canvas->hwnd,&rcTimer,FALSE);
}


typedef float VECTOR[3];	/* X Y Z */
typedef float MATRIX[9];	/* VERTICALLY INDEXED */
void Transform(VECTOR *v,const int n,const MATRIX m) {
	int i,j;
	VECTOR t;
	for(i=0;i<n;++i){
		memcpy(&t,&v[i],sizeof(VECTOR));
		for(j=0;j<3;++j)v[i][j]=t[0]*m[j]+t[1]*m[j+3]+t[2]*m[j+6];
	}
}
typedef struct face {
	VECTOR pt[3];	/* TOP LEFT, TOP RIGHT, BOTTOM LEFT AND IMPLICIT BOTTOM RIGHT */
	VECTOR normal;	/* NORMAL VECTOR FACING THE EXTERIOR */
} FACE,*PFACE;
typedef int MAP[4];	/* LEFT TOP RIGHT BOTTOM */
typedef struct solid {
	const int type;
	const int vertex;
	const FACE *face;
	const MAP *map;
} SOLID,*PSOLID;
typedef struct board {
	int type;		/* TYPE OF POLYHEDRON, VALID VALUES ARE 6,12 AND 30 */
	int vertex;		/* THE GREATER VERTEX FIGURE */
	int size;		/* SIZE OF BOARD ON A SINGLE FACE */
	int mine;		/* MINE COUNT */
	int progress;	/* NEGATIVE == UNSTARTED, 0 == GAME OVER */
	int goal;
	int data[SQUARE_MAXIMUM];
	FACE face[30];
	MAP map[30];
} BOARD,*PBOARD;
void InitBoard(PBOARD board,const SOLID *s,const int size,const int mine) {
	memset(board,0,sizeof(BOARD));
	board->type=s->type;
	board->vertex=s->vertex;
	board->size=size;
	board->mine=mine;
	board->progress=-1;
	board->goal=s->type*size*size-mine;
	memcpy(board->face,s->face,sizeof(FACE)*s->type);
	memcpy(board->map,s->map,sizeof(MAP)*s->type);
	int i=0,j;
	float *f=(float*)board->face;
	for(;i<s->type;++i,f+=12)for(j=0;j<9;++j)f[j]*=SCALE;
}
#define RotateBoard(b,m)		Transform((VECTOR*)&b.face,b.type*4,m)
void DrawBoard(PCANVAS canvas,const BOARD *board) {
	int i,j;
	POINT pt[3];
	const FACE *face;
	FillRect(canvas->hdc,&rcField,canvas->hbr);
	for(i=0;i<board->type;++i){
		face=&board->face[i];
		if(face->normal[2]<=0.0f)continue;
		for(j=0;j<3;++j){
			pt[j].x=(LONG)face->pt[j][0]+FIELD_LEFT+RADIUS;
			pt[j].y=(LONG)face->pt[j][1]+FIELD_TOP+RADIUS;
		}
		PlgBlt(canvas->hdc,pt,canvas->hdcboard,0,SQUARE_SIZE*board->size*i,SQUARE_SIZE*board->size,SQUARE_SIZE*board->size,NULL,0,0);
	}
	InvalidateRect(canvas->hwnd,&rcField,FALSE);
}
void DrawFace(PCANVAS canvas,const BOARD *board,const int n) {
	int i;
	POINT pt[3];
	const FACE *f=&board->face[n];
	if(f->normal[2]<=0.0f)return;
	for(i=0;i<3;++i){pt[i].x=(LONG)f->pt[i][0]+FIELD_LEFT+RADIUS;pt[i].y=(LONG)f->pt[i][1]+FIELD_TOP+RADIUS;}
	PlgBlt(canvas->hdc,pt,canvas->hdcboard,0,SQUARE_SIZE*board->size*n,SQUARE_SIZE*board->size,SQUARE_SIZE*board->size,NULL,0,0);
}

void InitCanvas(PCANVAS canvas,const BOARD *board) {
	int i,j;
	canvas->remain=board->mine;
	canvas->timer=0;
	BitBlt(canvas->hdc,COUNTER_LEFT,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->remain/100),0,SRCCOPY);
	BitBlt(canvas->hdc,COUNTER_LEFT+DIGIT_WIDTH,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->remain/10%10),0,SRCCOPY);
	BitBlt(canvas->hdc,COUNTER_LEFT+DIGIT_WIDTH*2,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,DIGIT_WIDTH*(canvas->remain%10),0,SRCCOPY);
	for(i=0;i<3;++i)BitBlt(canvas->hdc,TIMER_LEFT+DIGIT_WIDTH*i,TIMER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,0,0,SRCCOPY);
	BitBlt(canvas->hdc,SPRITE_LEFT,SPRITE_TOP,SPRITE_SIZE,SPRITE_SIZE,canvas->sprite,0,0,SRCCOPY);
	
	HDC hdc=GetDC(canvas->hwnd);
	DeleteObject(SelectObject(canvas->hdcboard,CreateCompatibleBitmap(hdc,SQUARE_SIZE*board->size,SQUARE_SIZE*board->size*board->type)));
	ReleaseDC(canvas->hwnd,hdc);
	for(i=0;i<board->size;++i)for(j=0;j<board->size*board->type;++j)BitBlt(canvas->hdcboard,SQUARE_SIZE*i,SQUARE_SIZE*j,SQUARE_SIZE,SQUARE_SIZE,canvas->mine,0,0,SRCCOPY);
	
	DrawBoard(canvas,board);
	KillTimer(canvas->hwnd,MAIN_TIMER);
	InvalidateRect(canvas->hwnd,NULL,FALSE);
}
void Reinitiate(PCANVAS canvas,PBOARD board) {
	board->progress=-1;
	memset(board->data,0,sizeof(int)*SQUARE_MAXIMUM);
	InitCanvas(canvas,board);
}

POSITION LocateCursor(const BOARD *board,const float x,const float y) {
	if(x*x+y*y>SQRADIUS)return (POSITION){-1,0,0};
	unsigned int i,j,k;
	float u,v,w,a,b,c,d,det;
	const float *f;
	for(k=0;k<board->type;++k){
		f=(float*)&board->face[k];
		if(f[11]<=0.0f)continue;
		u=x-f[0];		/* [ a c ] [ i ] = [ u ] */
		v=y-f[1];		/* [ b d ] [ j ] = [ v ] */
		a=f[3]-f[0];
		b=f[4]-f[1];
		c=f[6]-f[0];
		d=f[7]-f[1];
		det=((float)board->size)/(a*d-b*c);
		w=(u*d-v*c)*det;
		i=w<0?UINT_MAX:(unsigned int)w;
		w=(v*a-u*b)*det;
		j=w<0?UINT_MAX:(unsigned int)w;
		if(i<board->size&&j<board->size)return (POSITION){k,i,j};
	}
	return (POSITION){-1,0,0};
}
void MoveCursor(PCANVAS canvas,const BOARD *board,const POINT pt) {
	switch(canvas->clkstate){
		case CLK_RESTART:{
			BOOL i,j;
			i=PtInRect(&rcSprite,canvas->cursor);
			j=PtInRect(&rcSprite,pt);
			if(i^j)UpdateSprite(canvas,j);
			break;
		}
		case CLK_REVEAL:{
			POSITION p=canvas->pos;
			int n=p.x+(p.y+p.face*board->size)*board->size;
			if(EMPTY(board->data[n])){
				BitBlt(canvas->hdcboard,SQUARE_SIZE*p.x,SQUARE_SIZE*(p.face*board->size+p.y),SQUARE_SIZE,SQUARE_SIZE,canvas->mine,0,0,SRCCOPY);
				DrawFace(canvas,board,p.face);
			}
			p=LocateCursor(board,(float)(pt.x-FIELD_LEFT-RADIUS),(float)(pt.y-FIELD_TOP-RADIUS));
			n=p.x+(p.y+p.face*board->size)*board->size;
			if(EMPTY(board->data[n])){
				BitBlt(canvas->hdcboard,SQUARE_SIZE*p.x,SQUARE_SIZE*(p.face*board->size+p.y),SQUARE_SIZE,SQUARE_SIZE,canvas->mine,0,SQUARE_SIZE,SRCCOPY);
				DrawFace(canvas,board,p.face);
			}
			canvas->pos=p;
			InvalidateRect(canvas->hwnd,&rcField,FALSE);
		}
	}
	canvas->cursor=pt;
}
void ReleaseCursor(PCANVAS canvas,const BOARD *board) {
	if(canvas->clkstate==CLK_REVEAL){
		POSITION p=canvas->pos;
		int n=p.x+(p.y+p.face*board->size)*board->size;
		if(EMPTY(board->data[n])){
			BitBlt(canvas->hdcboard,SQUARE_SIZE*p.x,SQUARE_SIZE*(p.face*board->size+p.y),SQUARE_SIZE,SQUARE_SIZE,canvas->mine,0,0,SRCCOPY);
			DrawFace(canvas,board,p.face);
		}
	}
	if(board->progress!=0||canvas->clkstate==CLK_RESTART)UpdateSprite(canvas,0);
	canvas->clkstate=CLK_RELEASED;
	canvas->cursor.x=0;
	canvas->cursor.y=0;
	canvas->pos.face=-1;
}
void InitGame(PCANVAS canvas,PBOARD board,const POSITION pos) {
	const int width=board->size,widthx=board->size+2,span=board->size*board->size,spanx=widthx*widthx,size=board->type*board->size*board->size;
	int i,j,k,l,n,m,t,p,buf[size],len=size;
	for(i=0;i<size;++i)buf[i]=i;
	/* CLEAR NEIGHBOURHOOD */
	m=pos.x+2;n=m-3;if(n<0)n=0;if(m>width)m=width;
	p=pos.y+2;t=p-3;if(t<0)t=0;if(p>width)p=width;
	for(i=n,k=pos.face*span;i<m;++i)for(j=t;j<p;++j){buf[i+j*width+k]=-1;--len;}
	i=0;j=size-1;
	while(i<len){
		while(buf[i]>=0)++i;
		while(buf[j]<0)--j;
		buf[i++]=buf[j--];
	}
	/* LAY MINES */
	for(i=0;i<board->mine;++i){
		j=rand()%len;
		board->data[buf[j]]|=MINE;
		buf[j]=buf[--len];
	}
	/* COUNT MINES */
	int cnt[spanx];
	for(k=0;k<board->type;++k){
		/* FACE */
		memset(cnt,0,sizeof(int)*spanx);
		for(i=0,l=k*span;i<width;++i)for(j=0;j<width;++j)if(board->data[i+j*width+l]&MINE)for(n=0;n<3;++n)for(m=0;m<3;++m)++cnt[i+n+(j+m)*widthx];
		for(i=0;i<width;++i)for(j=0;j<width;++j)board->data[i+j*width+l]|=cnt[i+1+(j+1)*widthx];
		/* EDGE */
		for(i=0;i<4;++i){
			memset(cnt,0,sizeof(int)*widthx);
			n=board->map[k][i];
			for(j=0;j<4;++j)if(board->map[n][j]==k){m=j;break;}
			switch(m){
				case 0:j=span-width;l=-width;break;
				case 1:j=0;l=1;break;
				case 2:j=width-1;l=width;break;
				case 3:j=span-1;l=-1;
			}
			for(t=0,n=n*span;t<width;j+=l,++t)if(board->data[n+j]&MINE)for(p=0;p<3;++p)++cnt[t+p];
			switch(i){
				case 0:j=0;l=width;break;
				case 1:j=width-1;l=-1;break;
				case 2:j=span-1;l=-width;break;
				case 3:j=span-width;l=1;
			}
			for(t=0,n=k*span;t<width;j+=l,++t)board->data[n+j]+=cnt[t+1];
		}
		/* VERTEX */
		for(i=2,j=width-1;i>=0;i-=2,j=span-width){
			p=k;
			n=board->map[p][i];
			for(l=0;l<4;++l)if(board->map[n][l]==p){m=l;break;}
			for(t=3;t<board->vertex;++t){
				p=n;
				n=board->map[p][(m+1)%4];
				for(l=0;l<4;++l)if(board->map[n][l]==p){m=l;break;}
				switch(m){
					case 0:l=0;break;
					case 1:l=width-1;break;
					case 2:l=span-1;break;
					case 3:l=span-width;
				}
				if(board->data[n*span+l]&MINE)++board->data[k*span+j];
			}
		}
	}
	/* GAME START */
	board->progress=0;
	SetTimer(canvas->hwnd,MAIN_TIMER,1000,NULL);
	IncrementTimer(canvas);
}
void TerminateGame(PCANVAS canvas,PBOARD board,const BOOL win){
	board->progress=0;
	UpdateSprite(canvas,SPRITE_LOSS+win);
	int i,j,k;
	const int x=board->size,y=board->type*board->size;
	if(win){
		for(i=0;i<x;++i)for(j=0;j<y;++j)if(EMPTY(board->data[i+j*x]))BitBlt(canvas->hdcboard,SQUARE_SIZE*i,SQUARE_SIZE*j,SQUARE_SIZE,SQUARE_SIZE,canvas->mine,SQUARE_SIZE,0,SRCCOPY);
		for(i=0;i<3;++i)BitBlt(canvas->hdc,COUNTER_LEFT+DIGIT_WIDTH*i,COUNTER_TOP,DIGIT_WIDTH,DIGIT_HEIGHT,canvas->digit,0,0,SRCCOPY);
		InvalidateRect(canvas->hwnd,&rcCounter,FALSE);
	} else {
		for(i=0;i<x;++i)for(j=0;j<y;++j){
			k=i+j*x;
			if(board->data[k]&MINE){if(!(board->data[k]&(FLAG|TRIGGERED)))BitBlt(canvas->hdcboard,SQUARE_SIZE*i,SQUARE_SIZE*j,SQUARE_SIZE,SQUARE_SIZE,canvas->mine,SQUARE_SIZE,SQUARE_SIZE,SRCCOPY);}
			else if(board->data[k]&FLAG)BitBlt(canvas->hdcboard,SQUARE_SIZE*i,SQUARE_SIZE*j,SQUARE_SIZE,SQUARE_SIZE,canvas->mine,0,SQUARE_SIZE*2,SRCCOPY);
		}
	}
	KillTimer(canvas->hwnd,MAIN_TIMER);
	DrawBoard(canvas,board);
}
void FlagSquare(PCANVAS canvas,PBOARD board,const POSITION pos) {
	const int n=pos.x+(pos.y+pos.face*board->size)*board->size;
	if(board->data[n]&REVEALED)return;
	board->data[n]^=FLAG;
	if((board->data[n]&FLAG)&&canvas->remain==0){board->data[n]^=FLAG;return;}
	canvas->remain+=board->data[n]&FLAG?-1:1;
	UpdateCounter(canvas);
	BitBlt(canvas->hdcboard,SQUARE_SIZE*pos.x,SQUARE_SIZE*(pos.y+pos.face*board->size),SQUARE_SIZE,SQUARE_SIZE,canvas->mine,board->data[n]&FLAG?SQUARE_SIZE:0,0,SRCCOPY);
	DrawFace(canvas,board,pos.face);
	InvalidateRect(canvas->hwnd,&rcField,FALSE);
}
void RevealSquare(PCANVAS canvas,PBOARD board,const POSITION pos) {
	if(pos.face<0)return;
	if(pos.x>=board->size||pos.y>=board->size)return;
	const int n=pos.x+(pos.y+pos.face*board->size)*board->size;
	if(board->data[n]&(FLAG|REVEALED))return;
	board->data[n]|=REVEALED;
	
	if(board->progress<0)InitGame(canvas,board,pos);
	
	if(board->data[n]&MINE){
		board->data[n]|=TRIGGERED;
		canvas->clkstate|=CLK_LOSE;
		BitBlt(canvas->hdcboard,SQUARE_SIZE*pos.x,SQUARE_SIZE*(pos.y+pos.face*board->size),SQUARE_SIZE,SQUARE_SIZE,canvas->mine,SQUARE_SIZE,SQUARE_SIZE*2,SRCCOPY);
	} else {
		const int count=COUNT(board->data[n]);
		BitBlt(canvas->hdcboard,SQUARE_SIZE*pos.x,SQUARE_SIZE*(pos.y+pos.face*board->size),SQUARE_SIZE,SQUARE_SIZE,canvas->number,0,SQUARE_SIZE*count,SRCCOPY);
		if(count==0){int i,j;for(i=-1;i<=1;++i)for(j=-1;j<=1;++j)RevealSquare(canvas,board,(POSITION){pos.face,pos.x+i,pos.y+j});}
	}
	
	if(++board->progress==board->goal)canvas->clkstate|=CLK_WIN;
}
