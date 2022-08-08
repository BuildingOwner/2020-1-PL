// 1971233 최재완
// 1인용 두더지 잡기 게임
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define WIDTH 80
#define HEIGHT 24

#define LEFT_END 2
#define RIGHT_END 77

HANDLE COUT = 0;    // 콘솔 출력 장치
HANDLE CIN = 0;        // 콘솔 입력 장치
int score = 0;
int time_out = 60;

typedef struct {
	int x;
	int y;
	int remain;
	int score;
	int color;
}TARGET;

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos; // Windows.h 에 정의
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void removeCursor(void) { // 커서를 안보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void showCursor(void) { // 커서를 보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void cls(int text_color, int bg_color) // 화면 지우기
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void clearconsole() {
	COORD Coor = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 80 * 30, Coor, &dw); // 콘솔창 화면을 지운다.
}

void draw_box(int x1, int y1, int x2, int y2, char ch)
{
	int x, y;
	for (x = x1; x <= x2; x++) {
		gotoxy(x, y1);
		printf("%c", ch);
		gotoxy(x, y2);
		printf("%c", ch);
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		printf("%c", ch);
		gotoxy(x2, y);
		printf("%c", ch);
	}
}

void draw_box2(int x1, int y1, int x2, int y2, char* ch)
{
	int x, y;
	for (x = x1; x <= x2; x += 2) {
		gotoxy(x, y1);
		printf("%s", ch);
		gotoxy(x, y2);
		printf("%s", ch);
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		printf("%s", ch);
		gotoxy(x2, y);
		printf("%s", ch);
	}
}

void draw_box3(int x1, int y1, int x2, int y2)
{
	int x, y;
	for (x = x1; x <= x2; x += 2) {
		gotoxy(x, y1);
		printf("%s", "─");
		gotoxy(x, y2);
		printf("%s", "─");
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		printf("%s", "│");
		gotoxy(x2, y);
		printf("%s", "│");
	}
	gotoxy(x1, y1); printf("┌");
	gotoxy(x1, y2); printf("└");
	gotoxy(x2, y1); printf("┐");
	gotoxy(x2, y2); printf("┘");
}

int be_input()
{
	INPUT_RECORD input_record;
	DWORD input_count;

	PeekConsoleInput(CIN, &input_record, 1, &input_count);
	return input_count;
}

