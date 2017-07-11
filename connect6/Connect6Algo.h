// Samsung Go Tournament Form C Connect6Algo Header (g++-4.8.3)

// <--------------- 이 Code를 수정하면  작동하지 않을 수 있습니다 ------------------>

#pragma once

/****junsu****/
#include <iostream>
#include <list>
#include <vector>

#define DISABLE -1
#define CONNECTABLE 0
#define ALREADY_HAVE 1

#define EMPTY 0
#define MY 1
#define OP 2
#define BLOCK 3

#define V 0
#define H 1
#define LD 2
#define RD 3

#define B_SIZE 19

#define CONN_SUCCESS 6
#define CONNECTABLE_LENGTH 13
#define ONE_SIDE_EFFECT 6
#define ONE_SIDE_CONN 5
#define CONN_CENTER 6
#define AVAILABLE_CONNECT 7 // conn 1, 2, 3, 4, 5, 6,  0 - not needed but jst 

using namespace std;

typedef int Direction;

typedef struct ConnectionInfo {
	Direction dir;
	// const that define line
	int distinctConst;

	pair<int, int> connStart;
	pair<int, int> connEnd;

	int connAble[CONNECTABLE_LENGTH] = { CONNECTABLE };
	int connLevel;

	int startIndex;
	int endIndex;

}ConnectionInfo;


extern int width, height;
extern int terminateAI;
extern unsigned s_time;

extern int isFree(int x, int y);
extern int showBoard(int x, int y);
extern void init();
extern void restart();
extern void mymove(int x[], int y[], int cnt = 2);
extern void opmove(int x[], int y[], int cnt = 2);
extern void block(int, int);
extern int setLine(char *fmt, ...);
extern void domymove(int x[], int y[], int cnt = 2);

pair<int,int> conn4Finish();
pair<int, int> conn5Finish();
pair<int, int> getMyMove();

pair<int, int> defensiveSlideWindow(int, int, int);
pair<int, int> offenseMove();
pair<int, int> getOpmove1();
pair<int, int> getOpmove2();

void write(char*);
void close();
void doMyOneMove(int x, int y);
void myturn(int cnt = 2);
char info[];