#pragma once

/****junsu****/
#include <iostream>
#include <list>
#include <vector>
#include <map>

#define DISABLE -1
#define CONNECTABLE 0
#define ALREADY_HAVE 1

#define EMPTY 0
#define MY 1
#define OP 2
#define BLOCK 3

#define B_SIZE 19

#define VALID_CONN 6
#define CONN_ARR_LEN 13
#define HALF_EFFECT_RANGE 6
#define HALF_CONN_RANGE 5
#define CONN_CENTER 6	// TODO: remove it and calc -> CONN_ARR_LEN / 2
#define AVAILABLE_CONNECT 7 // TODO: remove it and calc -> VALID_CONN + 1

#define WIDTH 19
#define HEIGHT 19

using namespace std;

enum Direction
{
	V, H, RD, LD
};

typedef struct ConnectionInfo {

	Direction dir;

	// const that define line
	int distinctConst;

	pair<int, int> connStart;
	pair<int, int> connEnd;

	int connectablility[CONN_ARR_LEN] = { CONNECTABLE };
	int connLevel;

	// 자신의 돌이 connectability 에서 시작하는 index
	int startIndex;
	// 자신의 돌이 connectability 에서 끝나는 index
	int endIndex;

}ConnectionInfo;

void initCustomValue();
void removeConnInfo(ConnectionInfo* connInfo);
bool calcConnAvailable(ConnectionInfo* connInfo);
bool updateConnInfoByMyStone(ConnectionInfo* connInfo, int indexOfPosition);
vector<ConnectionInfo*> updateConnInfoByOpStone(ConnectionInfo* connInfo, int indexOfPosition);
void doMyOneMove(int x, int y);
ConnectionInfo* makeConn1(Direction dir, int x, int y);
ConnectionInfo* copyConnInfo(ConnectionInfo* conn);
void registerNewConn1(int x, int y);
void doOpOneMove(int x, int y);
void renewDensityBoard();
void initDensityBoard();
void threatSpaceSearch(int** board, int x, int y);