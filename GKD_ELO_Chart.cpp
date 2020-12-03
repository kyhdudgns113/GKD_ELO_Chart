#include "GKD_ELO_Chart.h"

#include <string>
#include <algorithm>
#include <io.h>
#include <iostream>
#include <vector>
#include <direct.h>
#include <random>
#include <time.h>
#include <Windows.h>

bool cmp_name_list(std::pair<int, std::string> _a, std::pair<int, std::string> _b) {
	return _a.first >= _b.first;
}

GKD_ELO_Chart::GKD_ELO_Chart() {
	FILE* fp = fopen(STRING_CARD_LIST, "rt");
	char sbuf[64] = { 0, };
	int ibuf = 0;
	for (int i = 0; i < CNT_CARD; i++) {
		fscanf(fp, " %s %d", sbuf, &ibuf);
		this->card_name[i] = sbuf;
	}
	this->set_t_zero();
	fclose(fp);
}
GKD_ELO_Chart::~GKD_ELO_Chart() {}

//  (완)
//	이름 리스트 파일을 읽어서 chart 의 list_name(이름 리스트) 에 저장함
//	반환 에러
//		0 : 에러 없음.
//		1 : 이름 리스트 파일이 없음
//		2 : 이미 해당 id 가 있음
//		21 : database 폴더로 이동 못함
//		22 : 초기 디렉토리로 이동 못함
//
int GKD_ELO_Chart::read_name_list() {
	
	FILE *fp = nullptr;
	int cnt = 0, err = 0;
	int ibuf = 0;
	char sbuf[64] = { 0, };

	if (_chdir(STRING_DIR_DB))
		return 21;

	fp = fopen(STRING_NAME_LIST, "rt");
	if (fp == nullptr)
		return 1;

	while (fscanf(fp, "%d %s", &ibuf, sbuf) != EOF) {
		cnt++;
		err = this->list_name.insert_name(ibuf, sbuf);	//	에러를 체크해야되서 push_back 호출하지 않는다.

		if (err == -1) {
			printf("(GKD_ELO_CHART->read_name_list) 에러발생 : 이미 해당 id 가 있음 : %d\n", ibuf);
			return 2;
		}
	}
	fclose(fp);

	if (_chdir("../"))
		return 22;

	return 0;
}

int GKD_ELO_Chart::write_name_list() {

	_chdir(STRING_DIR_DB);

	FILE* fp = fopen(STRING_NAME_LIST, "wt");

	ITERATOR_NAME it = this->list_name.name_list.begin();

	while (it != this->list_name.name_list.end()) {
		fprintf(fp, "%d    %s\n", it->first, it->second.c_str());
		it++;
	}
	
	fclose(fp);
	_chdir("../");
	
	return 0;
}

//	기본폴더에서 시작한다.
//	- Database 폴더 없으면 생성
//	- Database 폴더 내부에 Deck_DB 폴더 없으면 생성
//	- Deck_DB 폴더 내부에 고유 폴더(ex 1001) 없으면 생성
//	- 이 경우에는 당연히 base_info.txt 가 없기 때문에 생성
//	- 고유 폴더 있으면 score.txt 새로 작성
//	- record.txt 는 여기서 작성 안함
void GKD_ELO_Chart::write_each_score() {
	ITERATOR_NAME it_i, it_j;
	int i, j, id = 0, ret = 0;
	int w[7] = { 0, }, d = 0, l[7] = { 0, };
	char sbuf[32];
	SSTRING name;
	FILE *fp = nullptr;
	std::tuple<int*, int, int*> tp;

	//	Database 폴더
	if (access(STRING_DIR_DB, 00)) {
		if (mkdir(STRING_DIR_DB)) {
			printf("each_score : database 폴더가 없으며 생성을 못함\n");
			return;
		}
	}
	if (chdir(STRING_DIR_DB)) {
		printf("each_score : database 폴더가 있는데 왜 이동을 못하니\n");
		return;
	}

	//	Deck_DB 폴더
	if (access(STRING_DIR_DECK_DB, 00)) {
		if (mkdir(STRING_DIR_DECK_DB)) {
			printf("each_score : deck_db 폴더가 없으며 생성을 못함\n");
			return;
		}
	}
	if (chdir(STRING_DIR_DECK_DB)) {
		printf("each_score : deck_db 폴더가 있는데 왜 이동을 못하니\n");
		return;
	}

	//	각 개별마다의 디렉토리로 이동하여 파일을 생성하고 입력한다.
	//	기본정보파일 유무도 확인하고 없으면 넣어준다.
	it_i = this->list_name.name_list.begin();
	while (it_i != this->list_name.name_list.end()) {
		int len = 0;
		i = it_i->first;
		std::string si = itoa(i, sbuf, 10);
		
		//	개별 폴더 생성
		if (access(si.c_str(), 00)) {
			printf("%d : %s 는 폴더를 만들자\n", i, this->list_name.find_name(i).c_str());
			if (mkdir(si.c_str())) {
				printf("each_score : (%d, %s) 폴더가 없는데 왜 못만드니\n", i, this->list_name.find_name(i).c_str());
				it_i++;
				return;
			}
		}
		if (chdir(si.c_str())) {
			printf("each_score : %d 폴더가 있는데 왜 이동을 못하니\n", i);
			//	개별 디렉토리로 이동하지 못했으므로 이전 디렉토리로 이동하지 않는다.
			it_i++;
			continue;
		}

		//	개별 기본정보파일 유뮤 확인 및 생성
		//	개별 기본정보 파일이 없을때만 실행한다.
		//	현재 디렉토리 : 개별 디렉토리
		if (access((si + "_baseinfo.txt").c_str(), 00)) {
			int tlen = this->list_name.find_name(i).length();
			fp = fopen((si + "_baseinfo.txt").c_str(), "wt");
			if (fp == nullptr) {
				printf("each_score : (%d, %s) baseinfo 파일이 만들어지지 않음\n", i, this->list_name.find_name(i).c_str());
				//	Deck_DB 폴더로 이동해야한다.
				_chdir("../");
				it_i++;
				continue;
			}
			NODE_BIRTH* nb = &(this->deck_row[i].birth);
			fprintf(fp, "ID\t%d\n", i);
			fprintf(fp, "NAME    %s", this->list_name.find_name(i).c_str());
			for (int k = 0; k < 24 - tlen; k++)
				fprintf(fp, " ");
			fprintf(fp, " %d-%d-%d %d:%d\n", nb->year, nb->month, nb->day, nb->hour, nb->min);
			fclose(fp);
		}
		else {
			//	파일이 이미 존재한다면, 이름을 덮어 쓰는 용도로만 작동을 할텐데
			//	그 작업은 덱 수정 모드에서 해야한다.
			//	여기서 하면 덱 수정 모드에서 덮어쓰는 정보랑 충돌할 수 있으며
			//	덱 수정을 하지 않았는데 이름이 한 번 더 덮어씌워질 수 있다.
		}
		
		//	개별 스코어 파일 쓰기
		//	1. 개별 기본정보를 입력함.
		//	2. 개별 총 전적을 입력함
		//	3. 이름 리스트를 돌면서 id 순서대로 입력함
		//	현재 디렉토리 : 개별 디렉토리
		{
			fp = fopen((si + "_score.txt").c_str(), "wt");
			if (fp == nullptr) {
				printf("each_score : %d 의 score 파일이 쓰기 모드로 열리지 않았다고??\n", i);
				_chdir("../");
				it_i++;
				continue;
			}

			//	1. 개별 기본정보를 입력함.
			fprintf(fp, "ID\t%d\n", i);
			fprintf(fp, "NAME\t%s\n", it_i->second.c_str());
			fprintf(fp, "ELO\t%lf\n", this->deck_row[i].elo);

			//	2. 개별 총 전적을 입력함
			fprintf(fp, "\t\t\t\t\t  W6  W5  W4  W3  W2  W1   D  L1  L2  L3  L4  L5  L6\n");
			tp = this->get_total_score_id(i);
			fprintf(fp, "\tTotal\t\t\t\t");
			for (int it = 6; it >= 1; it--)
				fprintf(fp, "%4d", std::get<0>(tp)[it]);
			fprintf(fp, "%4d", std::get<1>(tp));
			for (int it = 1; it <= 6; it++)
				fprintf(fp, "%4d", std::get<2>(tp)[it]);
			fprintf(fp, "\n");

			//	3. 이름 리스트를 돌면서 id 순서대로 입력함
			it_j = this->list_name.name_list.begin();
			while (it_j != this->list_name.name_list.end()) {
				j = it_j->first;
				if (i == j) {
					it_j++;
					continue;
				}
				tp = this->deck_row[i].score_map[j].return_score();
				int tlen = it_j->second.length();
				fprintf(fp, "%d\t%s", j, it_j->second.c_str());
				for (int asd = 0; asd < 32 - tlen; asd++)
					fprintf(fp, " ");

				for (int it = 6; it >= 1; it--)
					fprintf(fp, "%4d", std::get<0>(tp)[it]);
				fprintf(fp, "%4d", std::get<1>(tp));
				for (int it = 1; it <= 6; it++)
					fprintf(fp, "%4d", std::get<2>(tp)[it]);

				fprintf(fp, "\n");

				it_j++;
			}
			fclose(fp);
		}
		//	현재 디렉토리 : 개별 디렉토리
		//	Deck_DB 폴더로 돌아간다.
		_chdir("../");
		it_i++;
	}
	//	Database/Deck_DB 에서 기본 폴더로 돌아간다.
	_chdir("../../");
	
}

