// Samsung Go Tournament Form C Connect6Algo (g++-4.8.3)

// <--------------- 이 Code를 수정하면  작동하지 않을 수 있습니다 ------------------>

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <map>
#include <functional>
#include "Connect6Algo.h"

// 각 connection에 따른 weight
#define connection1 2
#define connection2 5
#define connection3 10

#define evalNum  361 // evaluation할 free point 갯수

vector<ConnectionInfo*> myInfoMap[AVAILABLE_CONNECT];
int turnBoard[B_SIZE][B_SIZE];
int testTurnBoard[B_SIZE][B_SIZE];
int densityBoard[B_SIZE][B_SIZE];

/*********************************************/

/*****************junsu**********************/

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

ofstream output("loginfo.txt");
/*************/

pair<int, int> opmove1;
pair<int, int> opmove2;

pair<int, int> getOpmove1() {
	return opmove1;
}
pair<int, int> getOpmove2() {
	return opmove2;
}

unsigned s_time;
int terminateAI;
int width = 19, height = 19;
int cnt = 2;
int myColor;

static char cmd[256];
static HANDLE event1, event2;
#define BOARD_SIZE 20
int board[BOARD_SIZE][BOARD_SIZE];

static void getLine() {
	int c, bytes;

	bytes = 0;
	do {
		c = getchar();
		if (c == EOF) exit(0);
		if (bytes < sizeof(cmd)) cmd[bytes++] = (char)c;
	} while (c != '\n');
	cmd[bytes - 1] = 0;
	if (cmd[bytes - 2] == '\r') cmd[bytes - 2] = 0;
}

int setLine(char *fmt, ...) {
	int i;
	va_list va;
	va_start(va, fmt);
	i = vprintf(fmt, va);
	putchar('\n');
	fflush(stdout);
	va_end(va);
	return i;
}

static const char *getParam(const char *command, const char *input) {
	int n1, n2;
	n1 = (int)strlen(command);
	n2 = (int)strlen(input);
	if (n1 > n2 || _strnicmp(command, input, n1)) return NULL;
	input += strlen(command);
	while (isspace(input[0])) input++;
	return input;
}

static void stop() {
	terminateAI = 1;
	WaitForSingleObject(event2, INFINITE);
}

static void start() {
	s_time = GetTickCount();
	stop();
}

static void turn() {
	terminateAI = 0;
	ResetEvent(event2);
	SetEvent(event1);
}

void domymove(int x[], int y[], int cnt) {
	mymove(x, y, cnt);
	if (cnt == 1)
		setLine("%d,%d", x[0], y[0]);
	else
		setLine("%d,%d %d,%d", x[0], y[0], x[1], y[1]);
}

int showBoard(int x, int y) {
	return board[x][y];
}


static void doCommand() {
	const char *param;

	if ((param = getParam("START", cmd)) != 0) {
		start();
		init();
	}
	else if ((param = getParam("BEGIN", cmd)) != 0) {
		myColor = 1;
		cnt = 1;
		start();
		turn();
	}
	else if ((param = getParam("TURN", cmd)) != 0) {
		int x[2], y[2], r;
		if (((r = sscanf_s(param, "%d,%d %d,%d", &x[0], &y[0], &x[1], &y[1])) != 4 && r != 2)) {
			setLine("ERROR 형식에 맞지 않는 좌표가 입력되었습니다");
			return;
		}
		else {
			for (int i = 0; i < (r / 2); i++) {
				if (x[i] < 0 || x[i] >= width || y[i] < 0 || y[i] >= height) {
					setLine("ERROR 형식에 맞지 않는 좌표가 입력되었습니다");
					return;
				}
			}
			cnt = 2;
			opmove(x, y, r / 2);
			turn();
		}
	}
	else if ((param = getParam("INFO", cmd)) != 0) {
		setLine("%s", info);
	}
	else if ((param = getParam("BLOCK", cmd)) != 0) {
		int x, y;
		if (((sscanf_s(param, "%d,%d", &x, &y)) == 2)) {
			block(x, y);
			setLine("OK");
		}
	}
	else if ((param = getParam("QUIT", cmd)) != 0) {
		exit(0);
	}
}

static DWORD WINAPI threadLoop(LPVOID) {
	while (1) {
		WaitForSingleObject(event1, INFINITE);
		myturn(cnt);
		if (cnt == 1) cnt = 2;
		SetEvent(event2);
	}
}

int main() {
	initCustomValue();
	
	
	DWORD mode;
	if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode))
		puts("직접 실행 불가능한 파일입니다. 육목 알고리즘 대회 툴을 이용해 실행하세요.");

	DWORD tid;
	event1 = CreateEvent(0, FALSE, FALSE, 0);
	CreateThread(0, 0, threadLoop, 0, 0, &tid);
	event2 = CreateEvent(0, TRUE, TRUE, 0);

	while (1) {
		getLine();
		doCommand();
	}
	
	
	return 0;
}

int isFree(int x, int y)
{
	return x >= 0 && y >= 0 && x < width && y < height && board[x][y] == 0;
}

void init() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			board[i][j] = 0;
		}
	}
	setLine("OK");
}

void mymove(int x[], int y[], int cnt) {
	for (int i = 0; i < cnt; i++) {
		if (isFree(x[i], y[i])) {
			board[x[i]][y[i]] = 1;
		}
		else {
			setLine("ERROR 이미 돌이 있는 위치입니다. MY[%d, %d]", x[i], y[i]);
		}
	}
}

void opmove(int x[], int y[], int cnt) {

	opmove1.first = -1;
	opmove2.first = -1;
	for (int i = 0; i < cnt; i++) {
		if (isFree(x[i], y[i])) {
			board[x[i]][y[i]] = 2;
			doOpOneMove(x[i], y[i]);
			if (i == 0) {
				opmove1.first = x[i];
				opmove1.second = y[i];
			}
			if (i == 1) {
				opmove2.first = x[i];
				opmove2.second = y[i];
			}
		}
		else {
			setLine("ERROR 이미 돌이 있는 위치입니다. OP[%d, %d]", x[i], y[i]);
		}
	}
}

void block(int x, int y) {
	if (isFree(x, y)) {
		board[x][y] = 3;
		turnBoard[x][y] = BLOCK;
	}
}


/*****************junsu**********************/

void initCustomValue() {
	for (int i = 0; i < B_SIZE; i++) {
		for (int j = 0; j < B_SIZE; j++) {
			turnBoard[i][j] = EMPTY;
		}
	}
	opmove1.first = -1;
	opmove2.first = -1; 

	// TODO: generate ... by user
}

void removeConnInfo(ConnectionInfo* connInfo) {
	// TOOD: remove from connectionInfoMap
	free(connInfo);
}

bool isOnBoard(int x, int y) {
	return x >= 0 && x <= width && y >= 0 && y <= height;
}

