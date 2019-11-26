/*
*	n*n ��ŭ �ְԵ� ����̴�.
*	��/�� �� �ܿ������ ����Ѵ�.
*	���߿� ���̳ʸ��� �����ϴ°��� ����Ͽ� ������ ũ�⸦ �ִ��� ���δ�.
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