//
//	공백용
void GKD_ELO_Chart::write_entire_score() {
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	FILE *fp = nullptr;
	int n = this->list_name.name_list.size();

	if (_chdir(STRING_DIR_DB))
		return;

	fp = fopen(STRING_SCORE_ENTIRE, "wt");
	
	fprintf(fp, "%d\n", n);
	while (it_name != this->list_name.name_list.end()) {
		int id = it_name->first;
		std::string name = it_name->second;

		//	DEBUG : 여기서 잘못 받아온다.
		std::tuple<int*, int, int*> score = this->return_total_score_id(id);
		
		double rate = this->get_win_rate(id);

		fprintf(fp, "%d  %4.2lf\t%3.2lf%%\t", id, this->deck_row[id].elo, rate);
		for (int i = 1; i <= 6; i++)
			fprintf(fp, "%4d", std::get<0>(score)[7 - i]);
		fprintf(fp, "%4d", std::get<1>(score));
		for (int i = 1; i <= 6; i++)
			fprintf(fp, "%4d", std::get<2>(score)[i]);
		fprintf(fp, "\n");
		
		it_name++;
	}
	fprintf(fp, "\n");

	fclose(fp);
	_chdir("../");
}

//	Database 폴더에서 생성된다.
//
void GKD_ELO_Chart::write_each_record(NODE_Record_Entire nd) {
	
	int id[2] = { nd.r_id[0], nd.r_id[1] };
	char sbuf[32];
	FILE* fp[2];
	std::string sid[2];
	
	sid[0] = itoa(id[0], sbuf, 10);
	sid[1] = itoa(id[1], sbuf, 10);

	chdir(STRING_DIR_DECK_DB);
	if (access(sid[0].c_str(), 00)) {
		if (mkdir(sid[0].c_str())) {
			printf("DEBUG FUCKFUCK %d\n", id[0]);
			return;
		}
	}
	chdir(sid[0].c_str());
	fp[0] = fopen((sid[0] + "_record.txt").c_str(), "at");
	if (fp[0] == nullptr) {
		printf("%d 의 record 파일이 생성되지 않았다. 왜그러지??\n", id[0]);
		return;
	}
	this->write_record_node(fp[0], nd);
	fclose(fp[0]);
	chdir("../");

	if (access(sid[1].c_str(), 00)) {
		if (mkdir(sid[1].c_str())) {
			printf("DEBUG FUCKFUCK %d\n", id[0]);
			return;
		}
	}
	chdir(sid[1].c_str());
	fp[1] = fopen((sid[1] + "_record.txt").c_str(), "at");
	if (fp[1] == nullptr) {
		printf("%d 의 record 파일이 생성되지 않았다. 왜그러지??\n", id[1]);
		return;
	}
	this->write_record_node(fp[1], nd);
	fclose(fp[1]);
	chdir("../");

	//	DECK_DB -> Database
	chdir("../");
}

//
//	공백용
void GKD_ELO_Chart::write_entire_record() {
	ITERATOR_RECORD it_record = this->record_list.begin();
	FILE *fp = nullptr;

	if (_chdir(STRING_DIR_DB)) {
		printf("전체 기록 쓰는데 Database 폴더를 인식 못하네\n");
		return;
	}

	fp = fopen(STRING_RECORD_ENTIRE, "at");
	if (fp == nullptr) {
		printf("전체기록 파일이 생성되지 않았다고??\n");
		chdir("../");
		return;
	}
	while (it_record != this->record_list.end()) {
		this->write_record_node(fp, *it_record);

		this->write_each_record(*it_record);
		it_record++;
	}
	this->record_list.clear();
	fclose(fp);
	_chdir("../");
}

void GKD_ELO_Chart::write_modified_deck_name() {


	if (_chdir(STRING_DIR_DB)) {
		printf("이름 바꾼거 적는데 Database 폴더를 인식 못하네\n");
		return;
	}
}

//	파일 열기, 닫기는 여기서 안한다.
//	다 기록하고 공백을 추가한다.
void GKD_ELO_Chart::write_record_node(FILE *fp, NODE_Record_Entire nd) {
	fprintf(fp, "%d-%2d-%2d %2d:%2d\n", nd.r_year, nd.r_month, nd.r_date, nd.r_hour, nd.r_min);
	fprintf(fp, "%d            %d\n", nd.r_id[0], nd.r_id[1]);
	fprintf(fp, "%s                %s\n", nd.r_result[0], nd.r_result[1]);
	fprintf(fp, "%3d     %3d      %3d\n", nd.r_win_draw_win[0], nd.r_win_draw_win[1], nd.r_win_draw_win[2]);
	fprintf(fp, "%4.2lf      %4.2lf\n", nd.r_elo_prev[0], nd.r_elo_prev[1]);
	if (abs(nd.r_elo_delta[0]) < 100)
		fprintf(fp, " ");
	if (abs(nd.r_elo_delta[0]) < 10)
		fprintf(fp, "  ");
	fprintf(fp, "%4.2lf      ", nd.r_elo_delta[0]);
	if (abs(nd.r_elo_delta[1]) < 100)
		fprintf(fp, " ");
	if (abs(nd.r_elo_delta[1]) < 10)
		fprintf(fp, " ");
	fprintf(fp, "%4.2lf\n", nd.r_elo_delta[1]);
	fprintf(fp, "%4.2lf      %4.2lf\n", nd.r_elo_later[0], nd.r_elo_later[1]);
	fprintf(fp, "\n\n");
}

//	tw, td, tl 를 0으로 만든다.
//	tuple 을 쓰기위한 포인터를 쓰기위한 멤버변수이다.
void GKD_ELO_Chart::set_t_zero() {
	for (int i = 0; i < 7; i++) {
		this->tw[i] = 0;
		this->tl[i] = 0;
	}
	this->td = 0;
}