bool calcConnAvailable(ConnectionInfo* connInfo) {
	int startIndex = connInfo->startIndex;
	int endIndex = connInfo->endIndex;

	Direction dir = connInfo->dir;

	int curX;
	int curY;
	if (dir == V) {
		curX = connInfo->connStart.first;
		curY = connInfo->connStart.second + ONE_SIDE_CONN;
	}
	else if (dir == H) {
		curX = connInfo->connStart.first + ONE_SIDE_CONN;
		curY = connInfo->connStart.second;
	}
	else if (dir == LD) {
		curX = connInfo->connStart.first + ONE_SIDE_CONN;
		curY = connInfo->connStart.second - ONE_SIDE_CONN;
	}
	else if (dir == RD) {
		curX = connInfo->connStart.first + ONE_SIDE_CONN;
		curY = connInfo->connStart.second + ONE_SIDE_CONN;
	}

	bool disable = false;
	// for my stone + 1 is added
	for (int i = 1; i <= ONE_SIDE_EFFECT; i++) {
		if (disable == true) {
			connInfo->connAble[CONN_CENTER + i] = DISABLE;
			continue;
		}

		int refX, refY;
		if (dir == V) {
			refX = curX;
			refY = curY + i;
		}
		else if (dir == H) {
			refX = curX + i;
			refY = curY;
		}
		else if (dir == LD) {
			refX = curX + i;
			refY = curY - i;
		}
		else if (dir == RD) {
			refX = curX + i;
			refY = curY + i;
		}

		if (isOnBoard(refX, refY)) {
			int status = turnBoard[refX][refY];

			if (i == ONE_SIDE_EFFECT && status == MY) {
				if (status == MY) {
					connInfo->connAble[CONN_CENTER + i - 1] = DISABLE;
				}
				connInfo->connAble[CONN_CENTER + i] = DISABLE;
				continue;
			}

			if (status == EMPTY) {
				connInfo->connAble[CONN_CENTER + i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connAble[CONN_CENTER + i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connAble[CONN_CENTER + i - 1] = DISABLE;
				connInfo->connAble[CONN_CENTER + i] = DISABLE;
				disable = true;
			}
		}
		else {
			connInfo->connAble[CONN_CENTER + i] = DISABLE;
			disable = true;
		}
	}

	disable = false;
	for (int i = 1; i <= ONE_SIDE_EFFECT; i++) {
		if (disable == true) {
			connInfo->connAble[CONN_CENTER - i] = DISABLE;
			continue;
		}

		int refX, refY;
		if (dir == V) {
			refX = curX;
			refY = curY - i;
		}
		else if (dir == H) {
			refX = curX - i;
			refY = curY;
		}
		else if (dir == LD) {
			refX = curX - i;
			refY = curY + i;
		}
		else if (dir == RD) {
			refX = curX - i;
			refY = curY - i;
		}

		if (isOnBoard(refX, refY)) {
			int status = turnBoard[refX][refY];

			if (i == ONE_SIDE_EFFECT && status == MY) {
				if (status == MY) {
					connInfo->connAble[CONN_CENTER - i + 1] = DISABLE;
				}
				connInfo->connAble[CONN_CENTER - i] = DISABLE;
				continue;
			}


			if (status == EMPTY) {
				connInfo->connAble[CONN_CENTER - i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connAble[CONN_CENTER - i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connAble[CONN_CENTER - i + 1] = DISABLE;
				connInfo->connAble[CONN_CENTER - i] = DISABLE;
				disable = true;
			}
		}
		else {
			connInfo->connAble[CONN_CENTER - i] = DISABLE;
			disable = true;
		}
	}

	int connCount = 0;
	for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
		if (connInfo->connAble[i] == CONNECTABLE) {
			connCount++;
		}
	}
	if (connCount >= CONN_SUCCESS) {
		return true;
	}
	else {
		return false;
	}
}

ConnectionInfo* makeConn1(Direction dir, int x, int y) {
	ConnectionInfo* conn = (ConnectionInfo*)malloc(sizeof(ConnectionInfo));
	conn->dir = dir;
	conn->connAble[CONN_CENTER] = ALREADY_HAVE;
	conn->connLevel = 1;

	conn->startIndex = CONN_CENTER;
	conn->endIndex = CONN_CENTER;

	if (dir == V) {
		// x = C
		conn->connStart = make_pair(x, y - ONE_SIDE_CONN);
		conn->connEnd = make_pair(x, y + ONE_SIDE_CONN);
		conn->distinctConst = x;

	}
	else if (dir == H) {
		// y = C
		conn->connStart = make_pair(x - ONE_SIDE_CONN, y);
		conn->connEnd = make_pair(x + ONE_SIDE_CONN, y);
		conn->distinctConst = y;
	}
	else if (dir == LD) {
		// y + x = C
		conn->connStart = make_pair(x - ONE_SIDE_CONN, y + ONE_SIDE_CONN);
		conn->connEnd = make_pair(x + ONE_SIDE_CONN, y - ONE_SIDE_CONN);
		conn->distinctConst = y + x;

	}
	else if (dir == RD) {
		// y - x = C
		conn->connStart = make_pair(x - ONE_SIDE_CONN, y - ONE_SIDE_CONN);
		conn->connEnd = make_pair(x + ONE_SIDE_CONN, y + ONE_SIDE_CONN);
		conn->distinctConst = y - x;
	}

	bool result = calcConnAvailable(conn);

	if (result) {
		return conn;
	}
	else {
		free(conn);
		return NULL;
	}
}

void registerNewConn1(int x, int y) {
	ConnectionInfo* connV = makeConn1(V, x, y);
	ConnectionInfo* connH = makeConn1(H, x, y);
	ConnectionInfo* connLD = makeConn1(LD, x, y);
	ConnectionInfo* connRD = makeConn1(RD, x, y);

	// setting 이 안되어있으니 일단 conn level 을 수동으로 여기서 주자

	if (connV != NULL) {
		connV->connLevel = 1;
		myInfoMap[1].push_back(connV);
	}
	if (connH != NULL) {
		connH->connLevel = 1;
		myInfoMap[1].push_back(connH);
	}
	if (connLD != NULL) {
		connLD->connLevel = 1;
		myInfoMap[1].push_back(connLD);
	}
	if (connRD != NULL) {
		connRD->connLevel = 1;
		myInfoMap[1].push_back(connRD);
	}
}


void doOpOneMove(int x, int y) {
	turnBoard[x][y] = OP;

	vector<ConnectionInfo*> tmpConnInfoMap[AVAILABLE_CONNECT];


	// 1) connectionInfo line check
	for (int i = 1; i < CONN_SUCCESS; i++) {
		vector<ConnectionInfo*> connInfoList = myInfoMap[i];

		vector<ConnectionInfo*>::iterator iter = connInfoList.begin();
		for (iter; iter != connInfoList.end(); ++iter) {
			ConnectionInfo* conn = *iter;
			Direction dir = conn->dir;

			bool hasConnEffect = false;

			int indexOfPosition;

			if (dir == V) {
				if (x == conn->distinctConst) {
					if (y >= conn->connStart.second + 1 && y <= conn->connEnd.second - 1) {
						if (y != conn->connStart.second + 1 && y != conn->connEnd.second - 1) {
							hasConnEffect = true;
							indexOfPosition = y - conn->connStart.second + 1;
						}
					}
				}
			}
			else if (dir == H) {
				if (y == conn->distinctConst) {
					if (x >= conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
						if (x != conn->connStart.first + 1 && x != conn->connEnd.first - 1) {
							hasConnEffect = true;
							indexOfPosition = x - conn->connStart.first + 1;
						}
					}
				}
			}
			else if (dir == LD) {
				if (y + x == conn->distinctConst) {
					if (x >= conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
						if (x != conn->connStart.first + 1 && x != conn->connEnd.first - 1) {
							hasConnEffect = true;
							indexOfPosition = x - conn->connStart.first + 1;
						}
					}
				}
			}
			else if (dir == RD) {
				if (y - x == conn->distinctConst) {
					if (x >= conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
						if (x != conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
							hasConnEffect = true;
							indexOfPosition = x - (conn->connStart.first) + 1;
						}
					}
				}
			}

			if (hasConnEffect) {
				vector<ConnectionInfo*> tmpInfos = updateConnInfoByOpStone(conn, indexOfPosition);
				vector<ConnectionInfo*>::iterator iter = tmpInfos.begin();
				for (iter; iter != tmpInfos.end(); ++iter) {
					ConnectionInfo* tmpConn = *iter;
					tmpConnInfoMap[tmpConn->connLevel].push_back(tmpConn);
				}
			}
			else {
				tmpConnInfoMap[conn->connLevel].push_back(conn);
			}

		}
	}

	for (int i = 1; i < CONN_SUCCESS; i++) {
		myInfoMap[i] = tmpConnInfoMap[i];
	}
}

map<int, vector<ConnectionInfo*>> tryMyOneMove(int x, int y) {

	bool isVConsecutive = false;
	bool isHConsecutive = false;
	bool isLDConsecutive = false;
	bool isRDConsecutive = false;

	map<int, vector<ConnectionInfo*>> tmpConnInfoMap;


	// 1) connectionInfo line check
	for (int i = 1; i < CONN_SUCCESS; i++) {
		vector<ConnectionInfo*> connInfoList = myInfoMap[i];

		vector<ConnectionInfo*>::iterator iter = connInfoList.begin();
		for (iter; iter != connInfoList.end(); ++iter) {
			ConnectionInfo* originConn = *iter;
			ConnectionInfo* conn = copyConnInfo(originConn);
			Direction dir = conn->dir;

			bool hasEffect = false;
			bool hasConnEffect = false;
			bool isConsecutive = false;

			int indexOfPosition;

			if (dir == V) {
				if (x == conn->distinctConst) {
					if (y >= conn->connStart.second + 1 && y <= conn->connEnd.second - 1) {
						if (y != conn->connStart.second + 1 && y != conn->connEnd.second - 1) {
							hasConnEffect = true;
							indexOfPosition = y - conn->connStart.second + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isVConsecutive = true;
							}
						}
						hasEffect = true;
					}
				}
			}
			else if (dir == H) {
				if (y == conn->distinctConst) {
					if (x >= conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
						if (x != conn->connStart.first + 1 && x != conn->connEnd.first - 1) {
							hasConnEffect = true;
							indexOfPosition = x - conn->connStart.first + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isHConsecutive = true;
							}
						}
						hasEffect = true;

					}
				}
			}
			else if (dir == LD) {
				if (y + x == conn->distinctConst) {
					if (x >= conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
						if (x != conn->connStart.first + 1 && x != conn->connEnd.first - 1) {
							hasConnEffect = true;
							indexOfPosition = x - conn->connStart.first + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isLDConsecutive = true;
							}
						}
						hasEffect = true;
					}
				}
			}
			else if (dir == RD) {
				if (y - x == conn->distinctConst) {
					if (x >= conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
						if (x != conn->connStart.first + 1 && x <= conn->connEnd.first - 1) {
							hasConnEffect = true;
							indexOfPosition = x - (conn->connStart.first) + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isRDConsecutive = true;
							}
						}
						hasEffect = true;
					}
				}
			}

			if (hasEffect) {
				// check this move is consecutive

				if (hasConnEffect) {
					if (isConsecutive) { // consecutive stone
										 // remove origin and update renew info
						updateConnInfoByMyStone(conn, indexOfPosition);
						tmpConnInfoMap[conn->connLevel].push_back(conn);
					}
					else {
						// make new renew info
						ConnectionInfo* newConn = copyConnInfo(conn);
						updateConnInfoByMyStone(newConn, indexOfPosition);
						tmpConnInfoMap[newConn->connLevel].push_back(newConn);


						// and calc origin info
						bool result = calcConnAvailable(conn);
						if (result) {
							tmpConnInfoMap[conn->connLevel].push_back(conn);
						}
					}
				}
				else {
					bool result = calcConnAvailable(conn);
					if (result) {
						tmpConnInfoMap[conn->connLevel].push_back(conn);
					}
				}
			}
			else {
				tmpConnInfoMap[conn->connLevel].push_back(conn);
			}
		}
	}

	ConnectionInfo* connV;
	ConnectionInfo* connH;
	ConnectionInfo* connLD;
	ConnectionInfo* connRD;

	// setting 이 안되어있으니 일단 conn level 을 수동으로 여기서 주자
	if (!isVConsecutive) {
		connV = makeConn1(V, x, y);
		if (connV != NULL) {
			connV->connLevel = 1;
			tmpConnInfoMap[1].push_back(connV);
		}
	}
	if (!isHConsecutive) {
		connH = makeConn1(H, x, y);
		if (connH != NULL) {
			connH->connLevel = 1;
			tmpConnInfoMap[1].push_back(connH);
		}
	}
	if (!isLDConsecutive) {
		connLD = makeConn1(LD, x, y);
		if (connLD != NULL) {
			connLD->connLevel = 1;
			tmpConnInfoMap[1].push_back(connLD);
		}
	}
	if (!isRDConsecutive) {
		connRD = makeConn1(RD, x, y);
		if (connRD != NULL) {
			connRD->connLevel = 1;
			tmpConnInfoMap[1].push_back(connRD);
		}
	}
	return tmpConnInfoMap;
}

