#pragma warning(disable:4996)
#pragma warning(disable:6031)
#include <iostream>
#include <io.h>	
#include <algorithm>
#include <direct.h>
#include "GKD_ELO_Chart.h"

//using namespace std;


int main() {

	GKD_ELO_Chart gkd_chart;
	std::string input_order;

	while (true) {
		printf("Input order\n");
		printf(" 1 or : ���� �б�\n");
		printf(" 2 or : ���� ����\n");
		printf(" 3 or : ��Ʈ�� �����߰�\n");
		printf(" 4 or : ��� �Է�\n");
		printf(" 5 or : �̸� ����\n");
		printf(" 31 or : ����� row Ȯ��, id��\n");
		printf(" 32 or : ����� row Ȯ��, elo��\n");
		printf(" 33 or : ��� row Ȯ��, id��\n");
		printf(" 34 or : ��� row Ȯ��, elo��\n");
		printf(" 41 or : Ư�� row�� ���� col Ȯ��, id��\n");
		printf(" 42 or : Ư�� row�� ��� col Ȯ��, id��\n");
		printf(" 51 or : �������� ����_������ġ\n");
		printf(" 52 or : �������� ����_ELO �� ��ġ\n");
		printf(" -1 : ���� ���ϰ� ����\n");

		std::cin >> input_order;

		if (input_order == "1")
			gkd_chart.mode_1_read_file();
		else if (input_order == "2")
			gkd_chart.mode_2_write_file();
		else if (input_order == "3")
			gkd_chart.mode_3_add_deck();
		else if (input_order == "4")
			gkd_chart.mode_4_get_battle();
		else if (input_order == "5")
			gkd_chart.mode_5_modify_name();
		else if (input_order == "31")
			gkd_chart.mode_31_print_player_row_id();
		else if (input_order == "32")
			gkd_chart.mode_32_print_player_row_elo();
		else if (input_order == "33")
			gkd_chart.mode_33_print_all_row_id();
		else if (input_order == "34")
			gkd_chart.mode_34_print_all_row_elo();
		else if (input_order == "41")
			gkd_chart.mode_41_print_id_user_col();
		else if (input_order == "42")
			gkd_chart.mode_42_print_id_all_col();
		else if (input_order == "51")
			gkd_chart.mode_51_print_grouping_random();
		else if (input_order == "52")
			gkd_chart.mode_52_print_grouping_elo();
		else if (input_order == "cls" || input_order == "clr")
			system("cls");
		else if (input_order == "-1") {
			printf("Main while ��� ����� �����մϴ�. \n");
			break;
		}
		printf("\n\n\n\n");
	}
	printf("PROGRAM FINISHED\n");
	return 0;		
}