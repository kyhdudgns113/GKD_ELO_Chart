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
		printf("1 or : 파일 읽기\n");
		printf("2 or : 파일 쓰기\n");
		printf("3 or : 차트에 원소추가\n");
		printf("4 or : 결과 입력\n");
		printf("33 or : 모든 row 확인, id순\n");
		printf("34 or : 모든 row 확인, elo순\n");
		printf("41 or : 특정 row의 각 col 확인, id순\n");
		printf("-1 : 저장 안하고 종료\n");

		std::cin >> input_order;

		if (input_order == "1") {
			gkd_chart.mode_1_read_file();
		}
		else if (input_order == "2") {
			gkd_chart.mode_2_write_file();
		}
		else if (input_order == "3") {
			gkd_chart.mode_3_add_deck();
		}
		else if (input_order == "4") {
			gkd_chart.mode_4_get_battle();
		}
		else if (input_order == "33") {
			gkd_chart.mode_33_print_all_row_id();
		}
		else if (input_order == "34") {
			gkd_chart.mode_34_print_all_row_elo();
		}
		else if (input_order == "41") {
			gkd_chart.mode_41_print_id_all_col();
		}
		else if (input_order == "-1") {
			printf("Main while 취소 명령을 실행합니다. \n");
			break;
		}
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}
	printf("PROGRAM FINISHED\n");
	return 0;		
}