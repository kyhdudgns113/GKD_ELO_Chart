/*
*	n*n 만큼 있게될 노드이다.
*	승/패 시 잔여장수를 고려한다.
*	나중에 바이너리로 저장하는것을 고려하여 데이터 크기를 최대한 줄인다.
*/
#include "MotherHeader.h"
#include <stdio.h>
#include <tuple>
#pragma once

class NODE_Score {
public:
	
	int			win[7] = { 0, };
	int			draw = 0;
	int			lose[7] = { 0, };

	NODE_Score();
	NODE_Score(int[], int, int[]);
	~NODE_Score();

	int sum_win();
	int sum_lose();
	int sum_total();

	void set_zero();
	void set_score(int[], int, int[]);
	std::tuple<int*, int, int*> return_score();

	double rate_win_lose();

	void _debug_print_score();
};