//	입력 : 이름
//	에러 : 
//		0	: 정상작동
//		1	: 해당 이름이 이미 존재함
//		21	: DECK_DB 디렉토리로 이동하지 못함.
//		22	: 해당 덱의 디렉토리를 생성하지 못함
//		23	: 초기 디렉토리로 이동하지 못함.
//		24	: 기본 파일을 만들고 초기 디렉토리로 이동하지 못함.
//		31	: 이름에 속성이 없음.
//	기능 : 
//		'input_string' 이 이름 리스트에 없는 새로운 덱일때 제대로 작동한다.
//		id 를 찾는다.
//		chart 의 row, col 에 추가한다.
//		name_list 에도 추가한다.
//
int GKD_ELO_Chart::insert_new_deck(std::string input_string) {
	
	int id = 0, id_base = 0, id_idx = 1;
	int err = 0, ret = 0; // ret 는 추가로 호출한 함수들의 에러넘버를 받는 용도로 사용한다.

	//	1. 이름 보정구간
	std::pair<int, std::string> pbuf = this->convert_name(input_string);
	id_base = pbuf.first;
	input_string = pbuf.second;
	if (id_base == NULL) {
		return 31;
	}

	//	2. 해당 이름이 존재하는 경우 함수를 종료한다.
	//	에러메시지는 이 함수를 호출한 함수에서 출력한다.
	if (this->list_name.isExist_name(input_string)) {
		return 1;
	}

	//	3. id 를 부여한다. (변수명 : id)
	//	   n*1000 + 1부터 부여된다.
	ITERATOR_NAME it = this->list_name.name_list.begin();

	while (it != this->list_name.name_list.end()) {
		if (it->first / 1000 == id_base / 1000)
			id_idx++;
		if (it->first / 1000 > id_base / 1000)
			break;
		it++;
	}
	
	id = id_base + id_idx;

	this->deck_row[id].id = id;
	this->deck_row[id].deck_name = input_string;

	//	4. 생성 일자, 시간을 저장한다.

	time_t t = time(NULL);
	struct tm* tt = localtime(&t);
	this->deck_row[id].birth.year = tt->tm_year + 1900;
	this->deck_row[id].birth.month = tt->tm_mon + 1;
	this->deck_row[id].birth.day = tt->tm_mday;
	this->deck_row[id].birth.hour = tt->tm_hour;
	this->deck_row[id].birth.min = tt->tm_min;

	//	5. ROW 를 1칸 늘린다.
	//	5.1 ROW_NODE 를 만든다
	//	5.2 기존의 ID 들에 해당하는 col들을 전부 입력한다. (NAME_LIST 참고하자)
	//	5.3 이름 리스트에 이름을 추가한다.
	//	6 모든 ROW 에 새로운 col 을 입력한다.
	
	
	if (this->list_name.name_list.size() == 0) {
		//	기존에 노드가 없었을 경우
		this->deck_row[id].insert_new_column(id);	//	5.1, 6 현재 node_row 에 현재 col을 추가한다.
		this->list_name.insert_name(id, input_string);	//	5.3 이름리스트에 이름을 추가한다.
		
	}
	else {
		ITERATOR_NAME it_name_list = this->list_name.name_list.begin();

		//	5.1, 5.2, 기존 정보들을 이용하여 새로운 node_row 를 만든다.
		while (it_name_list != this->list_name.name_list.end()) {
			this->deck_row[id].insert_new_column(it_name_list->first);
			it_name_list++;
		}
		
		this->list_name.insert_name(id, input_string); // 5.3

		//	이름 리스트에 있는 순서대로 각 row 마다 새로운 col 을 추가해야 한다.

		it_name_list = this->list_name.name_list.begin();
		while (it_name_list != this->list_name.name_list.end()) {
			int temp_id = it_name_list->first;
			this->deck_row[temp_id].insert_new_column(id);

			it_name_list++;
		}
	}
	return 0;
}

//	입력 : id, 이름
//	에러 : 
//		0	: 정상작동
//		1	: 해당 이름이 이미 존재함
//
//	기능 : 덱을 생성할 때, 저장된 정보들을 불러온다.
//		   이름리스트에는 있어야 한다.
int GKD_ELO_Chart::insert_saved_deck(int id, std::string input_string) {
	//
	//	1. 차트에 (id, string) 을 추가한다.
	//	2. 디렉토리를 체크한다.
	//	3. 디렉토리의 파일에서 정보를 가져오고 차트의 각 노드에 입력한다.
	//
	
	int err = 0, ibuf = 0;
	double dbuf = 0;
	char sbuf[64];
	FILE* fp = nullptr;
	std::string str_id = itoa(id, sbuf, 10);

	//
	//	unordered_map 을 사용하여 2차원 배열을 만들었기 떄문에
	//	별다른 생성구문 없이 참조만 하여도 생성이 된다.
	//	
	this->deck_row[id].id = id;
	this->deck_row[id].deck_name = input_string;

	//	3. 디렉토리의 score,info 파일로부터 정보를 입력받는다.
	if (access(STRING_DIR_DB, 00))
		return 0;
	chdir(STRING_DIR_DB);

	if (access(STRING_DIR_DECK_DB, 00)) {
		chdir("../");
		return 0;
	}
	chdir(STRING_DIR_DECK_DB);

	if (access(str_id.c_str(), 00)) {
		chdir("../../");
		return 0;
	}
	chdir(str_id.c_str());

	//	SCORE 파일
	int w[7] = { 0, }, d = 0, l[7] = { 0, };
	
	fp = fopen((str_id + "_score.txt").c_str(), "rt");
	if (fp == nullptr) {	//	score 를 안 부르면 그만이다.
		chdir("../../../");
		return 0;
	}
	fscanf(fp, " %s %s", sbuf, sbuf);	//	ID ~~
	fscanf(fp, " %s %s", sbuf, sbuf);	//	NAME ~~
	fscanf(fp, " %s %lf", sbuf, &dbuf);	//	ELO	~~
	
	this->deck_row[id].elo = dbuf;
	for (int i = 0; i < 13; i++)
		fscanf(fp, " %s", sbuf);
	//	TOTAL 부분
	fscanf(fp, " %s %d %d %d %d %d %d %d %d %d %d %d %d %d", sbuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf);
	
	while (fscanf(fp, " %d %s", &ibuf, sbuf) != EOF) {
		for (int i = 6; i >= 1; i--)
			fscanf(fp, " %d", w + i);
		fscanf(fp, " %d", &d);	// draw 의 d, 순간 오타인줄;;
		for (int i = 1; i <= 6; i++)
			fscanf(fp, " %d", l + i);
		this->deck_row[id].score_map[ibuf].set_score(w, d, l);
	}
	
	fclose(fp);

	//	INFO 파일
	fp = fopen((str_id + "_info.txt").c_str(), "rt");
	if (fp == nullptr) {
		//printf("%d 의 구성 파일은 없습니다.\n", id);
		chdir("../../../");
		return 0;
	}
	int ibufarr[CNT_CARD] = { 0, };
	for (int i = 0; i < CNT_CARD; i++)
		fscanf(fp, " %s %d", sbuf, ibufarr + i);
	this->deck_row[id].insert_deck_form(ibufarr);
	fclose(fp);
	
	chdir("../../../");
	return 0;
}

