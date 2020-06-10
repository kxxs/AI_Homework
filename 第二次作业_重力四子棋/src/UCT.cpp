#include "UCT.h"


int max_size = 2500000;
double time_limit = 2.5;


UCT::UCT(const int M, const int N, int player, double c, int** Board, const int* Top, const int noX, const int noY, const int lastX, const int lastY) : M(M), N(N), c(c), lastX(lastX), lastY(lastY), noX(noX), noY(noY), start_t(clock()), end_t(clock()) {
	nodes = new Node[max_size];
	//cout << "start to init uct!!!" << endl;
	origBoard = new int* [M];
	for (int i = 0; i < M; i++) {
		origBoard[i] = new int[N];
		for (int j = 0; j < N; j++) {
			origBoard[i][j] = Board[i][j];
		}
	}
	origTop = new int[N];
	for (int j = 0; j < N; j++) {
		origTop[j] = Top[j];
	}

	currentBoard = new int* [M];
	for (int i = 0; i < M; i++) {
		currentBoard[i] = new int[N];
		for (int j = 0; j < N; j++) {
			currentBoard[i][j] = Board[i][j];
		}
	}
	currentTop = new int[N];
	for (int j = 0; j < N; j++) {
		currentTop[j] = Top[j];
	}



	size = 1;
	nodes[0].parent_id = -1;
	nodes[0].player = player;
	//nodes[0].avail = true;
	nodes[0].avail_child_num = 0;
	nodes[0].total_reward = 0;


	// 更新top
	for (int j = 0; j < N; j++) {
		nodes[0].top[j] = Top[j];
	}

	/*
	// 更新board
	for (int j = 0; j < M; j++) {
		for (int k = 0; k < N; k++) {
			nodes[0].board[j][k] = Board[j][k];
		}
	}
	*/

	/*cout << "node 0 "  << " board:" << endl;
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			cout << nodes[0].board[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;*/

	// 更新winner
	nodes[0].winner = 0;


	//更新expanded
	for (int i = 0; i < N; i++) {
		nodes[0].childs_id[i] = -1;
		nodes[0].expanded[i] = false;
	}

	//更新visit_num
	nodes[0].visit_num = 0;

	nodes[0].newX = -1;
	nodes[0].newY = -1;

	nodes[0].gameover = false;


};

UCT::~UCT() {
	/*
	for (int k = 0; k < size; k++) {
		for (int i = 0; i < M; i++) {
			delete[] nodes[k].board[i];
		}
		delete[] nodes[k].board;
		delete[] nodes[k].top;
	}
	*/
	delete[] nodes;

	for (int i = 0; i < M; i++) {
		delete[] currentBoard[i];
		delete[] origBoard[i];
	}
	delete[] currentBoard;
	delete[] currentTop;
	delete[] origBoard;
	delete[] origTop;
}

void UCT::UpdateCurrentBoard(int node_id) {
	//cout << "UCT board updating" << endl;
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			//cout << "board ij: " << currentBoard[i][j] << endl;
			currentBoard[i][j] = origBoard[i][j];
		}
	}
	//cout << "UCT board updatinging" << endl;
	for (int i = 0; i < N; i++) {
		currentTop[i] = nodes[node_id].top[i];
	}

	while (node_id > 0) {
		currentBoard[nodes[node_id].newX][nodes[node_id].newY] = nodes[node_id].player;
		node_id = nodes[node_id].parent_id;
	}

	//cout << "UCT board updated" << endl;
}

