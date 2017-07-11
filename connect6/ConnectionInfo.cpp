#include <set>
#include "ConnectionInfo.h"

set<ConnectionInfo*> myConnInfo[VALID_CONN + 1];

int turnBoard[B_SIZE][B_SIZE];
int testTurnBoard[B_SIZE][B_SIZE];
int densityBoard[B_SIZE][B_SIZE];

/*****************junsu**********************/

void removeConnInfo(ConnectionInfo* connInfo) {
	int connLevel = connInfo->connLevel;
	myConnInfo[connLevel].erase(connInfo);

	free(connInfo);
}

bool isOnBoard(int x, int y) {
	return x >= 0 && x <= WIDTH && y >= 0 && y <= HEIGHT;
}

bool calcConnAvailable(ConnectionInfo* connInfo) {
	int startIndex = connInfo->startIndex;
	int endIndex = connInfo->endIndex;

	Direction dir = connInfo->dir;

	int curX;
	int curY;

	if (dir == V) {
		curX = connInfo->connStart.first;
		curY = connInfo->connStart.second + HALF_CONN_RANGE;
	}
	else if (dir == H) {
		curX = connInfo->connStart.first + HALF_CONN_RANGE;
		curY = connInfo->connStart.second;
	}
	else if (dir == LD) {
		curX = connInfo->connStart.first + HALF_CONN_RANGE;
		curY = connInfo->connStart.second - HALF_CONN_RANGE;
	}
	else if (dir == RD) {
		curX = connInfo->connStart.first + HALF_CONN_RANGE;
		curY = connInfo->connStart.second + HALF_CONN_RANGE;
	}

	bool disable = false;
	// for my stone + 1 is added
	for (int i = 1; i <= HALF_EFFECT_RANGE; i++) {
		if (disable == true) {
			connInfo->connectablility[CONN_CENTER + i] = DISABLE;
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

			if (i == HALF_EFFECT_RANGE && status == MY) {
				if (status == MY) {
					connInfo->connectablility[CONN_CENTER + i - 1] = DISABLE;
				}
				connInfo->connectablility[CONN_CENTER + i] = DISABLE;
				continue;
			}

			if (status == EMPTY) {
				connInfo->connectablility[CONN_CENTER + i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connectablility[CONN_CENTER + i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connectablility[CONN_CENTER + i - 1] = DISABLE;
				connInfo->connectablility[CONN_CENTER + i] = DISABLE;
				disable = true;
			}
		}
		else {
			connInfo->connectablility[CONN_CENTER + i] = DISABLE;
			disable = true;
		}
	}

	disable = false;
	for (int i = 1; i <= HALF_EFFECT_RANGE; i++) {
		if (disable == true) {
			connInfo->connectablility[CONN_CENTER - i] = DISABLE;
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

			if (i == HALF_EFFECT_RANGE && status == MY) {
				if (status == MY) {
					connInfo->connectablility[CONN_CENTER - i + 1] = DISABLE;
				}
				connInfo->connectablility[CONN_CENTER - i] = DISABLE;
				continue;
			}


			if (status == EMPTY) {
				connInfo->connectablility[CONN_CENTER - i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connectablility[CONN_CENTER - i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connectablility[CONN_CENTER - i + 1] = DISABLE;
				connInfo->connectablility[CONN_CENTER - i] = DISABLE;
				disable = true;
			}
		}
		else {
			connInfo->connectablility[CONN_CENTER - i] = DISABLE;
			disable = true;
		}
	}

	int connCount = 0;
	for (int i = 0; i < CONN_ARR_LEN; i++) {
		if (connInfo->connectablility[i] == CONNECTABLE) {
			connCount++;
		}
	}
	if (connCount >= VALID_CONN) {
		return true;
	}
	else {
		return false;
	}
}

ConnectionInfo* buildConnectionLevel1(Direction dir, int x, int y) {
	ConnectionInfo* conn = (ConnectionInfo*)malloc(sizeof(ConnectionInfo));
	conn->dir = dir;
	conn->connectablility[CONN_CENTER] = ALREADY_HAVE;
	conn->connLevel = 1;

	conn->startIndex = CONN_CENTER;
	conn->endIndex = CONN_CENTER;

	if (dir == V) {
		// x = C
		conn->connStart = make_pair(x, y - HALF_CONN_RANGE);
		conn->connEnd = make_pair(x, y + HALF_CONN_RANGE);
		conn->distinctConst = x;

	}
	else if (dir == H) {
		// y = C
		conn->connStart = make_pair(x - HALF_CONN_RANGE, y);
		conn->connEnd = make_pair(x + HALF_CONN_RANGE, y);
		conn->distinctConst = y;
	}
	else if (dir == LD) {
		// y + x = C
		conn->connStart = make_pair(x - HALF_CONN_RANGE, y + HALF_CONN_RANGE);
		conn->connEnd = make_pair(x + HALF_CONN_RANGE, y - HALF_CONN_RANGE);
		conn->distinctConst = y + x;

	}
	else if (dir == RD) {
		// y - x = C
		conn->connStart = make_pair(x - HALF_CONN_RANGE, y - HALF_CONN_RANGE);
		conn->connEnd = make_pair(x + HALF_CONN_RANGE, y + HALF_CONN_RANGE);
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
	ConnectionInfo* connV = buildConnectionLevel1(V, x, y);
	ConnectionInfo* connH = buildConnectionLevel1(H, x, y);
	ConnectionInfo* connLD = buildConnectionLevel1(LD, x, y);
	ConnectionInfo* connRD = buildConnectionLevel1(RD, x, y);

	// setting 이 안되어있으니 일단 conn level 을 수동으로 여기서 주자

	if (connV != NULL) {
		connV->connLevel = 1;
		myConnInfo[1].insert(connV);
	}
	if (connH != NULL) {
		connH->connLevel = 1;
		myConnInfo[1].insert(connH);
	}
	if (connLD != NULL) {
		connLD->connLevel = 1;
		myConnInfo[1].insert(connLD);
	}
	if (connRD != NULL) {
		connRD->connLevel = 1;
		myConnInfo[1].insert(connRD);
	}
}


void doOpOneMove(int x, int y) {
	turnBoard[x][y] = OP;


	// 1) connectionInfo line check
	for (int i = 1; i < VALID_CONN; i++) {
		set<ConnectionInfo*> connInfoSet = myConnInfo[i];

		set<ConnectionInfo*>::iterator iter = connInfoSet.begin();
		for (iter; iter != connInfoSet.end(); ++iter) {
			ConnectionInfo* conn = *iter;
			Direction dir = conn->dir;

			bool hasConnEffect = false;

			// 해당 돌이 connectability array 내에서 어떤 index 에 있는가
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
				updateConnInfoByOpStone(conn, indexOfPosition);
			}

		}
	}
}

map<int, vector<ConnectionInfo*>> tryMyOneMove(int x, int y) {

	bool isVConsecutive = false;
	bool isHConsecutive = false;
	bool isLDConsecutive = false;
	bool isRDConsecutive = false;

	map<int, vector<ConnectionInfo*>> tmpConnInfoMap;


	// 1) connectionInfo line check
	for (int i = 1; i < VALID_CONN; i++) {
		set<ConnectionInfo*> connInfoSet = myConnInfo[i];

		set<ConnectionInfo*>::iterator iter = connInfoSet.begin();
		for (iter; iter != connInfoSet.end(); ++iter) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
		connV = buildConnectionLevel1(V, x, y);
		if (connV != NULL) {
			connV->connLevel = 1;
			tmpConnInfoMap[1].push_back(connV);
		}
	}
	if (!isHConsecutive) {
		connH = buildConnectionLevel1(H, x, y);
		if (connH != NULL) {
			connH->connLevel = 1;
			tmpConnInfoMap[1].push_back(connH);
		}
	}
	if (!isLDConsecutive) {
		connLD = buildConnectionLevel1(LD, x, y);
		if (connLD != NULL) {
			connLD->connLevel = 1;
			tmpConnInfoMap[1].push_back(connLD);
		}
	}
	if (!isRDConsecutive) {
		connRD = buildConnectionLevel1(RD, x, y);
		if (connRD != NULL) {
			connRD->connLevel = 1;
			tmpConnInfoMap[1].push_back(connRD);
		}
	}
	return tmpConnInfoMap;
}

void doMyOneMove(int x, int y) {
	turnBoard[x][y] = MY;

	bool isVConsecutive = false;
	bool isHConsecutive = false;
	bool isLDConsecutive = false;
	bool isRDConsecutive = false;
	
	// 1) connectionInfo line check
	for (int i = 1; i < VALID_CONN; i++) {

		set<ConnectionInfo*> connInfoSet = myConnInfo[i];
		set<ConnectionInfo*>::iterator iter = connInfoSet.begin();
		for (iter; iter != connInfoSet.end(); ++iter) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
							if (conn->connectablility[indexOfPosition - 1] == ALREADY_HAVE || conn->connectablility[indexOfPosition + 1] == ALREADY_HAVE) {
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
					}
					else {
						// make new renew info
						ConnectionInfo* newConn = copyConnInfo(conn);
						updateConnInfoByMyStone(newConn, indexOfPosition);
						myConnInfo[newConn->connLevel].insert(newConn);

						// and calc origin info
						bool result = calcConnAvailable(conn);
						if (!result) {
							myConnInfo[conn->connLevel].erase(conn);
						}
					}
				}
				else {
					bool result = calcConnAvailable(conn);
					if (!result) {
						myConnInfo[conn->connLevel].erase(conn);
					}
				}
			}
		}
	}

	ConnectionInfo* connV;
	ConnectionInfo* connH;
	ConnectionInfo* connLD;
	ConnectionInfo* connRD;

	// setting 이 안되어있으니 일단 conn level 을 수동으로 여기서 주자
	if (!isVConsecutive) {
		connV = buildConnectionLevel1(V, x, y);
		if (connV != NULL) {
			myConnInfo[1].insert(connV);
		}
	}
	if (!isHConsecutive) {
		connH = buildConnectionLevel1(H, x, y);
		if (connH != NULL) {
			myConnInfo[1].insert(connH);
		}
	}
	if (!isLDConsecutive) {
		connLD = buildConnectionLevel1(LD, x, y);
		if (connLD != NULL) {
			myConnInfo[1].insert(connLD);
		}
	}
	if (!isRDConsecutive) {
		connRD = buildConnectionLevel1(RD, x, y);
		if (connRD != NULL) {
			myConnInfo[1].insert(connRD);
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

	for (int i = 0; i < CONN_ARR_LEN; i++) {
		newConn->connectablility[i] = conn->connectablility[i];
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

		for (int i = indexOfPosition; i < CONN_ARR_LEN; i++) {
			conn1->connectablility[i] = DISABLE;
		}
		for (int i = indexOfPosition; i >= 0; i--) {
			conn2->connectablility[i] = DISABLE;
		}

		int connCount = 0;
		for (int i = 0; i < CONN_ARR_LEN; i++) {
			if (conn1->connectablility[i] == CONNECTABLE || conn1->connectablility[i] == ALREADY_HAVE) {
				connCount++;
			}
		}
		if (connCount >= VALID_CONN) {
			conn1->connLevel = connCount;
			updateConnInfo.push_back(conn1);
		}

		connCount = 0;
		for (int i = 0; i < CONN_ARR_LEN; i++) {
			if (conn2->connectablility[i] == CONNECTABLE || conn2->connectablility[i] == ALREADY_HAVE) {
				connCount++;
			}
		}
		if (connCount >= VALID_CONN) {
			conn2->connLevel = connCount;
			updateConnInfo.push_back(conn2);
		}

		return updateConnInfo;
	}
	
	// 반으로 나누는 건 아니고 옆에서 깔짝 effect 를 줬을 때
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
		for (int i = 1; i <= HALF_EFFECT_RANGE; i++) {
			if (disable == true) {
				connInfo->connectablility[startIndex + i] = DISABLE;
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

				if (i == HALF_EFFECT_RANGE) {
					if (status == MY) {
						connInfo->connectablility[startIndex + i - 1] = DISABLE;
					}
					for (int j = startIndex + i; j < CONN_ARR_LEN; j++) {
						connInfo->connectablility[j] = DISABLE;
					}
					break;
				}

				if (status == EMPTY) {
					connInfo->connectablility[startIndex + i] = CONNECTABLE;
				}
				else if (status == OP || status == BLOCK) {
					connInfo->connectablility[startIndex + i] = DISABLE;
					disable = true;
				}
				else if (status = MY) {
					connInfo->connectablility[startIndex + i] = ALREADY_HAVE;
				}
			}
			else {
				connInfo->connectablility[startIndex + i] = DISABLE;
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
		for (int i = 1; i <= HALF_EFFECT_RANGE; i++) {
			if (disable == true) {
				connInfo->connectablility[endIndex - i] = DISABLE;
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

				if (i == HALF_EFFECT_RANGE) {
					if (status == MY) {
						connInfo->connectablility[endIndex - i + 1] = DISABLE;
					}
					for (int j = endIndex - i; j >= 0; j--) {
						connInfo->connectablility[j] = DISABLE;
					}
					continue;
				}

				if (status == EMPTY) {
					connInfo->connectablility[endIndex - i] = CONNECTABLE;
				}
				else if (status == OP || status == BLOCK) {
					connInfo->connectablility[endIndex - i] = DISABLE;
					disable = true;
				}
				else if (status = MY) {
					connInfo->connectablility[endIndex - i] = ALREADY_HAVE;
				}
			}
			else {
				connInfo->connectablility[endIndex - i] = DISABLE;
				disable = true;
			}
		}
		int connCount = 0;
		for (int i = 0; i < CONN_ARR_LEN; i++) {
			if (connInfo->connectablility[i] == CONNECTABLE || connInfo->connectablility[i] == ALREADY_HAVE) {
				connCount++;
			}
		}
		if (connCount >= VALID_CONN) {
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
	for (int i = 1; i <= HALF_EFFECT_RANGE; i++) {
		if (disable == true) {
			connInfo->connectablility[startIndex + i] = DISABLE;
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

			if (i == HALF_EFFECT_RANGE) {
				if (status == MY) {
					connInfo->connectablility[startIndex + i - 1] = DISABLE;
				}
				for (int j = startIndex + i; j < CONN_ARR_LEN; j++) {
					connInfo->connectablility[j] = DISABLE;
				}
				break;
			}

			if (status == EMPTY) {
				connInfo->connectablility[startIndex + i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connectablility[startIndex + i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connectablility[startIndex + i] = ALREADY_HAVE;
			}
		}
		else {
			connInfo->connectablility[startIndex + i] = DISABLE;
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
	
	// 반대 방향 calc
	disable = false;
	for (int i = 1; i <= HALF_EFFECT_RANGE; i++) {
		if (disable == true) {
			connInfo->connectablility[endIndex - i] = DISABLE;
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

			if (i == HALF_EFFECT_RANGE) {
				if (status == MY) {
					connInfo->connectablility[endIndex - i + 1] = DISABLE;
				}
				for (int j = endIndex - i; j >= 0; j--) {
					connInfo->connectablility[j] = DISABLE;
				}
				continue;
			}


			if (status == EMPTY) {
				connInfo->connectablility[endIndex - i] = CONNECTABLE;
			}
			else if (status == OP || status == BLOCK) {
				connInfo->connectablility[endIndex - i] = DISABLE;
				disable = true;
			}
			else if (status = MY) {
				connInfo->connectablility[endIndex - i] = ALREADY_HAVE;
			}
		}
		else {
			connInfo->connectablility[endIndex - i] = DISABLE;
			disable = true;
		}
	}

	int connCount = 0;
	for (int i = 0; i < CONN_ARR_LEN; i++) {
		if (connInfo->connectablility[i] == CONNECTABLE) {
			connCount++;
		}
	}
	if (connCount >= VALID_CONN) {
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
	for (int i = 1; i < VALID_CONN; i++) {
		set<ConnectionInfo*> connInfoSet = myConnInfo[i];

		set<ConnectionInfo*>::iterator iter = connInfoSet.begin();
		for (iter; iter != connInfoSet.end(); ++iter) {
			ConnectionInfo* conn = *iter;
			Direction dir = conn->dir;

			int startX = conn->connStart.first;
			int startY = conn->connStart.second;
			for (int i = 0; i < CONN_ARR_LEN; i++) {
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
					if (turnBoard[curX][curY] == EMPTY && conn->connectablility[i] == CONNECTABLE) {
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
