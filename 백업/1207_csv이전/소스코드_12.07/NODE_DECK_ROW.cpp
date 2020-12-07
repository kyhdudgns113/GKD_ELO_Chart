#include "NODE_DECK_ROW.h"
#include <time.h>

using namespace std;

NODE_DECK_ROW::NODE_DECK_ROW() {
	this->deck_name = NULL_STRING;
	this->id = 0;
	this->elo = GKD_ELO_BASE;
	for (int i = 0; i < CNT_CARD; i++)
		this->form[i] = 0;
}

NODE_DECK_ROW::NODE_DECK_ROW(int id, string name) {
	this->id = id;
	this->deck_name = name;
	this->elo = GKD_ELO_BASE;
	for (int i = 0; i < CNT_CARD; i++)
		this->form[i] = 0;
}

NODE_DECK_ROW::~NODE_DECK_ROW() {}


//
//	win, draw, lose ������ �޾ƿ´�.
//	scoremap �� ũ�⸦ �����Ѵ�.
//
int NODE_DECK_ROW::insert_column(int id, int win[], int draw, int lose[]) {

	this->score_map[id].set_score(win, draw, lose);
	return this->score_map.size();
}

//
//	�Է� : id
//	��� : ���� ����Ʈ�� �ִ� �̸��� ��
//	��� : �ڽ� vs id �� �ش��ϴ� column node �� �߰��Ѵ�.
//		   ID ������ GKD_ELO_Chart ���� �Ѵ�.
//		   - ID �� Ȯ���ϴ� ���� ��尡 ��������⶧��
//
int NODE_DECK_ROW::insert_new_column(int id) {
	this->score_map[id].set_zero();

	return this->score_map.size();
}

void NODE_DECK_ROW::insert_deck_form(int info[CNT_CARD]) {
	for (int i = 0; i < CNT_CARD; i++)
		this->form[i] = info[i];
}