int UCT::BestChild(int node_id, double c) {
	int arg_maxr = -1;
	double max_reward = -1e5;
	//assert(nodes[node_id].visit_num > 0);
	//if (nodes[node_id].visit_num < 1) cout << "error in line 137" << endl;

	double log_parent_vis_num = 2 * log(nodes[node_id].visit_num);
	int child_id = 0;
	for (int i = 0; i < nodes[node_id].avail_child_num; i++) {
		child_id = nodes[node_id].childs_id[i];
		double child_vis_num = nodes[child_id].visit_num;
		//assert(child_vis_num > 0);
		//if (child_vis_num < 1) cout << "error in line 147" << endl;
		double reward_i = nodes[child_id].total_reward / child_vis_num + c * sqrt(log_parent_vis_num / child_vis_num);


		//如果是oppo则最小化reward
		/* //在计算total_reward的时候已经考虑到取负数了
		if (nodes[child_id].player == oppo)
			reward_i = -reward_i;
		*/
		if (reward_i > max_reward) {
			max_reward = reward_i;
			arg_maxr = i;
		}
	}
	//assert(arg_maxr > -1);
	if (arg_maxr < 0) return 0;
	//if (arg_maxr < 0)  cout << "error in 162" << endl;
	return arg_maxr;
}

int UCT::Expand(int node_id, int i) {
	/*
	if (nodes[node_id].expanded[i])
		continue;

	//不可落子，则跳过
	if (nodes[node_id].top[i] < 1) {
		nodes[node_id].expanded[i] = true;
		continue;
	}
	*/

	//可以落子，则扩充
	nodes[node_id].expanded[i] = true;
	int child_id = size++;
	nodes[node_id].childs_id[nodes[node_id].avail_child_num++] = child_id;
	nodes[child_id].parent_id = node_id;

	// 更新子节点newX, newY
	nodes[child_id].newY = i;
	nodes[child_id].newX = nodes[node_id].top[i] - 1;

	// 更新子节点player
	if (nodes[node_id].player == me) {
		nodes[child_id].player = oppo;
	}
	else {
		nodes[child_id].player = me;
	}

	// 更新子节点board
	/*
	for (int j = 0; j < M; j++) {
		for (int k = 0; k < N; k++) {
			nodes[child_id].board[j][k] = nodes[node_id].board[j][k];
		}
	}
	nodes[child_id].board[nodes[node_id].top[i] - 1][i] = nodes[child_id].player;
	*/

	/*
	cout << "node " << child_id << " board:" << endl;
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			cout << nodes[child_id].board[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
	*/

	// 更新子节点top
	for (int j = 0; j < N; j++) {
		nodes[child_id].top[j] = nodes[node_id].top[j];
	}
	nodes[child_id].top[i]--;
	// TODO:noX, noY有点搞混了?
	if (noY == i) {
		if (nodes[child_id].top[i] - 1 == noX) {
			nodes[child_id].top[i]--;
		}
	}


	// 更新子节点winner
	nodes[child_id].winner = 0;
	nodes[child_id].gameover = false;
	nodes[child_id].total_reward = 0;
	//TODO
	UpdateCurrentBoard(child_id);
	if (nodes[child_id].player == me) {
		//TODO:这个board参数是下棋之前还是之后？答：之后
		if (machineWin(nodes[node_id].top[i] - 1, i, M, N, currentBoard)) {
			nodes[child_id].winner = me;
			nodes[child_id].gameover = true;
			nodes[child_id].total_reward = 1;
		}
	}
	else if (nodes[child_id].player == oppo) {
		if (userWin(nodes[node_id].top[i] - 1, i, M, N, currentBoard)) {
			nodes[child_id].winner = oppo;
			nodes[child_id].gameover = true;
			nodes[child_id].total_reward = 1;
		}
	}
	if (nodes[child_id].winner == 0) {
		if (isTie(N, nodes[child_id].top))
			nodes[child_id].gameover = true;
	}


	//更新子节点visit_num
	nodes[child_id].visit_num = 1;
	for (int j = 0; j < N; j++) {
		nodes[child_id].childs_id[j] = -1;
		nodes[child_id].expanded[j] = false;
		nodes[child_id].can_expand[j] = false;
	}

	// 更新avail_child_num
	nodes[child_id].avail_child_num = 0;
	

	return child_id;
}

