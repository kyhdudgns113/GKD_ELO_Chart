#include "List_Name.h"

using namespace std;

//
//	�Է� : id, �̸�
//	��� : �����۵� : ����ִ� �̸��� ��
//		   -1 : �ش� ID �� �̹� ������
//	��� : (id, �̸�) ���� ������ ������ ����Ʈ�� �ִ´�.
//			id �� ���� ������� ���� �Ѵ�.
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
		else if (it->first > _id)	//	���� ��ġ�� ������ �ȴ�.
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
//	�Է� : id
//	��� : ����Ʈ�� ���� �� : id �� �ش��ϴ� �̸�
//		   ����Ʈ�� ���� �� : NULL_STRING
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
//	�Է� : �̸�
//	��� : ����Ʈ�� ���� �� : �̸��� id �ѹ�
//		   ����Ʈ�� ���� �� : 0
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

//	�Է� : id
//	��� : �ش� id �� �̸� ����Ʈ�� �����ϴ��� ����
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
//	_before : ����Ʈ�� �����ϴ��� ���⿡�� �ٽ� Ȯ���� �Ѵ�.
//	_after : Ÿ���� �̸����� ���⼭�� Ȯ������ �ۤ��´�.
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