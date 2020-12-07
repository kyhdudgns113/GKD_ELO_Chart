#include "NODE_Score.h"
#include <string.h>

using namespace std;

NODE_Score::NODE_Score() {
	for (int i = 0; i < 7; i++) {
		this->win[i] = 0;
		this->lose[i] = 0;
	}
	this->draw = 0;
}

NODE_Score::NODE_Score(int win[], int draw, int lose[]) {
	this->set_score(win, draw, lose);
}

NODE_Score::~NODE_Score(){
}

int NODE_Score::sum_win() {
	int ret = 0;
	for (int i = 0; i < 7; i++)
		ret += this->win[i];
	return ret;
}

int NODE_Score::sum_lose() {
	int ret = 0;
	for (int i = 0; i < 7; i++)
		ret += this->lose[i];
	return ret;
}

int NODE_Score::sum_total() {
	int ret = 0;
	for (int i = 0; i < 7; i++) {
		ret += this->win[i];
		ret += this->lose[i];
	}
	ret += draw;
	return ret;
}

void NODE_Score::set_zero() {
	for (int i = 0; i < 7; i++) {
		this->win[i] = 0;
		this->lose[i] = 0;
	}
	this->draw = 0;
}

void NODE_Score::set_score(int win[], int draw, int lose[]) {
	for (int i = 0; i < 7; i++) {
		this->win[i] = win[i];
		this->lose[i] = lose[i];
	}
	this->draw = draw;
}

//	포인터를 리턴하므로, 리턴받은 값을 수정하지 말자.
tuple<int*, int, int*> NODE_Score::return_score() {
	tuple<int*, int, int*> ret = make_tuple(this->win, this->draw, this->lose);
	return ret;
}


double NODE_Score::rate_win_lose() {
	int wins = this->sum_win();
	int loses = this->sum_lose();

	if (wins + loses == 0)
		return 0;
	else
		return (double)wins / (double)(wins + loses) * 100;
}

void NODE_Score::_debug_print_score() {
	printf("DEBUG_PRINT_NODE_SCORE\n");
	for (int i = 6; i >= 1; i--) 
		printf("%d ", this->win[i]);

	printf("%d ", this->draw);

	for (int i = 1; i <= 6; i++)
		printf("%d ", this->lose[i]);
	printf("\n");
}

void NODE_Score::increase_win(int _arg) {
	if (_arg < 1 || _arg > 6) {
		printf("(Node_Score) 잘못된 _arg %d 가 입력되었다. \n", _arg);
		return;
	}
	this->win[_arg]++;
}

void NODE_Score::increase_draw() {
	this->draw++;
}

void NODE_Score::increase_lose(int _arg) {
	if (_arg < 1 || _arg > 6) {
		printf("(Node_Score) 잘못된 _arg %d 가 입력되었다. \n", _arg);
		return;
	}
	this->lose[_arg]++;
}