//	입력 : win, lose, how_much
//		-	이름은 convert 되어서 들어온다.
//	기능 : process 를 실행한다.
//		   record 는 여기서 넣어준다.
//	draw 시에는 how_much 에 0 을 입력한다.
//	how_much : 승리자가 남아있는 갯수
int GKD_ELO_Chart::get_battle(std::string win, std::string lose, int how_much) {

	//	1. ELO 계산
	//		-	무는 무시한다.
	//	2. score 입력
	//	3. record 입력
	int idw = this->list_name.find_id(win), idl = this->list_name.find_id(lose);
	double delta_elo = 0;
	double ew = this->deck_row[idw].elo, el = this->deck_row[idl].elo;


	//	1. ELO 계산
	//	  비길때의 delta_constant 는 MotherHeader 에서 설정하면 된다.
	//	  이 주석을 작성한 시기에는 0으로 설정되어있다.
	double ews = pow(10, ew / GKD_ELO_RATE_BASE), els = pow(10, el / GKD_ELO_RATE_BASE);

	double lose_rate = els / (ews + els);	//	진 놈이 이긴 확률을 곱해야 한다.

	delta_elo = lose_rate * GKD_ELO_DELTA[how_much] + GKD_ELO_DELTA_PLUS[how_much];

	this->deck_row[idw].elo += delta_elo + 0.001;
	this->deck_row[idl].elo -= delta_elo;

	//	2. score 입력
	//	
	if (how_much) {
		this->deck_row[idw].score_map[idl].win[how_much]++;
		this->deck_row[idl].score_map[idw].lose[how_much]++;
	}
	else {
		this->deck_row[idw].score_map[idl].draw++;
		this->deck_row[idl].score_map[idw].draw++;
	}

	int twin = this->deck_row[idw].score_map[idl].sum_win();
	int tdraw = this->deck_row[idw].score_map[idl].draw;
	int tlose = this->deck_row[idw].score_map[idl].sum_lose();

	printf("\n\n");
	if (GKD_ELO_DELTA_PLUS[how_much])
		printf("총합      %3d 전, DELTA : (%.2lf + %.1lf) / (%.1lf) = %.2lf\n\n", twin + tdraw + tlose, lose_rate * GKD_ELO_DELTA[how_much],
			GKD_ELO_DELTA_PLUS[how_much],  GKD_ELO_DELTA[how_much], delta_elo);
	else
		printf("총합      %3d 전, DELTA : %.2lf / %.1lf\n\n", twin + tdraw + tlose, delta_elo, GKD_ELO_DELTA[how_much]);

	printf("승: ");
	this->print_color_deck_number(idw);
	printf(" : %3d 승 ", twin);
	this->print_color_deck_name_return_length(idw, NULL);
	printf(" : %.2lf -> %.2lf\n", ew, ew + delta_elo);

	printf(" 무승부   %3d 무\n", tdraw);

	printf("패: ");
	this->print_color_deck_number(idl);
	printf(" : %3d 승 ", tlose);
	this->print_color_deck_name_return_length(idl, NULL);
	printf(" : %.2lf -> %.2lf\n", el, el - delta_elo);
	

	//	3. record 입력

	NODE_Record_Entire nd;

	time_t gt = time(NULL);
	struct tm* gtm = localtime(&gt);

	nd.r_year = gtm->tm_year + 1900;
	nd.r_month = gtm->tm_mon + 1;
	nd.r_date = gtm->tm_mday;
	nd.r_hour = gtm->tm_hour;
	nd.r_min = gtm->tm_min;

	nd.r_id[0] = idw;
	nd.r_id[1] = idl;

	nd.r_result[0][0] = 'W';
	nd.r_result[0][1] = how_much + '0';
	nd.r_result[0][2] = 0;
	nd.r_result[1][0] = 'L';
	nd.r_result[1][1] = how_much + '0';
	nd.r_result[1][2] = 0;

	nd.r_win_draw_win[0] = this->deck_row[idw].score_map[idl].sum_win();
	nd.r_win_draw_win[1] = this->deck_row[idw].score_map[idl].draw;
	nd.r_win_draw_win[2] = this->deck_row[idl].score_map[idw].sum_win();

	nd.r_elo_prev[0] = ew;
	nd.r_elo_prev[1] = el;

	nd.r_elo_delta[0] = delta_elo;
	nd.r_elo_delta[1] = -delta_elo;

	nd.r_elo_later[0] = this->deck_row[idw].elo;
	nd.r_elo_later[1] = this->deck_row[idl].elo;

	this->record_list.push_back(nd);

	return 0;
}



//	(완)
//	입력 : 변환할 이름
//	출력 : (id_base, 변환된 이름)
//	에러 : int = 0 이면 이름이 잘못 된것이다.
//	기능 : 이름의 속성 부분을 대문자로 변환해준다.	
//
std::pair<int, std::string> GKD_ELO_Chart::convert_name(std::string input_string) {
	int id_base = 0;
	int idx = 0;
	if ((input_string[0] == 'G' || input_string[0] == 'g') &&
		(input_string[1] == 'R' || input_string[1] == 'r') &&
		(input_string[2] == 'A' || input_string[2] == 'a') &&
		(input_string[3] == 'S' || input_string[3] == 's') &&
		(input_string[4] == 'S' || input_string[4] == 's')) {
		id_base = ID_BASE_GRASS;
		input_string[0] = 'G';
		input_string[1] = 'R';
		input_string[2] = 'A';
		input_string[3] = 'S';
		input_string[4] = 'S';
		idx = 5;
	}
	else if (
		(input_string[0] == 'F' || input_string[0] == 'f') &&
		(input_string[1] == 'I' || input_string[1] == 'i') &&
		(input_string[2] == 'R' || input_string[2] == 'r') &&
		(input_string[3] == 'E' || input_string[3] == 'e')) {
		id_base = ID_BASE_FIRE;
		input_string[0] = 'F';
		input_string[1] = 'I';
		input_string[2] = 'R';
		input_string[3] = 'E';
		idx = 4;
	}
	else if (
		(input_string[0] == 'W' || input_string[0] == 'w') &&
		(input_string[1] == 'A' || input_string[1] == 'a') &&
		(input_string[2] == 'T' || input_string[2] == 't') &&
		(input_string[3] == 'E' || input_string[3] == 'e') &&
		(input_string[4] == 'R' || input_string[4] == 'r')) {
		id_base = ID_BASE_WATER;
		input_string[0] = 'W';
		input_string[1] = 'A';
		input_string[2] = 'T';
		input_string[3] = 'E';
		input_string[4] = 'R';
		idx = 5;
	}
	else if (
		(input_string[0] == 'L' || input_string[0] == 'l') &&
		(input_string[1] == 'I' || input_string[1] == 'i') &&
		(input_string[2] == 'G' || input_string[2] == 'g') &&
		(input_string[3] == 'H' || input_string[3] == 'h') &&
		(input_string[4] == 'T' || input_string[4] == 't') &&
		(input_string[5] == 'N' || input_string[5] == 'n') &&
		(input_string[6] == 'I' || input_string[6] == 'i') &&
		(input_string[7] == 'N' || input_string[7] == 'n') &&
		(input_string[8] == 'G' || input_string[8] == 'g')) {
		id_base = ID_BASE_LIGHTNING;
		input_string[0] = 'L';
		input_string[1] = 'I';
		input_string[2] = 'G';
		input_string[3] = 'H';
		input_string[4] = 'T';
		input_string[5] = 'N';
		input_string[6] = 'I';
		input_string[7] = 'N';
		input_string[8] = 'G';
		idx = 9;
	}
	else if (
		(input_string[0] == 'F' || input_string[0] == 'f') &&
		(input_string[1] == 'I' || input_string[1] == 'i') &&
		(input_string[2] == 'G' || input_string[2] == 'g') &&
		(input_string[3] == 'H' || input_string[3] == 'h') &&
		(input_string[4] == 'T' || input_string[4] == 't')) {
		id_base = ID_BASE_FIGHT;
		input_string[0] = 'F';
		input_string[1] = 'I';
		input_string[2] = 'G';
		input_string[3] = 'H';
		input_string[4] = 'T';
		idx = 5;
	}
	else if (
		(input_string[0] == 'P' || input_string[0] == 'p') &&
		(input_string[1] == 'S' || input_string[1] == 's') &&
		(input_string[2] == 'Y' || input_string[2] == 'y') &&
		(input_string[3] == 'C' || input_string[3] == 'c') &&
		(input_string[4] == 'H' || input_string[4] == 'h') &&
		(input_string[5] == 'I' || input_string[5] == 'i') &&
		(input_string[6] == 'C' || input_string[6] == 'c')) {
		id_base = ID_BASE_PSYCHIC;
		input_string[0] = 'P';
		input_string[1] = 'S';
		input_string[2] = 'Y';
		input_string[3] = 'C';
		input_string[4] = 'H';
		input_string[5] = 'I';
		input_string[6] = 'C';
		idx = 7;
	}
	else if (
		(input_string[0] == 'N' || input_string[0] == 'n') &&
		(input_string[1] == 'O' || input_string[1] == 'o') &&
		(input_string[2] == 'R' || input_string[2] == 'r') &&
		(input_string[3] == 'M' || input_string[3] == 'm') &&
		(input_string[4] == 'A' || input_string[4] == 'a') &&
		(input_string[5] == 'L' || input_string[5] == 'l')) {
		id_base = ID_BASE_NORMAL;
		input_string[0] = 'N';
		input_string[1] = 'O';
		input_string[2] = 'R';
		input_string[3] = 'M';
		input_string[4] = 'A';
		input_string[5] = 'L';
		idx = 6;
	}
	else if (
		(input_string[0] == 'B' || input_string[0] == 'b') &&
		(input_string[1] == 'O' || input_string[1] == 'o') &&
		(input_string[2] == 'S' || input_string[2] == 's') &&
		(input_string[3] == 'S' || input_string[3] == 's')) {
		id_base = ID_BASE_BOSS;
		input_string[0] = 'B';
		input_string[1] = 'O';
		input_string[2] = 'S';
		input_string[3] = 'S';
		idx = 4;
	}
	else if (
		(input_string[0] == 'N' || input_string[0] == 'n') &&
		(input_string[1] == 'P' || input_string[1] == 'p') &&
		(input_string[2] == 'C' || input_string[2] == 'c')) {
		id_base = ID_BASE_NPC;
		input_string[0] = 'N';
		input_string[1] = 'P';
		input_string[2] = 'C';
		idx = 3;
	}
	if (input_string[idx] != '-') {
		id_base = 0;
	}
	return std::pair<int, std::string>(id_base, input_string);
}


