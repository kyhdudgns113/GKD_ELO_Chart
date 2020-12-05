#include "List_Name.h"

using namespace std;

//
//	입력 : id, 이름
//	출력 : 정상작동 : 들어있는 이름의 수
//		   -1 : 해당 ID 가 이미 존재함
//	기능 : (id, 이름) 으로 구성된 정보를 리스트에 넣는다.
//			id 가 작은 순서대로 들어가게 한다.
//	
//
int List_Name::insert_name(int _id, string _name) {
	auto it = this->name_list.begin();

	int err = 0;

	while (it != this->name_list.end()) {
		if (it->first == _id) {
			err = -1;
			break;
		}
		else if (it->first > _id)	//	현재 위치에 넣으면 된다.
			break;

		it++;
	}
	if (err) {
		return -1;
	}

	this->name_list.insert(it, pair<int, string>(_id, _name));

	return this->name_list.size();
}

//
//	입력 : id
//	출력 : 리스트에 있을 때 : id 에 해당하는 이름
//		   리스트에 없을 때 : NULL_STRING
//	
string List_Name::find_name(int _id) {
	auto it = this->name_list.begin();

	while (it != this->name_list.end()) {
		if (it->first == _id)
			break;

		it++;
	}

	if (it == this->name_list.end()) 
		return NULL_STRING;

	return it->second;
}

//
//	입력 : 이름
//	출력 : 리스트에 있을 때 : 이름의 id 넘버
//		   리스트에 없을 때 : 0
//
int List_Name::find_id(string name) {
	auto it = this->name_list.begin();

	while (it != this->name_list.end()) {
		if (it->second == name)
			break;

		it++;
	}

	if (it == this->name_list.end())
		return 0;

	return it->first;
}

//	입력 : id
//	출력 : 해당 id 가 이름 리스트에 존재하는지 여부
bool List_Name::isExist_id(int id) {
	auto it = this->name_list.begin();

	while (it != this->name_list.end()) {
		if (it->first == id)
			return true;

		it++;
	}
	return false;
}

bool List_Name::isExist_name(string name) {
	auto it = this->name_list.begin();

	while (it != this->name_list.end()) {
		if (it->second == name)
			return true;

		it++;
	}
	return false;
}

//
//	_before : 리스트에 존재하는지 여기에서 다시 확인을 한다.
//	_after : 타당한 이름인지 여기서는 확인하지 앟ㄴ는다.
//
bool List_Name::modify_name(string _before, string _after) {
	list<pair<int, string>>::iterator it;


	it = this->name_list.begin();

	while (it != this->name_list.end()) {
		if (it->second == _before)
			break;
		it++;
	}

	if (it == this->name_list.end())
		return false;

	it->second = _after;

	return true;
}