/*
Bubble Bobble
player1은 AWSD 이동, QE 공격
player2는 JIKL 이동, UO 공격
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdlib.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib");

#define PLAYER1 "Ａ"
#define PLAYER2 "Ｂ"
#define ITEM1 "♥"
#define ITEM2 "♡"

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

#define ESC 0x1b

#define UP 'i'
#define DOWN 'k'
#define LEFT 'j'
#define RIGHT 'l'
#define BUBBLE_LEFT 'u'
#define BUBBLE_RIGHT 'o'

#define UP2 'w'
#define DOWN2 's'
#define LEFT2 'a'
#define RIGHT2 'd'
#define BUBBLE_LEFT2 'q'
#define BUBBLE_RIGHT2 'e'

#define BLANK ' '

#define WIDTH 80
#define HEIGHT 24

typedef struct {
	int x;
	int y;
}Point;

typedef struct {
	int oldx, oldy, newx, newy;
}Player;

typedef struct {
	int x, y, call;
}Bubble;

Player player_1 = { 0 };
Player player_2 = { 0 };

Bubble bubble1_l;  // player1의 왼쪽 버블
Bubble bubble1_r;  // player1의 오른쪽 버블
Bubble bubble2_l;  // player2의 왼쪽 버블
Bubble bubble2_r;  // player2의 오른쪽 버블

int n;
int time_out = 60;  // 제한시간
int score[2] = { 0 };  // 점수
int life[2] = { 3,3 };  // 생명
int called[2] = { 0,0 };
int keep_moving = 0;
int Delay = 1;
int items[HEIGHT][WIDTH] = { 0 };
int frame_count = 0;
int ob1_frame_sync = 50;
int ob2_frame_sync = 70;
int ob3_frame_sync = 60;
int item1_frame_sync = 500;
int item2_frame_sync = 100;
int bubble1_call = 0;
int bubble2_call = 0;
int bubble_frame_sync = 3;
int player1_bubble = 0; // 1 왼쪽, 2 오른쪽
int player2_bubble = 0; // 1 왼쪽, 2 오른쪽


// 커서를 안 보이게 한다.
void removeCursor(void) 
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

// (x, y)로 이동
void gotoxy(int x, int y) 
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void putstar(int x, int y, char ch)
{
	gotoxy(x, y);
	putchar(ch);
}

void erasestar(int x, int y)
{
	gotoxy(x, y);
	putchar(BLANK);
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);

}

void draw_box1(int x1, int y1, int x2, int y2, char* ch)
{
	int x, y;
	int len = strlen(ch);
	for (x = x1; x <= x2; x += len) {
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

void draw_box2(int x1, int y1, int x2, int y2)
{
	int x, y;
	for (x = x1; x <= x2; x += 2) { // 한글은 2칸씩 차지한다.
		gotoxy(x, y1);
		printf("%s", "─"); // ㅂ 누르고 한자키 누르고 선택
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

void draw_fill_box(int x1, int y1, int x2, int y2, char* ch)
{
	for (int y = y1; y <= y2; y++)
	{
		for (int x = x1; x <= x2; x += 2)
		{
			gotoxy(x, y);
			printf("%s", ch);
		}
		printf("\n");
	}
}

void show_rule()
{
	textcolor(WHITE, BLACK);
	draw_box2(0, 0, WIDTH, HEIGHT);
	gotoxy(35, 9);
	printf("★   -1 life");
	gotoxy(32, 11);
	textcolor(MAGENTA2, BLACK);
	printf("♥ ");
	textcolor(WHITE, BLACK);
	printf("+500");
	gotoxy(40, 11);
	textcolor(YELLOW2, BLACK);
	printf("♡ ");
	textcolor(WHITE, BLACK);
	printf("+100 point");
	gotoxy(25, 13);
	printf("hit ★ using bubble, get +250 point");
	gotoxy(35, 15);
	printf("Enter to start");
}

// 선택된 플레이어 수를 character로 반환
char start_game() 
{
	char ch;
	cls(BLACK, GREEN2);
	while (1)
	{
		char color[] = { BLUE2, GREEN2, MAGENTA2, CYAN2 };
		textcolor(color[n % 4], BLACK);
		draw_box1(0, 0, WIDTH, HEIGHT, "■");
		textcolor(BLACK, color[n % 4]);
		n++;
		gotoxy(30, 7);
		printf(" B U B B L E ");
		gotoxy(40, 8);
		printf(" B O B B L E ");
		gotoxy(25, 14);
		printf(" Hit (1) to 1 player (2) to 2player ");
		Sleep(300);
		if (_kbhit())
		{
			ch = _getch();
			return ch;
		}
	}
}

// 첫번째 맵
int map1[24][40] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

void show_map1()
{
	int c;
	for (int y = 0; y < 24; y++)
	{
		c = rand() % 6 + 9;
		for (int x = 0; x < 40; x++)
		{	
			textcolor(c, BLACK);
			gotoxy(x * 2, y);
			if (map1[y][x] == 0)
				printf("  ");
			else
				printf("■");
		}
	}

}

void show_score(int player)
{
	textcolor(WHITE, BLACK);
	switch (player) {
	case 0: // player 1
		gotoxy(10, 0);
		printf("Player1 : %d", score[player]);
		break;
	case 1: // player 2
		gotoxy(60, 0);
		printf("Player2 : %d", score[player]);
		break;
	}
	textcolor(BLACK, WHITE);
}

void show_life(int player)
{
	textcolor(WHITE, BLACK);
	switch (player) {
	case 0:
		gotoxy(12, 1);
		printf("      ");
		gotoxy(12, 1);
		for (int i = 0; i < life[0]; i++)
			printf("●");
		break;
	case 1:
		gotoxy(62, 1);
		printf("      ");
		gotoxy(62, 1);
		for (int i = 0; i < life[1]; i++)
			printf("●");
		break;
	}
}

void show_time(int remain_time)
{
	textcolor(WHITE, BLACK);
	gotoxy(32, 0);
	printf("남은 시간 : %02d", remain_time);
}

void show_left_bubble(int x, int y, int player)
{
	bubble1_call = 0;
	bubble2_call = 0;

	int temp;
	if (x % 2 == 0)
		temp = x - 2;
	else
		temp = x - 3; 

	textcolor(WHITE, BLACK);
	// player가 누군지에 따라 그 플레이어의 버블을 초기화 해준다.
	switch (player) {
	case 0:
		bubble1_l.x = temp;
		bubble1_l.y = y;		
		bubble1_l.call = 1;
		player1_bubble = 1;
		break;
	case 1:
		bubble2_l.x = temp;
		bubble2_l.y = y;
		bubble2_l.call = 1;
		player2_bubble = 1;
		break;
	}
}

void move_left_bubble(int player)
{
	textcolor(WHITE, BLACK);
	removeCursor();
	switch (player){
	case 0:  // player1
		if (bubble1_call == 0)  // 버블이 처음 발생했을 떈 찍기만
		{
			gotoxy(bubble1_l.x, bubble1_l.y);
			printf("○");
			bubble1_call++;
			bubble1_l.y--;
		}
		else
		{
			if (map1[bubble1_l.y][bubble1_l.x / 2])
			{
				gotoxy(bubble1_l.x, bubble1_l.y + 1);
				printf("  ");
				bubble1_l.y--;
				gotoxy(bubble1_l.x, bubble1_l.y);
				printf("○");
				bubble1_l.y--;
			}
			else
			{
				gotoxy(bubble1_l.x, bubble1_l.y + 1);
				printf("  ");
				gotoxy(bubble1_l.x, bubble1_l.y);
				printf("○");
				bubble1_l.y--;
				if (bubble1_l.y == 5)
				{
					gotoxy(bubble1_l.x, bubble1_l.y + 1);
					Sleep(100);
					printf("  ");
					player1_bubble = 0;
				}
			}
		}
		if (items[bubble1_l.y + 1][bubble1_l.x] == 4)
		{
			score[0] += 250;
			show_score(0);
		}
		break;
	case 1:  // player2
		if (bubble2_call == 0)
		{
			gotoxy(bubble2_l.x, bubble2_l.y);
			printf("○");
			bubble2_call++;
			bubble2_l.y--;
		}
		else
		{
			if (map1[bubble2_l.y][bubble2_l.x / 2])
			{
				gotoxy(bubble2_l.x, bubble2_l.y + 1);
				printf("  ");
				bubble2_l.y--;
				gotoxy(bubble2_l.x, bubble2_l.y);
				printf("○");
				bubble2_l.y--;
			}
			else
			{
				gotoxy(bubble2_l.x, bubble2_l.y + 1);
				printf("  ");
				gotoxy(bubble2_l.x, bubble2_l.y);
				printf("○");
				bubble2_l.y--;
				if (bubble2_l.y == 5)
				{
					gotoxy(bubble2_l.x, bubble2_l.y + 1);
					Sleep(100);
					printf("  ");
					player2_bubble = 0;
				}
			}
		}
		if (items[bubble2_l.y + 1][bubble2_l.x] == 4)
		{
			score[1] += 250;
			show_score(1);
		}
		break;
	}
}

void show_right_bubble(int x, int y,int player)
{
	bubble1_call = 0;
	bubble2_call = 0;

	int temp;
	if (x % 2 == 0)
		temp = x + 2;
	else
		temp = x + 3;
	
	textcolor(WHITE, BLACK);
	switch (player) {
	case 0:
		bubble1_r.x = temp;
		bubble1_r.y = y;
		bubble1_r.call = 1;
		player1_bubble = 2;
		break;
	case 1:
		bubble2_r.x = temp;
		bubble2_r.y = y;
		bubble2_r.call = 1;
		player2_bubble = 2;
		break;
	}
}

void move_right_bubble(int player)
{
	textcolor(WHITE, BLACK);
	removeCursor();
	switch (player) {
	case 0:  // player1
		if (bubble1_call == 0)
		{
			gotoxy(bubble1_r.x, bubble1_r.y);
			printf("○");
			bubble1_call++;
			bubble1_r.y--;
		}
		else
		{
			if (map1[bubble1_r.y][bubble1_r.x / 2])
			{
				gotoxy(bubble1_r.x, bubble1_r.y + 1);
				printf("  ");
				bubble1_r.y--;
				gotoxy(bubble1_r.x, bubble1_r.y);
				printf("○");
				bubble1_r.y--;
			}
			else
			{
				gotoxy(bubble1_r.x, bubble1_r.y + 1);
				printf("  ");
				gotoxy(bubble1_r.x, bubble1_r.y);
				printf("○");
				bubble1_r.y--;
				if (bubble1_r.y == 5)
				{
					gotoxy(bubble1_r.x, bubble1_r.y + 1);
					Sleep(100);
					printf("  ");
					player1_bubble = 0;
				}
			}
		}
		if (items[bubble1_r.y + 1][bubble1_r.x] == 4)
		{
			score[0] += 250;
			show_score(0);
		}
		break;
	case 1:  // player2
		if (bubble2_call == 0)
		{
			gotoxy(bubble2_r.x, bubble2_r.y);
			printf("○");
			bubble2_call++;
			bubble2_r.y--;
		}
		else
		{
			if (map1[bubble2_r.y][bubble2_r.x / 2])
			{
				gotoxy(bubble2_r.x, bubble2_r.y + 1);
				printf("  ");
				bubble2_r.y--;
				gotoxy(bubble2_r.x, bubble2_r.y);
				printf("○");
				bubble2_r.y--;
			}
			else
			{
				gotoxy(bubble2_r.x, bubble2_r.y + 1);
				printf("  ");
				gotoxy(bubble2_r.x, bubble2_r.y);
				printf("○");
				bubble2_r.y--;
				if (bubble2_r.y == 5)
				{
					gotoxy(bubble2_r.x, bubble2_r.y + 1);
					Sleep(100);
					printf("  ");
					player2_bubble = 0;
				}
			}
		}
		if (items[bubble2_r.y + 1][bubble2_r.x] == 4)
		{
			score[1] += 250;
			show_score(1);
		}
		break;
	}
}

void player1(unsigned char ch)
{
	int move_flag = 0;
	static char last_ch = 0;

	if (called[0] == 0)  // 처음 또는 Restart
	{
		player_1.oldx = 2;
		player_1.oldy = 22;
		player_1.newx = 2; 
		player_1.newy = 22;
		textcolor(GREEN2, BLACK);
		gotoxy(player_1.oldx, player_1.oldy);
		printf(PLAYER1);
		called[0] = 1;
		last_ch = 0;
		ch = 0;
	}
	if (keep_moving && ch == 0)
		ch = last_ch;
	last_ch = ch;

	switch (ch) {
	case UP2:
		if (player_1.oldy > 1)
		{
			if (map1[player_1.oldy - 3][player_1.oldx / 2] == 1)
				player_1.newy = player_1.oldy - 4;
		}
		move_flag = 1;
		break;
	case DOWN2:
		if (player_1.oldy < HEIGHT - 1)
		{
			if (map1[player_1.oldy + 5][player_1.oldx / 2] == 1)
				player_1.newy = player_1.oldy + 4;
		}
		move_flag = 1;
		break;
	case LEFT2:
		if ((player_1.oldx > 0) && (map1[player_1.oldy + 1][player_1.oldx / 2 - 1] == 1))
			player_1.newx = player_1.oldx - 2;
		move_flag = 1;
		break;
	case RIGHT2:
		if ((player_1.oldx < WIDTH - 1) && (map1[player_1.oldy + 1][player_1.oldx/2 + 1] == 1))
			player_1.newx = player_1.oldx + 2;
		move_flag = 1;
		break;
	}
	if (move_flag)
	{
		textcolor(GREEN2, BLACK);
		gotoxy(player_1.oldx, player_1.oldy);
		printf("  ");
		gotoxy(player_1.newx, player_1.newy);
		printf(PLAYER1);
		player_1.oldx = player_1.newx;
		player_1.oldy = player_1.newy;
		if (items[player_1.newy][player_1.newx] == 4)
		{
			life[0]--;
			show_life(0);
		}
		if (items[player_1.newy][player_1.newx] == 500)
		{
			score[0] += 500;
			show_score(0);
		}
		if (items[player_1.newy][player_1.newx] == 100)
		{
			score[0] += 100;
			show_score(0);
		}
	}
	
	
}

void player2(unsigned char ch)
{
	int move_flag = 0;
	static char last_ch;

	if (called[1] == 0)
	{
		player_2.oldx = 78;
		player_2.oldy = 22;
		player_2.newx = 78;
		player_2.newy = 22;
		textcolor(BLUE2, BLACK);
		gotoxy(player_2.oldx, player_2.oldy);
		printf(PLAYER2);
		called[1] = 1;
		last_ch = 0;
		ch = 0;
	}
	if (keep_moving && ch == 0)
		ch = last_ch;
	last_ch = ch;
	switch (ch) {
	case UP:
		if (player_2.oldy > 1)
		{
			if (map1[player_2.oldy - 3][player_2.oldx / 2] == 1)
				player_2.newy = player_2.oldy - 4;
		}
		move_flag = 1;
		break;
	case DOWN:
		if (player_2.oldy < HEIGHT - 1)
		{
			if (map1[player_2.oldy + 5][player_2.oldx / 2] == 1)
				player_2.newy = player_2.oldy + 4;
		}
		move_flag = 1;
		break;
	case LEFT:
		if ((player_2.oldx > 0) && (map1[player_2.oldy + 1][player_2.oldx / 2 - 1] == 1))
			player_2.newx = player_2.oldx - 2;
		move_flag = 1;
		break;
	case RIGHT:
		if ((player_2.oldx < WIDTH - 1) && (map1[player_2.oldy + 1][player_2.oldx / 2 + 1] == 1))
			player_2.newx = player_2.oldx + 2;
		move_flag = 1;
		break;
	}
	if (move_flag)
	{
		textcolor(BLUE2, BLACK);
		gotoxy(player_2.oldx, player_2.oldy);
		printf("  ");
		gotoxy(player_2.newx, player_2.newy);
		printf(PLAYER2);
		player_2.oldx = player_2.newx;
		player_2.oldy = player_2.newy;

		if (items[player_2.newy][player_2.newx] == 4)
		{
			life[1]--;
			show_life(1);
		}
		if (items[player_2.newy][player_2.newx] == 500)
		{
			score[1] += 500;
			show_score(1);
		}
		if (items[player_2.newy][player_2.newx] == 100)
		{
			score[1] += 100;
			show_score(1);
		}
	}
}

void move_ob1()
{
	int arr1[] = { 0,2,4,6,8,10,12,10,8,6,4,2 };  // old
	int arr2[] = { 2,4,6,8,10,12,10,8,6,4,2,0 };  // new
	static int oldx, newx, y;
	static int count = 0;
	static int ob1_call = 0;
	
	oldx = arr1[count % 12];
	newx = arr2[count % 12];
	y = 18;
	if (ob1_call == 0)
	{
		textcolor(WHITE, BLACK);
		gotoxy(newx, y);
		printf("★");
		items[y][newx] = 4;
		ob1_call++;
	}
	else
	{
		textcolor(BLACK, BLACK);
		gotoxy(oldx, y);
		printf("  ");
		items[y][oldx] = 0;

		textcolor(WHITE, BLACK);
		gotoxy(newx, y);
		printf("★");		
		items[y][newx] = 4;
	}
	count++;
}

void move_ob2()
{
	int arr1[] = { 36,38,40,42,40,38 };  // old
	int arr2[] = { 38,40,42,40,38,36 };  // new
	static int oldx, newx, y;
	static int count = 0;
	static int ob2_call = 0;

	oldx = arr1[count % 6];
	newx = arr2[count % 6];
	y = 14;
	if (ob2_call == 0)
	{
		textcolor(WHITE, BLACK);
		gotoxy(newx, y);
		printf("★");
		items[y][newx] = 4;
		ob2_call++;
	}
	else
	{
		textcolor(BLACK, BLACK);
		gotoxy(oldx, y);
		printf("  ");
		items[y][oldx] = 0;

		textcolor(WHITE, BLACK);
		gotoxy(newx, y);
		printf("★");		
		items[y][newx] = 4;
	}
	count++;
}

void move_ob3()
{
	int arr1[] = { 62,64,66,68,70,72,70,68,66,64 };
	int arr2[] = { 64,66,68,70,72,70,68,66,64,62 };
	static int oldx, newx, y;
	static int count = 0;
	static int ob3_call = 0;

	oldx = arr1[count % 10];
	newx = arr2[count % 10];
	y = 10;
	if (ob3_call == 0)
	{
		textcolor(WHITE, BLACK);
		gotoxy(newx, y);
		printf("★");
		items[y][newx] = 4;
		ob3_call++;
	}
	else
	{
		textcolor(WHITE, BLACK);
		gotoxy(oldx, y);
		printf("  ");
		items[y][oldx] = 0;

		textcolor(WHITE, BLACK);
		gotoxy(newx, y);
		printf("★");
		items[y][newx] = 4;
	}
	count++;
}

void item1()
{
	Point item1[] = { {6,22},{10,22},{18,22},{26,22},{30,22},{36,22},{38,22},
						   {0,18},{8,18},{36,18},{40,18},{62,18},{66,18},{70,18},{78,18,},
						   {2,14},{18,14},{20,14},{52,14},{72,14},
						   {6,10},{10,10},{34,10}, {62,10} };
	static int oldx1, newx1, oldx2, newx2, oldy1, newy1, oldy2, newy2;
	static int item1_call = 0;
	int r1, r2, r3;
	srand(time(NULL));
	do {
		r1 = rand() % 24;
		r2 = rand() % 24;
	} while ((r1 == r2));

	newx1 = item1[r1].x;
	newy1 = item1[r1].y;
	newx2 = item1[r2].x;
	newy2 = item1[r2].y;

	if (item1_call == 0)
	{
		textcolor(MAGENTA2, BLACK);

		gotoxy(newx1, newy1);
		printf(ITEM1);		
		items[newy1][newx1] = 500;
		items[newy1][newx1 + 1] = 500;

		gotoxy(newx2, newy2);
		printf(ITEM1);
		items[newy2][newx2] = 500;
		items[newy2][newx2 + 1] = 500;

		item1_call++;
	}
	else
	{		
		textcolor(MAGENTA2, BLACK);

		gotoxy(oldx1, oldy1);  // old 지우고
		printf("  ");
		items[oldy1][oldx1] = 0;
		items[oldy1][oldx1 + 1] = 0;

		gotoxy(newx1, newy1);  // new 찍기
		printf(ITEM1);		
		items[newy1][newx1] = 500;
		items[newy1][newx1 + 1] = 500;

		gotoxy(oldx2, oldy2);
		printf("  ");
		items[oldy2][oldx2] = 0;
		items[oldy2][oldx2 + 1] = 0;

		gotoxy(newx2, newy2);
		printf(ITEM1);
		items[newy2][newx2] = 500;
		items[newy2][newx2 + 1] = 500;
	}
	oldx1 = newx1;
	oldy1 = newy1;
	oldx2 = newx2;
	oldy2 = newy2;
}

void item2()
{
	Point item2[] = { {4,22},{8,22},{14,22},{20,22},{24,22},{32,22},{44,22},{48,22},{56,22},
							{4,18},{34,18},{38,18},{60,18},{64,18},{76,18},
							{0,14},{4,14},{22,14},{38,14},{76,14},
							{8,10},{12,10},{36,10},{64,10} };
	static int oldx1, newx1, oldx2, newx2, oldx3, newx3, oldy1, newy1, oldy2, newy2, oldy3, newy3;
	static int item2_call = 0;
	int r1, r2, r3;
	srand(time(NULL));
	do {
		r1 = rand() % 24;
		r2 = rand() % 24;
		r3 = rand() % 24;
	} while ((r1 == r2) || (r2 == r3) || (r3 == r1));
	newx1 = item2[r1].x;
	newy1 = item2[r1].y;
	newx2 = item2[r2].x;
	newy2 = item2[r2].y;
	newx3 = item2[r3].x;
	newy3 = item2[r3].y;
	if (item2_call == 0)  // 처음 호출되면 new를 찍기만 한다
	{
		textcolor(YELLOW2, BLACK);

		gotoxy(newx1, newy1);
		printf(ITEM2);
		items[newy1][newx1] = 100;
		items[newy1][newx1 + 1] = 100;

		gotoxy(newx2, newy2);
		printf(ITEM2);
		items[newy2][newx2] = 100;
		items[newy2][newx2 + 1] = 100;

		gotoxy(newx3, newy3);
		printf(ITEM2);
		items[newy3][newx3] = 100;
		items[newy3][newx3 + 1] = 100;
		item2_call++;
	}
	else
	{
		textcolor(YELLOW2, BLACK);

		gotoxy(oldx1, oldy1);
		printf("  ");
		items[oldy1][oldx1] = 0;
		items[oldy1][oldx1 + 1] = 0;

		gotoxy(newx1, newy1);
		printf(ITEM2);
		items[newy1][newx1] = 100;
		items[newy1][newx1 + 1] = 100;


		gotoxy(oldx2, oldy2);
		printf("  ");
		items[oldy2][oldx2] = 0;
		items[oldy2][oldx2 + 1] = 0;

		gotoxy(newx2, newy2);
		printf(ITEM2);
		items[newy2][newx2] = 100;
		items[newy2][newx2 + 1] = 100;


		gotoxy(oldx3, oldy3);
		printf("  ");
		items[oldy3][oldx3] = 0;
		items[oldy3][oldx3 + 1] = 0;

		gotoxy(newx3, newy3);
		printf(ITEM2);
		items[newy3][newx3] = 100;
		items[newy3][newx3 + 1] = 100;

	}
	oldx1 = newx1;
	oldy1 = newy1;
	oldx2 = newx2;
	oldy2 = newy2;
	oldx3 = newx3;
	oldy3 = newy3;
}

void init_game()
{
	srand(time(NULL));
	char cmd[100];
	score[0] = score[1] = 0;
	life[0] = life[1] = 3;
	called[0] = called[1] = 0;
	for (int i = 0; i < HEIGHT; i++)
		for (int j = 0; j < WIDTH; j++)
			items[i][j] = 0;
	keep_moving = 0;
	bubble1_call = 0;
	bubble2_call = 0;
	player1_bubble = 0;
	player2_bubble = 0;
	cls(WHITE, BLACK);
	removeCursor();
	textcolor(BLACK, WHITE);
	sprintf(cmd, "mode con cols=%d lines=%d", 82, 26);
	system(cmd);
}

int main(void)
{
	PlaySound(TEXT("Bubble Bobble"), NULL, SND_ASYNC | SND_LOOP);
	unsigned char ch;
	int run_time, start_time, remain_time, last_remain_time;
	char player_num;
START:
	init_game();
	player_num = start_game();
	cls(BLACK, WHITE);
	show_rule();
	getchar();
	start_time = time(NULL);
	last_remain_time = remain_time = time_out;
	cls(BLACK, WHITE);
	
	show_map1();
	show_score(0);
	show_score(1);
	show_life(0);
	show_life(1);

	while (1)
	{
		run_time = time(NULL) - start_time;
		remain_time = time_out - run_time;
		if (remain_time < last_remain_time)
		{
			show_time(remain_time);
			last_remain_time = remain_time;
		}
		if (remain_time == 0)
			break;
		switch (player_num){
		case '1':  // 플레이어 한명
			if (_kbhit() == 1)  // 키보드가 눌러져 있으면
			{
				ch = _getch();  // key 값을 읽고
				if (ch == ESC)  // ESC면 종료
				{
					gotoxy(25, 11);
					exit(0);
					break;
				}
				switch (ch) {  // player1은 AWSD로 이동
				case UP2:
					player1(ch);
					break;
				case DOWN2:
					player1(ch);
					break;
				case LEFT2:
					player1(ch);
					break;
				case RIGHT2:
					player1(ch);
					break;
				case BUBBLE_LEFT2:
					show_left_bubble(player_1.oldx, player_1.oldy, 0);
					break;
				case BUBBLE_RIGHT2:
					show_right_bubble(player_1.oldx, player_1.oldy, 0);
					break;
				default:
					player1(0);
				}
			}
			else
			{
				player1(0);
			}
			if (frame_count % ob1_frame_sync == 0)
			{
				move_ob1();
			}
			if (frame_count % ob2_frame_sync == 0)
			{
				move_ob2();
			}
			if (frame_count % ob3_frame_sync == 0)
			{
				move_ob3();
			}
			if (frame_count % item1_frame_sync == 0)
			{
				item1();
			}
			if (frame_count % item2_frame_sync == 0)
			{
				item2();
			}
			if (frame_count % bubble_frame_sync == 0)
			{
				if (bubble1_l.call || bubble1_r.call)
				{
					if (player1_bubble == 1)
						move_left_bubble(0);
					else if (player1_bubble == 2)
						move_right_bubble(0);
				}
			}
			if (life[0] == 0)
				goto GAMEOVER;
			Sleep(Delay);
			frame_count++;
			break;


		case '2':  // 플레이어 두명
			if (_kbhit() == 1)
			{
				ch = _getch();
				if (ch == ESC)
				{
					gotoxy(25, 11);
					exit(0);
					break;
				}
				switch (ch) {
				case UP:  // player2 
					player1(0);
					player2(ch);
					break;
				case DOWN:
					player1(0);
					player2(ch);
					break;
				case LEFT:
					player1(0);
					player2(ch);
					break;
				case RIGHT:
					player1(0);
					player2(ch);
					break;
				case BUBBLE_LEFT:
					player1(0);
					show_left_bubble(player_2.oldx, player_2.oldy, 1);
					break;
				case BUBBLE_RIGHT:
					player1(0);
					show_right_bubble(player_2.oldx, player_2.oldy, 1);
					break;


				case UP2:  // player1
					player1(ch);
					player2(0);
					break;
				case DOWN2:
					player1(ch);
					player2(0);
					break;
				case LEFT2:
					player1(ch);
					player2(0);
					break;
				case RIGHT2:
					player1(ch);
					player2(0);
					break;
				case BUBBLE_LEFT2:
					show_left_bubble(player_1.oldx, player_1.oldy, 0);
					player2(0);
					break;
				case BUBBLE_RIGHT2:
					show_right_bubble(player_1.oldx, player_1.oldy, 0);
					player2(0);
					break;
				default:
					player1(0);
					player2(0);
				}
				
			}
			else  // 키보드가 눌리지 않은 경우
			{
				player1(0);
				player2(0);
			}
			// 전체적인 싱크 조절
			if (frame_count % ob1_frame_sync == 0)  // 장애물1
			{
				move_ob1();
			}
			if (frame_count % ob2_frame_sync == 0)  // 장애물2
			{
				move_ob2();
			}
			if (frame_count % ob3_frame_sync == 0)  // 장애물3
			{
				move_ob3();
			}
			if (frame_count % item1_frame_sync == 0)  // 아이템1
			{
				item1();
			}
			if (frame_count % item2_frame_sync == 0)  // 아이템2
			{
				item2();
			}
			if (frame_count % bubble_frame_sync == 0)  // 버블
			{
				if (bubble1_l.call || bubble1_r.call)  // player1의 버블
				{
					if (player1_bubble == 1)
						move_left_bubble(0);
					else if (player1_bubble == 2)
						move_right_bubble(0);
				}
				if (bubble2_l.call || bubble2_r.call)  // player2의 버블
				{
					if (player2_bubble == 1)
						move_left_bubble(1);
					else if (player2_bubble == 2)
						move_right_bubble(1);
				}
			}
			if (life[0] == 0 || life[1] == 0)
				goto GAMEOVER;
			Sleep(Delay);
			frame_count++;
			break;
		}
	}
	textcolor(WHITE, BLACK);
	draw_box2(18, 7, 60, 20);
	textcolor(BLACK, BLACK);
	draw_fill_box(20, 8, 58, 19, "■");
	textcolor(BLACK, WHITE);
	gotoxy(30, 7);
	if (score[0] > score[1])
		printf(" WINNER IS PLAYER1!! ");
	else if (score[0] < score[1])
		printf(" WINNER IS PLAYER2!! ");
	else
		printf("       DRAW!!        ");
GAMEOVER:
	while (1)
	{
		int c1, c2;
		do {
			c1 = rand() % 16;
			c2 = rand() % 16;
		} while (c1 == c2);
		textcolor(c1, c2);
		gotoxy(32, 10);
		printf("G A M E  O V E R");
		gotoxy(24, 13);
		textcolor(WHITE, BLACK);
		printf("Hit (R) to Restart (Q) to Quit");
		Sleep(300);
		if (_kbhit())
		{
			ch = _getch();
			if (ch == 'r' || ch == 'R')
				goto START;
			if (ch == 'q' || ch == 'Q')
				goto END;
		}
	}
END:
	gotoxy(0, HEIGHT - 1);
	return 0;
}