void doMyOneMove(int x, int y) {
	output<< "doMyMove" << "  x: " << x << "  y: " << y << endl;
	turnBoard[x][y] = MY;

	bool isVConsecutive = false;
	bool isHConsecutive = false;
	bool isLDConsecutive = false;
	bool isRDConsecutive = false;

	vector<ConnectionInfo*> tmpConnInfoMap[AVAILABLE_CONNECT];


	// 1) connectionInfo line check
	for (int i = 1; i < CONN_SUCCESS; i++) {
		vector<ConnectionInfo*> connInfoList = myInfoMap[i];

		vector<ConnectionInfo*>::iterator iter = connInfoList.begin();
		for (iter; iter != connInfoList.end(); ++iter) {
			ConnectionInfo* conn = *iter;
			Direction dir = conn->dir;

			bool hasEffect = false;
			bool hasConnEffect = false;
			bool isConsecutive = false;

			int indexOfPosition;

			if (dir == V) {
				if (x == conn->distinctConst) {
					if (y >= conn->connStart.second - 1 && y <= conn->connEnd.second + 1) {
						if (y != conn->connStart.second - 1 && y != conn->connEnd.second + 1) {
							hasConnEffect = true;
							indexOfPosition = y - conn->connStart.second + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isVConsecutive = true;
							}
						}
						hasEffect = true;
					}
				}
			}
			else if (dir == H) {
				if (y == conn->distinctConst) {
					if (x >= conn->connStart.first - 1 && x <= conn->connEnd.first + 1) {
						if (x != conn->connStart.first - 1 && x != conn->connEnd.first + 1) {
							hasConnEffect = true;
							indexOfPosition = x - conn->connStart.first + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isHConsecutive = true;
							}
						}
						hasEffect = true;

					}
				}
			}
			else if (dir == LD) {
				if (y + x == conn->distinctConst) {
					if (x >= conn->connStart.first - 1 && x <= conn->connEnd.first + 1) {
						if (x != conn->connStart.first - 1 && x != conn->connEnd.first + 1) {
							hasConnEffect = true;
							indexOfPosition = x - conn->connStart.first + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isLDConsecutive = true;
							}
						}
						hasEffect = true;
					}
				}
			}
			else if (dir == RD) {
				if (y - x == conn->distinctConst) {
					if (x >= conn->connStart.first - 1 && x <= conn->connEnd.first + 1) {
						if (x != conn->connStart.first - 1 && x <= conn->connEnd.first + 1) {
							hasConnEffect = true;
							indexOfPosition = x - (conn->connStart.first) + 1;
							if (conn->connAble[indexOfPosition - 1] == ALREADY_HAVE || conn->connAble[indexOfPosition + 1] == ALREADY_HAVE) {
								isConsecutive = true;
								isRDConsecutive = true;
							}
						}
						hasEffect = true;
					}
				}
			}

			if (hasEffect) {
				// check this move is consecutive

				if (hasConnEffect) {
					if (isConsecutive) { // consecutive stone
						// remove origin and update renew info
						updateConnInfoByMyStone(conn, indexOfPosition);
						tmpConnInfoMap[conn->connLevel].push_back(conn);
					}
					else {
						// make new renew info
						ConnectionInfo* newConn = copyConnInfo(conn);
						updateConnInfoByMyStone(newConn, indexOfPosition);
						tmpConnInfoMap[newConn->connLevel].push_back(newConn);


						// and calc origin info
						bool result = calcConnAvailable(conn);
						if (result) {
							tmpConnInfoMap[conn->connLevel].push_back(conn);
						}
					}
				}
				else {
					bool result = calcConnAvailable(conn);
					if (result) {
						tmpConnInfoMap[conn->connLevel].push_back(conn);
					}
				}
			}
			else {
				tmpConnInfoMap[conn->connLevel].push_back(conn);
			}
		}
	}

	for (int i = 1; i < CONN_SUCCESS; i++) {
		myInfoMap[i] = tmpConnInfoMap[i];
	}

	ConnectionInfo* connV;
	ConnectionInfo* connH;
	ConnectionInfo* connLD;
	ConnectionInfo* connRD;

	// setting 이 안되어있으니 일단 conn level 을 수동으로 여기서 주자
	if (!isVConsecutive) {
		connV = makeConn1(V, x, y);
		if (connV != NULL) {
			connV->connLevel = 1;
			myInfoMap[1].push_back(connV);
		}
	}
	if (!isHConsecutive) {
		connH = makeConn1(H, x, y);
		if (connH != NULL) {
			connH->connLevel = 1;
			myInfoMap[1].push_back(connH);
		}
	}
	if (!isLDConsecutive) {
		connLD = makeConn1(LD, x, y);
		if (connLD != NULL) {
			connLD->connLevel = 1;
			myInfoMap[1].push_back(connLD);
		}
	}
	if (!isRDConsecutive) {
		connRD = makeConn1(RD, x, y);
		if (connRD != NULL) {
			connRD->connLevel = 1;
			myInfoMap[1].push_back(connRD);
		}
	}
}

