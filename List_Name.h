#pragma once
#include "MotherHeader.h"
#include <list>
#include <string>

class List_Name {

public :
	//	�����ͺ��� ū�ͱ��� ������� ã�ƾ� �ϱ� ������ ����Ʈ�� �Ѵ�.
	std::list<std::pair<int, std::string>> name_list;

	int insert_name(int, std::string);
	std::string find_name(int);
	int find_id(std::string);
	bool isExist_id(int);
	bool isExist_name(std::string);

	bool modify_name(std::string, std::string);
};

