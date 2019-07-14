#include "STcpClient.h"
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include <ctime>
#include <cassert>
using namespace std;

typedef pair<int, int> pii;
#define INF -10000

int friendWeight, moveWeight, eatWeight, is_right, ep_weight; // weight for every attribute in heuristic function
int endPoint, endPoint2; //two goal coloum
bool last;//for change color from game1 to game2

struct jump
{
	vector<pii>mov; //this move's action detail
	int cntFriend; // number of same color chess at near to endpoint of this move
	int eat, weat; // eat: this move kill how many oppoment chess ,weat: amount of oppement next best move kill our chess 
	int rnd;
	friend bool operator <(const jump& cmp, const jump& cmp2) { // heuristic function
		return cmp.cntFriend*friendWeight + (cmp.mov[cmp.mov.size() - 1].second - cmp.mov[0].second)*moveWeight + (cmp.eat - cmp.weat)*eatWeight + 
		(((cmp.mov[0].second != endPoint) && (cmp.mov[0].second != endPoint2)) && ((cmp.mov[cmp.mov.size() - 1].second == endPoint) || (cmp.mov[cmp.mov.size() - 1].second == endPoint2))) * ep_weight
	> cmp2.cntFriend*friendWeight + (cmp2.mov[cmp2.mov.size() - 1].second - cmp2.mov[0].second)*moveWeight + (cmp2.eat - cmp2.weat)*eatWeight + 
		(((cmp2.mov[0].second != endPoint) && (cmp2.mov[0].second != endPoint2)) && ((cmp2.mov[cmp2.mov.size() - 1].second == endPoint) || (cmp2.mov[cmp2.mov.size() - 1].second == endPoint2))) * ep_weight;
	}
};


set<pii> chess[2];// chess[0]: black position ,chess[1]:white position
int cnt;
vector<pii> best_mov;
vector<jump>jump_mov;
int best_eat;
int best_weat;
int eaten;

set<pii> origin_black;



void init(bool is_black)//initial and set value for global variable
{
	ep_weight = 10;
	if (cnt < 8) {
		moveWeight = 10;
		eatWeight = 100;
		friendWeight = 1;
	}
	else {
		int rd = rand() & 1;
		if (rd) {
			moveWeight = 12;
			eatWeight = 100;
			friendWeight = 4;
		}
		else {
			moveWeight = 5;
			eatWeight = 100;
			friendWeight = 5;
		}
	}
	if (!is_black)moveWeight *= -1;
	if (is_black) endPoint = 6, endPoint2 = 7;
	else endPoint = 1, endPoint2 = 0;
	origin_black.clear();
	chess[0].clear();
	chess[1].clear();
	best_mov.clear();
	jump_mov.clear();
}


