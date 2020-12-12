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
public:
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
	void write_each_record_csv(NODE_Record_Entire);
	void write_entire_record_csv();
	void write_record_node_csv(FILE*, NODE_Record_Entire);
	void write_record_csv_top_title(FILE*);
	void write_modified_deck_name();

	void set_t_zero();
	int insert_new_deck(std::string);
	int insert_saved_deck(int, std::string);
	int get_battle(std::string, std::string, int);
	std::string if_exist_convert_or_create(std::string _input_string);
	std::pair<std::string, int> input_result_and_get_battle(std::string _a, std::string _b);
	std::vector<NODE_PRINTED_ROW> insert_deck_into_vector_by_printed_row(int _npc);

	std::pair<int, std::string> convert_name(std::string);
	std::string find_name_with_id(int);
	std::string find_name_with_input_string(std::string);
	int find_id(std::string);
	void set_text_color_by_type(int id);
	int print_id_name(int id, int mode);
	int print_id_name(std::string, int mode);
	int print_color_deck_name_return_length(int, int);
	int print_color_deck_name_return_length(std::string, int);
	int print_color_deck_number(int, int);
	int print_insert_new_deck_error(int, std::string);
	void print_relative_score_top(int _id);
	void print_relative_score(int _id, int _npc);
	void print_grouping(int npc);
	void print_calculating_score(int _mode);

	std::tuple<int*, int, int*> return_total_score_id(int);
	std::tuple<int*, int, int*> get_total_score_id(int);
	double return_win_rate(int);
	double get_win_rate(int);
	double return_elo(int);
	double get_elo(int);
	double return_elo(std::string);
	double get_elo(std::string);
	int return_tot_win(int);
	int get_tot_win(int);
	int return_tot_lose(int);
	int get_tot_lose(int);
	int return_tot_draw(int);
	int get_tot_draw(int);
	int get_size_include_npc();
	int get_size_exclude_npc();

	void mode_1_read_file();
	void mode_2_write_file();
	void mode_3_add_deck();
	void mode_4_get_battle();
	void mode_5_modify_name();
	void mode_31_print_player_row_id();
	void mode_32_print_player_row_elo();
	void mode_33_print_all_row_id();
	void mode_34_print_all_row_elo();
	void mode_41_print_id_user_col();
	void mode_42_print_id_all_col();
	void mode_51_print_grouping_random();
	void mode_52_print_grouping_elo();
	void mode_61_calculate_final_score_id();
	void mode_62_calculate_final_score_elo();
};