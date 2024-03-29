/*
*	프로그램 전반에 걸쳐서 사용할 상수등을 정의한다.
*/
#pragma once
#pragma warning(disable:4996)	//	printf, scanf
#pragma warning(disable:6031)
#pragma warning(disable:4018)	//	sign, unsigned
#pragma warning(disable:6385)

#define NULL_STRING			"NULL"
const char STRING_SCORE_ENTIRE[] = "entire_score.txt";
const char STRING_SCORE_CARD[] = "card_score.txt";
const char STRING_RECORD_ENTIRE[] = "entire_record.txt";
const char STRING_RECORD_ENTIRE_CSV[] = "entire_record.csv";
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
#define GKD_ELO_A			4
#define GKD_ELO_B			0
#define GKD_ELO_COEFFICIENT(x)	COEF_CALCULATE_SQRT_16(x)

//	{sqrt(16+ELO_B) + ELO_A} / {sqrt(x+ELO_B) + ELO_A}
#define COEF_CALCULATE_SQRT_16(x)	((sqrt(16 + GKD_ELO_B) + GKD_ELO_A)/(sqrt(x + GKD_ELO_B) + GKD_ELO_A))

//	GKD_ELO_DELTA[i] : i 차이일때의 기본 점수
//		i == 0 : 비겼을때의 기본 점수
//	GLD_ELO_DELTA_PLUS : 기본 증감 점수, 변화량 = GED[i] + GEDA 
const double GKD_ELO_DELTA[] = { 0, 15, 16.5, 18, 19.5, 21, 22.5};
const double GKD_ELO_DELTA_PLUS[] = {0, 0.1, 0.2, 0.3, 0.5, 0.8, 1.2};


#define ID_BASE_GRASS		1000
#define ID_BASE_FIRE		2000
#define ID_BASE_WATER		3000
#define ID_BASE_LIGHTNING	4000
#define ID_BASE_FIGHT		5000
#define ID_BASE_PSYCHIC		6000
#define ID_BASE_NORMAL		7000
#define ID_BASE_BOSS		8000
#define ID_BASE_NPC			9000

#define GET_TYPE_DECK(a)	((int)(a/1000))
#define GET_DECK_TYPE(a)	GET_TYPE_DECK(a)

#define ITERATOR_NAME		std::list<std::pair<int, std::string>>::iterator
#define ITERATOR_RECORD		std::list<NODE_Record_Entire>::iterator

#define SSTRING				std::string


#define SET_COLOR(a)		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a)
#define COLOR_BLACK			0
#define COLOR_DARK_BLUE		1
#define COLOR_DARK_GREEN	2
#define COLOR_DARK_SKY		3
#define COLOR_DARK_RED		4//
#define COLOR_DARK_PINK		5
#define COLOR_DARK_YELLOW	6//
#define COLOR_DARK_WHITE	7//
#define COLOR_GREY			8
#define COLOR_BLUE			9//
#define COLOR_GREEN			10//
#define COLOR_SKY			11
#define COLOR_RED			12
#define COLOR_PINK			13//
#define COLOR_YELLOW		14//
#define COLOR_WHITE			15//

#define COLOR_DECK_GRASS		COLOR_GREEN
#define COLOR_DECK_FIRE			COLOR_DARK_RED
#define COLOR_DECK_WATER		COLOR_BLUE
#define COLOR_DECK_LIGHTNING	COLOR_YELLOW
#define COLOR_DECK_FIGHT		COLOR_DARK_YELLOW
#define COLOR_DECK_PSYCHIC		COLOR_PINK
#define COLOR_DECK_NORMAL		COLOR_WHITE
#define COLOR_DECK_BOSS			COLOR_SKY
#define COLOR_DECK_NPC			COLOR_GREY

#define PRINT_BLANK_BASE	24
#define PRINT_AFTER_LENGTH_BLANK	1
#define PRINT_AFTER_ENTER			2
#define PRINT_NAME_NAME				4
#define PRINT_ID_ID					8
#define PRINT_ID_AFTER_COMMA		16
#define PRINT_PREV_BLANK			32
#define PRINT_PREV_TAB				64


#define PRINT_INCLUDE_NPC	1
#define PRINT_EXCLUDE_NPC	0

#define GROUPING_RANDOM		51
#define GROUPING_ELO		52

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
void flush_input_buffer();