std::string GKD_ELO_Chart::find_name_with_id(int _id) {
	return this->list_name.find_name(_id);
}

//
//	입력 : Input string
//	기능 : ID 혹은 NAME 을 입력받아서 차트에 존재한다면 Convert 된 이름을 출력한다.
//	출력 : 
//		- Convert 된 이름
//		- NULL_STRING : 이름이 차트에 없다.
//		- "-1" : 취소 명령
//
std::string GKD_ELO_Chart::find_name_with_input_string(std::string _input_string) {

	bool isExist = false;
	int deck_id = 0;

	if (_input_string == "-1") {
		return "-1";
	}
	_input_string = this->convert_name(_input_string).second;

	isExist = this->list_name.isExist_name(_input_string);

	if (isExist == false) {
		deck_id = atoi(_input_string.c_str());
		if (this->list_name.isExist_id(deck_id) == false)
			return NULL_STRING;
		else
			_input_string = this->list_name.find_name(deck_id);
	}

	return _input_string;
}

//
//	입력
//		_id : 이름을 출력할 id
//		_mode : & 1 == 길이만큼 스페이스를 출력할지
//				& 2 == 스페이스 여부 이후에 \n 을 할지
//
//	기능
//		id 에 해당하는 덱의 이름을 타입에 맞는 색으로 출력함.
//		id 에 해당하는 덱이 없으면 작동 안해야됨
//	
//	리턴
//		-1 : 해당 id 가 없을때
//		else : 이름의 길이
//
int GKD_ELO_Chart::print_color_deck_name_return_length(int _id, int _mode) {
	std::string deck_name = this->find_name_with_id(_id);
	int deck_type = _id / 1000;
	int len = deck_name.size();
	int i = 0;

	if (deck_name == NULL_STRING) {
		printf("ID : %d 는 존재하지 않습니다. \n", _id);
		return -1;
	}

	switch (deck_type) {
	case 1:
		SET_COLOR(COLOR_GREEN);
		break;
	case 2:
		SET_COLOR(COLOR_DARK_RED);
		break;
	case 3:
		SET_COLOR(COLOR_BLUE);
		break;
	case 4:
		SET_COLOR(COLOR_YELLOW);
		break;
	case 5:
		SET_COLOR(COLOR_DARK_YELLOW);
		break;
	case 6:
		SET_COLOR(COLOR_PINK);
		break;
	default:
		break;
	}
	printf("%s", deck_name.c_str());

	SET_COLOR(COLOR_DARK_WHITE);

	if (_mode & PRINT_LENGTH_BLANK) {
		for (i = 0; i < PRINT_BLANK_BASE - len; i++)
			printf(" ");
	}
	if (_mode & PRINT_ENTER)
		printf("\n");

	return len;
}

//
//	입력
//		input_string : 이름을 출력할 덱의 이름
//		_mode : & 1 == 길이만큼 스페이스를 출력할지
//				& 2 == 스페이스 여부 이후에 \n 을 할지
//
//	기능
//		_name 이 해당하는 색으로 이름을 출력함
//		-name 이 리스트에 없어도 출력함
//
//	리턴
//		-1 : 해당 타입이 없을때
//		else : 이름의 길이
//
int GKD_ELO_Chart::print_color_deck_name_return_length(std::string input_string, int _mode) {
	int id_base = 0;
	int idx = 0;
	if ((input_string[0] == 'G' || input_string[0] == 'g') &&
		(input_string[1] == 'R' || input_string[1] == 'r') &&
		(input_string[2] == 'A' || input_string[2] == 'a') &&
		(input_string[3] == 'S' || input_string[3] == 's') &&
		(input_string[4] == 'S' || input_string[4] == 's')) {
		id_base = ID_BASE_GRASS;
		input_string[0] = 'G';
		input_string[1] = 'R';
		input_string[2] = 'A';
		input_string[3] = 'S';
		input_string[4] = 'S';
		idx = 5;
	}
	else if (
		(input_string[0] == 'F' || input_string[0] == 'f') &&
		(input_string[1] == 'I' || input_string[1] == 'i') &&
		(input_string[2] == 'R' || input_string[2] == 'r') &&
		(input_string[3] == 'E' || input_string[3] == 'e')) {
		id_base = ID_BASE_FIRE;
		input_string[0] = 'F';
		input_string[1] = 'I';
		input_string[2] = 'R';
		input_string[3] = 'E';
		idx = 4;
	}
	else if (
		(input_string[0] == 'W' || input_string[0] == 'w') &&
		(input_string[1] == 'A' || input_string[1] == 'a') &&
		(input_string[2] == 'T' || input_string[2] == 't') &&
		(input_string[3] == 'E' || input_string[3] == 'e') &&
		(input_string[4] == 'R' || input_string[4] == 'r')) {
		id_base = ID_BASE_WATER;
		input_string[0] = 'W';
		input_string[1] = 'A';
		input_string[2] = 'T';
		input_string[3] = 'E';
		input_string[4] = 'R';
		idx = 5;
	}
	else if (
		(input_string[0] == 'L' || input_string[0] == 'l') &&
		(input_string[1] == 'I' || input_string[1] == 'i') &&
		(input_string[2] == 'G' || input_string[2] == 'g') &&
		(input_string[3] == 'H' || input_string[3] == 'h') &&
		(input_string[4] == 'T' || input_string[4] == 't') &&
		(input_string[5] == 'N' || input_string[5] == 'n') &&
		(input_string[6] == 'I' || input_string[6] == 'i') &&
		(input_string[7] == 'N' || input_string[7] == 'n') &&
		(input_string[8] == 'G' || input_string[8] == 'g')) {
		id_base = ID_BASE_LIGHTNING;
		input_string[0] = 'L';
		input_string[1] = 'I';
		input_string[2] = 'G';
		input_string[3] = 'H';
		input_string[4] = 'T';
		input_string[5] = 'N';
		input_string[6] = 'I';
		input_string[7] = 'N';
		input_string[8] = 'G';
		idx = 9;
	}
	else if (
		(input_string[0] == 'F' || input_string[0] == 'f') &&
		(input_string[1] == 'I' || input_string[1] == 'i') &&
		(input_string[2] == 'G' || input_string[2] == 'g') &&
		(input_string[3] == 'H' || input_string[3] == 'h') &&
		(input_string[4] == 'T' || input_string[4] == 't')) {
		id_base = ID_BASE_FIGHT;
		input_string[0] = 'F';
		input_string[1] = 'I';
		input_string[2] = 'G';
		input_string[3] = 'H';
		input_string[4] = 'T';
		idx = 5;
	}
	else if (
		(input_string[0] == 'P' || input_string[0] == 'p') &&
		(input_string[1] == 'S' || input_string[1] == 's') &&
		(input_string[2] == 'Y' || input_string[2] == 'y') &&
		(input_string[3] == 'C' || input_string[3] == 'c') &&
		(input_string[4] == 'H' || input_string[4] == 'h') &&
		(input_string[5] == 'I' || input_string[5] == 'i') &&
		(input_string[6] == 'C' || input_string[6] == 'c')) {
		id_base = ID_BASE_PSYCHIC;
		input_string[0] = 'P';
		input_string[1] = 'S';
		input_string[2] = 'Y';
		input_string[3] = 'C';
		input_string[4] = 'H';
		input_string[5] = 'I';
		input_string[6] = 'C';
		idx = 7;
	}
	else if (
		(input_string[0] == 'N' || input_string[0] == 'n') &&
		(input_string[1] == 'O' || input_string[1] == 'o') &&
		(input_string[2] == 'R' || input_string[2] == 'r') &&
		(input_string[3] == 'M' || input_string[3] == 'm') &&
		(input_string[4] == 'A' || input_string[4] == 'a') &&
		(input_string[5] == 'L' || input_string[5] == 'l')) {
		id_base = ID_BASE_NORMAL;
		input_string[0] = 'N';
		input_string[1] = 'O';
		input_string[2] = 'R';
		input_string[3] = 'M';
		input_string[4] = 'A';
		input_string[5] = 'L';
		idx = 6;
	}
	else if (
		(input_string[0] == 'B' || input_string[0] == 'b') &&
		(input_string[1] == 'O' || input_string[1] == 'o') &&
		(input_string[2] == 'S' || input_string[2] == 's') &&
		(input_string[3] == 'S' || input_string[3] == 's')) {
		id_base = ID_BASE_BOSS;
		input_string[0] = 'B';
		input_string[1] = 'O';
		input_string[2] = 'S';
		input_string[3] = 'S';
		idx = 4;
	}
	else if (
		(input_string[0] == 'N' || input_string[0] == 'n') &&
		(input_string[1] == 'P' || input_string[1] == 'p') &&
		(input_string[2] == 'C' || input_string[2] == 'c')) {
		id_base = ID_BASE_NPC;
		input_string[0] = 'N';
		input_string[1] = 'P';
		input_string[2] = 'C';
		idx = 3;
	}
	if (input_string[idx] != '-') {
		id_base = 0;
	}
	int deck_type = id_base / 1000;
	int len = input_string.size();
	int i = 0;

	switch (deck_type) {
	case 1:
		SET_COLOR(COLOR_GREEN);
		break;
	case 2:
		SET_COLOR(COLOR_DARK_RED);
		break;
	case 3:
		SET_COLOR(COLOR_BLUE);
		break;
	case 4:
		SET_COLOR(COLOR_YELLOW);
		break;
	case 5:
		SET_COLOR(COLOR_DARK_YELLOW);
		break;
	case 6:
		SET_COLOR(COLOR_PINK);
		break;
	default:
		break;
	}
	printf("%s", input_string.c_str());

	SET_COLOR(COLOR_DARK_WHITE);

	if (_mode & PRINT_LENGTH_BLANK) {
		for (i = 0; i < PRINT_BLANK_BASE - len; i++)
			printf(" ");
	}
	if (_mode & PRINT_ENTER)
		printf("\n");

	return len;
}