void dfs2(int now, int px, int py, set<pii>stb, set<pii>stw, int eat)
{
	if (now != 0 && best_weat < eat)
	{
		best_weat = eat;
	}
	if (now > 6)return;
	int dx[4] = { 0,0,1,-1 };
	int dy[4] = { 1,-1,0,0 };

	for (int di = 0; di < 4; di++)
	{
		int x1 = px + dx[di];
		int y1 = py + dy[di];
		int x2 = x1 + dx[di];
		int y2 = y1 + dy[di];
		if (stb.count(pii(x1, y1)) && !stb.count(pii(x2, y2)) && !stw.count(pii(x2, y2)) && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8)
		{
			stb.erase(pii(px, py));
			dfs2(now + 1, x2, y2, stb, stw, eat);
			stb.insert(pii(px, py));
		}
		if (stw.count(pii(x1, y1)) && !stb.count(pii(x2, y2)) && !stw.count(pii(x2, y2)) && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8)
		{
			stw.erase(pii(x1, y1));
			dfs2(now + 1, x2, y2, stb, stw, eat + 1);
			stw.insert(pii(x1, y1));
		}
	}
}
void dfs(int now, int px, int py, vector<pii>mov, set<pii>stb, set<pii>stw, int eat) {
	if (now > 8) return;
	int dx[4] = { 0,0,1,-1 };
	int dy[4] = { 1,-1,0,0 };
	if (now != 0)
	{
		best_weat = 0;
		set<pii> rem_white;
		set<pii>rem_black;
		for (auto j : stw)
		{
			rem_white.insert(j);
		}
		for (auto j : origin_black)
		{
			rem_black.insert(j);
		}
		rem_black.insert(pii(px, py));
		for (auto j : stw)
		{
			rem_white.erase(j);
			dfs2(0, j.first, j.second, rem_white, rem_black, 0);
			rem_white.insert(j);
		}
		int diff = eat - best_weat;
		jump add;
		add.mov.assign(mov.begin(), mov.end());
		add.eat = eat;
		add.weat = best_weat;
		add.cntFriend = 0;
		for (int i = 0; i < 4; i++) {
			if (origin_black.find({ px + dx[i],py + dy[i] }) != origin_black.end()) {
				add.cntFriend++;
			}
		}
		add.rnd = rand() % 10;
		jump_mov.push_back(add);
	}

	for (int di = 0; di < 4; di++)
	{
		int x1 = px + dx[di];
		int y1 = py + dy[di];
		int x2 = x1 + dx[di];
		int y2 = y1 + dy[di];
		if (stb.count(pii(x1, y1)) && !stb.count(pii(x2, y2)) && !stw.count(pii(x2, y2)) && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8)
		{
			mov.push_back(pii(x2, y2));
			stb.erase(pii(x1, y1));
			dfs(now + 1, x2, y2, mov, stb, stw, eat);
			stb.insert(pii(x1, y1));
			mov.pop_back();
		}
		if (stw.count(pii(x1, y1)) && !stb.count(pii(x2, y2)) && !stw.count(pii(x2, y2)) && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8)
		{
			mov.push_back(pii(x2, y2));
			stw.erase(pii(x1, y1));
			dfs(now + 1, x2, y2, mov, stb, stw, eat + 1);
			mov.pop_back();
			stw.insert(pii(x1, y1));
		}
	}
}

void dfs3(int now, int px, int py, set<pii> stb, set<pii> stw, int eat)
{
	if (now > 10)return;
	if (now != 0 && eaten < eat) eaten = eat;
	int dx[4] = { 0,0,1,-1 };
	int dy[4] = { 1,-1,0,0 };

	for (int di = 0; di < 4; di++)
	{
		int x1 = px + dx[di];
		int y1 = py + dy[di];
		int x2 = x1 + dx[di];
		int y2 = y1 + dy[di];
		if (stw.count(pii(x1, y1)) && !stb.count(pii(x2, y2)) && !stw.count(pii(x2, y2)) && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8)
		{
			stb.erase(pii(px, py));
			dfs3(now + 1, x2, y2, stb, stw, eat + 1);
			stb.insert(pii(px, py));
		}
		if (stb.count(pii(x1, y1)) && !stb.count(pii(x2, y2)) && !stw.count(pii(x2, y2)) && x2 >= 0 && x2 < 8 && y2 >= 0 && y2 < 8)
		{
			stb.erase(pii(px, py));
			dfs3(now + 1, x2, y2, stb, stw, eat);
			stb.insert(pii(px, py));
		}
	}
}



