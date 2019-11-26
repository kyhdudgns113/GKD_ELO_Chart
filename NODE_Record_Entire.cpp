#include "NODE_Record_Entire.h"
#include <string.h>

NODE_Record_Entire::NODE_Record_Entire() {}

NODE_Record_Entire::~NODE_Record_Entire() {}

NODE_Record_Entire::NODE_Record_Entire(int r_year, int r_month, int r_date,
	int r_hour, int r_min, int r_id[], char r_result[][3], int r_win_draw_win[],
	double r_elo_prev[], double r_elo_delta[], double r_elo_later[]) {

	this->r_year = r_year;
	this->r_month = r_month;
	this->r_date = r_date;
	this->r_hour = r_hour;
	this->r_min = r_min;
	memcpy(this->r_id, r_id, sizeof(int) * 2);
	strcpy(this->r_result[0], r_result[0]);	//	두 string 이 다른 메모리에 있을 수 있다.
	strcpy(this->r_result[1], r_result[1]);
	memcpy(this->r_win_draw_win, r_win_draw_win, sizeof(int) * 3);
	memcpy(this->r_elo_prev, r_elo_prev, sizeof(double) * 2);
	memcpy(this->r_elo_delta, r_elo_delta, sizeof(double) * 2);
	memcpy(this->r_elo_later, r_elo_later, sizeof(double) * 2);
}