ConnectionInfo* copyConnInfo(ConnectionInfo* conn) {
	ConnectionInfo* newConn = (ConnectionInfo*)malloc(sizeof(ConnectionInfo));
	newConn->dir = conn->dir;
	newConn->distinctConst = conn->distinctConst;

	newConn->connStart.first = conn->connStart.first;
	newConn->connStart.second = conn->connStart.second;
	newConn->connEnd.first = conn->connEnd.first;
	newConn->connEnd.second = conn->connEnd.second;

	for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
		newConn->connAble[i] = conn->connAble[i];
	}
	newConn->connLevel = conn->connLevel;
	newConn->startIndex = conn->startIndex;
	newConn->endIndex = conn->endIndex;

	return newConn;
}

vector<ConnectionInfo*> updateConnInfoByOpStone(ConnectionInfo* connInfo, int indexOfPosition) {
	vector<ConnectionInfo*> updateConnInfo;

	if (indexOfPosition >= connInfo->startIndex && indexOfPosition <= connInfo->endIndex) {
		// 반 나눌 때
		ConnectionInfo* conn1 = copyConnInfo(connInfo);
		ConnectionInfo* conn2 = copyConnInfo(connInfo);

		for (int i = indexOfPosition; i < CONNECTABLE_LENGTH; i++) {
			conn1->connAble[i] = DISABLE;
		}
		for (int i = indexOfPosition; i >= 0; i--) {
			conn2->connAble[i] = DISABLE;
		}

		int connCount = 0;
		for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
			if (conn1->connAble[i] == CONNECTABLE || conn1->connAble[i] == ALREADY_HAVE) {
				connCount++;
			}
		}
		if (connCount >= CONN_SUCCESS) {
			conn1->connLevel = connCount;
			updateConnInfo.push_back(conn1);
		}

		connCount = 0;
		for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
			if (conn2->connAble[i] == CONNECTABLE || conn2->connAble[i] == ALREADY_HAVE) {
				connCount++;
			}
		}
		if (connCount >= CONN_SUCCESS) {
			conn2->connLevel = connCount;
			updateConnInfo.push_back(conn2);
		}

		return updateConnInfo;
	}
	else {

		int startIndex = connInfo->startIndex;
		int endIndex = connInfo->endIndex;

		Direction dir = connInfo->dir;

		int curX;
		int curY;
		if (dir == V) {
			curX = connInfo->connStart.first;
			curY = connInfo->connStart.second + connInfo->startIndex - 1;
		}
		else if (dir == H) {
			curX = connInfo->connStart.first + connInfo->startIndex - 1;
			curY = connInfo->connStart.second;
		}
		else if (dir == LD) {
			curX = connInfo->connStart.first + connInfo->startIndex - 1;
			curY = connInfo->connStart.second - (connInfo->startIndex - 1);
		}
		else if (dir == RD) {
			curX = connInfo->connStart.first + connInfo->startIndex - 1;
			curY = connInfo->connStart.second + connInfo->startIndex - 1;
		}

		bool disable = false;
		// for my stone + 1 is added
		for (int i = 1; i <= ONE_SIDE_EFFECT; i++) {
			if (disable == true) {
				connInfo->connAble[startIndex + i] = DISABLE;
				continue;
			}

			int refX, refY;
			if (dir == V) {
				refX = curX;
				refY = curY + i;
			}
			else if (dir == H) {
				refX = curX + i;
				refY = curY;
			}
			else if (dir == LD) {
				refX = curX + i;
				refY = curY - i;
			}
			else if (dir == RD) {
				refX = curX + i;
				refY = curY + i;
			}

			if (isOnBoard(refX, refY)) {
				int status = turnBoard[refX][refY];

				if (i == ONE_SIDE_EFFECT) {
					if (status == MY) {
						connInfo->connAble[startIndex + i - 1] = DISABLE;
					}
					for (int j = startIndex + i; j < CONNECTABLE_LENGTH; j++) {
						connInfo->connAble[j] = DISABLE;
					}
					break;
				}

				if (status == EMPTY) {
					connInfo->connAble[startIndex + i] = CONNECTABLE;
				}
				else if (status == OP || status == BLOCK) {
					connInfo->connAble[startIndex + i] = DISABLE;
					disable = true;
				}
				else if (status = MY) {
					connInfo->connAble[startIndex + i] = ALREADY_HAVE;
				}
			}
			else {
				connInfo->connAble[startIndex + i] = DISABLE;
				disable = true;
			}
		}

		if (dir == V) {
			curX = connInfo->connStart.first;
			curY = connInfo->connStart.second + connInfo->endIndex - 1;
		}
		else if (dir == H) {
			curX = connInfo->connStart.first + connInfo->endIndex - 1;
			curY = connInfo->connStart.second;
		}
		else if (dir == LD) {
			curX = connInfo->connStart.first + connInfo->endIndex - 1;
			curY = connInfo->connStart.second - (connInfo->endIndex - 1);
		}
		else if (dir == RD) {
			curX = connInfo->connStart.first + connInfo->endIndex - 1;
			curY = connInfo->connStart.second + connInfo->endIndex - 1;
		}

		disable = false;
		for (int i = 1; i <= ONE_SIDE_EFFECT; i++) {
			if (disable == true) {
				connInfo->connAble[endIndex - i] = DISABLE;
				continue;
			}

			int refX, refY;
			if (dir == V) {
				refX = curX;
				refY = curY - i;
			}
			else if (dir == H) {
				refX = curX - i;
				refY = curY;
			}
			else if (dir == LD) {
				refX = curX - i;
				refY = curY + i;
			}
			else if (dir == RD) {
				refX = curX - i;
				refY = curY - i;
			}

			if (isOnBoard(refX, refY)) {
				int status = turnBoard[refX][refY];

				if (i == ONE_SIDE_EFFECT) {
					if (status == MY) {
						connInfo->connAble[endIndex - i + 1] = DISABLE;
					}
					for (int j = endIndex - i; j >= 0; j--) {
						connInfo->connAble[j] = DISABLE;
					}
					continue;
				}


				if (status == EMPTY) {
					connInfo->connAble[endIndex - i] = CONNECTABLE;
				}
				else if (status == OP || status == BLOCK) {
					connInfo->connAble[endIndex - i] = DISABLE;
					disable = true;
				}
				else if (status = MY) {
					connInfo->connAble[endIndex - i] = ALREADY_HAVE;
				}
			}
			else {
				connInfo->connAble[endIndex - i] = DISABLE;
				disable = true;
			}
		}
		int connCount = 0;
		for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
			if (connInfo->connAble[i] == CONNECTABLE || connInfo->connAble[i] == ALREADY_HAVE) {
				connCount++;
			}
		}
		if (connCount >= CONN_SUCCESS) {
			updateConnInfo.push_back(connInfo);
			return updateConnInfo;
		}
		else {
			return updateConnInfo;
		}
	}
}