vector<vector<int> > GetStep(vector<vector<int> >& board, bool is_black)
{
	init(is_black);
	vector<vector<int> > step;

	chess[0].clear();
	chess[1].clear();
	// chess[0]:our chess
	// chess[1]:opponent chess
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (is_black) {
				if (board[i][j] == 1)
				{
					chess[0].insert(pii(i, j));
				}
				else if (board[i][j] == 2)
				{
					chess[1].insert(pii(i, j));
				}
			}
			else {
				if (board[i][j] == 2)
				{
					chess[0].insert(pii(i, j));
				}
				else if (board[i][j] == 1)
				{
					chess[1].insert(pii(i, j));
				}
			}
		}
	}
	for (auto i : chess[0]) { // choose every our chess to be start point and try all jump move
		set<pii>rem_black;
		origin_black.clear();
		for (auto j : chess[0]) {
			if (j != i) {
				rem_black.insert(j);
				origin_black.insert(j);
			}
		}
		vector<pii>mov;
		mov.push_back(pii(i.first, i.second));
		dfs(0, i.first, i.second, mov, rem_black, chess[1], 0);
	}

	for (auto i : chess[0]) // choose every our chess to be start point and try all walk move
	{
		int dx[4] = { 0,0,1,-1 };
		int dy[4] = { 1,-1,0,0 };
		for (int di = 0; di < 4; di++)
		{
			eaten = 0;
			int px = i.first;
			int py = i.second;
			int x1 = px + dx[di];
			int y1 = py + dy[di];
			if (!chess[0].count(pii(x1, y1)) && !chess[1].count(pii(x1, y1)) && x1 < 8 && x1 >= 0 && y1 < 8 && y1 >= 0)
			{
				set<pii> stb;
				for (auto k : chess[0])
				{
					stb.insert(k);
				}
				stb.erase(i);
				stb.insert(pii(x1, y1));
				set<pii> origin_white;
				for (auto j : chess[1])
				{
					origin_white.insert(j);
				}
				for (auto j : chess[1])
				{
					origin_white.erase(j);
					dfs3(0, j.first, j.second, origin_white, stb, 0);
					origin_white.insert(j);
				}
				jump w;
				vector<pii>nv;
				w.mov.push_back(i);
				w.mov.push_back(pii(x1, y1));
				w.cntFriend = 0;
				for (int dj = 0; dj < 4; dj++)
				{
					int xx = x1 + dx[dj];
					int yy = y1 + dy[dj];
					if (xx != px && yy != py && stb.count(pii(xx, yy)))
						w.cntFriend++;
				}
				jump_mov.push_back(w);
			}
		}
	}
	random_shuffle(jump_mov.begin(), jump_mov.end());//for some randomize due to some of may have same value by heuristic function
	sort(jump_mov.begin(), jump_mov.end());
	step.clear();
	for (pii j : jump_mov[0].mov)
	{
		vector<int> www(2);
		www[0] = j.first;
		www[1] = j.second;
		step.push_back(www);
	}
	if (abs(step[0][0] - step[1][0]) + abs(step[0][1] - step[1][1]) == 2) {//check if this action cause terminal state and we will lose or tie
		int bpt = 0;
		int wpt = 0;
		set<pii>tstw, tstb;
		int endf = 1;
		for (auto i : chess[1]) {
			tstw.insert(i);
		}
		for (auto i : chess[0]) {
			tstb.insert(i);
		}
		for (int i = 1; i < step.size(); i++) {
			int jpx = (step[i][0] + step[i - 1][0]) / 2;
			int jpy = (step[i][1] + step[i - 1][1]) / 2;
			if (tstw.find({ jpx,jpy }) != tstw.end()) {
				tstw.erase({ jpx,jpy });
			}
		}
		if (tstw.size()) {
			for (auto i : tstw) {
				if (i.second != 7 - endPoint && i.second != 7 - endPoint2)endf = 0;
			}
			if (endf) {
				for (auto i : tstw) {
					if (i.second == 7 - endPoint || i.second == 7 - endPoint2)wpt++;
				}
				for (auto i : tstb) {
					if (i.second == 7 - endPoint || i.second == 7 - endPoint2)bpt++;
				}
				if (bpt <= wpt) {//if lose or tie and terminal then we will take a jump or walk to avoid it
					vector<vector<int> >ret;
					int dd = -1;
					if (is_black) dd = 1;
					int dist = 100;
					for (auto i : chess[0]) {
						int x = i.first;
						int y = i.second + dd;
						int yy = i.second + 2 * dd;
						if (y > 7 || y < 0)continue;
						if (yy >= 0 && yy <= 7) {
							if (board[x][y] == 1) {
								ret.clear();
								vector<int>nn(2);
								nn[0] = x;
								nn[1] = i.second;
								ret.push_back(nn);
								nn[1] = yy;
								ret.push_back(nn);
								return ret;
							}
						}
						if (board[x][y] == 0 && abs(y - endPoint2) < dist)
						{
							dist = abs(y - endPoint2);
							ret.clear();
							vector<int>nn(2);
							nn[0] = x;
							nn[1] = i.second;
							ret.push_back(nn);
							nn[1] = y;
							ret.push_back(nn);
						}
					}
					return ret;
				}
			}
		}
	}
	return step;
}



int main() {
	srand(time(NULL));
	int id_package;
	std::vector<std::vector<int>> board, step;
	bool is_black;
	cnt = 0;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;
		if (last != is_black) {
			cnt = 0;//count number of move is this game
		}
		last = is_black;
		cnt++;
		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
	return 0;
}
