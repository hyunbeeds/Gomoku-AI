#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>
#include <windows.system.h>
#pragma warning(disable:4996)

using namespace std;

#define Black 1
#define White 2

typedef struct point {
	int x = 0;
	int y = 0;
}Point;

int pan[19][19];	// 0 이면 빈칸, 1이면 흑 , 2이면 백
int mode = 0;
int turn = 1;
int limit = 3;
int next_sel_x = -1;
int next_sel_y = -1;
Point dol_history[300];

int dx[8] = { -1,1,0,0,-1,1,-1,1 };
int dy[8] = { 0,0,-1,1,-1,1,1,-1 };

// 전방향으로 전부 탐색할때 씀
void init_pan();
void set_pan(int x, int y, int color);
void print_pan();
boolean insert_dol(int dol);
void mode_select();
int get_StoneCount(int x, int y, int color, int nDir);
boolean win_check(int x, int y, int color);
boolean is_Five(int x, int y, int color, int nDir);
boolean is_Four(int x, int y, int color, int nDir);
boolean is_OpenFour(int x, int y, int color, int nDir);
boolean is_OpenThree(int x, int y, int color, int nDir);
boolean is_DoubleThree(int x, int y, int color);
boolean is_Two(int x, int y, int color, int nDir);
boolean is_OpenTwo(int x, int y, int color, int nDir);
void print_status(int x, int y, int color);
int evaluation(int color);
int min_value_black(int alpha, int beta, int depth);
int max_value_black(int alpha, int beta, int depth);
int min_value_white(int alpha, int beta, int depth);
int max_value_white(int alpha, int beta, int depth);

void init_pan() {
	for (int i = 0;i < 19;i++) {
		for (int j = 0;j < 19;j++) {
			pan[i][j] = 0;
		}
	}
}
void set_pan(int x, int y, int color) {
	if (pan[x][y] == 0) {
		pan[x][y] = color;
	}
	else {
		printf("error!\n");
	}
}
void print_pan() {
	printf("   ");
	for (int i = 0;i < 19;i++) {
		if (i >= 10) {
			printf("%d", i);
			continue;
		}
		printf("%-2d", i);
	}
	printf("\n");
	for (int i = 0;i < 19;i++) {
		printf("%2d ", i);
		for (int j = 0;j < 19;j++) {
			if (pan[i][j] == 0) {
				printf("□");
			}
			else if (pan[i][j] == 1) {
				printf("●");
			}
			else if (pan[i][j] == 2) {
				printf("○");
			}
		}printf("\n");
	}
}

boolean insert_dol(int dol) {
	int x, y;
	if (dol == Black) {
		printf("흑돌의 차례입니다. 둘 칸을 선택해 주세요\n");
		scanf("%d %d", &x, &y);
	}
	else if (dol == White) {
		printf("백돌의 차례입니다. 둘 칸을 선택해 주세요\n");
		scanf("%d %d", &x, &y);
	}
	if (x >= 19 || x < 0 || y >= 19 || y < 0) {
		printf("범위가 잘못되었습니다. 다시 입력해주세요(0-18 사이로...)");
		insert_dol(dol);

	}
	if (pan[x][y] == 0) {
		pan[x][y] = dol;
		dol_history[turn].x = x;
		dol_history[turn].y = y;
		turn++;

		if (is_DoubleThree(x, y, dol)) {
			printf("삼삼입니다. 다시두세요\n");
			pan[x][y] = 0;
			insert_dol(dol);
			return false;
		}
		if (win_check(x, y, dol)) {
			system("cls");
			print_pan();
			return true;
		}
	}
	else if (dol == Black || dol == White) {
		printf("돌이 있습니다 다시 두세요\n");
		insert_dol(dol);
		return false;
	}
	return false;
}

void mode_select() {
	printf("모드를 선택하세요(0-> 사용자둘이서 두기, 1-> 컴퓨터 백돌, 2-> 컴퓨터 흑돌 3->인공지능끼리 두기)\n");
	scanf("%d", &mode);
	if (mode > 3 || mode < 0) {
		printf("잘못입력하셨습니다. 0, 1, 2, 중에 하나를 선택해주세요.\n");
		mode_select();
	}
	system("cls");
}

