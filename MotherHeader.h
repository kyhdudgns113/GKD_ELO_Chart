/*
*	프로그램 전반에 걸쳐서 사용할 상수등을 정의한다.
*/
#pragma once
#pragma warning(disable:4996)	//	printf, scanf
#pragma warning(disable:6031)

#define NULL_STRING			"NULL"
const char STRING_SCORE_ENTIRE[] = "entire_score.txt";
const char STRING_SCORE_CARD[] = "card_score.txt";
const char STRING_RECORD_ENTIRE[] = "entire_record.txt";
const char STRING_NAME_LIST[] = "name_list.txt";
const char STRING_CARD_LIST[] = "card_list.txt";
const char STRING_DIR_DB[] = 	"Database";
const char STRING_DIR_DECK_DB[] = 	"Deck_DB";
const char STRING_DIR_RESULT[] = "Result";

#define CNT_CARD			224

#define GKD_ELO_BASE		2000
#define GKD_ELO_RATE_BASE	1000

//	GKD_ELO_DELTA[i] : i 차이일때의 기본 점수
//	i == 0 : 비겼을때의 기본 점수
const int GKD_ELO_DELTA[] = { 0, 15, 16, 17, 18, 19, 20 };


#define ID_BASE_GRASS		1000
#define ID_BASE_FIRE		2000
#define ID_BASE_WATER		3000
#define ID_BASE_LIGHTNING	4000
#define ID_BASE_FIGHT		5000
#define ID_BASE_PSYCHIC		6000
#define ID_BASE_NORMAL		7000
#define ID_BASE_BOSS		8000
#define ID_BASE_NPC			9000

#define ITERATOR_NAME		std::list<std::pair<int, std::string>>::iterator
#define ITERATOR_RECORD		std::list<NODE_Record_Entire>::iterator

#define SSTRING				std::string

typedef struct gkd_node_birth  {
	int year = 2019;
	int month = 1;
	int day = 1;
	int hour = 0;
	int min = 0;
}NODE_BIRTH;

typedef struct gkd_node_printed_row {
	char name[32];
	int id;
	double elo;
	double win_rate;
}NODE_PRINTED_ROW;


bool cmp_row_elo(NODE_PRINTED_ROW a, NODE_PRINTED_ROW b);