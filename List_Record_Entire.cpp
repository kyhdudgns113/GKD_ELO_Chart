#include "List_Record_Entire.h"

List_Record_Entire::List_Record_Entire() {}
List_Record_Entire::~List_Record_Entire() {}

//
//	�Է� : ���� ����
//			- ��(-2000), ��, ��,
//			- �� �� ��
//			- ID, ���(WL), �������
//			- ��, ��ȭ, �� ELO
//	��� : ����� �������� ����
//
int List_Record_Entire::insert_record(int r_year, int r_month, int r_date,
	int r_hour, int r_min, int r_id[], char r_result[][3], int r_win_draw_win[],
	double r_elo_prev[], double r_elo_delta[], double r_elo_later[]) {

	NODE_Record_Entire temp_node(r_year, r_month, r_date, r_hour, r_min, r_id, r_result, r_win_draw_win, r_elo_prev, r_elo_delta, r_elo_later);

	this->record_list.push_back(temp_node);

	return this->record_list.size();
}