int get_StoneCount(int x, int y, int color, int nDir) {	// 어느 방향에 대해 돌이 몇개인지 세주는 함수
	int cnt = 0;	// 돌의 개수
	int xd, yd;
	int tx, ty;
	tx = x;		//초기의 x,y 저장해놓는 곳
	ty = y;
	if (pan[x][y] == 0) {	// (x,y)에 돌이 안놓여져 있으면 종료
		return 0;
	}
	xd = dx[nDir];
	yd = dy[nDir];
	for (;pan[x][y] == color;x += xd, y += yd)cnt++;

	//반대편 방향 조사하기 위해서 dx, dy바꿔줌;
	if (nDir % 2 == 1) {
		xd = dx[nDir - 1];
		yd = dy[nDir - 1];
	}
	else {
		xd = dx[nDir + 1];
		yd = dy[nDir + 1];
	}
	x = tx + xd;
	y = ty + yd;	// 자기자신 또 안세기 위해 더해줌
	for (;pan[x][y] == color;x += xd, y += yd)cnt++;

	return cnt;
}

boolean win_check(int x, int y, int color) {	//(x,y)에 놓인 color돌이 5개 되면 true리턴
	for (int i = 0;i < 8;i = i + 2) {
		if (get_StoneCount(x, y, color, i) == 5) {
			return true;
		}
	}
	return false;
}

// 여기서 부터는 평가함수와 관련된 것입니다.

Point is_Empty(int x, int y, int color, int nDir) {	// 그 방향쪽으로 가서 empty한 칸을 찾아주는것, 끊길때 다른 돌에 의해 끊기면 (-1,-1) 반환
	Point temp;
	temp.x = -1;
	temp.y = -1;
	for (;pan[x][y] == color;x += dx[nDir], y += dy[nDir]);
	if (pan[x][y] == 0) {
		temp.x = x;
		temp.y = y;
	}
	return temp;
}

boolean is_Five(int x, int y, int color, int nDir) {	//nDir쪽 방향에 5인가 확인.
	if (get_StoneCount(x, y, color, nDir) == 5) return true;
	else return false;
}

boolean is_Four(int x, int y, int color, int nDir) { // nDir쪽 방향이 4가 있는지 확인, 막힌 4여도 표시해준다.
	int tx, ty; // 원래의 x y좌표
	if (nDir % 2 == 1) nDir = nDir - 1; // 어차피 양쪽방향 다볼꺼니... 형식통일을 위해..

	for (int i = 0;i < 2;i++) {
		tx = x;
		ty = y;
		Point temp = is_Empty(tx, ty, color, nDir + i);
		if (temp.x == -1) continue;
		else {
			pan[temp.x][temp.y] = color;
			if (is_Five(temp.x, temp.y, color, nDir + i)) {
				pan[temp.x][temp.y] = 0;
				return true;
			}
			pan[temp.x][temp.y] = 0;	//원상복구
		}
	}
	return false;
}

boolean is_OpenFour(int x, int y, int color, int nDir) {	//양쪽으로 empty한 곳에 놓았을 때 둘다 5가 될수 있으면, open 4다.
	int tx, ty;
	int sum = 0;
	if (nDir % 2 == 1) nDir = nDir - 1;
	for (int i = 0;i < 2;i++) {
		tx = x;
		ty = y;
		Point temp = is_Empty(tx, ty, color, nDir + i);
		if (temp.x == -1)continue;
		else {
			pan[temp.x][temp.y] = color;
			if (is_Five(temp.x, temp.y, color, nDir + i)) {
				/*pan[temp.x][temp.y] = 0;
				return true;
				*/
				sum = sum + 1;
			}
			pan[temp.x][temp.y] = 0;	//원상복구
		}
	}
	if (sum == 2)return true;
	else return false;
}

boolean is_OpenThree(int x, int y, int color, int nDir) {	//열린 3개수 반환
	int tx, ty;

	if (nDir % 2 == 1) nDir = nDir - 1;	//어차피 양쪽검사하므로..
	for (int i = 0;i < 2;i++) {
		tx = x;
		ty = y;
		Point temp = is_Empty(tx, ty, color, nDir + i);
		if (temp.x == -1)continue;
		else {
			pan[temp.x][temp.y] = color;
			if (is_OpenFour(tx, ty, color, nDir)) {
				pan[temp.x][temp.y] = 0;
				return true;
			}
			pan[temp.x][temp.y] = 0;
		}
	}
	return false;
}