//
//	입력
//		_id : id 를 출력할 id
//
//	기능
//		해당 덱의 id를 타입에 맞는 색으로 출력함.
//	
//	리턴
//		-1 : 해당 id 가 없을때
//		else : 이름의 길이
//
int GKD_ELO_Chart::print_color_deck_number(int _id) {
	std::string deck_name = this->find_name_with_id(_id);

	if (deck_name == NULL_STRING) {
		printf("ID : %d 는 존재하지 않습니다. \n", _id);
		return -1;
	}

	int deck_type = _id / 1000;

	switch (deck_type) {
	case 1:
		SET_COLOR(COLOR_GREEN);
		break;
	case 2:
		SET_COLOR(COLOR_DARK_RED);
		break;
	case 3:
		SET_COLOR(COLOR_BLUE);
		break;
	case 4:
		SET_COLOR(COLOR_YELLOW);
		break;
	case 5:
		SET_COLOR(COLOR_DARK_YELLOW);
		break;
	case 6:
		SET_COLOR(COLOR_PINK);
		break;
	default:
		break;
	}
	printf("%d", _id);

	SET_COLOR(COLOR_DARK_WHITE);

	return deck_name.size();
}

void GKD_ELO_Chart::print_insert_new_deck_error(int _error, std::string _additional) {

	if (!_error)
		return;
	else {
		std::cout << _additional << " ";
		switch (_error) {
		case 1:
			printf("해당 덱이 이미 존재함\n");
			break;
		case 21:
			printf("DECK_DB 디렉토리로 이동을 못함\n");
			break;
		case 22:
			printf("해당 덱의 디렉토리를 생성하지 못함\n");
			break;
		case 23:
			printf("초기 디렉토리로 이동하지 못함\n");
			break;
		case 24:
			printf("기본 파일은 만들고 초기 디렉토리로 이동하지 못함\n");
			break;
		case 31:
			printf("이름에 속성이 없음\n");
			break;
		default:
			printf("알 수 없는 에러\n");
			break;
		}
	}

}

//
//	해당 id 의 전체 score 를 리턴한다.
//
std::tuple<int*, int, int*> GKD_ELO_Chart::return_total_score_id(int id) {
	
	this->set_t_zero();
	
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	std::tuple<int*, int, int*> temp_tuple;
	while (it_name != this->list_name.name_list.end()) {
		int tid = it_name->first;
		temp_tuple = this->deck_row[id].score_map[tid].return_score();
		for (int i = 1; i <= 6; i++) {
			this->tw[i] += std::get<0>(temp_tuple)[i];
			this->tl[i] += std::get<2>(temp_tuple)[i];
		}
		this->td += std::get<1>(temp_tuple);
		it_name++;
	}
	temp_tuple = std::make_tuple(this->tw, this->td, this->tl);


	return temp_tuple;
}

//	이름만 다른 함수이다.
//	get == return
std::tuple<int*, int, int*> GKD_ELO_Chart::get_total_score_id(int id) {
	return this->return_total_score_id(id);
}

//	입력 : id
//	출력 : 해당 id 의 전체 승/(승+패)
double GKD_ELO_Chart::return_win_rate(int id) {
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	int tw = 0, tl = 0;
	double ret = 0;
	while (it_name != this->list_name.name_list.end()) {
		std::tuple<int*, int, int*> tp = this->get_total_score_id(id);
		for (int i = 1; i <= 6; i++) {
			tw += std::get<0>(tp)[i];
			tl += std::get<2>(tp)[i];
		}

		it_name++;
	}

	if (tw + tl == 0)
		return 0;
	else
		return (double)tw / (double)(tw + tl)*100;
}

//	이름만 다른 함수이다
//
double GKD_ELO_Chart::get_win_rate(int id) {
	return this->return_win_rate(id);
}

int GKD_ELO_Chart::return_tot_win(int id) {
	int ret = 0;
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	while (it_name != this->list_name.name_list.end()) {
		int id2 = it_name->first;
		std::string name = it_name->second;
		ret += this->deck_row[id].score_map[id2].sum_win();
		it_name++;
	}
	return ret;
}
int GKD_ELO_Chart::get_tot_win(int id) {
	return this->return_tot_win(id);
}
int GKD_ELO_Chart::return_tot_lose(int id) {
	int ret = 0;
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	while (it_name != this->list_name.name_list.end()) {
		int id2 = it_name->first;
		std::string name = it_name->second;
		ret += this->deck_row[id].score_map[id2].sum_lose();
		it_name++;
	}
	return ret;
}
int GKD_ELO_Chart::get_tot_lose(int id) {
	return return_tot_lose(id);
}
int GKD_ELO_Chart::return_tot_draw(int id) {
	int ret = 0;
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	while (it_name != this->list_name.name_list.end()) {
		int id2 = it_name->first;
		std::string name = it_name->second;
		ret += this->deck_row[id].score_map[id2].draw;

		it_name++;
	}
	return ret;
}
int GKD_ELO_Chart::get_tot_draw(int id) {
	return this->return_tot_draw(id);
}



//	완
//	전체 스코어 파일 어떻게 구성할건지 생각해야한다.
//	작동 순서
//		1. 이름 리스트를 불러온다.
//			-	파일이 없으면 읽어올 정보가 없으므로 종료한다.
//		2. 
//
void GKD_ELO_Chart::mode_1_read_file() {
	int err = 0, ibuf = 0, len = 0, i = 0;
	char sbuf[64] = { 0, };

	printf("Mode 1 : read_file 을 실행합니다.\n");
	if (this->isFiled) {
		printf("이미 파일을 읽었습니다. \n");
		return;
	}

	//	1. 이름 리스트 읽고 넣기
	err = this->read_name_list();
	if (err) {
		printf("이름 리스트 파일이 없습니다.\n");
		return;
	}
	
	//	스코어 읽어와야지
	ITERATOR_NAME it_name = this->list_name.name_list.begin();

	while (it_name != this->list_name.name_list.end()) {
		int id = it_name->first;
		std::string name = it_name->second;

		this->insert_saved_deck(id, name);
		it_name++;
	}
	this->isFiled = true;
}

