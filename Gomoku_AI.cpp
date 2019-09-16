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

int pan[19][19];	// 0 �̸� ��ĭ, 1�̸� �� , 2�̸� ��
int mode = 0;
int turn = 1;
int limit = 3;
int next_sel_x = -1;
int next_sel_y = -1;
Point dol_history[300];

int dx[8] = { -1,1,0,0,-1,1,-1,1 };
int dy[8] = { 0,0,-1,1,-1,1,1,-1 };

// ���������� ���� Ž���Ҷ� ��
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
				printf("��");
			}
			else if (pan[i][j] == 1) {
				printf("��");
			}
			else if (pan[i][j] == 2) {
				printf("��");
			}
		}printf("\n");
	}
}

boolean insert_dol(int dol) {
	int x, y;
	if (dol == Black) {
		printf("�浹�� �����Դϴ�. �� ĭ�� ������ �ּ���\n");
		scanf("%d %d", &x, &y);
	}
	else if (dol == White) {
		printf("�鵹�� �����Դϴ�. �� ĭ�� ������ �ּ���\n");
		scanf("%d %d", &x, &y);
	}
	if (x >= 19 || x < 0 || y >= 19 || y < 0) {
		printf("������ �߸��Ǿ����ϴ�. �ٽ� �Է����ּ���(0-18 ���̷�...)");
		insert_dol(dol);

	}
	if (pan[x][y] == 0) {
		pan[x][y] = dol;
		dol_history[turn].x = x;
		dol_history[turn].y = y;
		turn++;

		if (is_DoubleThree(x, y, dol)) {
			printf("����Դϴ�. �ٽõμ���\n");
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
		printf("���� �ֽ��ϴ� �ٽ� �μ���\n");
		insert_dol(dol);
		return false;
	}
	return false;
}

void mode_select() {
	printf("��带 �����ϼ���(0-> ����ڵ��̼� �α�, 1-> ��ǻ�� �鵹, 2-> ��ǻ�� �浹 3->�ΰ����ɳ��� �α�)\n");
	scanf("%d", &mode);
	if (mode > 3 || mode < 0) {
		printf("�߸��Է��ϼ̽��ϴ�. 0, 1, 2, �߿� �ϳ��� �������ּ���.\n");
		mode_select();
	}
	system("cls");
}

int get_StoneCount(int x, int y, int color, int nDir) {	// ��� ���⿡ ���� ���� ����� ���ִ� �Լ�
	int cnt = 0;	// ���� ����
	int xd, yd;
	int tx, ty;
	tx = x;		//�ʱ��� x,y �����س��� ��
	ty = y;
	if (pan[x][y] == 0) {	// (x,y)�� ���� �ȳ����� ������ ����
		return 0;
	}
	xd = dx[nDir];
	yd = dy[nDir];
	for (;pan[x][y] == color;x += xd, y += yd)cnt++;

	//�ݴ��� ���� �����ϱ� ���ؼ� dx, dy�ٲ���;
	if (nDir % 2 == 1) {
		xd = dx[nDir - 1];
		yd = dy[nDir - 1];
	}
	else {
		xd = dx[nDir + 1];
		yd = dy[nDir + 1];
	}
	x = tx + xd;
	y = ty + yd;	// �ڱ��ڽ� �� �ȼ��� ���� ������
	for (;pan[x][y] == color;x += xd, y += yd)cnt++;

	return cnt;
}

boolean win_check(int x, int y, int color) {	//(x,y)�� ���� color���� 5�� �Ǹ� true����
	for (int i = 0;i < 8;i = i + 2) {
		if (get_StoneCount(x, y, color, i) == 5) {
			return true;
		}
	}
	return false;
}

// ���⼭ ���ʹ� ���Լ��� ���õ� ���Դϴ�.

Point is_Empty(int x, int y, int color, int nDir) {	// �� ���������� ���� empty�� ĭ�� ã���ִ°�, ���涧 �ٸ� ���� ���� ����� (-1,-1) ��ȯ
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

boolean is_Five(int x, int y, int color, int nDir) {	//nDir�� ���⿡ 5�ΰ� Ȯ��.
	if (get_StoneCount(x, y, color, nDir) == 5) return true;
	else return false;
}

boolean is_Four(int x, int y, int color, int nDir) { // nDir�� ������ 4�� �ִ��� Ȯ��, ���� 4���� ǥ�����ش�.
	int tx, ty; // ������ x y��ǥ
	if (nDir % 2 == 1) nDir = nDir - 1; // ������ ���ʹ��� �ٺ�����... ���������� ����..

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
			pan[temp.x][temp.y] = 0;	//���󺹱�
		}
	}
	return false;
}

boolean is_OpenFour(int x, int y, int color, int nDir) {	//�������� empty�� ���� ������ �� �Ѵ� 5�� �ɼ� ������, open 4��.
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
			pan[temp.x][temp.y] = 0;	//���󺹱�
		}
	}
	if (sum == 2)return true;
	else return false;
}