boolean is_Three(int x, int y, int color, int nDir) {	//막힌 3개수 반환
	int tx, ty;
	int cnt = 0;
	if (nDir % 2 == 1) nDir = nDir - 1;	//어차피 양쪽검사하므로..
	for (int i = 0;i < 2;i++) {
		tx = x;
		ty = y;
		Point temp = is_Empty(tx, ty, color, nDir + i);
		if (temp.x == -1)continue;
		else {
			pan[temp.x][temp.y] = color;
			if (is_Four(tx, ty, color, nDir)) {
				pan[temp.x][temp.y] = 0;
				return true;
			}
			pan[temp.x][temp.y] = 0;
		}
	}
	return false;
}
boolean is_OpenTwo(int x, int y, int color, int nDir) {
	int tx, ty;
	if (nDir % 2 == 1) nDir = nDir - 1;	//어차피 양쪽검사하므로..

	for (int i = 0;i < 2;i++) {
		tx = x;
		ty = y;
		Point temp = is_Empty(tx, ty, color, nDir + i);
		if (temp.x == -1)continue;
		else {
			pan[temp.x][temp.y] = color;
			if (get_StoneCount(temp.x, temp.y, color, nDir + i) == 3 && is_OpenThree(tx, ty, color, nDir)) {
				pan[temp.x][temp.y] = 0;
				return true;
			}
			pan[temp.x][temp.y] = 0;
		}
	}
	return false;
}

boolean is_Two(int x, int y, int color, int nDir) {
	int tx, ty;
	for (int i = 0;i < 2;i++) {
		tx = x;
		ty = y;
		Point temp = is_Empty(tx, ty, color, nDir + i);
		if (temp.x == -1)continue;
		else {
			pan[temp.x][temp.y] = color;
			if (get_StoneCount(temp.x, temp.y, color, nDir + i) == 3 && is_Three(tx, ty, color, nDir)) {
				pan[temp.x][temp.y] = 0;
				return true;
			}
			pan[temp.x][temp.y] = 0;
		}
	}
	return false;
}

boolean is_DoubleThree(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_OpenThree(x, y, color, i))cnt++;
		if (cnt >= 2) return true;
	}
	return false;
}
int linetwo(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i<8;i++) {
		if (pan[x + dx[i]][y + dy[i]] == color) {
			cnt++;
		}
	}
	return cnt;
}

int five_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_Five(x, y, color, i)) cnt++;
	}
	return cnt;
}
int four_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_Four(x, y, color, i)) cnt++;
	}
	return cnt;
}
int openfour_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_OpenFour(x, y, color, i))cnt++;
	}
	return cnt;
}
int openthree_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_OpenThree(x, y, color, i)) cnt++;
	}
	return cnt;
}
int three_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_Three(x, y, color, i)) cnt++;
	}
	return cnt;
}
int opentwo_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_OpenTwo(x, y, color, i)) cnt++;
	}
	return cnt;
}
int two_count(int x, int y, int color) {
	int cnt = 0;
	for (int i = 0;i < 8;i += 2) {
		if (is_Two(x, y, color, i)) cnt++;
	}
	return cnt;
}
void print_status(int x, int y, int color) {
	printf("5개수 : %d\n", five_count(x, y, color));
	printf("4개수 : %d\n", four_count(x, y, color));
	printf("open4개수 : %d\n", openfour_count(x, y, color));
	printf("open3개수 : %d\n", openthree_count(x, y, color));
	printf("3개수 : %d\n", three_count(x, y, color));
	printf("open2개수 : %d\n", opentwo_count(x, y, color));
	printf("2개수 : %d\n", two_count(x, y, color));
}

