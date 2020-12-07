#pragma once
#pragma warning(disable:4996)
/*
*		������ ��忡 �ش��ϴ� Ŭ�����̴�.
*		�ϴ��� ��ü ������ ����ϴµ��� ������ �Ѵ�.
*		���� ������ ����Ҷ��� ���� ������ ����̴�.
*/
class NODE_Record_Entire {

public :

	int		r_year = 0;				//	����
	int		r_month = 0;			//	��
	int		r_date = 0;				//	��
	int		r_hour = 0; 			//	�ð� 0~23
	int		r_min = 0;				//	�� 0~60
	int		r_id[2] = { 0,0 };			//	ID
	char		r_result[2][3] = { 0,0,0,0,0,0 };		//	r_result[0] = W4 ���� ����� ����ȴ�.
	int		r_win_draw_win[3] = { 0,0,0 };	//	0��° ��, ��, 1��° ���� Ƚ��
	double		r_elo_prev[2] = { 0,0 };
	double		r_elo_delta[2] = { 0,0 };		//	delta[0]+delta[1] = 0
	double		r_elo_later[2] = { 0,0 };

	NODE_Record_Entire();
	NODE_Record_Entire(int, int, int, int, int, int[],char[][3], int[],double[],double[],double[]);
	~NODE_Record_Entire();

};

