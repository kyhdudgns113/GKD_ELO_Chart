/*
*		전적 기록 리스트이다.
*		GKD_ELO_Chart 에서 호출한다.
*		-기록, 저장, 로드를 여기서 해결할까?
*		서로의 데이터를 얻어와야 하기 때문에 기록_정보입력을 여기서 할 순 없다.
*		데이터를 그대로 노드에 넣는것만 하자
*		텍스트에 기록할때는 기존내용 뒤에 이어서 저장하자.
*/

#pragma once
#include <list>
#include <stdio.h>
#include "NODE_Record_Entire.h"

class List_Record_Entire {

public :
	std::list<NODE_Record_Entire> record_list;

	List_Record_Entire();
	~List_Record_Entire();

	int insert_record(int, int, int, int, int, int[], char[][3], int[], double[], double[], double[]);
};