int evaluation(int color) {		// 지금 판 상태에 대한 값을 배출해주는 함수, color에 대해..
	int open_two_cnt = 0;		// 내꺼에 대한 상태 배출해주는 변수들
	int line_two_cnt = 0;
	int close_two_cnt = 0;
	int open_three_cnt = 0;
	int open_four_cnt = 0;
	int close_four_cnt = 0;
	int close_three_cnt = 0;
	int five_cnt = 0;

	int eval = 0;
	int now_x, now_y;

	for (int i = 1;i < turn;i++) {		// 각 색깔의 모든 돌에 대해서 계산을 해본다. 그래서 판에 대한 정보를 종합한다! 조금 오래걸릴수도... ㅠㅠ.., 나중에 함수없애고 최적화하자 ㅠㅠ..
		if (color == Black) {
			if (i % 2 == 1) {
				now_x = dol_history[i].x;
				now_y = dol_history[i].y;
				//printf("(%d %d) ", now_x, now_y);
			}
			else continue;
		}
		if (color == White) {
			if (i % 2 == 0) {
				now_x = dol_history[i].x;
				now_y = dol_history[i].y;
				//printf("(%d %d) ", now_x, now_y);
			}
			else continue;
		}
		line_two_cnt += linetwo(now_x, now_y, color);
		open_two_cnt += opentwo_count(now_x, now_y, color);
		close_two_cnt += two_count(now_x, now_y, color);
		open_three_cnt += openthree_count(now_x, now_y, color);
		close_three_cnt += three_count(now_x, now_y, color);
		close_four_cnt += four_count(now_x, now_y, color);
		open_four_cnt += openfour_count(now_x, now_y, color);
		five_cnt += five_count(now_x, now_y, color);
	}
	line_two_cnt = line_two_cnt / 2;
	open_two_cnt = open_two_cnt / 2;
	close_two_cnt = close_two_cnt / 2 - open_two_cnt;
	open_three_cnt = open_three_cnt / 3;
	close_three_cnt = close_three_cnt / 3 - open_three_cnt;
	open_four_cnt = open_four_cnt / 4;
	close_four_cnt = close_four_cnt / 4 - open_four_cnt;
	five_cnt = five_cnt / 5;

	/*
	printf("\n5개수 : %d\n", five_cnt);
	printf("open4개수 : %d\n", open_four_cnt);
	printf("close4개수 : %d\n", close_four_cnt);
	printf("open3개수 : %d\n", open_three_cnt);
	printf("close3개수 : %d\n", close_three_cnt);
	printf("open2개수 : %d\n", open_two_cnt);
	printf("close2개수 : %d\n", close_two_cnt);
	*/

	eval += five_cnt * 500000;
	if (open_four_cnt >= 1) eval += 2000;	// 열린4하나
	if (close_four_cnt >= 2) eval += 2000;	// 닫힌 4두개
	if (close_four_cnt == 1 && open_three_cnt >= 1) eval += 2000; // 4-3
	if (open_three_cnt >= 2) eval -= 100000;	// 33 절대 못두게 함!
												//if (open_two_cnt >= 2) eval += 1;
	eval = eval + close_two_cnt + open_two_cnt * 2 + close_three_cnt * 3 + open_three_cnt * 7 + close_four_cnt * 9;
	//line two뺏다
	//printf("점수 : %d\n", eval);
	return eval;
}
int op_evaluation(int color) {
	int open_three_cnt = 0;
	int open_four_cnt = 0;
	int close_four_cnt = 0;
	int five_cnt = 0;

	int eval = 0;
	int now_x, now_y;

	for (int i = 1;i < turn;i++) {		// 각 색깔의 모든 돌에 대해서 계산을 해본다. dol_history안에있는것..
		if (color == Black) {
			if (i % 2 == 1) {
				now_x = dol_history[i].x;
				now_y = dol_history[i].y;
				//printf("(%d %d) ", now_x, now_y);
			}
			else continue;
		}
		if (color == White) {
			if (i % 2 == 0) {
				now_x = dol_history[i].x;
				now_y = dol_history[i].y;
				//printf("(%d %d) ", now_x, now_y);
			}
			else continue;
		}
		open_three_cnt += openthree_count(now_x, now_y, color);
		close_four_cnt += four_count(now_x, now_y, color);
		open_four_cnt += openfour_count(now_x, now_y, color);
		five_cnt += five_count(now_x, now_y, color);
	}
	open_three_cnt = open_three_cnt / 3;
	open_four_cnt = open_four_cnt / 4;
	close_four_cnt = close_four_cnt / 4 - open_four_cnt;
	five_cnt = five_cnt / 5;

	eval += five_cnt * 500000;
	if (close_four_cnt >= 1 && open_three_cnt >= 1) eval += 5000;
	eval = eval + open_four_cnt * 2000;

	//printf("점수 : %d\n", eval);
	return eval;
}
/*
boolean computer_black_state_ver(int color) {	//전에 백이 둔거 기준으로 둔다. 결국 depth 2랑 똑같은 함수인데 실행속도를 높인것..
int max_eval = -10000;
int eval;
int past_x = dol_history[turn - 1].x;
int past_y= dol_history[turn - 1].y;
int future_x = -1;
int future_y = -1;
int tx, ty;
//printf("past x : %d past y : %d\n", past_x, past_y);
for (int i = -5;i <= 5;i++) {
for (int j = -5;j <= 5;j++) {
tx = past_x + i;
ty = past_y + j;
if (tx < 0 || tx >= 19 || ty < 0 || ty >= 19) continue;
if (pan[tx][ty] == 0) {
pan[tx][ty] = color;
dol_history[turn].x = tx;
dol_history[turn].y = ty;
turn++;
if (color == Black) {
eval = evaluation(Black)-op_evaluation(White);
//printf("%d %d : %d\n", tx, ty, eval);
}
else if (color == White) {
eval = evaluation(White)-op_evaluation(Black);
}
pan[tx][ty] = 0;
turn--;
}
if (eval > max_eval) {
future_x = tx;
future_y = ty;
max_eval = eval;
}
}
}
pan[future_x][future_y] = color;
dol_history[turn].x = future_x;
dol_history[turn].y = future_y;
turn++;
if (win_check(future_x, future_y, color))return true;
else return false;
}*/

