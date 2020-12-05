#include <time.h>
#include "NODE_Modified_Deck.h"

NODE_Modified_Deck::NODE_Modified_Deck(int _id, std::string _new_name) {
	this->set_birth();
	this->set_id(_id);
	this->set_name(_new_name);
}

NODE_Modified_Deck::NODE_Modified_Deck(NODE_BIRTH _birth, int _id, std::string _new_name) {
	this->birth = _birth;
	this->set_id(_id);
	this->set_name(_new_name);
}


NODE_Modified_Deck NODE_Modified_Deck::operator=(NODE_Modified_Deck& _node) {
	this->birth = _node.return_birth();
	this->id = _node.return_id();
	this->new_name = _node.return_name();

	return NODE_Modified_Deck(this->birth, this->id, this->new_name);
}


void NODE_Modified_Deck::set_birth() {
	time_t t = time(NULL);
	struct tm *tt = localtime(&t);

	this->birth.year = tt->tm_year + 1900;
	this->birth.month = tt->tm_mon + 1;
	this->birth.day = tt->tm_mday;
	this->birth.hour = tt->tm_hour;
	this->birth.min = tt->tm_min;
}

void NODE_Modified_Deck::set_birth(NODE_BIRTH _node) {
	this->birth = _node;
}

void NODE_Modified_Deck::set_id(int _id) {
	this->id = _id;
}

void NODE_Modified_Deck::set_name(std::string _new_name) {
	this->new_name = _new_name;
}

NODE_BIRTH NODE_Modified_Deck::return_birth() {
	return this->birth;
}

int NODE_Modified_Deck::return_id() {
	return this->id;
}

std::string NODE_Modified_Deck::return_name() {
	return this->new_name;
}