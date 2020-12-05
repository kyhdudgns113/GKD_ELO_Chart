#pragma once
#include "MotherHeader.h"
#include <string>

class NODE_Modified_Deck {
public:
	NODE_BIRTH birth;
	int id = 0;
	std::string new_name;

	NODE_Modified_Deck() {}
	NODE_Modified_Deck(int _id, std::string _new_name);
	NODE_Modified_Deck(NODE_BIRTH _birth, int _id, std::string _new_name);
	~NODE_Modified_Deck() {}

	NODE_Modified_Deck operator=(NODE_Modified_Deck& _node);

	void set_birth();
	void set_birth(NODE_BIRTH _node);
	void set_id(int _id);
	void set_name(std::string _new_name);

	NODE_BIRTH return_birth();
	int return_id();
	std::string return_name();
};