int temp_x = 0;
int temp_y = 0;
int next_x = 0;
int next_y = 0;
int time = 0;
int time_limit = 0;
int definite_x = -1;
int definite_y = -1;
int timeout = 0;		//timeout 되면 1로 됨

boolean AI_minmax_Black() {

	time = GetTickCount();
	timeout = 0;
	for (limit = 3;limit <= 1000;limit++) {
		max_value_black(-1000000, 1000000, 1);
		if (timeout == 0) {
			temp_x = next_x;
			temp_y = next_y;	// 한단계 끝날때마다 저장
		}
		else if(timeout==1) break;
	}

	if (definite_x > 0 && definite_y > 0) {
		pan[definite_x][definite_y] = Black;
		return true;
	}
	//결국 완벽하게 완료된 depth의 값이 컴퓨터가 둠
	pan[temp_x][temp_y] = Black;
	dol_history[turn].x = temp_x;
	dol_history[turn].y = temp_y;
	turn++;
	return win_check(temp_x, temp_y, Black);
}

int max_value_black(int alpha, int beta, int depth) {
	int eval;
	int past_x, past_y;
	int tx, ty;
	int min;
	if (GetTickCount() - time >= time_limit) {
		timeout = 1;
		return -1;
	}
	if (depth >= limit) {
		int temp = evaluation(Black) - op_evaluation(White);
		/*for (int i = 1;i < turn;i++) {
		printf("(%d %d) ", dol_history[i].x, dol_history[i].y);
		}
		printf("\n점수 리턴 : %d\n", temp);*/
		return temp;
	}
	eval = -1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -전에 내가 둔돌에서 5칸이하로 떨어진놈들 다 비교
		for (int j = -5;j <= 5;j++) {
			tx = past_x + i;
			ty = past_y + j;
			if (tx < 0 || tx >= 19 || ty < 0 || ty >= 19) continue;
			if (pan[tx][ty] == 0) {
				pan[tx][ty] = Black;
				if (win_check(tx, ty, Black)) {
					pan[tx][ty] = 0;
					if (depth == 1) {
						definite_x = tx;
						definite_y = ty;
					}
					return 500000;
				}
				dol_history[turn].x = tx;
				dol_history[turn].y = ty;
				turn++;
				min = min_value_black(alpha, beta, depth + 1);
				if (eval < min) {
					if (depth == 1) {
						next_x = tx;
						next_y = ty;
					}
					eval = min;
					//printf("max - next_x : %d next_y : %d, eval : %d \n", next_x, next_y,eval);
				}
				if (eval >= beta) {	//prunning
					pan[tx][ty] = 0;
					turn--;
					//printf("prunning : %d\n", eval);
					return eval;
				}
				if (alpha < eval) alpha = eval;
				turn--;
				pan[tx][ty] = 0;
			}
		}
	}
	return eval;
}
int min_value_black(int alpha, int beta, int depth) {
	int eval;
	int past_x, past_y;
	int tx, ty;
	int max;
	if (GetTickCount() - time >= time_limit) {
		timeout = 1;
		return -1;
	}
	if (depth >= limit) {
		int temp = evaluation(Black) - op_evaluation(White);
		/*for (int i = 1;i < turn;i++) {
		printf("(%d %d) ", dol_history[i].x, dol_history[i].y);
		}*/
		//printf("\n점수 return : %d\n", temp);
		return temp;
	}
	eval = 1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -전에 내가 둔돌에서 5칸이하로 떨어진놈들 다 비교
		for (int j = -5;j <= 5;j++) {
			tx = past_x + i;
			ty = past_y + j;
			if (tx < 0 || tx >= 19 || ty < 0 || ty >= 19) continue;
			if (pan[tx][ty] == 0) {
				pan[tx][ty] = White;
				if (win_check(tx, ty, White)) {
					pan[tx][ty] = 0;
					return -500000;
				}
				dol_history[turn].x = tx;
				dol_history[turn].y = ty;
				turn++;
				max = max_value_black(alpha, beta, depth + 1);
				if (eval > max) {
					eval = max;
				}
				if (eval <= alpha) {	//prunning
					pan[tx][ty] = 0;
					turn--;
					//printf("prunning : %d\n", eval);
					return eval;
				}
				if (beta > eval) beta = eval;
				turn--;
				pan[tx][ty] = 0;
			}
		}
	}
	return eval;
}

