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

//	GKD_ELO_BASE : 시작할때의 ELO
//	GKD_ELO_BASE_RATE : 승기 기대값 10배 인 ELO 차이
//	PREDICT_NUM_GAME : 향후 n 경기후 점수 예측에 사용할 n 값
#define GKD_ELO_BASE		2000
#define GKD_ELO_RATE_BASE	1000
#define PREDICT_NUM_GAME	5

//	GKD_ELO_DELTA[i] : i 차이일때의 기본 점수
//		i == 0 : 비겼을때의 기본 점수
//	GLD_ELO_DELTA_PLUS : 기본 증감 점수, 변화량 = GED[i] + GEDA 
const double GKD_ELO_DELTA[] = { 0, 15, 16.5, 18, 19.5, 21, 22.5};
const double GKD_ELO_DELTA_PLUS[] = {0, 0, 0, 0, 0, 1, 1.5};


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


#define SET_COLOR(a)		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a)
#define COLOR_BLACK			0
#define COLOR_DARK_BLUE		1
#define COLOR_DARK_GREEN	2
#define COLOR_DARK_SKY		3
#define COLOR_DARK_RED		4
#define COLOR_DARK_PINK		5
#define COLOR_DARK_YELLOW	6
#define COLOR_DARK_WHITE	7
#define COLOR_GREY			8
#define COLOR_BLUE			9
#define COLOR_GREEN			10
#define COLOR_SKY			11
#define COLOR_RED			12
#define COLOR_PINK			13
#define COLOR_YELLOW		14
#define COLOR_WHITE			15

#define PRINT_BLANK_BASE	24
#define PRINT_LENGTH_BLANK	1
#define PRINT_ENTER			2

typedef struct gkd_node_birth  {
	int year = 2020;
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