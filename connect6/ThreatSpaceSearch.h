
#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>

#define TRAVERSE_LENGTH 11
#define MAX_CONNECT 6

using namespace std;

void init();

pair<int, int>* threatSpaceSearch(int** board, int x, int y, int attacker);

