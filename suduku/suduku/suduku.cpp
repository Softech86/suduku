#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <string>

#define True true
#define False false

using namespace std;

class Suduku {
	private:
		int data[10][10] = { { 0 } };
		bool findZero(int &x, int &y);
		bool numLegal(const int x, const int y, const int val);
	public:
		bool check();
		void load(const char* route);
		bool solve(const int depth);
		bool fill(const int x, const int y, const int val);
		void print();
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
		for (int j = 0; j < 9; ++j)
			data[i][j] = int(s[j]) - 48;
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

bool Suduku::fill(const int x, const int y, const int val) {
	if (data[x][y] || numLegal(x, y, val))
		return False;
	data[x][y] = val;
	return True;
}

void Suduku::print() {
	for (int i = 0; i < 9; ++i) {
		if (!(i % 3))
			cout << "+-------+-------+-------+" << endl;
		cout << "| ";
		for (int j = 0; j < 9; ++j) {
			cout << data[i][j] << (j == 2 || j == 5 ? " | " : " ");
		}
		cout << "|";
		cout << endl;
	}
	cout << "+-------+-------+-------+" << endl;
}

int main() {
	cout << "SUDUKU!" << endl;
	Suduku suduku;
	suduku.load("data/suduku_1.txt");
	assert(suduku.check());
	suduku.solve(0);
	suduku.print();
	system("pause");
}