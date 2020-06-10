#include "Judge.h"
#include "Point.h"
#include <iostream>
//#include <conio.h>
#include <math.h>
#include <stdlib.h>
//#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAX_M 12 //M:行数
#define MAX_N 12 //N:列数

#define oppo 1
#define me 2

struct Node {
	//bool can_expand; //是否可以拓展
	//bool avail; //是否可以落子
	bool expanded[MAX_N]; //子节点是否已拓展
	bool can_expand[MAX_N]; //子节点是否可以拓展
	int player; // 1:oppo 2:me
	int winner; // 0:not finish; 1:oppo win; 2: me win
	//int id;
	int parent_id;
	int avail_child_num;
	int childs_id[MAX_N];
	int top[MAX_N];
	//int board[MAX_M][MAX_N];
	double visit_num;
	double total_reward;
	bool gameover; //游戏结束
	//相比于父节点，多下了哪个子？
	int newX;
	int newY;
	Node() : player(0), winner(0), parent_id(-1), avail_child_num(0), visit_num(0), total_reward(0), gameover(0), newX(-1), newY(-1)
	{
		for (int i = 0; i < MAX_N; i++) {
			childs_id[i] = -1;
			expanded[i] = false;
			can_expand[i] = false;
		}
		//memset(board, 0, sizeof(board));
		memset(top, 0, sizeof(top));
	}
	// double I;

};

class UCT {
	int size;
	int M, N;
	Node* nodes;
	int** currentBoard;
	int* currentTop;
	int** origBoard;
	int* origTop;
	double c;
	int lastX, lastY, noX, noY;
	clock_t start_t, end_t;

public:
	UCT(const int M, const int N, int player, double c, int** Board, const int* Top, const int noX, const int noY, const int lastX, const int lastY);
	~UCT();
	int Expand(int node_id, int i);
	int BestChild(int node_id, double c);
	double DefaultPolicy(int node_id);
	void UpdateCurrentBoard(int node_id);
	void Backup(int node_id, double delta);
	int TreePolicy(int node_id);
	Point UCTSearch();
	Point OneStepWin();
	void PrintNodes();
};