/*
*	��Ʈ�� ROW �� �� �����̴�.
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
	std::unordered_map<int, NODE_Score> score_map;	//	id �� key �� ����.
	NODE_BIRTH		birth;	//	���� ����� �⺻ ������ �����Ϸ��� ����ð��� �����س����Ѵ�.

	NODE_DECK_ROW();
	NODE_DECK_ROW(int, std::string);
	~NODE_DECK_ROW();

	NODE_DECK_ROW& operator = (NODE_DECK_ROW& node);

	int insert_column(int,int[],int,int[]);	//	������ ������ �ε��� �� ����Ѵ�.
	int insert_new_column(int);	//	���� �߰��� �� ����Ѵ�.

	void insert_deck_form(int[CNT_CARD]);


};

