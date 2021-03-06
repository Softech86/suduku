#include <iostream>
#include <fstream>
#include <conio.h>
#include <cassert>
#include <cstring>
#include <string>
#include <windows.h>

#define True true
#define False false

using namespace std;

int color[10] = { 0, 15, 14, 5, 8, 4, 10, 11, 12, 13 };

void gotoxy(HANDLE hout, const int X, const int Y)
{
	COORD coord;
	coord.X = X;
	coord.Y = Y;
	SetConsoleCursorPosition(hout, coord);
}

/* 设置指定的颜色

颜色的取值由背景色+前景色组成，各16种
fg_color：0-7    黑   蓝   绿   青   红   粉   黄   白
8-15 亮黑 亮蓝 亮绿 亮青 亮红 亮粉 亮黄 亮白
bg_color：0-7    黑   蓝   绿   青   红   粉   黄   白
8-15 亮黑 亮蓝 亮绿 亮青 亮红 亮粉 亮黄 亮白
最终的颜色为 背景色*16+前景色 */
void setcolor(HANDLE hout, const int bg_color, const int fg_color)
{
	SetConsoleTextAttribute(hout, bg_color * 16 + fg_color);
}

/* 在指定位置，用指定颜色，显示一个字符 */
void showch(HANDLE hout, const int X, const int Y, const int bg_color, const int fg_color, const char ch, const int n)
{
	int i;
	gotoxy(hout, X, Y);
	setcolor(hout, bg_color, fg_color);

	/* 循环n次，打印字符ch */
	for (i = 0; i<n; i++)
		putchar(ch);
}

bool pushxy(HANDLE hout, const int i, const int j, const int bg_color, const int fg_color, const char ch) {
	//if (i < 0 || i > 8 || j < 0 || j > 8)
	//	return False;
	int px = 2 * j + 5 + 2 * int(j / 3), \
		py = i + 2 + int(i / 3);
	showch(hout, px, py, bg_color, fg_color, ch, 1);
	return True;
}

//--------------------------

class Suduku {
	private:
		int data[10][10] = { { 0 } };
		bool isOriginal[10][10] = { {0} };
		bool findZero(int &x, int &y);
		bool numLegal(const int x, const int y, const int val);
		bool fill(const int x, const int y, const int val);
		bool original(const int x, const int y);
	public:
		bool check();
		void load(const char* route);
		bool solve(const int depth);
		bool fillin(HANDLE hout);
		void print(HANDLE hout);
};

bool Suduku::findZero(int &x, int &y) {
	for (x = 0; x < 9; ++x)
		for (y = 0; y < 9; ++y)
			if (!data[x][y])
				return True;
	x = y = -1;
	return False;
}

bool Suduku::numLegal(const int x, const int y, const int val) {
	//if (data[x][y])
	//	return False;
	for (int i = 0; i < 9; ++i) {
		if ((i != y && data[x][i] == val) || (x != i && data[i][y] == val))
			return False;
	}
	
	int xFrom = int(x / 3) * 3, yFrom = int(y / 3) * 3;
	int xTo = xFrom + 3, yTo = yFrom + 3;
	for (int i = xFrom; i < xTo; ++i)
		for (int j = yFrom; j < yTo; ++j)
			if ((x != i || y != j) && data[i][j] == val)
				return False;

	return True;
}

bool Suduku::check() {
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			if (data[i][j] && !numLegal(i, j, data[i][j])) {
				cout << i << j << endl;
				return False;
			}
	return True;
}

void Suduku::load(const char* route) {
	ifstream fin;
	fin.open(route);
	for (int i = 0; i < 9; ++i) {
		string s;
		fin >> s;
		for (int j = 0; j < 9; ++j) {
			data[i][j] = int(s[j]) - 48;
			isOriginal[i][j] = (data[i][j] != 0);
		}
	}
	fin.close();
}

bool Suduku::solve(const int depth) {
	/*int i, j;
	while (findZero(i, j)) {
		cout << i << j << "\n";
		for (int v = 1; v <= 9; ++v)
			cout << (numLegal(i, j, v) ? v : 0);
		cin.get();
		data[i][j] = -1;
	}*/
	int i, j;
	if (!findZero(i, j))
		return True;
	for (int v = 1; v <= 9; ++v) {
		if (numLegal(i, j, v)) {
			data[i][j] = v;
			if (solve(depth + 1))
				return True;
			data[i][j] = 0;
		}
	}
	return False;
}

bool Suduku::original(const int x, const int y) {
	return isOriginal[x][y];
}

bool Suduku::fill(const int x, const int y, const int val) {
	if (data[x][y] || numLegal(x, y, val))
		return False;
	data[x][y] = val;
	return True;
}

