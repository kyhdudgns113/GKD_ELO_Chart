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

//  (��)
//	�̸� ����Ʈ ������ �о chart �� list_name(�̸� ����Ʈ) �� ������
//	��ȯ ����
//		0 : ���� ����.
//		1 : �̸� ����Ʈ ������ ����
//		2 : �̹� �ش� id �� ����
//		21 : database ������ �̵� ����
//		22 : �ʱ� ���丮�� �̵� ����
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
		err = this->list_name.insert_name(ibuf, sbuf);	//	������ üũ�ؾߵǼ� push_back ȣ������ �ʴ´�.

		if (err == -1) {
			printf("(GKD_ELO_CHART->read_name_list) �����߻� : �̹� �ش� id �� ���� : %d\n", ibuf);
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

//	�⺻�������� �����Ѵ�.
//	- Database ���� ������ ����
//	- Database ���� ���ο� Deck_DB ���� ������ ����
//	- Deck_DB ���� ���ο� ���� ����(ex 1001) ������ ����
//	- �� ��쿡�� �翬�� base_info.txt �� ���� ������ ����
//	- ���� ���� ������ score.txt ���� �ۼ�
//	- record.txt �� ���⼭ �ۼ� ����
void GKD_ELO_Chart::write_each_score() {
	ITERATOR_NAME it_i, it_j;
	int i, j, id = 0, ret = 0;
	int w[7] = { 0, }, d = 0, l[7] = { 0, };
	char sbuf[32];
	SSTRING name;
	FILE *fp = nullptr;
	std::tuple<int*, int, int*> tp;

	//	Database ����
	if (access(STRING_DIR_DB, 00)) {
		if (mkdir(STRING_DIR_DB)) {
			printf("each_score : database ������ ������ ������ ����\n");
			return;
		}
	}
	if (chdir(STRING_DIR_DB)) {
		printf("each_score : database ������ �ִµ� �� �̵��� ���ϴ�\n");
		return;
	}

	//	Deck_DB ����
	if (access(STRING_DIR_DECK_DB, 00)) {
		if (mkdir(STRING_DIR_DECK_DB)) {
			printf("each_score : deck_db ������ ������ ������ ����\n");
			return;
		}
	}
	if (chdir(STRING_DIR_DECK_DB)) {
		printf("each_score : deck_db ������ �ִµ� �� �̵��� ���ϴ�\n");
		return;
	}

	//	�� ���������� ���丮�� �̵��Ͽ� ������ �����ϰ� �Է��Ѵ�.
	//	�⺻�������� ������ Ȯ���ϰ� ������ �־��ش�.
	it_i = this->list_name.name_list.begin();
	while (it_i != this->list_name.name_list.end()) {
		int len = 0;
		i = it_i->first;
		std::string si = itoa(i, sbuf, 10);
		
		//	���� ���� ����
		if (access(si.c_str(), 00)) {
			printf("%d : %s �� ������ ������\n", i, this->list_name.find_name(i).c_str());
			if (mkdir(si.c_str())) {
				printf("each_score : (%d, %s) ������ ���µ� �� �������\n", i, this->list_name.find_name(i).c_str());
				it_i++;
				return;
			}
		}
		if (chdir(si.c_str())) {
			printf("each_score : %d ������ �ִµ� �� �̵��� ���ϴ�\n", i);
			//	���� ���丮�� �̵����� �������Ƿ� ���� ���丮�� �̵����� �ʴ´�.
			it_i++;
			continue;
		}

		//	���� �⺻�������� ���� Ȯ�� �� ����
		//	���� �⺻���� ������ �������� �����Ѵ�.
		//	���� ���丮 : ���� ���丮
		if (access((si + "_baseinfo.txt").c_str(), 00)) {
			int tlen = this->list_name.find_name(i).length();
			fp = fopen((si + "_baseinfo.txt").c_str(), "wt");
			if (fp == nullptr) {
				printf("each_score : (%d, %s) baseinfo ������ ��������� ����\n", i, this->list_name.find_name(i).c_str());
				//	Deck_DB ������ �̵��ؾ��Ѵ�.
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
			//	������ �̹� �����Ѵٸ�, �̸��� ���� ���� �뵵�θ� �۵��� ���ٵ�
			//	�� �۾��� �� ���� ��忡�� �ؾ��Ѵ�.
			//	���⼭ �ϸ� �� ���� ��忡�� ����� ������ �浹�� �� ������
			//	�� ������ ���� �ʾҴµ� �̸��� �� �� �� ������� �� �ִ�.
		}
		
		//	���� ���ھ� ���� ����
		//	1. ���� �⺻������ �Է���.
		//	2. ���� �� ������ �Է���
		//	3. �̸� ����Ʈ�� ���鼭 id ������� �Է���
		//	���� ���丮 : ���� ���丮
		{
			fp = fopen((si + "_score.txt").c_str(), "wt");
			if (fp == nullptr) {
				printf("each_score : %d �� score ������ ���� ���� ������ �ʾҴٰ�??\n", i);
				_chdir("../");
				it_i++;
				continue;
			}

			//	1. ���� �⺻������ �Է���.
			fprintf(fp, "ID\t%d\n", i);
			fprintf(fp, "NAME\t%s\n", it_i->second.c_str());
			fprintf(fp, "ELO\t%lf\n", this->deck_row[i].elo);

			//	2. ���� �� ������ �Է���
			fprintf(fp, "\t\t\t\t\t  W6  W5  W4  W3  W2  W1   D  L1  L2  L3  L4  L5  L6\n");
			tp = this->get_total_score_id(i);
			fprintf(fp, "\tTotal\t\t\t\t");
			for (int it = 6; it >= 1; it--)
				fprintf(fp, "%4d", std::get<0>(tp)[it]);
			fprintf(fp, "%4d", std::get<1>(tp));
			for (int it = 1; it <= 6; it++)
				fprintf(fp, "%4d", std::get<2>(tp)[it]);
			fprintf(fp, "\n");

			//	3. �̸� ����Ʈ�� ���鼭 id ������� �Է���
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
		//	���� ���丮 : ���� ���丮
		//	Deck_DB ������ ���ư���.
		_chdir("../");
		it_i++;
	}
	//	Database/Deck_DB ���� �⺻ ������ ���ư���.
	_chdir("../../");
	
}

//
//	�����
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

		//	DEBUG : ���⼭ �߸� �޾ƿ´�.
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

//	Database �������� �����ȴ�.
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
		printf("%d �� record ������ �������� �ʾҴ�. �ֱ׷���??\n", id[0]);
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
		printf("%d �� record ������ �������� �ʾҴ�. �ֱ׷���??\n", id[1]);
		return;
	}
	this->write_record_node(fp[1], nd);
	fclose(fp[1]);
	chdir("../");

	//	DECK_DB -> Database
	chdir("../");
}

//
//	�����
void GKD_ELO_Chart::write_entire_record() {
	ITERATOR_RECORD it_record = this->record_list.begin();
	FILE *fp = nullptr;

	if (_chdir(STRING_DIR_DB)) {
		printf("��ü ��� ���µ� Database ������ �ν� ���ϳ�\n");
		return;
	}

	fp = fopen(STRING_RECORD_ENTIRE, "at");
	if (fp == nullptr) {
		printf("��ü��� ������ �������� �ʾҴٰ�??\n");
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
		printf("�̸� �ٲ۰� ���µ� Database ������ �ν� ���ϳ�\n");
		return;
	}
}

//	���� ����, �ݱ�� ���⼭ ���Ѵ�.
//	�� ����ϰ� ������ �߰��Ѵ�.
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

//	tw, td, tl �� 0���� �����.
//	tuple �� �������� �����͸� �������� ��������̴�.
void GKD_ELO_Chart::set_t_zero() {
	for (int i = 0; i < 7; i++) {
		this->tw[i] = 0;
		this->tl[i] = 0;
	}
	this->td = 0;
}

//	�Է� : �̸�
//	���� : 
//		0	: �����۵�
//		1	: �ش� �̸��� �̹� ������
//		21	: DECK_DB ���丮�� �̵����� ����.
//		22	: �ش� ���� ���丮�� �������� ����
//		23	: �ʱ� ���丮�� �̵����� ����.
//		24	: �⺻ ������ ����� �ʱ� ���丮�� �̵����� ����.
//		31	: �̸��� �Ӽ��� ����.
//	��� : 
//		'input_string' �� �̸� ����Ʈ�� ���� ���ο� ���϶� ����� �۵��Ѵ�.
//		id �� ã�´�.
//		chart �� row, col �� �߰��Ѵ�.
//		name_list ���� �߰��Ѵ�.
//
int GKD_ELO_Chart::insert_new_deck(std::string input_string) {
	
	int id = 0, id_base = 0, id_idx = 1;
	int err = 0, ret = 0; // ret �� �߰��� ȣ���� �Լ����� �����ѹ��� �޴� �뵵�� ����Ѵ�.

	//	1. �̸� ��������
	std::pair<int, std::string> pbuf = this->convert_name(input_string);
	id_base = pbuf.first;
	input_string = pbuf.second;
	if (id_base == NULL) {
		return 31;
	}

	//	2. �ش� �̸��� �����ϴ� ��� �Լ��� �����Ѵ�.
	//	�����޽����� �� �Լ��� ȣ���� �Լ����� ����Ѵ�.
	if (this->list_name.isExist_name(input_string)) {
		return 1;
	}

	//	3. id �� �ο��Ѵ�. (������ : id)
	//	   n*1000 + 1���� �ο��ȴ�.
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

	//	4. ���� ����, �ð��� �����Ѵ�.

	time_t t = time(NULL);
	struct tm* tt = localtime(&t);
	this->deck_row[id].birth.year = tt->tm_year + 1900;
	this->deck_row[id].birth.month = tt->tm_mon + 1;
	this->deck_row[id].birth.day = tt->tm_mday;
	this->deck_row[id].birth.hour = tt->tm_hour;
	this->deck_row[id].birth.min = tt->tm_min;

	//	5. ROW �� 1ĭ �ø���.
	//	5.1 ROW_NODE �� �����
	//	5.2 ������ ID �鿡 �ش��ϴ� col���� ���� �Է��Ѵ�. (NAME_LIST ��������)
	//	5.3 �̸� ����Ʈ�� �̸��� �߰��Ѵ�.
	//	6 ��� ROW �� ���ο� col �� �Է��Ѵ�.
	
	
	if (this->list_name.name_list.size() == 0) {
		//	������ ��尡 ������ ���
		this->deck_row[id].insert_new_column(id);	//	5.1, 6 ���� node_row �� ���� col�� �߰��Ѵ�.
		this->list_name.insert_name(id, input_string);	//	5.3 �̸�����Ʈ�� �̸��� �߰��Ѵ�.
		
	}
	else {
		ITERATOR_NAME it_name_list = this->list_name.name_list.begin();

		//	5.1, 5.2, ���� �������� �̿��Ͽ� ���ο� node_row �� �����.
		while (it_name_list != this->list_name.name_list.end()) {
			this->deck_row[id].insert_new_column(it_name_list->first);
			it_name_list++;
		}
		
		this->list_name.insert_name(id, input_string); // 5.3

		//	�̸� ����Ʈ�� �ִ� ������� �� row ���� ���ο� col �� �߰��ؾ� �Ѵ�.

		it_name_list = this->list_name.name_list.begin();
		while (it_name_list != this->list_name.name_list.end()) {
			int temp_id = it_name_list->first;
			this->deck_row[temp_id].insert_new_column(id);

			it_name_list++;
		}
	}
	return 0;
}

//	�Է� : id, �̸�
//	���� : 
//		0	: �����۵�
//		1	: �ش� �̸��� �̹� ������
//
//	��� : ���� ������ ��, ����� �������� �ҷ��´�.
//		   �̸�����Ʈ���� �־�� �Ѵ�.
int GKD_ELO_Chart::insert_saved_deck(int id, std::string input_string) {
	//
	//	1. ��Ʈ�� (id, string) �� �߰��Ѵ�.
	//	2. ���丮�� üũ�Ѵ�.
	//	3. ���丮�� ���Ͽ��� ������ �������� ��Ʈ�� �� ��忡 �Է��Ѵ�.
	//
	
	int err = 0, ibuf = 0;
	double dbuf = 0;
	char sbuf[64];
	FILE* fp = nullptr;
	std::string str_id = itoa(id, sbuf, 10);

	//
	//	unordered_map �� ����Ͽ� 2���� �迭�� ������� ������
	//	���ٸ� �������� ���� ������ �Ͽ��� ������ �ȴ�.
	//	
	this->deck_row[id].id = id;
	this->deck_row[id].deck_name = input_string;

	//	3. ���丮�� score,info ���Ϸκ��� ������ �Է¹޴´�.
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

	//	SCORE ����
	int w[7] = { 0, }, d = 0, l[7] = { 0, };
	
	fp = fopen((str_id + "_score.txt").c_str(), "rt");
	if (fp == nullptr) {	//	score �� �� �θ��� �׸��̴�.
		chdir("../../../");
		return 0;
	}
	fscanf(fp, " %s %s", sbuf, sbuf);	//	ID ~~
	fscanf(fp, " %s %s", sbuf, sbuf);	//	NAME ~~
	fscanf(fp, " %s %lf", sbuf, &dbuf);	//	ELO	~~
	
	this->deck_row[id].elo = dbuf;
	for (int i = 0; i < 13; i++)
		fscanf(fp, " %s", sbuf);
	//	TOTAL �κ�
	fscanf(fp, " %s %d %d %d %d %d %d %d %d %d %d %d %d %d", sbuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf, &ibuf);
	
	while (fscanf(fp, " %d %s", &ibuf, sbuf) != EOF) {
		for (int i = 6; i >= 1; i--)
			fscanf(fp, " %d", w + i);
		fscanf(fp, " %d", &d);	// draw �� d, ���� ��Ÿ����;;
		for (int i = 1; i <= 6; i++)
			fscanf(fp, " %d", l + i);
		this->deck_row[id].score_map[ibuf].set_score(w, d, l);
	}
	
	fclose(fp);

	//	INFO ����
	fp = fopen((str_id + "_info.txt").c_str(), "rt");
	if (fp == nullptr) {
		//printf("%d �� ���� ������ �����ϴ�.\n", id);
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

//	�Է� : win, lose, how_much
//		-	�̸��� convert �Ǿ ���´�.
//	��� : process �� �����Ѵ�.
//		   record �� ���⼭ �־��ش�.
//	draw �ÿ��� how_much �� 0 �� �Է��Ѵ�.
//	how_much : �¸��ڰ� �����ִ� ����
int GKD_ELO_Chart::get_battle(std::string win, std::string lose, int how_much) {

	//	1. ELO ���
	//		-	���� �����Ѵ�.
	//	2. score �Է�
	//	3. record �Է�
	int idw = this->list_name.find_id(win), idl = this->list_name.find_id(lose);
	double delta_elo = 0;
	double ew = this->deck_row[idw].elo, el = this->deck_row[idl].elo;


	//	1. ELO ���
	//	  ��涧�� delta_constant �� MotherHeader ���� �����ϸ� �ȴ�.
	//	  �� �ּ��� �ۼ��� �ñ⿡�� 0���� �����Ǿ��ִ�.
	double ews = pow(10, ew / GKD_ELO_RATE_BASE), els = pow(10, el / GKD_ELO_RATE_BASE);

	double lose_rate = els / (ews + els);	//	�� ���� �̱� Ȯ���� ���ؾ� �Ѵ�.

	delta_elo = lose_rate * GKD_ELO_DELTA[how_much] + GKD_ELO_DELTA_PLUS[how_much];

	this->deck_row[idw].elo += delta_elo + 0.001;
	this->deck_row[idl].elo -= delta_elo;

	//	2. score �Է�
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
		printf("����      %3d ��, DELTA : (%.2lf + %.1lf) / (%.1lf) = %.2lf\n\n", twin + tdraw + tlose, lose_rate * GKD_ELO_DELTA[how_much],
			GKD_ELO_DELTA_PLUS[how_much],  GKD_ELO_DELTA[how_much], delta_elo);
	else
		printf("����      %3d ��, DELTA : %.2lf / %.1lf\n\n", twin + tdraw + tlose, delta_elo, GKD_ELO_DELTA[how_much]);

	printf("��: ");
	this->print_color_deck_number(idw);
	printf(" : %3d �� ", twin);
	this->print_color_deck_name_return_length(idw, NULL);
	printf(" : %.2lf -> %.2lf\n", ew, ew + delta_elo);

	printf(" ���º�   %3d ��\n", tdraw);

	printf("��: ");
	this->print_color_deck_number(idl);
	printf(" : %3d �� ", tlose);
	this->print_color_deck_name_return_length(idl, NULL);
	printf(" : %.2lf -> %.2lf\n", el, el - delta_elo);
	

	//	3. record �Է�

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



//	(��)
//	�Է� : ��ȯ�� �̸�
//	��� : (id_base, ��ȯ�� �̸�)
//	���� : int = 0 �̸� �̸��� �߸� �Ȱ��̴�.
//	��� : �̸��� �Ӽ� �κ��� �빮�ڷ� ��ȯ���ش�.	
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
//	�Է� : Input string
//	��� : ID Ȥ�� NAME �� �Է¹޾Ƽ� ��Ʈ�� �����Ѵٸ� Convert �� �̸��� ����Ѵ�.
//	��� : 
//		- Convert �� �̸�
//		- NULL_STRING : �̸��� ��Ʈ�� ����.
//		- "-1" : ��� ���
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
//	�Է�
//		_id : �̸��� ����� id
//		_mode : & 1 == ���̸�ŭ �����̽��� �������
//				& 2 == �����̽� ���� ���Ŀ� \n �� ����
//
//	���
//		id �� �ش��ϴ� ���� �̸��� Ÿ�Կ� �´� ������ �����.
//		id �� �ش��ϴ� ���� ������ �۵� ���ؾߵ�
//	
//	����
//		-1 : �ش� id �� ������
//		else : �̸��� ����
//
int GKD_ELO_Chart::print_color_deck_name_return_length(int _id, int _mode) {
	std::string deck_name = this->find_name_with_id(_id);
	int deck_type = _id / 1000;
	int len = deck_name.size();
	int i = 0;

	if (deck_name == NULL_STRING) {
		printf("ID : %d �� �������� �ʽ��ϴ�. \n", _id);
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
//	�Է�
//		input_string : �̸��� ����� ���� �̸�
//		_mode : & 1 == ���̸�ŭ �����̽��� �������
//				& 2 == �����̽� ���� ���Ŀ� \n �� ����
//
//	���
//		_name �� �ش��ϴ� ������ �̸��� �����
//		-name �� ����Ʈ�� ��� �����
//
//	����
//		-1 : �ش� Ÿ���� ������
//		else : �̸��� ����
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
//	�Է�
//		_id : id �� ����� id
//
//	���
//		�ش� ���� id�� Ÿ�Կ� �´� ������ �����.
//	
//	����
//		-1 : �ش� id �� ������
//		else : �̸��� ����
//
int GKD_ELO_Chart::print_color_deck_number(int _id) {
	std::string deck_name = this->find_name_with_id(_id);

	if (deck_name == NULL_STRING) {
		printf("ID : %d �� �������� �ʽ��ϴ�. \n", _id);
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
			printf("�ش� ���� �̹� ������\n");
			break;
		case 21:
			printf("DECK_DB ���丮�� �̵��� ����\n");
			break;
		case 22:
			printf("�ش� ���� ���丮�� �������� ����\n");
			break;
		case 23:
			printf("�ʱ� ���丮�� �̵����� ����\n");
			break;
		case 24:
			printf("�⺻ ������ ����� �ʱ� ���丮�� �̵����� ����\n");
			break;
		case 31:
			printf("�̸��� �Ӽ��� ����\n");
			break;
		default:
			printf("�� �� ���� ����\n");
			break;
		}
	}

}

//
//	�ش� id �� ��ü score �� �����Ѵ�.
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

//	�̸��� �ٸ� �Լ��̴�.
//	get == return
std::tuple<int*, int, int*> GKD_ELO_Chart::get_total_score_id(int id) {
	return this->return_total_score_id(id);
}

//	�Է� : id
//	��� : �ش� id �� ��ü ��/(��+��)
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

//	�̸��� �ٸ� �Լ��̴�
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



//	��
//	��ü ���ھ� ���� ��� �����Ұ��� �����ؾ��Ѵ�.
//	�۵� ����
//		1. �̸� ����Ʈ�� �ҷ��´�.
//			-	������ ������ �о�� ������ �����Ƿ� �����Ѵ�.
//		2. 
//
void GKD_ELO_Chart::mode_1_read_file() {
	int err = 0, ibuf = 0, len = 0, i = 0;
	char sbuf[64] = { 0, };

	printf("Mode 1 : read_file �� �����մϴ�.\n");
	if (this->isFiled) {
		printf("�̹� ������ �о����ϴ�. \n");
		return;
	}

	//	1. �̸� ����Ʈ �а� �ֱ�
	err = this->read_name_list();
	if (err) {
		printf("�̸� ����Ʈ ������ �����ϴ�.\n");
		return;
	}
	
	//	���ھ� �о�;���
	ITERATOR_NAME it_name = this->list_name.name_list.begin();

	while (it_name != this->list_name.name_list.end()) {
		int id = it_name->first;
		std::string name = it_name->second;

		this->insert_saved_deck(id, name);
		it_name++;
	}
	this->isFiled = true;
}

//	(��)
//		��ġ�� �� ���� �ִ�.
//
//	���� �б�ʹ� �ٸ��� ���ھ� ���ϵ� ����Ͽ��� �Ѵ�.
//
//	1. �̸� ����Ʈ ������ ����Ѵ�.
//	2. ���ھ ������ ����Ѵ�.
//
void GKD_ELO_Chart::mode_2_write_file() {

	int n = 0, i = 0;
	FILE *fp = nullptr;

	printf("Mode 2 : write_file �� �����մϴ�.\n");

	//	1. �̸� ����Ʈ ���
	//	2. ��ü ���ھ� ���
	//	3. ���� ���ھ� ���
	//	4. �� ���� ���
	//	5. ���� ���� ����� 4������ ȣ���Ѵ�.
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

//	---   ���Ĺ��� �ϼ�   ---
//	---   �߰��� ������ ������ �� ������ �����Ѵ�.
//	���ο� ���� �߰��Ѵ�. (�Է��� ���������� ����)
//	�̹� �����ߴ� ���̶�� ������ �ʰ� ����Ѵ�.
//	�߰��� ������ ������ ������ ���⼭ "�ȸ����"
void GKD_ELO_Chart::mode_3_add_deck() {
	//
	//	���ο� ���� �߰��ϴ� ���̹Ƿ� insert_new_deck �� ȣ���ϰ� �����ڵ常 �д´�.
	std::string input_string;
	int err = 0;
	printf("\nMODE_3 : ADD_DECK �� �����մϴ�.\n");
	printf("�̸��� �Է��Ͻʽÿ� : ");

	std::cin >> input_string;
	if (input_string == "-1") {
		printf("����մϴ�.\n");
		return;
	}
	this->print_insert_new_deck_error(this->insert_new_deck(input_string), "(ADD_DECK : " + input_string + ")");
}

//
//	2,3,4 �� ��� ������ �����ؾ� �Ѵ�.
//
void GKD_ELO_Chart::mode_4_get_battle() {
	//	1. a �� b �� �Է¹޴´�.
	//		id, name ��ɷ� �Է��ص� �ȴ�.
	//		������ �������� ���θ� �����.
	//	2. ����� �Է¹޴´�.
	//		��� ��Ÿ���θ� �����ؾ��Ѵ�.
	//		- score, record �� ȣ���� �Լ����� ����Ѵ�.
	std::string a, b, c, d, res_a, res_b;
	int ret = 0;
	int ibuf = 0;
	int ia = 0, ib = 0;

	//	1. a �� b �� �Է¹޴� �κ�
	//		���� ���� ���ο� �������θ� Ȯ���Ѵ�.
	printf("\nMODE_4 �� �����մϴ�.\n\n");

	printf("a �� �Է��Ͽ� �ֽʽÿ� : ");
	std::cin >> a;
	res_a = this->find_name_with_input_string(a);

	if (res_a == "-1") {
		printf("����մϴ�.\n");
		return;
	}
	else if (res_a == "NULL") {
		a = this->convert_name(a).second;
		this->print_color_deck_name_return_length(a, NULL);
		printf(" �� ��Ʈ�� �����ϴ�.\n");
		printf("���� ������? (Y/N) : ");
		std::cin >> c;
		if (c[0] == 'y' || c[0] == 'Y')
			this->print_insert_new_deck_error(this->insert_new_deck(a), "(MODE_4_a) : ");
		else {
			printf("������ �ʰ� �����մϴ�.\n");
			return;
		}
	}
	else
		a = res_a;


	printf("b �� �Է��Ͽ� �ֽʽÿ� : ");
	std::cin >> b;
	res_b = this->find_name_with_input_string(b);

	if (res_b == "-1") {
		printf("����մϴ�.\n");
		return;
	}
	else if (res_b == "NULL") {
		b = this->convert_name(b).second;
		this->print_color_deck_name_return_length(b, NULL);
		printf(" �� ��Ʈ�� �����ϴ�.\n");
		printf("���� ������? (Y/N) : ");
		std::cin >> c;
		if (c[0] == 'y' || c[0] == 'Y') {
			this->print_insert_new_deck_error(this->insert_new_deck(b), "(MODE_4_b) : ");
		}
		else {
			printf("������ �ʰ� �����մϴ�.\n");
			return;
		}
	}
	else
		b = res_b;

	//	2. ����� �Է¹ް� battle �� �����ϴ� �κ�
	int how_much = 0;

	printf("\n");
	printf("\na : ");
	this->print_color_deck_name_return_length(a, PRINT_ENTER);
	printf("b : ");
	this->print_color_deck_name_return_length(b, PRINT_ENTER);
input_result:
	printf("\n���� �̰峪��? (a or b or draw or -1) : ");
	std::cin >> c;
	
	if (c[0] == 'a' || c[0] == 'A') {
		printf("\n�󸶳� ���̰� ������? : ");
	win_a:
		scanf("%d", &ibuf);
		if (ibuf == -1) {
			printf("����մϴ�\n");
			return;
		}
		else if (ibuf < 1 || ibuf > 6) {
			printf("�߸��� ���� �Է��Ͽ����ϴ�. �ٽ� �Է��Ͽ� �ֽʽÿ�. : ");
			goto win_a;
		}
		this->get_battle(a, b, ibuf);
	}
	else if (c[0] == 'b' || c[0] == 'B') {
		printf("�󸶳� ���̰� ������? : ");
	win_b:
		scanf("%d", &ibuf);
		if (ibuf == -1) {
			printf("����մϴ�\n");
			return;
		}
		else if (ibuf < 1 || ibuf > 6) {
			printf("�߸��� ���� �Է��Ͽ����ϴ�. �ٽ� �Է��Ͽ� �ֽʽÿ�. : ");
			goto win_b;
		}
		this->get_battle(b, a, ibuf);
	}
	else if (c == "-1") {
		printf("����մϴ�.\n");
		return;
	}
	else if(c[0] == 'd' || c[0] == 'D'){
		this->get_battle(a, b, 0);
	}
	else {
		printf("�߸��� ����� �Է��Ͽ����ϴ�. �ٽ� �Է��Ͽ� �ֽʽÿ�.\n");
		goto input_result;
	}
}

//
//	��� : ���� �̸��� �ٲ۴�.
//	- GKD_ELO_Chart.list_name �� �ִ� �̸��� �ٲ۴�.
//	- name_list.txt �� ��ϵ� ������ mode_2_write ���� ȣ��Ǵ� write_name_list ���� �ݿ��ȴ�.
//	- base_info.txt �� ��ϵ� ������ '�̸� ���� ť' �� ����� ���ҵ��� pop �ϸ鼭 �ݿ��Ѵ�.
//
//	1. �̸��� �Է¹ް� �˻��Ѵ�.
//	2. �ٲ� �̸��� �Է��ϰ� �˻��Ѵ�.
//	3. Chart.list_name �� ������ �̸��� �ݿ��Ѵ�.
//		- name_list.txt �� ����� ����
//	4. �ش��ϴ� row �� ������ �̸��� �ݿ��Ѵ�.
//	5. ������ ��� queue �� ���������� �����Ѵ�.
//		- base_info.txt �� ����� ����
//
void GKD_ELO_Chart::mode_5_modify_name() {
	printf("Mode 5 : modify_deck �� �����մϴ�.\n");
	printf("\n�̸��� �ٲ� �� �ֽ��ϴ�.\n");
	printf("���� ������ �ȵǾ����ϴ�.\n");

	std::string input_string;
	int deck_id = 0, ret = 0;

	//
	//	1. �̸��� �Է¹ް� �˻��Ѵ�.
	//
	printf("���� �̸��̳� ID �� �Է��Ͻÿ�. \n");
	std::cin >> input_string;
	if (input_string == "-1") {
		printf("����մϴ�.\n");
		return;
	}
	input_string = this->convert_name(input_string).second;
	std::cout << "Result is " << input_string << std::endl;
	ret = this->list_name.isExist_name(input_string);
	if (ret == false) {
		deck_id = atoi(input_string.c_str());
		if (this->list_name.isExist_id(deck_id) == false) {
			printf("%s �� ��Ʈ�� �����ϴ�.\n", input_string.c_str());
		}
		else {
			input_string = this->list_name.find_name(deck_id);
		}
	}


}

//
//	��� row �� id ������ ������ ����Ѵ�.
//	
//
void GKD_ELO_Chart::mode_33_print_all_row_id() {
	ITERATOR_NAME it_name = this->list_name.name_list.begin();
	int cnt = 0;

	printf("\n�� �� �� : %d\n\n", this->list_name.name_list.size());

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

//	��� row �� elo������ ����Ѵ�.
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
//	�� id �� ��� col ���� score �� ����Ѵ�.
//	���·�, �����·��� ���� ����Ѵ�.
//
void GKD_ELO_Chart::mode_41_print_id_all_col() {
	printf("\nMODE_41 �� �����մϴ�.\n");
	std::string input_string, res_input;
	int id = 0;
	double expected_future = 0;
	double total_win_rate = 0;

	printf("��� ���� ���ðڽ��ϱ�? : ");
	std::cin >> input_string;
	input_string = this->convert_name(input_string).second;

	res_input = this->find_name_with_input_string(input_string);
	if (res_input == "-1") {
		printf("����մϴ�. \n");
		return;
	}
	else if (res_input == NULL_STRING) {
		printf("%s �� ����Ʈ�� �����ϴ�. \n", input_string.c_str());
		return;
	}
	else
		input_string = res_input;


	id = this->list_name.find_id(input_string);
	total_win_rate = this->get_win_rate(id)/100.0;

	ITERATOR_NAME it_name = this->list_name.name_list.begin();

	printf("\n\n%d, ", id);
	this->print_color_deck_name_return_length(id, NULL);
	printf(" ���� ��������� ���ϴ�. \n\n");

	printf("�� ���� ELO : %.2lf\n", this->deck_row[id].elo);
	printf("�� ���� �·� : %.2lf%% = %d / %d\n", this->get_win_rate(id), this->return_tot_win(id), this->return_tot_lose(id) + this->return_tot_win(id));
	for (int i = 0; i < 45; i++)
		printf(" ");
	printf("W    D    L    ����·�  ���·�       ELO   Future\n");
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

		//	�� ���� ���Ͽ� ��� ����� PREDICT_NUM_GAME ��ŭ�� ���� ����, ����Ǵ� ���� ������ ���
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
	printf("\n%d ����� ���� �������� %3.2lf -> %4.2lf\n", PREDICT_NUM_GAME, expected_future, this->deck_row[id].elo+expected_future);

}

//
//	���� �������� deck ��� �κ� ���׵��� �����Ͽ� ����Ѵ�.
//
void GKD_ELO_Chart::mode_51_print_grouping() {
	printf("\nMODE_51 �׷����� �����մϴ�.\n");

	int entire_number = 0, i = 0, j = 0;
	int member_number = 0;
	int size_list = this->list_name.name_list.size();

	printf("��ü ������ ���� �Է��Ͻÿ� (1~%d, -1 �Է½� ����)\n", this->deck_row.size());

	std::cin >> entire_number;

	if (entire_number > this->deck_row.size() || entire_number < 1) {
		printf("�ʰ��� ������ �Է��Դϴ�.\n");
		return;
	}


	printf("�׷캰 ������ ���� �Է��Ͻÿ� (%d �� ���, -1 �Է½� ����)\n", this->deck_row.size());

	std::cin >> member_number;

	if (member_number < 1 || entire_number % member_number != 0) {
		printf("�߸��� �Է��Դϴ�. \n");
		return;
	}

	std::pair<int, std::string> *entire_array = new std::pair<int, std::string>[size_list];

	auto it = this->list_name.name_list.begin();

	while (it != this->list_name.name_list.end()) {
		entire_array[i++] = (*it++);
	}

	//	���� ���´�
	std::random_device rd;
	std::mt19937 gen(rd());
	for (i = 0; i < size_list; i++) {
		//	dis(a, b) : a ���� b ���� ������ ����
		std::uniform_int_distribution<int> dis(0, size_list - 1);

		int temp = dis(gen);

		std::pair<int, std::string> temp_pair = entire_array[temp];
		entire_array[temp] = entire_array[size_list - i - 1];
		entire_array[size_list - i - 1] = temp_pair;
	}
	
	//	���� �׷캰�� ������.
	for (i = 0; i < entire_number / member_number; i++) {
		printf("\n�׷� %d �� ���\n", i + 1);
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

