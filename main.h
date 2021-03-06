#define WINDOW_STYLE		WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX

#define DIGIT_WIDTH			13
#define DIGIT_HEIGHT		23
#define MINUS_OFFSET		(DIGIT_WIDTH*10)
#define SQUARE_SIZE			32
#define SPRITE_SIZE			26
#define HEADER_HEIGHT		32
#define SPRITE_TOPPAD		3
#define DIGIT_TOPPAD		4
#define HEADER_SIDEPAD		5
#define BORDER_SIZE			10
#define FIELD_SIZE			560

#define GLOBAL_WIDTH		(BORDER_SIZE*2+FIELD_SIZE)
#define GLOBAL_HEIGHT		(BORDER_SIZE*3+HEADER_HEIGHT+FIELD_SIZE)
#define COUNTER_LEFT		(BORDER_SIZE+HEADER_SIDEPAD)
#define COUNTER_TOP			(BORDER_SIZE+DIGIT_TOPPAD)
#define COUNTER_RIGHT		(BORDER_SIZE+HEADER_SIDEPAD+DIGIT_WIDTH*3)
#define COUNTER_BOTTOM		(BORDER_SIZE+DIGIT_TOPPAD+DIGIT_HEIGHT)
#define SPRITE_LEFT			(BORDER_SIZE+(FIELD_SIZE-SPRITE_SIZE)/2)
#define SPRITE_TOP			(BORDER_SIZE+SPRITE_TOPPAD)
#define SPRITE_RIGHT		(BORDER_SIZE+(FIELD_SIZE+SPRITE_SIZE)/2)
#define SPRITE_BOTTOM		(BORDER_SIZE+SPRITE_TOPPAD+SPRITE_SIZE)
#define TIMER_LEFT			(BORDER_SIZE+FIELD_SIZE-HEADER_SIDEPAD-DIGIT_WIDTH*3)
#define TIMER_TOP			(BORDER_SIZE+DIGIT_TOPPAD)
#define TIMER_RIGHT			(BORDER_SIZE+FIELD_SIZE-HEADER_SIDEPAD)
#define TIMER_BOTTOM		(BORDER_SIZE+DIGIT_TOPPAD+DIGIT_HEIGHT)
#define FIELD_LEFT			(BORDER_SIZE)
#define FIELD_TOP			(BORDER_SIZE*2+HEADER_HEIGHT)
#define FIELD_RIGHT			(BORDER_SIZE+FIELD_SIZE)
#define FIELD_BOTTOM		(BORDER_SIZE*2+HEADER_HEIGHT+FIELD_SIZE)

#define SCALE				120.0f
#define RADIUS				280
#define SQRADIUS			78400.0f

#define COUNT(n)			(n&15)
#define MINE				16
#define FLAG				32
#define REVEALED			64
#define EMPTY(n)			(n<32)
#define TRIGGERED			128

#define SQUARE_MAXIMUM		1200
#define SIZE_MAX_HEXA		11
#define SIZE_MAX_DODECA		8
#define SIZE_MAX_TRIACONTA	5

#define CLK_RELEASED		0
#define CLK_RESTART			1
#define CLK_REVEAL			2
#define CLK_WIN				4
#define CLK_LOSE			8

#define SPRITE_NORMAL		0
#define SPRITE_PRESSED		1
#define SPRITE_REVEAL		2
#define SPRITE_LOSS			3
#define SPRITE_VICTORY		4

#define MAIN_MENU			100
#define ID_TYPE				111
#define ID_6HEDRON			111
#define ID_12HEDRON			112
#define ID_30HEDRON			113
#define ID_SIZE				121
#define ID_TINY				121
#define ID_STANDARD			122
#define ID_LARGE			123
#define ID_MEGA				124
#define ID_GIGA				125
#define ID_DENSITY			131
#define ID_LOW				131
#define ID_MODERATE			132
#define ID_HIGH				133
#define ID_EXIT				199

#define MAIN_TIMER			200

#define BM_BACKGROUND		300
#define BM_SPRITE			310
#define BM_DIGIT			320
#define BM_NUMBER			330
#define BM_MINE				440