// 컴퓨터 white일때 AI
boolean AI_minmax_White() {
	time = GetTickCount();
	timeout = 0;

	for (limit = 3;limit <= 1000;limit++) {
		max_value_white(-100000, 100000, 1);
		if (timeout == 0) {
			temp_x = next_x;
			temp_y = next_y;	// 한단계 끝날때마다 저장
		}
		else if (timeout == 1) break;
	}
	if (definite_x > 0 && definite_y > 0) {	//무조건 이길때
		pan[definite_x][definite_y] = Black;
		return true;
	}
	pan[temp_x][temp_y] = White;
	dol_history[turn].x = temp_x;
	dol_history[turn].y = temp_y;
	turn++;

	return win_check(temp_x, temp_y, White);
}

int max_value_white(int alpha, int beta, int depth) {
	int eval;
	int past_x, past_y;
	int tx, ty;
	int min;
	if (GetTickCount() - time >= time_limit) {
		timeout = 1;
		return -1;
	}
	if (depth >= limit) {
		int temp = evaluation(White) - op_evaluation(Black);
		/*for (int i = 1;i < turn;i++) {
		printf("(%d %d) ", dol_history[i].x, dol_history[i].y);
		}
		printf("\n점수 리턴 : %d\n", temp);*/
		return temp;
	}
	eval = -1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -전에 내가 둔돌에서 5칸이하로 떨어진놈들 다 비교
		for (int j = -5;j <= 5;j++) {
			tx = past_x + i;
			ty = past_y + j;
			if (tx < 0 || tx >= 19 || ty < 0 || ty >= 19) continue;
			if (pan[tx][ty] == 0) {
				pan[tx][ty] = White;
				if (win_check(tx, ty, White)) {
					pan[tx][ty] = 0;
					if (depth == 1) {
						definite_x = tx;
						definite_y = ty;
					}
					return 500000;
				}
				dol_history[turn].x = tx;
				dol_history[turn].y = ty;
				turn++;
				min = min_value_white(alpha, beta, depth + 1);
				if (eval < min) {
					if (depth == 1) {
						next_x = tx;
						next_y = ty;
					}
					eval = min;
					//printf("max - next_x : %d next_y : %d, eval : %d \n", next_x, next_y,eval);
				}
				if (eval >= beta) {	//prunning
					pan[tx][ty] = 0;
					turn--;
					//printf("prunning : %d\n", eval);
					return eval;
				}
				if (alpha < eval) alpha = eval;
				turn--;
				pan[tx][ty] = 0;
			}
		}
	}
	return eval;
}
int min_value_white(int alpha, int beta, int depth) {
	int eval;
	int past_x, past_y;
	int tx, ty;
	int max;
	if (GetTickCount() - time >= time_limit) {
		timeout = 1;
		return -1;
	}
	if (depth >= limit) {
		int temp = evaluation(White) - op_evaluation(Black);
		/*for (int i = 1;i < turn;i++) {
		printf("(%d %d) ", dol_history[i].x, dol_history[i].y);
		}*/
		//printf("\n점수 return : %d\n", temp);
		return temp;
	}
	eval = 1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -전에 내가 둔돌에서 5칸이하로 떨어진놈들 다 비교
		for (int j = -5;j <= 5;j++) {
			tx = past_x + i;
			ty = past_y + j;
			if (tx < 0 || tx >= 19 || ty < 0 || ty >= 19) continue;
			if (pan[tx][ty] == 0) {
				pan[tx][ty] = Black;
				if (win_check(tx, ty, Black)) {
					pan[tx][ty] = 0;
					return -500000;
				}
				dol_history[turn].x = tx;
				dol_history[turn].y = ty;
				turn++;
				max = max_value_white(alpha, beta, depth + 1);
				if (eval > max) {
					eval = max;
				}
				if (eval <= alpha) {	//prunning
					pan[tx][ty] = 0;
					turn--;
					//printf("prunning : %d\n", eval);
					return eval;
				}
				if (beta > eval) beta = eval;
				turn--;
				pan[tx][ty] = 0;
			}
		}
	}
	return eval;
}
// 평가함수 끝
int main() {
	int order = Black;
	init_pan();
	mode_select();
	printf("Time limit(단위 0.001초, 1000이상 입력하셔야 원활한 게임진행이 가능합니다) : ");
	scanf("%d", &time_limit);

	if (mode == 2) {		// 컴퓨터가 흑
		pan[9][9] = Black;
		order = White;
		dol_history[turn].x = 9;
		dol_history[turn].y = 9;
		turn++;

		while (1)
		{
			print_pan();
			printf("last limit : %d\n", limit);
			if (order == Black) {
				if (AI_minmax_Black()) break;
				order = White;
			}
			else if (order == White) {
				if (insert_dol(order)) break;
				order = Black;
			}
			system("cls");
		}
	}
	else if (mode == 1) {
		int tx, ty;
		int first_x, first_y;
		int min = 10000;
		print_pan();
		insert_dol(order);
		order = White;

		for (int i = 0;i < 8;i++) {					// 흑돌놓은거 중심으로 중앙에서 제일 가까운놈으로 선택됨 
			tx = dol_history[1].x + dx[i];
			ty = dol_history[1].y + dy[i];
			if ((abs(9 - tx) + abs(9 - ty)) < min) {
				min = abs(9 - tx) + abs(9 - ty);
				first_x = tx;
				first_y = ty;
			}
		}

		pan[first_x][first_y] = order;
		dol_history[turn].x = first_x;
		dol_history[turn].y = first_y;
		turn++;
		order = Black;
		system("cls");

		while (1)
		{
			print_pan();
			if (order == Black) {
				if (insert_dol(order)) break;
				order = White;
			}
			else if (order == White) {
				if (AI_minmax_White()) break;
				order = Black;
			}
			system("cls");
		}
	}
	else if (mode == 0) {
		while (1) {
			print_pan();
			if (order == White) {
				if (insert_dol(order))break;
				order = Black;
			}
			else if (order == Black) {
				if (insert_dol(order))break;
				order = White;
			}
			system("cls");
		}
	}
	else if (mode == 3) {
		pan[9][9] = Black;
		order = White;
		dol_history[turn].x = 9;
		dol_history[turn].y = 9;
		turn++;
		pan[8][8] = White;
		order = Black;
		dol_history[turn].x = 8;
		dol_history[turn].y = 8;
		turn++;
		while (1) {
			print_pan();
			if (order == Black) {
				if (AI_minmax_Black()) break;
				order = White;
			}
			else if (order == White) {
				if (AI_minmax_White()) break;
				order = Black;
			}
			//Sleep(1000);
			system("cls");
		}
	}

	system("cls");
	print_pan();
	if (order == Black) {
		printf("흑돌이 이겼습니다.\n");
	}
	else {
		printf("백돌이 이겼습니다.\n");
	}
}