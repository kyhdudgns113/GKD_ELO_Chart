#pragma once
#pragma warning(disable:4996)
/*
*		전적의 노드에 해당하는 클래스이다.
*		일단은 전체 전적을 기록하는데에 쓰려고 한다.
*		개별 전적을 기록할때도 같이 쓸지는 고민이다.
*/
class NODE_Record_Entire {

public :

	int		r_year = 0;				//	연도
	int		r_month = 0;			//	월
	int		r_date = 0;				//	일
	int		r_hour = 0; 			//	시간 0~23
	int		r_min = 0;				//	분 0~60
	int		r_id[2] = { 0,0 };			//	ID
	char		r_result[2][3] = { 0,0,0,0,0,0 };		//	r_result[0] = W4 같이 결과가 저장된다.
	int		r_win_draw_win[3] = { 0,0,0 };	//	0번째 승, 무, 1번째 승의 횟수
	double		r_elo_prev[2] = { 0,0 };
	double		r_elo_delta[2] = { 0,0 };		//	delta[0]+delta[1] = 0
	double		r_elo_later[2] = { 0,0 };

	NODE_Record_Entire();
	NODE_Record_Entire(int, int, int, int, int, int[],char[][3], int[],double[],double[],double[]);
	~NODE_Record_Entire();

};

