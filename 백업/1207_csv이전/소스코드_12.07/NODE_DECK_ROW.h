/*
*	차트의 ROW 의 한 원소이다.
*/
#pragma once
#include <unordered_map>
#include <string>
#include "NODE_Score.h"

class NODE_DECK_ROW {

public :
	std::string		deck_name;
	int				id;
	double			elo;
	int				form[CNT_CARD];
	std::unordered_map<int, NODE_Score> score_map;	//	id 를 key 로 쓴다.
	NODE_BIRTH		birth;	//	파일 쓰기시 기본 파일을 생성하려면 출생시간을 저장해놔야한다.

	NODE_DECK_ROW();
	NODE_DECK_ROW(int, std::string);
	~NODE_DECK_ROW();

	NODE_DECK_ROW& operator = (NODE_DECK_ROW& node);

	int insert_column(int,int[],int,int[]);	//	기존의 정보를 로드할 때 사용한다.
	int insert_new_column(int);	//	새로 추가할 때 사용한다.

	void insert_deck_form(int[CNT_CARD]);


};