double UCT::DefaultPolicy(int node_id) {
	if (nodes[node_id].gameover) {
		int winer = nodes[node_id].winner;
		if (winer == me) {
			//cout << "simulate player 1 wins" << endl;
			return 1; //玩家胜，reward=1
		}
		else if (winer == oppo) {
			//cout << "simulate player 2 wins" << endl;
			return -1; //对方胜，reward=-1
		}
		else {
			//cout << "simulate ties" << endl;
			return 0; //平局, reward = 0
		}
	}


	UpdateCurrentBoard(node_id);
	/*
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			cout << currentBoard[i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
	cout << "Default Policy start simulating" << endl;
	*/
	// node_id相比于父节点所多出来的一步
	int new_X = nodes[node_id].newX;
	int new_Y = nodes[node_id].newY;

	int currentPlayer = 3 - nodes[node_id].player;
	//cout << "currentPlayer: " << currentPlayer << endl;
	int pos_list[MAX_N] = { 0 };
	while (true) {
		int avail_pos_num = 0;
		for (int i = 0; i < N; i++) {
			//判断是否可以落子
			if (currentTop[i] < 1) continue;
			pos_list[avail_pos_num++] = i;
		}
		if (avail_pos_num < 1) {
			//cout << "error in line 309" << endl;
			return 0;
		}
		int next_step = pos_list[rand() % avail_pos_num];
		//cout << "next step: " << next_step << endl;
		currentBoard[currentTop[next_step] - 1][next_step] = currentPlayer;

		/*
		cout << "board: " << endl;
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				cout << currentBoard[i][j] << ' ';
			}
			cout << endl;
		}
		cout << endl;
		*/

		if (currentPlayer == oppo) {
			//cout << "top i = " << currentTop[next_step] - 1 << endl;
			if (userWin(currentTop[next_step] - 1, next_step, M, N, currentBoard)) {
				//cout << "userWin!" << endl;
				return -1; //对方胜，reward=-1
			}
		}
		if (currentPlayer == me) {
			//cout << "top i = " << currentTop[next_step] - 1 << endl;
			if (machineWin(currentTop[next_step] - 1, next_step, M, N, currentBoard)) {
				//cout << "machineWin!" << endl;
				return 1; //玩家胜，reward=1
			}
		}
		if (isTie(N, currentTop)) {
			//cout << "simulate ties" << endl;
			return 0; //平局, reward = 0
		}

		if (currentPlayer == me)
			currentPlayer = oppo;
		else currentPlayer = me;

		currentTop[next_step]--;
		if (next_step == noY) {
			if (currentTop[next_step] - 1 == noX) {
				currentTop[next_step]--;
			}
		}

		/*
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				cout << currentBoard[i][j] << ' ';
			}
			cout << endl;
		}
		cout << endl;
		*/
	}

}

void UCT::Backup(int node_id, double delta) {
	while (node_id > -1) {
		nodes[node_id].visit_num++;
		if (nodes[node_id].player == me)
			nodes[node_id].total_reward += delta;
		else
			//如果是AI则加上负的reward
			nodes[node_id].total_reward -= delta;
		node_id = nodes[node_id].parent_id;
	}
}

int UCT::TreePolicy(int node_id) {
	while (!nodes[node_id].gameover) {
		/*
		if (node_id == 3)
			cout << "visiting node 3" << endl;
		*/

		for (int i = 0; i < N; i++) {
			if (nodes[node_id].expanded[i])
				continue;

			//不可落子，则跳过
			if (nodes[node_id].top[i] < 1) {
				nodes[node_id].expanded[i] = true;
				continue;
			}

			//可以落子且尚未expand
			nodes[node_id].can_expand[i] = true;
			return Expand(node_id, i);
		}

		int bstChild = BestChild(node_id, this->c);
		/*
		if (node_id == 0)
			cout << "BestChild: " << bstChild << endl;
		*/
		//assert(bstChild > -1);
		//if (bstChild < 0) cout << "error in line 408" << endl;
		node_id = nodes[node_id].childs_id[bstChild];

	}
	return node_id;
}