bool Suduku::fillin(HANDLE hout) {
	print(hout);
	int tx, ty;
	int xNow = -1, yNow;

	while (findZero(tx, ty)) {
		if (xNow < 0) {
			xNow = tx;
			yNow = ty;
		}
		pushxy(hout, xNow, yNow, 15, color[data[xNow][yNow]], data[xNow][yNow] + 48);
		int command = getch();
		switch (command) {
		case 72: {
			if (xNow != 0) {
				pushxy(hout, xNow, yNow, 0, color[data[xNow][yNow]], data[xNow][yNow] + 48);
				--xNow;
				pushxy(hout, xNow, yNow, 15, color[data[xNow][yNow]], data[xNow][yNow] + 48);
			}
			break;
		}
		case 80: {
			if (xNow != 8) {
				pushxy(hout, xNow, yNow, 0, color[data[xNow][yNow]], data[xNow][yNow] + 48);
				++xNow;
				pushxy(hout, xNow, yNow, 15, color[data[xNow][yNow]], data[xNow][yNow] + 48);
			}
			break;
		}
		case 75: {
			if (yNow != 0) {
				pushxy(hout, xNow, yNow, 0, color[data[xNow][yNow]], data[xNow][yNow] + 48);
				--yNow;
				pushxy(hout, xNow, yNow, 15, color[data[xNow][yNow]], data[xNow][yNow] + 48);
			}
			break;
		}
		case 77: {
			if (yNow != 8) {
				pushxy(hout, xNow, yNow, 0, color[data[xNow][yNow]], data[xNow][yNow] + 48);
				++yNow;
				pushxy(hout, xNow, yNow, 15, color[data[xNow][yNow]], data[xNow][yNow] + 48);
			}
			break;
		}
		case 48:case 49:case 50:case 51:case 52:case 53:case 54:case 55:case 56:case 57: {
			if (original(xNow, yNow))
				break;

			data[xNow][yNow] = command - 48;
			int bg = (!data[xNow][yNow] || numLegal(xNow, yNow, data[xNow][yNow]) ? 0 : 4);
			pushxy(hout, xNow, yNow, bg, color[data[xNow][yNow]], data[xNow][yNow] + 48);
			break;
		}
		default:;
		}

		//check after every move
		for (int i = 0; i < 9; ++i)
			for (int j = 0; j < 9; ++j) 
				if (data[i][j] && !numLegal(i, j, data[i][j]))
					pushxy(hout, i, j, (original(i, j) ? 3 : 5), color[data[i][j]], data[i][j] + 48);
				else
					pushxy(hout, i, j, 0, color[data[i][j]], data[i][j] + 48);
	}
	return True;
}

void Suduku::print(HANDLE hout) {

	cout << "           --SUDUKU--" << endl;
	for (int i = 0; i < 9; ++i) {
		if (!(i % 3))
			cout << "   +-------+-------+-------+ " << endl;
		cout << "   |       |       |       | " << endl;
	}
	cout << "   +-------+-------+-------+ " << endl;

	
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			pushxy(hout, i, j, 0, color[data[i][j]], data[i][j] + 48);
		}
	}
	gotoxy(hout, 0, 14);
}

//--------------------------

char *init(HANDLE hout) {
	char fileName[100][100] = { "" };
	int fileNum = 0;
	//获取目录文件
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(".\\data\\*.txt", &fd);
	while (hFind != INVALID_HANDLE_VALUE) {
		strcpy(fileName[fileNum++], fd.cFileName);
		if (!FindNextFile(hFind, &fd))
			break;
	}
	FindClose(hFind);

	system("mode con cols=31 lines=16");
	cout << "选择文件名:" << endl;
	for (int i = 0; i < fileNum; ++i)
		cout << fileName[i] << endl;

	const int dist = 1;
	int rowNow = 0;
	char cmd = '0';
	while (cmd - 13) {
		int rowBefore = rowNow;
		switch (cmd) {
		case 72: {
			rowNow = (rowNow - 1 + fileNum) % fileNum;
			break;
		}
		case 80: {
			rowNow = (rowNow + 1) % fileNum;
			break;
		}
		default:;
		}
		//cmd = '0';

		for (int t = 0; t < strlen(fileName[rowBefore]); ++t)
			showch(hout, t, rowBefore + dist, 0, 15, fileName[rowBefore][t], 1);
		for (int t = 0; t < strlen(fileName[rowNow]); ++t)
			showch(hout, t, rowNow + dist, 15, 0, fileName[rowNow][t], 1);
		char p;
		cmd = getch();
	}

	Sleep(100);
	for (int i = 0; i < 2; ++i) {
		for (int t = 0; t < strlen(fileName[rowNow]); ++t)
			showch(hout, t, rowNow + dist, 0, 15, fileName[rowNow][t], 1);
		Sleep(200);
		for (int t = 0; t < strlen(fileName[rowNow]); ++t)
			showch(hout, t, rowNow + dist, 15, 0, fileName[rowNow][t], 1);
		Sleep(200);
	}
	showch(hout, 0, 0, 0, 15, ' ', 1);
	system("cls");
	return fileName[rowNow];
}

int main() {
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE); //取标准输出设备对应的句柄
	//char* fileName = init(hout);
	//string path = init(hout);
	//path = "data/" + path;
	string path = "data/suduku_1.txt";
	Suduku suduku;
	suduku.load(path.c_str());
	assert(suduku.check());
	//suduku.solve(0);
	suduku.fillin(hout);
	//suduku.print(hout);
	//system("pause");
	while (1);
}