//	(완)
//		패치를 할 수는 있다.
//
//	파일 읽기와는 다르게 스코어 파일도 기록하여야 한다.
//
//	1. 이름 리스트 파일을 기록한다.
//	2. 스코어를 파일을 기록한다.
//
void GKD_ELO_Chart::mode_2_write_file() {

	int n = 0, i = 0;
	FILE *fp = nullptr;

	printf("Mode 2 : write_file 을 실행합니다.\n");

	//	1. 이름 리스트 기록
	//	2. 전체 스코어 기록
	//	3. 개별 스코어 기록
	//	4. 총 전적 기록
	//	5. 개별 전적 기록은 4번에서 호출한다.
	printf("write_name_list\n");
	this->write_name_list();
	printf("write_entire_score\n");
	this->write_entire_score();
	printf("write_each_score\n");
	this->write_each_score();
	printf("write_entire_record\n");
	this->write_entire_record();
	printf("write_modified_deck_name\n");
}

//	---   알파버전 완성   ---
//	---   추가로 생성할 파일이 더 생길경우 수정한다.
//	새로운 덱을 추가한다. (입력할 대전정보가 없다)
//	이미 존재했던 덱이라면 만들지 않고 취소한다.
//	추가로 생성할 파일이 있으면 여기서 "안만든다"
void GKD_ELO_Chart::mode_3_add_deck() {
	//
	//	새로운 덱을 추가하는 곳이므로 insert_new_deck 만 호출하고 에러코드만 읽는다.
	std::string input_string;
	int err = 0;
	printf("\nMODE_3 : ADD_DECK 을 실행합니다.\n");
	printf("이름을 입력하십시오 : ");

	std::cin >> input_string;
	if (input_string == "-1") {
		printf("취소합니다.\n");
		return;
	}
	this->print_insert_new_deck_error(this->insert_new_deck(input_string), "(ADD_DECK : " + input_string + ")");
}

//
//	2,3,4 를 어떻게 묶을지 생각해야 한다.
//
void GKD_ELO_Chart::mode_4_get_battle() {
	//	1. a 와 b 를 입력받는다.
	//		id, name 어떤걸로 입력해도 된다.
	//		없으면 생성할지 여부를 물어본다.
	//	2. 결과를 입력받는다.
	//		결과 오타여부를 점검해야한다.
	//		- score, record 는 호출한 함수에서 기록한다.
	std::string a, b, c, d, res_a, res_b;
	int ret = 0;
	int ibuf = 0;
	int ia = 0, ib = 0;

	//	1. a 와 b 를 입력받는 부분
	//		각각 존재 여부와 생성여부를 확인한다.
	printf("\nMODE_4 를 실행합니다.\n\n");

	printf("a 를 입력하여 주십시오 : ");
	std::cin >> a;
	res_a = this->find_name_with_input_string(a);

	if (res_a == "-1") {
		printf("취소합니다.\n");
		return;
	}
	else if (res_a == "NULL") {
		a = this->convert_name(a).second;
		this->print_color_deck_name_return_length(a, NULL);
		printf(" 는 차트에 없습니다.\n");
		printf("새로 만들까요? (Y/N) : ");
		std::cin >> c;
		if (c[0] == 'y' || c[0] == 'Y')
			this->print_insert_new_deck_error(this->insert_new_deck(a), "(MODE_4_a) : ");
		else {
			printf("만들지 않고 종료합니다.\n");
			return;
		}
	}
	else
		a = res_a;


	printf("b 를 입력하여 주십시오 : ");
	std::cin >> b;
	res_b = this->find_name_with_input_string(b);

	if (res_b == "-1") {
		printf("취소합니다.\n");
		return;
	}
	else if (res_b == "NULL") {
		b = this->convert_name(b).second;
		this->print_color_deck_name_return_length(b, NULL);
		printf(" 는 차트에 없습니다.\n");
		printf("새로 만들까요? (Y/N) : ");
		std::cin >> c;
		if (c[0] == 'y' || c[0] == 'Y') {
			this->print_insert_new_deck_error(this->insert_new_deck(b), "(MODE_4_b) : ");
		}
		else {
			printf("만들지 않고 종료합니다.\n");
			return;
		}
	}
	else
		b = res_b;

	//	2. 결과를 입력받고 battle 을 실행하는 부분
	int how_much = 0;

	printf("\n");
	printf("\na : ");
	this->print_color_deck_name_return_length(a, PRINT_ENTER);
	printf("b : ");
	this->print_color_deck_name_return_length(b, PRINT_ENTER);
input_result:
	printf("\n누가 이겼나요? (a or b or draw or -1) : ");
	std::cin >> c;
	
	if (c[0] == 'a' || c[0] == 'A') {
		printf("\n얼마나 차이가 나나요? : ");
	win_a:
		scanf("%d", &ibuf);
		if (ibuf == -1) {
			printf("취소합니다\n");
			return;
		}
		else if (ibuf < 1 || ibuf > 6) {
			printf("잘못된 수를 입력하였습니다. 다시 입력하여 주십시오. : ");
			goto win_a;
		}
		this->get_battle(a, b, ibuf);
	}
	else if (c[0] == 'b' || c[0] == 'B') {
		printf("얼마나 차이가 나나요? : ");
	win_b:
		scanf("%d", &ibuf);
		if (ibuf == -1) {
			printf("취소합니다\n");
			return;
		}
		else if (ibuf < 1 || ibuf > 6) {
			printf("잘못된 수를 입력하였습니다. 다시 입력하여 주십시오. : ");
			goto win_b;
		}
		this->get_battle(b, a, ibuf);
	}
	else if (c == "-1") {
		printf("취소합니다.\n");
		return;
	}
	else if(c[0] == 'd' || c[0] == 'D'){
		this->get_battle(a, b, 0);
	}
	else {
		printf("잘못된 결과를 입력하였습니다. 다시 입력하여 주십시오.\n");
		goto input_result;
	}
}

//
//	기능 : 덱의 이름을 바꾼다.
//	- GKD_ELO_Chart.list_name 에 있는 이름을 바꾼다.
//	- name_list.txt 에 기록될 내용은 mode_2_write 에서 호출되는 write_name_list 에서 반영된다.
//	- base_info.txt 에 기록될 내용은 '이름 수정 큐' 에 저장된 원소들을 pop 하면서 반영한다.
//
//	1. 이름을 입력받고 검사한다.
//	2. 바꿀 이름을 입력하고 검사한다.
//	3. Chart.list_name 에 수정된 이름을 반영한다.
//		- name_list.txt 에 기록할 목적
//	4. 해당하는 row 에 수정된 이름을 반영한다.
//	5. 수정된 기록 queue 에 수정사항을 저장한다.
//		- base_info.txt 에 기록할 목적
//
void GKD_ELO_Chart::mode_5_modify_name() {
	printf("Mode 5 : modify_deck 을 실행합니다.\n");
	printf("\n이름만 바꿀 수 있습니다.\n");
	printf("아직 구현이 안되었습니다.\n");

	std::string input_string;
	int deck_id = 0, ret = 0;

	//
	//	1. 이름을 입력받고 검사한다.
	//
	printf("원래 이름이나 ID 를 입력하시오. \n");
	std::cin >> input_string;
	if (input_string == "-1") {
		printf("취소합니다.\n");
		return;
	}
	input_string = this->convert_name(input_string).second;
	std::cout << "Result is " << input_string << std::endl;
	ret = this->list_name.isExist_name(input_string);
	if (ret == false) {
		deck_id = atoi(input_string.c_str());
		if (this->list_name.isExist_id(deck_id) == false) {
			printf("%s 는 차트에 없습니다.\n", input_string.c_str());
		}
		else {
			input_string = this->list_name.find_name(deck_id);
		}
	}


}

//
//	모든 row 를 id 순으로 정보를 출력한다.
//	
//
void GKD_ELO_Chart::mode_33_print_all_row_id() {
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	int cnt = 0;

	printf("\n총 덱 수 : %d\n\n", this->list_name.name_list.size());

	while (it_name != this->list_name.name_list.end()) {
		int temp_id = it_name->first;
		printf(" ");
		this->print_color_deck_number(temp_id);
		printf(": ");

		int tlen = this->print_color_deck_name_return_length(temp_id, PRINT_LENGTH_BLANK);
		printf(" : %.2lf, %.2lf%%\n", this->deck_row[temp_id].elo, this->get_win_rate(temp_id));

		if (cnt % 4 == 3)
			printf("\n");
		it_name++;
		cnt++;
	}
}

