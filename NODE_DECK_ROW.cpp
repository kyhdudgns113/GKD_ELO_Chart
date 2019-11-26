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

//	이거 잘 안돌아가
NODE_DECK_ROW& NODE_DECK_ROW::operator = (NODE_DECK_ROW& node) {

	NODE_DECK_ROW ret(node.id, node.deck_name);

	ret.elo = node.elo;
	for (int i = 0; i < CNT_CARD; i++)
		ret.form[i] = node.form[i];

	ret.score_map = node.score_map;

	ret.birth.year = node.birth.year;
	ret.birth.month = node.birth.month;
	ret.birth.day = node.birth.day;
	ret.birth.hour = node.birth.hour;
	ret.birth.min = node.birth.min;

	return ret;
}

//
//	win, draw, lose 정보를 받아온다.
//	scoremap 의 크기를 리턴한다.
//
int NODE_DECK_ROW::insert_column(int id, int win[], int draw, int lose[]) {

	this->score_map[id].set_score(win, draw, lose);
	return this->score_map.size();
}

//
//	입력 : id
//	출력 : 현재 리스트에 있는 이름의 수
//	기능 : 자신 vs id 에 해당하는 column node 를 추가한다.
//		   ID 검증은 GKD_ELO_Chart 에서 한다.
//		   - ID 를 확인하는 순간 노드가 만들어지기때문
//
int NODE_DECK_ROW::insert_new_column(int id) {
	this->score_map[id].set_zero();

	return this->score_map.size();
}

void NODE_DECK_ROW::insert_deck_form(int info[CNT_CARD]) {
	for (int i = 0; i < CNT_CARD; i++)
		this->form[i] = info[i];
}