boolean is_OpenThree(int x, int y, int color, int nDir) {	//���� 3���� ��ȯ
	int tx, ty;

	if (nDir % 2 == 1) nDir = nDir - 1;	//������ ���ʰ˻��ϹǷ�..
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

boolean is_Three(int x, int y, int color, int nDir) {	//���� 3���� ��ȯ
	int tx, ty;
	int cnt = 0;
	if (nDir % 2 == 1) nDir = nDir - 1;	//������ ���ʰ˻��ϹǷ�..
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
	if (nDir % 2 == 1) nDir = nDir - 1;	//������ ���ʰ˻��ϹǷ�..

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
	printf("5���� : %d\n", five_count(x, y, color));
	printf("4���� : %d\n", four_count(x, y, color));
	printf("open4���� : %d\n", openfour_count(x, y, color));
	printf("open3���� : %d\n", openthree_count(x, y, color));
	printf("3���� : %d\n", three_count(x, y, color));
	printf("open2���� : %d\n", opentwo_count(x, y, color));
	printf("2���� : %d\n", two_count(x, y, color));
}

int evaluation(int color) {		// ���� �� ���¿� ���� ���� �������ִ� �Լ�, color�� ����..
	int open_two_cnt = 0;		// ������ ���� ���� �������ִ� ������
	int line_two_cnt = 0;
	int close_two_cnt = 0;
	int open_three_cnt = 0;
	int open_four_cnt = 0;
	int close_four_cnt = 0;
	int close_three_cnt = 0;
	int five_cnt = 0;

	int eval = 0;
	int now_x, now_y;

	for (int i = 1;i < turn;i++) {		// �� ������ ��� ���� ���ؼ� ����� �غ���. �׷��� �ǿ� ���� ������ �����Ѵ�! ���� �����ɸ�����... �Ф�.., ���߿� �Լ����ְ� ����ȭ���� �Ф�..
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
	printf("\n5���� : %d\n", five_cnt);
	printf("open4���� : %d\n", open_four_cnt);
	printf("close4���� : %d\n", close_four_cnt);
	printf("open3���� : %d\n", open_three_cnt);
	printf("close3���� : %d\n", close_three_cnt);
	printf("open2���� : %d\n", open_two_cnt);
	printf("close2���� : %d\n", close_two_cnt);
	*/

	eval += five_cnt * 500000;
	if (open_four_cnt >= 1) eval += 2000;	// ����4�ϳ�
	if (close_four_cnt >= 2) eval += 2000;	// ���� 4�ΰ�
	if (close_four_cnt == 1 && open_three_cnt >= 1) eval += 2000; // 4-3
	if (open_three_cnt >= 2) eval -= 100000;	// 33 ���� ���ΰ� ��!
												//if (open_two_cnt >= 2) eval += 1;
	eval = eval + close_two_cnt + open_two_cnt * 2 + close_three_cnt * 3 + open_three_cnt * 7 + close_four_cnt * 9;
	//line two����
	//printf("���� : %d\n", eval);
	return eval;
}
int op_evaluation(int color) {
	int open_three_cnt = 0;
	int open_four_cnt = 0;
	int close_four_cnt = 0;
	int five_cnt = 0;

	int eval = 0;
	int now_x, now_y;

	for (int i = 1;i < turn;i++) {		// �� ������ ��� ���� ���ؼ� ����� �غ���. dol_history�ȿ��ִ°�..
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

	//printf("���� : %d\n", eval);
	return eval;
}
/*
boolean computer_black_state_ver(int color) {	//���� ���� �а� �������� �д�. �ᱹ depth 2�� �Ȱ��� �Լ��ε� ����ӵ��� ���ΰ�..
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
int timeout = 0;		//timeout �Ǹ� 1�� ��

boolean AI_minmax_Black() {

	time = GetTickCount();
	timeout = 0;
	for (limit = 3;limit <= 1000;limit++) {
		max_value_black(-1000000, 1000000, 1);
		if (timeout == 0) {
			temp_x = next_x;
			temp_y = next_y;	// �Ѵܰ� ���������� ����
		}
		else if(timeout==1) break;
	}

	if (definite_x > 0 && definite_y > 0) {
		pan[definite_x][definite_y] = Black;
		return true;
	}
	//�ᱹ �Ϻ��ϰ� �Ϸ�� depth�� ���� ��ǻ�Ͱ� ��
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
		printf("\n���� ���� : %d\n", temp);*/
		return temp;
	}
	eval = -1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -���� ���� �е����� 5ĭ���Ϸ� ��������� �� ��
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
		//printf("\n���� return : %d\n", temp);
		return temp;
	}
	eval = 1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -���� ���� �е����� 5ĭ���Ϸ� ��������� �� ��
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

// ��ǻ�� white�϶� AI
boolean AI_minmax_White() {
	time = GetTickCount();
	timeout = 0;

	for (limit = 3;limit <= 1000;limit++) {
		max_value_white(-100000, 100000, 1);
		if (timeout == 0) {
			temp_x = next_x;
			temp_y = next_y;	// �Ѵܰ� ���������� ����
		}
		else if (timeout == 1) break;
	}
	if (definite_x > 0 && definite_y > 0) {	//������ �̱涧
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
		printf("\n���� ���� : %d\n", temp);*/
		return temp;
	}
	eval = -1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -���� ���� �е����� 5ĭ���Ϸ� ��������� �� ��
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
		//printf("\n���� return : %d\n", temp);
		return temp;
	}
	eval = 1000000;

	past_x = dol_history[turn - 2].x;
	past_y = dol_history[turn - 2].y;
	for (int i = -5;i <= 5;i++) {	//action -���� ���� �е����� 5ĭ���Ϸ� ��������� �� ��
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
// ���Լ� ��
int main() {
	int order = Black;
	init_pan();
	mode_select();
	printf("Time limit(���� 0.001��, 1000�̻� �Է��ϼž� ��Ȱ�� ���������� �����մϴ�) : ");
	scanf("%d", &time_limit);

	if (mode == 2) {		// ��ǻ�Ͱ� ��
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

		for (int i = 0;i < 8;i++) {					// �浹������ �߽����� �߾ӿ��� ���� ���������� ���õ� 
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
		printf("�浹�� �̰���ϴ�.\n");
	}
	else {
		printf("�鵹�� �̰���ϴ�.\n");
	}
}