Point UCT::OneStepWin() {


	for (int i = 0; i < N; i++) {
		if (currentTop[i] < 1) continue;
		currentBoard[currentTop[i] - 1][i] = 1;
		if (userWin(currentTop[i] - 1, i, M, N, currentBoard)) {
			return Point(currentTop[i] - 1, i);
		}
		currentBoard[currentTop[i] - 1][i] = 2;
		if (machineWin(currentTop[i] - 1, i, M, N, currentBoard)) {
			return Point(currentTop[i] - 1, i);
		}
		currentBoard[currentTop[i] - 1][i] = 0;
	}
	return Point(-1, -1);

}

Point UCT::UCTSearch() {
	double iter_t;
	double total_t = 0;
	int iter_num = 0;
	int node_id;
	double delta;
	while (total_t < time_limit && size < max_size - 100) {
		node_id = TreePolicy(0);
		//cout << "TreePolicy giving node" << node_id << endl;
		delta = DefaultPolicy(node_id);
		//cout << "Default policy finished" << node_id << endl;
		Backup(node_id, delta);

		end_t = clock();
		total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		//cout << "iter " << iter_num << " using time(s): " << iter_t << endl;
		//cout << "total time: " << total_t << endl;
		iter_num++;

	}
	
	/*
	if (total_t > time_limit - 0.05)
		cout << "time_lim" << endl;
	if (size > max_size - 110)
		cout << "size_lim" << endl;
	*/
		
	
	int bstChild = BestChild(0, 0);
	int bst_id = nodes[0].childs_id[bstChild];
	for (int i = 0; i < N; i++) {
		if (nodes[0].top[i] != nodes[bst_id].top[i])
			return Point(nodes[0].top[i] - 1, i);
	}
	return Point(0, 0);
}

void UCT::PrintNodes() {
	cout << "nodes num = " << size << endl;
	for (int k = 0; k < size; k++) {
		cout << "node " << k << " :" << endl;

		// print player
		cout << "player: " << nodes[k].player << endl << "winner: " << nodes[k].winner << endl << "parent_id: " << nodes[k].parent_id << endl;
		cout << "visit_num: " << nodes[k].visit_num << endl << "ave_reward: " << nodes[k].total_reward / nodes[k].visit_num << endl;
		cout << "newX: " << nodes[k].newX << " , newY:" << nodes[k].newY << endl;
		cout << "childs: ";
		for (int i = 0; i < nodes[k].avail_child_num; i++) {
			cout << nodes[k].childs_id[i] << ' ';
		}

		if (nodes[k].parent_id == 0) {
			double log_parent_vis_num = 2 * log(nodes[0].visit_num);
			cout << "reward 1 for node " << k << " = " << nodes[k].total_reward / nodes[k].visit_num << endl;
			cout << "reward 2 for node " << k << " = " << nodes[k].total_reward / nodes[k].visit_num + c * sqrt(log_parent_vis_num / nodes[k].visit_num) << endl;
		}

		// print expanded
		/*
		cout << "can expand:" << endl;
		for (int i = 0; i < N; i++)
			cout << nodes[k].can_expand[i] << ' ';
		cout << endl;

		cout << "expanded:" << endl;
		for (int i = 0; i < N; i++)
			cout << nodes[k].expanded[i] << ' ';
		cout << endl;
		*/



		// print board
		UpdateCurrentBoard(k);
		cout << "board: " << endl;
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				cout << currentBoard[i][j] << ' ';
			}
			cout << endl;
		}
		cout << endl;
		

		// print top
		cout << "top:" << endl;
		for (int i = 0; i < N; i++)
			cout << nodes[k].top[i] << ' ';
		cout << endl;


		// print gameover
		cout << "gameover: " << nodes[k].gameover << endl << endl;

	}
}