int get_input(WORD* vkey, COORD* pos)
{
	INPUT_RECORD input_record;
	DWORD input_count;

	ReadConsoleInput(CIN, &input_record, 1, &input_count);
	switch (input_record.EventType) {
	case MOUSE_EVENT:
		if (pos && (input_record.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) {
			CONSOLE_SCREEN_BUFFER_INFO csbi;

			GetConsoleScreenBufferInfo(COUT, &csbi);

			*pos = input_record.Event.MouseEvent.dwMousePosition;
			pos->X -= csbi.srWindow.Left;
			pos->Y -= csbi.srWindow.Top;

			return MOUSE_EVENT;
		}
		break;
	}

	FlushConsoleInputBuffer(CIN);
	return 0;
}

void show_time(int remain_time)
{
	gotoxy(11, 4);
	printf("남은 시간은 %2d초야", remain_time);
}

void map(TARGET tar[]) {
	draw_box3(55, 3, 77, 7);  // stage
	draw_box3(55, 9, 77, 13);  // score
	draw_box3(55, 15, 77, 27);  // tip
	draw_box2(5, 7, 49, 28, "■");  // 울타리
	gotoxy(7, 3);
	printf("●");
	gotoxy(7, 4);
	printf("ㅅ");
	gotoxy(7, 5);
	printf("ㅅ");
	draw_box3(9, 1, 49, 5);
	gotoxy(11, 2);
	printf("두더지를 잡아 우리 농장을 구해줘!");
	gotoxy(59, 18);
	printf("● : 1 포인트");
	gotoxy(59, 21);
	textcolor(RED2, BLACK);
	printf("●");
	textcolor(WHITE, BLACK);
	printf(" : -2 포인트");
	gotoxy(59, 24);
	textcolor(YELLOW2, BLACK);
	printf("●");
	textcolor(WHITE, BLACK);
	printf(" : 3 포인트");
}

void get_point(TARGET tar) {
	int n;
	n = tar.remain;
	if (n == 0) {
		return;
	}
	else {
		score += tar.score;
		gotoxy(62, 11);
		printf("점수 : %2d", score);
	}
}

void intro() {
	draw_box2(0, 0, 80, 30, "■");
	draw_box3(35, 19, 45, 21);
	gotoxy(37, 20);
	printf("시작하기");
	draw_box3(35, 23, 45, 25);
	gotoxy(38, 24);
	printf("도움말");
	gotoxy(18, 5); printf("  ■■■        ■      ■      ■  ■■■■■");
	gotoxy(18, 6); printf("■             ■■      ■    ■   ■        ");
	gotoxy(18, 7); printf("  ■■■      ■  ■      ■  ■    ■■■■■");
	gotoxy(18, 8); printf("        ■   ■■■■      ■■     ■        ");
	gotoxy(18, 9); printf("  ■■■    ■      ■      ■      ■■■■■");
	gotoxy(18, 11); printf("■■■■■      ■      ■■■■    ■      ■");
	gotoxy(18, 12); printf("■             ■■     ■      ■  ■■  ■■");
	gotoxy(18, 13); printf("■■■■■    ■  ■    ■■■■    ■  ■  ■");
	gotoxy(18, 14); printf("■           ■■■■   ■    ■    ■      ■");
	gotoxy(18, 15); printf("■          ■      ■  ■      ■  ■      ■");
	gotoxy(18, 19); printf("●");
	gotoxy(18, 20); printf("ㅅ");
	gotoxy(18, 21); printf("ㅅ");
	gotoxy(10, 25); printf("●");
	gotoxy(10, 26); printf("ㅅ");
	gotoxy(10, 27); printf("ㅅ");
	gotoxy(25, 24); printf("●");
	gotoxy(25, 25); printf("ㅅ");
	gotoxy(25, 26); printf("ㅅ");
	gotoxy(55, 18); printf(" ■■ ");
	gotoxy(55, 19); printf("■OO■");
	gotoxy(55, 20); printf("■■■");
	gotoxy(70, 17); printf(" ■■ ");
	gotoxy(70, 18); printf("■OO■");
	gotoxy(70, 19); printf("■■■");
	gotoxy(63, 23); printf(" ■■ ");
	gotoxy(63, 24); printf("■OO■");
	gotoxy(63, 25); printf("■■■");
}

void help() {
	clearconsole();
	draw_box2(0, 0, 80, 30, "■");
	gotoxy(10, 3);
	printf("도움말");
	gotoxy(10, 5);
	printf("이 게임은 두더지 잡기 게임입니다.");
	gotoxy(10, 6);
	printf("농장에 작물을 망치는 두더지를 잡아 농장을 지키세요.");
	gotoxy(10, 7);
	printf("조작은 마우스 클릭으로 두더지를 잡아 주세요.");
	gotoxy(10, 8);
	printf("스테이지당 제한 시간은 1분입니다.");
	gotoxy(10, 9);
	printf("두더지는 눈을 클릭해야 잡을 수 있습니다.");
	gotoxy(10, 28);
	printf("아무키나 누르면 타이틀로 넘어갑니다.");
}

void end_win() {
	clearconsole();
	gotoxy(35, 18);
	printf("내 점수 : %d", score);
	gotoxy(25, 19);
	printf("종료하려면 아무키나 눌러주세요.");
	draw_box2(0, 0, 80, 30, "■");
	int color;
	int frame = 0, sync = 5, tog = 1;
	while (1) {
		color = rand() % 15 + 1;
		textcolor(color, BLACK);
		gotoxy(11, 7); printf("  ■■■    ■          ■■■■■  ■■■■■  ■■■■  ");
		gotoxy(11, 8); printf("■      ■  ■          ■          ■          ■      ■");
		gotoxy(11, 9); printf("■          ■          ■■■■■  ■■■■■  ■■■■  ");
		gotoxy(11, 10); printf("■      ■  ■          ■          ■          ■    ■  ");
		gotoxy(11, 11); printf("  ■■■    ■■■■■  ■■■■■  ■■■■■  ■      ■");
		frame++;
		if (frame % sync == 0) {
			if (tog % 2 == 0) {
				gotoxy(7, 17); printf("      ");
				gotoxy(7, 18); printf("      ");
				gotoxy(7, 19); printf("      ");
				gotoxy(40, 25); printf("      ");
				gotoxy(40, 26); printf("      ");
				gotoxy(40, 27); printf("      ");
				gotoxy(68, 24); printf("      ");
				gotoxy(68, 25); printf("      ");
				gotoxy(68, 26); printf("      ");
				gotoxy(70, 17); printf("      ");
				gotoxy(70, 18); printf("      ");
				gotoxy(70, 19); printf("      ");
				gotoxy(74, 10); printf("  ");
				gotoxy(74, 11); printf("  ");
				gotoxy(74, 12); printf("  ");
				gotoxy(17, 21); printf("  ");
				gotoxy(17, 22); printf("  ");
				gotoxy(17, 23); printf("  ");
				gotoxy(8, 8); printf("  ");
				gotoxy(8, 9); printf("  ");
				gotoxy(8, 10); printf("  ");
				gotoxy(58, 18); printf(" ■■ ");
				gotoxy(58, 19); printf("■XX■");
				gotoxy(58, 20); printf("■■■");
				gotoxy(3, 8); printf(" ■■ ");
				gotoxy(3, 9); printf("■XX■");
				gotoxy(3, 10); printf("■■■");
				gotoxy(17, 21); printf(" ■■ ");
				gotoxy(17, 22); printf("■XX■");
				gotoxy(17, 23); printf("■■■");
				gotoxy(70, 10); printf(" ■■ ");
				gotoxy(70, 11); printf("■XX■");
				gotoxy(70, 12); printf("■■■");
				gotoxy(7, 17); printf("●");
				gotoxy(7, 18); printf("ㅅ");
				gotoxy(7, 19); printf("ㅅ");
				gotoxy(40, 26); printf("●");
				gotoxy(40, 27); printf("ㅅ");
				gotoxy(40, 28); printf("ㅅ");
				gotoxy(68, 24); printf("●");
				gotoxy(68, 25); printf("ㅅ");
				gotoxy(68, 26); printf("ㅅ");
				tog++;
			}
			else {
				gotoxy(58, 18); printf("      ");
				gotoxy(58, 19); printf("      ");
				gotoxy(58, 20); printf("      ");
				gotoxy(3, 8); printf("      ");
				gotoxy(3, 9); printf("      ");
				gotoxy(3, 10); printf("      ");
				gotoxy(17, 21); printf("      ");
				gotoxy(17, 22); printf("      ");
				gotoxy(17, 23); printf("      ");
				gotoxy(70, 10); printf("      ");
				gotoxy(70, 11); printf("      ");
				gotoxy(70, 12); printf("      ");
				gotoxy(7, 17); printf("  ");
				gotoxy(7, 18); printf("  ");
				gotoxy(7, 19); printf("  ");
				gotoxy(40, 26); printf("  ");
				gotoxy(40, 27); printf("  ");
				gotoxy(40, 28); printf("  ");
				gotoxy(68, 24); printf("  ");
				gotoxy(68, 25); printf("  ");
				gotoxy(68, 26); printf("  ");
				gotoxy(7, 17); printf(" ■■ ");
				gotoxy(7, 18); printf("■XX■");
				gotoxy(7, 19); printf("■■■");
				gotoxy(40, 25); printf(" ■■ ");
				gotoxy(40, 26); printf("■XX■");
				gotoxy(40, 27); printf("■■■");
				gotoxy(68, 24); printf(" ■■ ");
				gotoxy(68, 25); printf("■XX■");
				gotoxy(68, 26); printf("■■■");
				gotoxy(70, 17); printf(" ■■ ");
				gotoxy(70, 18); printf("■XX■");
				gotoxy(70, 19); printf("■■■");
				gotoxy(74, 10); printf("●");
				gotoxy(74, 11); printf("ㅅ");
				gotoxy(74, 12); printf("ㅅ");
				gotoxy(17, 21); printf("●");
				gotoxy(17, 22); printf("ㅅ");
				gotoxy(17, 23); printf("ㅅ");
				gotoxy(8, 8); printf("●");
				gotoxy(8, 9); printf("ㅅ");
				gotoxy(8, 10); printf("ㅅ");
				tog++;
			}
		}
		Sleep(200);
		if (kbhit()) {
			break;
		}
	}
}

void end_lose() {
	clearconsole();
	gotoxy(35, 20);
	printf("내 점수 : %d", score);
	gotoxy(25, 21);
	printf("종료하려면 아무키나 눌러주세요.");
	draw_box2(0, 0, 80, 30, "■");
	int color;
	int frame = 0, sync = 5, tog = 1;
	while (1) {
		color = rand() % 15 + 1;
		textcolor(color, BLACK);
		gotoxy(18, 6); printf("  ■■■        ■      ■      ■  ■■■■■");
		gotoxy(18, 7); printf("■             ■■     ■■  ■■  ■        ");
		gotoxy(18, 8); printf("■  ■■■    ■  ■    ■  ■  ■  ■■■■■");
		gotoxy(18, 9); printf("■      ■   ■■■■   ■      ■  ■        ");
		gotoxy(18, 10); printf("  ■■■    ■      ■  ■      ■  ■■■■■");
		gotoxy(18, 12); printf("  ■■■    ■      ■  ■■■■■  ■■■■  ");
		gotoxy(18, 13); printf("■      ■   ■    ■   ■          ■      ■");
		gotoxy(18, 14); printf("■      ■    ■  ■    ■■■■■  ■■■■  ");
		gotoxy(18, 15); printf("■      ■     ■■     ■          ■    ■  ");
		gotoxy(18, 16); printf("  ■■■        ■      ■■■■■  ■      ■");
		frame++;
		if (frame % sync == 0) {
			if (tog % 2 == 0) {
				gotoxy(7, 17); printf("      ");
				gotoxy(7, 18); printf("      ");
				gotoxy(7, 19); printf("      ");
				gotoxy(40, 25); printf("      ");
				gotoxy(40, 26); printf("      ");
				gotoxy(40, 27); printf("      ");
				gotoxy(68, 24); printf("      ");
				gotoxy(68, 25); printf("      ");
				gotoxy(68, 26); printf("      ");
				gotoxy(70, 17); printf("      ");
				gotoxy(70, 18); printf("      ");
				gotoxy(70, 19); printf("      ");
				gotoxy(68, 10); printf("      ");
				gotoxy(17, 21); printf("      ");
				gotoxy(8, 8); printf("      ");
				gotoxy(58, 18); printf(" ■■ ");
				gotoxy(58, 19); printf("■^^■");
				gotoxy(58, 20); printf("■■■");
				gotoxy(8, 8); printf(" ■■ ");
				gotoxy(8, 9); printf("■^^■");
				gotoxy(8, 10); printf("■■■");
				gotoxy(17, 21); printf(" ■■ ");
				gotoxy(17, 22); printf("■^^■");
				gotoxy(17, 23); printf("■■■");
				gotoxy(70, 10); printf(" ■■ ");
				gotoxy(70, 11); printf("■^^■");
				gotoxy(70, 12); printf("■■■");
				gotoxy(7, 17); printf("●TL");
				gotoxy(40, 26); printf("●TL");
				gotoxy(68, 24); printf("●TL");
				tog++;
			}
			else {
				gotoxy(58, 18); printf("      ");
				gotoxy(58, 19); printf("      ");
				gotoxy(58, 20); printf("      ");
				gotoxy(8, 8); printf("      ");
				gotoxy(8, 9); printf("      ");
				gotoxy(8, 10); printf("      ");
				gotoxy(17, 21); printf("      ");
				gotoxy(17, 22); printf("      ");
				gotoxy(17, 23); printf("      ");
				gotoxy(70, 10); printf("      ");
				gotoxy(70, 11); printf("      ");
				gotoxy(70, 12); printf("      ");
				gotoxy(7, 17); printf("      ");
				gotoxy(40, 26); printf("      ");
				gotoxy(68, 24); printf("      ");
				gotoxy(7, 17); printf(" ■■ ");
				gotoxy(7, 18); printf("■^^■");
				gotoxy(7, 19); printf("■■■");
				gotoxy(40, 25); printf(" ■■ ");
				gotoxy(40, 26); printf("■^^■");
				gotoxy(40, 27); printf("■■■");
				gotoxy(68, 24); printf(" ■■ ");
				gotoxy(68, 25); printf("■^^■");
				gotoxy(68, 26); printf("■■■");
				gotoxy(70, 17); printf(" ■■ ");
				gotoxy(70, 18); printf("■^^■");
				gotoxy(70, 19); printf("■■■");
				gotoxy(68, 10); printf("●TL");
				gotoxy(17, 21); printf("●TL");
				gotoxy(8, 8); printf("●TL");
				tog++;
			}
		}
		Sleep(200);
		if (kbhit()) {
			break;
		}
	}
}

void main()
{
	removeCursor();
	DWORD mode;
	WORD key;
	COORD pos;

	int event;        // 마우스 이벤트에 이용
	int x;            // 마우스의 x좌표 저장소
	int y;            // 마우스의 y좌표 저장소

	CIN = GetStdHandle(STD_INPUT_HANDLE);
	COUT = GetStdHandle(STD_OUTPUT_HANDLE);
	// 마우스 활성화
	GetConsoleMode(CIN, &mode);
	SetConsoleMode(CIN, mode | ENABLE_MOUSE_INPUT);

	int run_time, start_time, remain_time, last_remain_time;
	srand(time(NULL));
	last_remain_time = remain_time = time_out;

	int num = 0, stage = 1;
	int frame_count = 0;
	int remain_sync = 10;
	int appear_sync = 100;
	int time_sync = 100;
	int complete = 0;
	int st1 = stage;
	int st2 = stage;
	int ddcolor[] = { YELLOW2, RED2, WHITE };
	int dcolor;
	TARGET target[9];
	target[0].x = 13; target[0].y = 10;
	target[1].x = 27; target[1].y = 12;
	target[2].x = 40; target[2].y = 11;
	target[3].x = 18; target[3].y = 16;
	target[4].x = 35; target[4].y = 17;
	target[5].x = 43; target[5].y = 19;
	target[6].x = 14; target[6].y = 24;
	target[7].x = 24; target[7].y = 21;
	target[8].x = 37; target[8].y = 23;
	intro();
	while (1) {
		if (be_input())
		{
			if (get_input(&key, &pos) != 0)
			{
				MOUSE_EVENT;
				x = pos.X;
				y = pos.Y;
				if ((x >= 35 && x <= 45) && (y >= 19 && y <= 21)) {
					clearconsole();
					draw_box2(0, 0, 80, 30, "■");
					map(target);
					gotoxy(10, 17);
					printf("여기에 두더지가 나타납니다.");
					gotoxy(10, 19);
					printf("아무키나 누르면 게임이 시작됩니다.");
					getch();
					gotoxy(10, 17);
					printf("                           ");
					gotoxy(10, 19);
					printf("                                  ");

					for (int i = 0; i < 10; i++) {
						gotoxy(target[i].x, target[i].y + 1);
						printf("─");
					}
					for (int i = 0; i < 9; i++) {
						target[i].remain = 0;
					}
					gotoxy(62, 11);
					printf("점수 : %2d", score);
					gotoxy(62, 5);
					printf("stage : %d", stage);
					start_time = time(NULL);
					while (1)
					{
						run_time = time(NULL) - start_time;
						remain_time = time_out - run_time;
						show_time(remain_time);
						if (remain_time < last_remain_time) {
							show_time(remain_time);
							last_remain_time = remain_time;
						}
						//remain_time = 0;
						if (remain_time == 0) {
							break;
						}
						if (frame_count % appear_sync == 0) {
							int q = rand() % 9;
							while (target[q].remain != 0) {
								q = rand() % 9;
							}
							target[q].remain = 10;
							dcolor = rand() % 10;
							//stage=6
							switch (stage) {
							case 1:
								remain_sync = 10;
								appear_sync = 70;
								if (dcolor >= 2) {
									dcolor = 2;
								}
								break;
							case 2:
								remain_sync = 8;
								appear_sync = 60;
								if (dcolor >= 3) {
									dcolor = 2;
								}
								else if (dcolor >= 1) {
									dcolor = 1;
								}
								break;
							case 3:
								remain_sync = 5;
								appear_sync = 50;
								if (dcolor >= 4) {
									dcolor = 2;
								}
								else if (dcolor >= 1) {
									dcolor = 1;
								}
								break;
							case 4:
								remain_sync = 3;
								appear_sync = 40;
								if (dcolor >= 5) {
									dcolor = 2;
								}
								else if (dcolor >= 1) {
									dcolor = 1;
								}
								break;
							case 5:
								remain_sync = 3;
								appear_sync = 30;
								if (dcolor >= 5) {
									dcolor = 2;
								}
								else if (dcolor >= 1) {
									dcolor = 1;
								}
								break;
							case 6:
								remain_sync = 3;
								appear_sync = 50;
								if (dcolor >= 5) {
									dcolor = 2;
								}
								else if (dcolor >= 1) {
									dcolor = 1;
								}
								gotoxy(11, 2);
								printf("                                 ");
								gotoxy(11, 2);
								printf("마지막 스테이지야.");
								gotoxy(11, 3);
								printf("60초동안 최대한 많은 두더지를 잡아줘!");
								break;
							default:
								break;
							}

							target[q].color = ddcolor[dcolor];
							switch (target[q].color) {
							case WHITE:
								target[q].score = 1;
								break;
							case RED2:
								target[q].score = -2;
								break;
							default:
								target[q].score = 3;
								break;
							}
							textcolor(target[q].color, BLACK);
							gotoxy(target[q].x - 2, target[q].y - 1); printf(" ■■ ");
							gotoxy(target[q].x - 2, target[q].y); printf("■OO■");
							gotoxy(target[q].x - 2, target[q].y + 1); printf("■■■");
						}
						if (frame_count % remain_sync == 0) {
							for (int i = 0; i < 10; i++) {
								if (target[i].remain) {
									target[i].remain--;
								}
							}
						}
						for (int i = 0; i < 9; i++) {
							if (target[i].remain == 0) {
								gotoxy(target[i].x - 2, target[i].y - 1); printf("      ");
								gotoxy(target[i].x - 2, target[i].y); printf("      ");
								gotoxy(target[i].x - 2, target[i].y + 1); printf("      ");
							}
						}

						textcolor(WHITE, BLACK);
						if (st1 == 1 && score >= 10) {
							stage = 2;
							st2 = stage;
							gotoxy(62, 5);
							printf("stage : %d", stage);
						}
						else if (st1 == 2 && score >= 20) {
							stage = 3;
							st2 = stage;
							gotoxy(62, 5);
							printf("stage : %d", stage);
						}
						else if (st1 == 3 && score >= 30) {
							stage = 4;
							st2 = stage;
							gotoxy(62, 5);
							printf("stage : %d", stage);
						}
						else if (st1 == 4 && score >= 40) {
							stage = 5;
							st2 = stage;
							gotoxy(62, 5);
							printf("stage : %d", stage);
						}
						else if (st1 == 5 && score >= 50) {
							stage = 6;
							st2 = stage;
							gotoxy(62, 5);
							printf("stage : %d", stage);
						}

						if (st1 != st2) {
							start_time = time(NULL);
							st1 = st2;
						}

						if (be_input())
						{
							if (get_input(&key, &pos) != 0)
							{
								MOUSE_EVENT;
								x = pos.X;
								y = pos.Y;
								for (int j = 0; j < 10; j++) {
									if ((x < target[j].x + 2 && x >= target[j].x) && y == target[j].y) {
										get_point(target[j]);
										target[j].remain = 0;
									}
								}
							}
						}

						frame_count++;
						Sleep(10);
						if (num == 9) num = 0;
					}
					if (stage == 6) {
						complete = 1;
					}
					break;
				}
				if ((x >= 35 && x <= 45) && (y >= 23 && y <= 25)) {
					help();
					getch();
					clearconsole();
					intro();
				}
			}
		}
	}
	//complete = 1;
	if (complete == 1) {
		end_win();
	}
	else {
		end_lose();
	}
	textcolor(WHITE, BLACK);
	gotoxy(2, 29);
}