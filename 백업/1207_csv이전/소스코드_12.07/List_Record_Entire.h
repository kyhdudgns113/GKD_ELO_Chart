/*
*		���� ��� ����Ʈ�̴�.
*		GKD_ELO_Chart ���� ȣ���Ѵ�.
*		-���, ����, �ε带 ���⼭ �ذ��ұ�?
*		������ �����͸� ���;� �ϱ� ������ ���_�����Է��� ���⼭ �� �� ����.
*		�����͸� �״�� ��忡 �ִ°͸� ����
*		�ؽ�Ʈ�� ����Ҷ��� �������� �ڿ� �̾ ��������.
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

