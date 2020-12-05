#pragma once

#include <unordered_map>
#include <vector>

#include "List_Record_Entire.h"
#include "List_Name.h"
#include "NODE_DECK_ROW.h"
#include "NODE_CARD_Score.h"
#include "NODE_Modified_Deck.h"


class GKD_ELO_Chart {
	int tw[7], td, tl[7];
	bool isFiled = false;
public :
	std::unordered_map<int, NODE_DECK_ROW> deck_row;
	List_Name list_name;	//	Tail 에 넣어야 한다.
	NODE_CARD_Score card_score[CNT_CARD][CNT_CARD];
	std::string card_name[CNT_CARD];
	std::list<NODE_Record_Entire> record_list;	//	Tail 에 넣어야 한다.
	std::list<NODE_Modified_Deck> modified_list;

	GKD_ELO_Chart();
	~GKD_ELO_Chart();

	int read_name_list();
	int write_name_list();
	void write_each_score();
	void write_entire_score();
	void write_each_record(NODE_Record_Entire);
	void write_entire_record();
	void write_modified_deck_name();
	void write_record_node(FILE*, NODE_Record_Entire);
	void set_t_zero();
	int insert_new_deck(std::string);
	int insert_saved_deck(int, std::string);
	int get_battle(std::string, std::string, int);

	std::pair<int, std::string> convert_name(std::string);	
	std::string find_name_with_id(int);
	std::string find_name_with_input_string(std::string);
	int print_color_deck_name_return_length(int, int);
	int print_color_deck_name_return_length(std::string, int);
	int print_color_deck_number(int);
	void print_insert_new_deck_error(int, std::string);
	std::tuple<int*, int, int*> return_total_score_id(int);
	std::tuple<int*, int, int*> get_total_score_id(int);
	double return_win_rate(int);
	double get_win_rate(int);
	int return_tot_win(int);
	int get_tot_win(int);
	int return_tot_lose(int);
	int get_tot_lose(int);
	int return_tot_draw(int);
	int get_tot_draw(int);

	void mode_1_read_file();
	void mode_2_write_file();
	void mode_3_add_deck();
	void mode_4_get_battle();
	void mode_5_modify_name();
	void mode_33_print_all_row_id();
	void mode_34_print_all_row_elo();
	void mode_41_print_id_all_col();
	void mode_51_print_grouping();

};