//	모든 row 를 elo순으로 출력한다.
//
void GKD_ELO_Chart::mode_34_print_all_row_elo() {
	
	std::vector<NODE_PRINTED_ROW> vec;
	NODE_PRINTED_ROW temp_row;

	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	int cnt = 0;

	printf("\n");
	while (it_name != this->list_name.name_list.end()) {
		int id = it_name->first;

		temp_row.id = id;
		strcpy(temp_row.name, it_name->second.c_str());
		temp_row.elo = this->deck_row[id].elo;
		temp_row.win_rate = this->get_win_rate(id);

		vec.push_back(temp_row);
		it_name++;		
	}

	std::sort(vec.begin(), vec.end(), cmp_row_elo);

	int len = vec.size();

	for (int i = 0; i < len; i++) {
		int temp_id = vec[i].id;

		printf(" ");
		this->print_color_deck_number(temp_id);
		printf(": ");
		int tlen = this->print_color_deck_name_return_length(temp_id, PRINT_LENGTH_BLANK);
		printf(" : %.2lf, %.2lf%%\n", this->deck_row[temp_id].elo, this->get_win_rate(temp_id));

		if (i % 4 == 3)
			printf("\n");
	}
}

//
//	한 id 의 모든 col 과의 score 를 출력한다.
//	기대승률, 실제승률을 각각 출력한다.
//
void GKD_ELO_Chart::mode_41_print_id_all_col() {
	printf("\nMODE_41 을 실행합니다.\n");
	std::string input_string, res_input;
	int id = 0;
	double expected_future = 0;
	double total_win_rate = 0;

	printf("어느 덱을 보시겠습니까? : ");
	std::cin >> input_string;
	input_string = this->convert_name(input_string).second;

	res_input = this->find_name_with_input_string(input_string);
	if (res_input == "-1") {
		printf("취소합니다. \n");
		return;
	}
	else if (res_input == NULL_STRING) {
		printf("%s 는 리스트에 없습니다. \n", input_string.c_str());
		return;
	}
	else
		input_string = res_input;


	id = this->list_name.find_id(input_string);
	total_win_rate = this->get_win_rate(id)/100.0;

	ITERATOR_NAME it_name = this->list_name.name_list.begin();

	printf("\n\n%d, ", id);
	this->print_color_deck_name_return_length(id, NULL);
	printf(" 와의 상대전적을 봅니다. \n\n");

	printf("이 덱의 ELO : %.2lf\n", this->deck_row[id].elo);
	printf("이 덱의 승률 : %.2lf%% = %d / %d\n", this->get_win_rate(id), this->return_tot_win(id), this->return_tot_lose(id) + this->return_tot_win(id));
	for (int i = 0; i < 45; i++)
		printf(" ");
	printf("W    D    L    현재승률  기대승률       ELO   Future\n");
	int cnt = 0;
	while (it_name != this->list_name.name_list.end()) {
		if (it_name->second == input_string) {
			it_name++;
			continue;
		}

		int id2 = it_name->first;
		std::string name2 = it_name->second;
		int w = this->deck_row[id].score_map[id2].sum_win();
		int d = this->deck_row[id].score_map[id2].draw;
		int l = this->deck_row[id].score_map[id2].sum_lose();

		double rr = this->deck_row[id].score_map[id2].rate_win_lose();

		double ew = pow(10, this->deck_row[id].elo / GKD_ELO_RATE_BASE);
		double el = pow(10, this->deck_row[id2].elo / GKD_ELO_RATE_BASE);
		double er = (ew) / (ew + el) * 100;
		double elos = this->deck_row[id2].elo;

		this->print_color_deck_number(id2);
		printf(": ");

		int tlen = this->print_color_deck_name_return_length(id2, NULL);
		for (int i = 0; i < 32 - tlen; i++)
			printf(" ");

		printf("  : %4d %4d %4d    ", w, d, l);

		if (rr < 100)
			printf(" ");
		if (rr < 10)
			printf(" ");
		if (w + l == 0)
			printf("-----    ");
		else
			printf("%3.2lf%%    ", rr);

		if (er < 100)
			printf(" ");
		if (er < 10)
			printf(" ");
		printf("%3.2lf%%   ", er);

		if (elos < 1000)
			printf(" ");
		if (elos < 100)
			printf(" ");
		if (elos < 10)
			printf(" ");
		printf("%.2lf  ", elos);

		//	각 덱에 대하여 모든 팀들과 PREDICT_NUM_GAME 만큼의 게임 이후, 예상되는 점수 변동폭 계산
		double win_rate = rr / 100;
		double lose_rate = 1 - win_rate;
		double expected_win_rate = er / 100;
		double expected_lose_rate = 1 - expected_win_rate;
		double delta = 0;
		if (w + l != 0)
			delta = (expected_lose_rate * win_rate - expected_win_rate * lose_rate) * 20 * PREDICT_NUM_GAME;
		else
			delta = (expected_lose_rate * total_win_rate - expected_win_rate * (1 - total_win_rate)) * 20 * PREDICT_NUM_GAME;
		expected_future += delta;
		if (delta >= 100)
			printf(" ");
		else if (delta >= 10)
			printf("  ");
		else if (delta >= 0)
			printf("   ");
		else if (delta > -10)
			printf("  ");
		else if (delta > -100)
			printf(" ");
		printf("%4.2lf\n", delta);

		if (cnt % 4 == 3)
			printf("\n");
		cnt++;
		it_name++;
	}
	printf("\n%d 경기후 예상 변동량은 %3.2lf -> %4.2lf\n", PREDICT_NUM_GAME, expected_future, this->deck_row[id].elo+expected_future);

}

//
//	현재 보유중인 deck 들로 부분 리그들을 구성하여 출력한다.
//
void GKD_ELO_Chart::mode_51_print_grouping() {
	printf("\nMODE_51 그룹핑을 실행합니다.\n");

	int entire_number = 0, i = 0, j = 0;
	int member_number = 0;
	int size_list = this->list_name.name_list.size();

	printf("전체 구성원 수를 입력하시오 (1~%d, -1 입력시 종료)\n", this->deck_row.size());

	std::cin >> entire_number;

	if (entire_number > this->deck_row.size() || entire_number < 1) {
		printf("초과된 범위의 입력입니다.\n");
		return;
	}


	printf("그룹별 구성원 수를 입력하시오 (%d 의 약수, -1 입력시 종료)\n", this->deck_row.size());

	std::cin >> member_number;

	if (member_number < 1 || entire_number % member_number != 0) {
		printf("잘못된 입력입니다. \n");
		return;
	}

	std::pair<int, std::string> *entire_array = new std::pair<int, std::string>[size_list];

	auto it = this->list_name.name_list.begin();

	while (it != this->list_name.name_list.end()) {
		entire_array[i++] = (*it++);
	}

	//	덱을 섞는다
	std::random_device rd;
	std::mt19937 gen(rd());
	for (i = 0; i < size_list; i++) {
		//	dis(a, b) : a 부터 b 까지 랜덤한 정수
		std::uniform_int_distribution<int> dis(0, size_list - 1);

		int temp = dis(gen);

		std::pair<int, std::string> temp_pair = entire_array[temp];
		entire_array[temp] = entire_array[size_list - i - 1];
		entire_array[size_list - i - 1] = temp_pair;
	}
	
	//	덱을 그룹별로 나눈다.
	for (i = 0; i < entire_number / member_number; i++) {
		printf("\n그룹 %d 의 멤버\n", i + 1);
		int start_idx = size_list - i * member_number - member_number;
		int end_idx = size_list - i * member_number;
		std::sort(entire_array + start_idx, entire_array + end_idx, cmp_name_list);

		for (j = 0; j < member_number; j++) {
			int temp_idx = size_list - (1 + i * member_number + j);
			printf("    ");
			this->print_color_deck_number(entire_array[temp_idx].first);
			printf(", ");
			this->print_color_deck_name_return_length(entire_array[temp_idx].first, NULL);
			printf("\n"); 
		}
			
	}

	delete[] entire_array;
}