bool updateConnInfoByMyStone(ConnectionInfo* connInfo, int indexOfPosition) {

	connInfo->connLevel++;

	if (indexOfPosition < connInfo->startIndex) {
		connInfo->startIndex = indexOfPosition;
	}
	else if (indexOfPosition > connInfo->endIndex) {
		connInfo->endIndex = indexOfPosition;
	}

	int startIndex = connInfo->startIndex;
	int endIndex = connInfo->endIndex;

	Direction dir = connInfo->dir;

	int curX;
	int curY;
	if (dir == V) {
		curX = connInfo->connStart.first;
		curY = connInfo->connStart.second + connInfo->startIndex - 1;
	}
	else if (dir == H) {
		curX = connInfo->connStart.first + connInfo->startIndex - 1;
		curY = connInfo->connStart.second;
	}
	else if (dir == LD) {
		curX = connInfo->connStart.first + connInfo->startIndex - 1;
		curY = connInfo->connStart.second - (connInfo->startIndex - 1);
	}
	else if (dir == RD) {
		curX = connInfo->connStart.first + connInfo->startIndex - 1;
		curY = connInfo->connStart.second + connInfo->startIndex - 1;
	}

	bool disable = false;
	// for my stone + 1 is added
	for (int i = 1; i <= ONE_SIDE_EFFECT; i++) {
		if (disable == true) {
			connInfo->connAble[startIndex + i] = DISABLE;
			continue;
		}

		int refX, refY;
		if (dir == V) {
			refX = curX;
			refY = curY + i;
		}
		else if (dir == H) {
			refX = curX + i;
			refY = curY;
		}
		else if (dir == LD) {
			refX = curX + i;
			refY = curY - i;
		}
		else if (dir == RD) {
			refX = curX + i;
			refY = curY + i;
		}

		if (isOnBoard(refX, refY)) {
			int status = turnBoard[refX][refY];

			if (i == ONE_SIDE_EFFECT) {
				if (status == MY) {
					connInfo->connAble[startIndex + i - 1] = DISABLE;
				}
				for (int j = startIndex + i; j < CONNECTABLE_LENGTH; j++) {
					connInfo->connAble[j] = DISABLE;
				}
				break;
			}

			if (status == EMPTY) {
				connInfo->connAble[startIndex + i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connAble[startIndex + i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connAble[startIndex + i] = ALREADY_HAVE;
			}
		}
		else {
			connInfo->connAble[startIndex + i] = DISABLE;
			disable = true;
		}
	}

	if (dir == V) {
		curX = connInfo->connStart.first;
		curY = connInfo->connStart.second + connInfo->endIndex - 1;
	}
	else if (dir == H) {
		curX = connInfo->connStart.first + connInfo->endIndex - 1;
		curY = connInfo->connStart.second;
	}
	else if (dir == LD) {
		curX = connInfo->connStart.first + connInfo->endIndex - 1;
		curY = connInfo->connStart.second - (connInfo->endIndex - 1);
	}
	else if (dir == RD) {
		curX = connInfo->connStart.first + connInfo->endIndex - 1;
		curY = connInfo->connStart.second + connInfo->endIndex - 1;
	}

	disable = false;
	for (int i = 1; i <= ONE_SIDE_EFFECT; i++) {
		if (disable == true) {
			connInfo->connAble[endIndex - i] = DISABLE;
			continue;
		}

		int refX, refY;
		if (dir == V) {
			refX = curX;
			refY = curY - i;
		}
		else if (dir == H) {
			refX = curX - i;
			refY = curY;
		}
		else if (dir == LD) {
			refX = curX - i;
			refY = curY + i;
		}
		else if (dir == RD) {
			refX = curX - i;
			refY = curY - i;
		}

		if (isOnBoard(refX, refY)) {
			int status = turnBoard[refX][refY];

			if (i == ONE_SIDE_EFFECT) {
				if (status == MY) {
					connInfo->connAble[endIndex - i + 1] = DISABLE;
				}
				for (int j = endIndex - i; j >= 0; j--) {
					connInfo->connAble[j] = DISABLE;
				}
				continue;
			}


			if (status == EMPTY) {
				connInfo->connAble[endIndex - i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connAble[endIndex - i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connAble[endIndex - i] = ALREADY_HAVE;
			}
		}
		else {
			connInfo->connAble[endIndex - i] = DISABLE;
			disable = true;
		}
	}

	int connCount = 0;
	for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
		if (connInfo->connAble[i] == CONNECTABLE) {
			connCount++;
		}
	}
	if (connCount >= CONN_SUCCESS) {
		return true;
	}
	else {
		return false;
	}
}

void renewDensityBoard() {
	initDensityBoard();
	int maxValue = 0;
	int hX = -1;
	int hY = -1;

	// 1) connectionInfo line check
	for (int i = 1; i < CONN_SUCCESS; i++) {
		vector<ConnectionInfo*> connInfoList = myInfoMap[i];

		vector<ConnectionInfo*>::iterator iter = connInfoList.begin();
		for (iter; iter != connInfoList.end(); ++iter) {
			ConnectionInfo* conn = *iter;
			Direction dir = conn->dir;

			int startX = conn->connStart.first;
			int startY = conn->connStart.second;
			for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
				int curX, curY;
				if (dir == V) {
					curX = startX;
					curY = startY + i;
				}
				else if (dir == H) {
					curX = startX + i;
					curY = startY;
				}
				else if (dir == LD) {
					curX = startX + i;
					curY = startY - i;
				}
				else if (dir == RD) {
					curX = startX + i;
					curY = startY - i;
				}
				if (isOnBoard(curX, curY)) {
					if (turnBoard[curX][curY] == EMPTY && conn->connAble[i] == CONNECTABLE) {
						if (conn->connLevel == 4) densityBoard[curX][curY] += 1000000;
						if (conn->connLevel == 5) densityBoard[curX][curY] += 2000000;
						if (conn->connLevel == 6) densityBoard[curX][curY] += 20000000;
						densityBoard[curX][curY] += conn->connLevel;
					}
				}
			}
		}
	}

	for (int i = 0; i < B_SIZE; i++) {
		for (int j = 0; j < B_SIZE; j++) {
			if (turnBoard[i][j] == MY || turnBoard[i][j] == OP || turnBoard[i][j] == BLOCK) {
				densityBoard[i][j] = -1;
			}
		}
	}
}

void initDensityBoard() {
	for (int i = 0; i < B_SIZE; i++) {
		for (int j = 0; j < B_SIZE; j++) {
			densityBoard[i][j] = 0;
		}
	}
}

pair<int, int> conn4Finish() {
	output << " conn4Finish is called" << endl;
	// conn4 있는지 확인
	if (myInfoMap[4].size() == 0) {
		output << "no 4" << endl;
		return make_pair(-1, -1);
	}
	else {
		output << "yes 4" << endl;
		ConnectionInfo* conn = myInfoMap[4][0];

		int startX = conn->connStart.first;
		int startY = conn->connStart.second;
		Direction dir = conn->dir;
		for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
			int curX, curY;
			if (dir == V) {
				curX = startX;
				curY = startY + i;
			}
			else if (dir == H) {
				curX = startX + i;
				curY = startY;
			}
			else if (dir == LD) {
				curX = startX + i;
				curY = startY - i;
			}
			else if (dir == RD) {
				curX = startX + i;
				curY = startY - i;
			}
			if (isOnBoard(curX, curY)) {
				if (turnBoard[curX][curY] == EMPTY && conn->connAble[i] == CONNECTABLE) {
					return make_pair(curX, curY);
				}
			}
		}
	}
}

pair<int, int> conn5Finish() {
	output << " conn5Finish is called" << endl;
	// conn4 있는지 확인
	if (myInfoMap[5].size() == 0) {
		output << "no 5" << endl;
		return make_pair(-1, -1);
	}
	else {
		output << "yes 5" << endl;
		ConnectionInfo* conn = myInfoMap[5][0];

		int startX = conn->connStart.first;
		int startY = conn->connStart.second;
		Direction dir = conn->dir;
		for (int i = 0; i < CONNECTABLE_LENGTH; i++) {
			int curX, curY;
			if (dir == V) {
				curX = startX;
				curY = startY + i;
			}
			else if (dir == H) {
				curX = startX + i;
				curY = startY;
			}
			else if (dir == LD) {
				curX = startX + i;
				curY = startY - i;
			}
			else if (dir == RD) {
				curX = startX + i;
				curY = startY - i;
			}
			if (isOnBoard(curX, curY)) {
				if (turnBoard[curX][curY] == EMPTY && conn->connAble[i] == CONNECTABLE) {
					return make_pair(curX, curY);
				}
			}
		}
	}
}

/*************/

void write(char* charchar) {
	output << charchar << endl;
}
void close() {
	output.close();
}

pair<int, int> defensiveSlideWindow(int x, int y, int cnt) {
	int mCount[11];

	output << "turnBoard info" << endl;
	for (int i = 0; i < B_SIZE; i++) {
		for (int j = 0; j < B_SIZE; j++) {
			output << turnBoard[i][j] << " ";
		}

		output << endl;
	}

	fill_n(mCount, 11, 0);
	// 방향별로, 서치해야함
	int window[6];
	fill_n(window, 6, 0);
	//1. horizontal
	output << "horizontal" << endl;
	for (int q = 0; q < 6; q++) {
		
			//처음 것부터 돈다.
			int attackerStone = 0;
			int markedCount[6];
			fill_n(markedCount, 6, 0);
			int markCount = 0;
			int defenderCount = 0;
			int count = 0;
			//윈도우를 만든다.

			// TODO: window 크기가 틀림
			for (int i = x - 5 + q; i <= x + q; i++, count++) {
				window[count] = turnBoard[i][y];
			}

			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "hor 여기까진 왔다...1111 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}

			fill_n(window, 6, 0);

			//처음 것부터 돈다.
			attackerStone = 0;
			fill_n(markedCount, 6, 0);
			markCount = 0;
			defenderCount = 0;
			count = 0;

			//윈도우를 만든다.
			
			for (int i = x + q; i >= x - 5 + q; i--, count++) {
				window[count] = turnBoard[i][y];
			}
			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "hor 여기까진 왔다...2222 " << endl;
				for (int j = 0; j < 6; j++) {

					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}
	}
	int threatCount = 0;
	pair<int, int> possiblePair;
	int max = 0;
	if(cnt == 0) {
		for (int i = 0; i < 11; i++) {
			output << "mCount[i]: " << i << "   " << mCount[i] << endl;
			if (mCount[i] > max) {
				max = mCount[i];
				possiblePair = make_pair(x - 5 + i, y);
				threatCount += 1;
			}
		}
	}
	else {
		max = 0;
		for (int i = 11; i > 0; i--) {
			if (mCount[i] > max) {
				max = mCount[i];
				possiblePair = make_pair(x - 5 + i, y);
				threatCount += 1;
			}
		}
	}
	if (threatCount != 0) {
		write("c11111111111");
		output << "possiblePair: " << possiblePair.first << " , " << possiblePair.second << endl;
		return possiblePair;
	}

	//2. vertical
	fill_n(mCount, 11, 0);
	fill_n(window, 6, 0);
	output << "vertical" << endl;
	for (int q = 0; q < 6; q++) {
		
			//처음 것부터 돈다.
			int attackerStone = 0;
			int markedCount[6];
			fill_n(markedCount, 6, 0);
			int markCount = 0;
			int defenderCount = 0;
			int count = 0;
			
			//윈도우를 만든다.
			for (int i = y - 5 + q; i <= y + q; i++, count++) {
				window[count] = turnBoard[x][i];
				
			}

			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "ver 여기까진 왔다...1111 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}
		
		fill_n(window, 6, 0);
		
			//처음 것부터 돈다.
			attackerStone = 0;
			fill_n(markedCount, 6, 0);
			markCount = 0;
			defenderCount = 0;
			count = 0;
			
			//윈도우를 만든다.
			for (int i = y + q; i >= y - 5 + q; i--, count++) {
				window[count] = turnBoard[x][i];
			}
			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "ver 여기까진 왔다...22222 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}
		
	}
	int threatCountV = 0;
	pair<int, int> possiblePairV;
	int maxV = 0;
	
	if (cnt == 0) {
		for (int i = 0; i < 11; i++) {
		output << "mCount[i]: " << i << "   " << mCount[i] << endl;
		if (mCount[i] > maxV) {
			maxV = mCount[i];
			possiblePairV = make_pair(x, y - 5 + i);
			threatCountV += 1;
		}
	}
	}
	else {
		maxV = 0;
		for (int i = 11; i > 0; i--) {
			if (mCount[i] > maxV) {
				maxV = mCount[i];
				possiblePairV = make_pair(x, y - 5 + i);
				threatCountV += 1;
			}
		}
	}

	if (threatCountV != 0) {
		write("c22222222222222");
		output << "possiblePairV: " << possiblePairV.first << " , " << possiblePairV.second << endl;
		return possiblePairV;
	}

	//3. 왼쪽 대각선.

	fill_n(mCount, 11, 0);
	fill_n(window, 6, 0);
	output << "ld" << endl;
	for (int q = 0; q < 6; q++) {
			//처음 것부터 돈다.
			int attackerStone = 0;
			int markedCount[6];
			fill_n(markedCount, 6, 0);
			int markCount = 0;
			int defenderCount = 0;
			int count = 0;
			int yCount = y + 5 - q;
			//윈도우를 만든다.
			for (int i = x - 5 + q; i <= x + q; i++, count++, yCount--) {
				window[count] = turnBoard[i][yCount];
			}
			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "ldld 여기까진 왔다...1111 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}
		fill_n(window, 6, 0);
			//처음 것부터 돈다.
			attackerStone = 0;
			fill_n(markedCount, 6, 0);
			markCount = 0;
			defenderCount = 0;
			count = 0;
			yCount = y + q;
			//윈도우를 만든다.
			for (int i = x + q; i >= x -5 + q; i--, count++, yCount++) {
				window[count] = turnBoard[i][yCount];
			}
			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "ldld 여기까진 왔다...2222 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;

					}
				}
			}
	}
	int threatCountLD = 0;
	pair<int, int> possiblePairLD;
	int maxLD = 0;

	if (cnt == 0) {
		for (int i = 0; i < 11; i++) {
			output << "mCount[i]: " << i << "   " << mCount[i] << endl;
			if (mCount[i] > maxLD) {
				maxLD = mCount[i];
				possiblePairLD = make_pair(x - 5 + i, y + 5 - i);
				threatCountLD += 1;
			}
		}
	}
	else {
		maxLD = 0;
		for (int i = 11; i > 0; i--) {
			if (mCount[i] > maxLD) {
				maxLD = mCount[i];
				possiblePairLD = make_pair(x - 5 + i, y + 5 - i);
				threatCountLD += 1;
			}
		}
	}

	if (threatCountLD != 0) {
		write("c3333333333333333");
		output << "possiblePairLD: " << possiblePairLD.first << " , " << possiblePairLD.second << endl;
		return possiblePairLD;
	}

	// 4. 오른쪽 대각선.
	fill_n(mCount, 11, 0);
	fill_n(window, 6, 0);
	output << "rd" << endl;
	for (int q = 0; q < 6; q++) {
			//처음 것부터 돈다.
			int attackerStone = 0;
			int markedCount[6];
			fill_n(markedCount, 6, 0);
			int markCount = 0;
			int defenderCount = 0;
			int count = 0;
			int yCount = y - 5 + q;
			//윈도우를 만든다.
			for (int i = x - 5 + q; i <= x + q; i++, count++, yCount++) {
				window[count] = turnBoard[i][yCount];
			}
			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "rdrd 여기까진 왔다...1111 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}
			//처음 것부터 돈다.
			attackerStone = 0;
			fill_n(markedCount, 6, 0);
			markCount = 0;
			defenderCount = 0;
			count = 0;
			yCount = y + q;
			//윈도우를 만든다.
			for (int i = x + q; i >= x - 5 + q ; i--, count++, yCount--) {
				window[count] = turnBoard[i][yCount];
			}
			for (int i = 0; i < 6; i++) {
				if (mCount[i + q] == 0 && window[i] == 0) { // 마크가 되어있지 않다면, 마크를 해야함.
					markedCount[i] = i + q + 1;
				}
				if (window[i] == 1) { // defender가 있는지 체크한다.
					defenderCount += 1;
				}
				else if (window[i] == 2 || window[i] == 3) { //attacker가 있는지 체크한다.
					attackerStone += 1;
				}
			}

			for (int i = 0; i < 6; i++) {
				if (markedCount[i] != 0) {
					markCount += 1;
				}
			}
			if ((markCount == 0 || defenderCount == 0) && attackerStone >= 4) {
				output << "rdrd 여기까진 왔다...22222 " << endl;
				for (int j = 0; j < 6; j++) {
					if (markedCount[j] != 0) {
						mCount[markedCount[j] - 1] += 1;
					}
				}
			}
	}
	int threatCountRD = 0;
	pair<int, int> possiblePairRD;
	int maxRD = 0;

	if (cnt == 0) {
		for (int i = 0; i < 11; i++) {
			if (mCount[i] > maxRD) {
				output << "mCount[i]: " << i << "   " << mCount[i] << endl;
				maxRD = mCount[i];
				possiblePairRD = make_pair(x - 5 + i, y - 5 + i);
				threatCountRD += 1;
			}
		}
	}
	else {
		maxRD = 0;
		for (int i = 11; i > 0; i--) {
			if (mCount[i] > maxRD) {
				maxRD = mCount[i];
				possiblePairRD = make_pair(x - 5 + i, y - 5 + i);
				threatCountRD += 1;
			}
		}
	}

	if (threatCountRD != 0) {
		write("c44444444444");
		output << "possiblePairLD: " << possiblePairRD.first << " , " << possiblePairRD.second << endl;
		return possiblePairRD;
	}
	else {
		write("nononononononoothreeeeeeaaaattt");
		return make_pair(-1, -1);
	}

}
int evaluationScore(int x, int y) {
	int score = 0;

	// 둔 돌로 connection list 갱신

	map<int, vector<ConnectionInfo*>> newConnectionList = tryMyOneMove(x, y);

	map<int, vector<ConnectionInfo*>> ::iterator iter;

	// connection list 다 돌면서 connection 1, connection 2, connection 3에 대해 weight를 주어 점수 계산

	iter = newConnectionList.find(1);
	if (iter != newConnectionList.end()) {
		int c1 = newConnectionList.find(1)->second.size();
		score += c1 * connection1;
	}

	iter = newConnectionList.find(2);
	if (iter != newConnectionList.end()) {
		int c1 = newConnectionList.find(2)->second.size();
		score += c1 * connection2;
	}

	iter = newConnectionList.find(3);
	if (iter != newConnectionList.end()) {
		int c1 = newConnectionList.find(3)->second.size();
		score += c1 * connection3;
	}


	iter = newConnectionList.find(4);
	if (iter != newConnectionList.end()) {
		score += 10000000;
	}

	iter = newConnectionList.find(5);
	if (iter != newConnectionList.end()) {
		score += 20000000;
	}

	iter = newConnectionList.find(6);
	if (iter != newConnectionList.end()) {
		score += 200000000;
	}


	return score;
}
vector<pair<int, int>> OffenseCandidate() {
	output << "offense cand" << endl;
	vector<pair<int, int> > candidatePoint;                    // evaluation할 point
	multimap<int, pair<int, int>, greater<int>> tempCandidate;  // 내림차순으로 정렬된
	multimap<int, pair<int, int>> ::iterator iter;
	multimap<int, pair<int, int>> ::iterator siter;

	int myAvailablePoint = 0;

	renewDensityBoard();
	// free 한 돌 갯수 구함

	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			if (densityBoard[i][j] >= 0) {
				myAvailablePoint++;
			}
		}
	} 
	
	// free한 돌 갯수가 최대넘버보다 작을때는 전체 available을 candidate에 넣음
	if (myAvailablePoint <= evalNum) {
		for (int i = 0; i < 19; i++) {
			for (int j = 0; j < 19; j++) {
				if (densityBoard[i][j] >= 0) {
					candidatePoint.push_back(make_pair(i, j));
				}
			}
		}

		return candidatePoint;
	}

	// free한 돌 갯수가 최대넘버보다 클 경우
	// 전체 density map 만듬
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			// board 전체 탐색, 둘수있는돌의 density를 map에 담는다.
			if (densityBoard[i][j] >= 0) {
				tempCandidate.insert(pair<int, pair<int, int>>(densityBoard[i][j], make_pair(i, j)));
			}
		}
	}

	int cur_candidateNum = 0;
	int x = 0;
	int y = 0;

	for (iter = tempCandidate.begin(); iter != tempCandidate.end(); iter++) {
		// candidate 후보 돌면서 candidate Point에 채워줌
		if (cur_candidateNum < evalNum) {

			x = iter->second.first;
			y = iter->second.second;
			candidatePoint.push_back(make_pair(x, y));
			cur_candidateNum++;

		}
	}

	return candidatePoint;
}

// 내 density board만 가지고 offense하게 돌을 놓음
pair<int, int> offenseMove() {
    pair<int, int> result;
    
    vector<pair<int, int> > candidate;
    int maxEval = -1;
    int curEval = 0;
    int x = 0;
    int y = 0;
    
    candidate = OffenseCandidate();
    
    for (int i = 0; i < candidate.size(); i++) {
        x = candidate[i].first;
        y = candidate[i].second;
		turnBoard[x][y] = MY;
        curEval = evaluationScore(x, y);
		turnBoard[x][y] = EMPTY;
        // 현재 점이 max evaluation score를 가지고 있다면 정답으로
		if (curEval > maxEval) {
			maxEval = curEval;
			result = make_pair(x, y);
		}
    }
    
    return result;
}


