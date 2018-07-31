// EastFlow.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include <stdlib.h>
#include "stdafx.h"
#include <fstream>

using namespace std;

const int MAX_N = 8;
const char* colorRefer = "RBGY";
bool flag = false; //mark whether the solution is found

char graph[MAX_N + 5][MAX_N + 5];
char grap[MAX_N + 5][MAX_N + 5];

int gr[] = { 0, 1, 0, -1 }, gc[] = { 1, 0, -1, 0 }; //moving direction arrays
int color[MAX_N][MAX_N] = { 0 };

//STL
map<char, int> toNum;
vector<pair<int, int> > paint[4 + 5];

bool vis(int row, int column) {
	//check if element graph[row][column] is visited
	return color[row][column]; //state compression
}

void printGraph() {
	//repaint the graph
	for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++) {
			grap[i][j] = graph[i][j];
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < paint[i].size(); j++) {
			int x = paint[i][j].first, y = paint[i][j].second;
			grap[x][y] = colorRefer[i];
		}
	}
	printf("\n");
	//print the solution
	/*	for (int i = 0; i < MAX_N; i++) {
			for (int j = 0; j < MAX_N; j++) {
				printf("%c", grap[i][j]);
			}
			printf("\n");
		} */
}


void printStatus() {

	for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++) {
			printf("%d", color[i][j]);
		}
		printf("\n");
	}
}


void dfs(int row, int column, char ch) {

	if (flag) return;

	int newRow, newCol;
	for (int k = 0; k < 4; k++) {
		newRow = row + gr[k];
		newCol = column + gc[k];
		if (0 <= newRow && newRow < MAX_N && 0 <= newCol && newCol < MAX_N && !vis(newRow, newCol)) {

			if (graph[newRow][newCol] == 'W') {
				color[newRow][newCol] = 1;
				paint[toNum[ch]].push_back(make_pair(newRow, newCol));
				dfs(newRow, newCol, ch);
				if (flag) return;

				paint[toNum[ch]].pop_back();
				color[newRow][newCol] = 0;
			}

			else if (graph[newRow][newCol] == ch && !vis(newRow, newCol)) {
				//reached the same color, path found
				color[newRow][newCol] = 1;
				bool finished = true;

				//find other colors' paths
				for (int i = 0; i < MAX_N && finished; i++) {
					for (int j = 0; j < MAX_N && finished; j++) {
						if (graph[i][j] != 'W' && !vis(i, j)) {
							color[i][j] = 1;
							dfs(i, j, graph[i][j]);

							if (flag) return;
							finished = false;
							color[i][j] = 0;
						}
					}
				}
				if (finished) {
					bool full = true;
					for (int i = 0; i < MAX_N && full; i++) {
						for (int j = 0; j < MAX_N && full; j++) {
							if (!vis(i, j)) {
								full = false;
							}
						}
					}
					if (full) {
						flag = true;
						printGraph();
						return;
					}
					else {
						color[newRow][newCol] = 0;
					}
				}
				else {
					color[newRow][newCol] = 0;
				}
			}
		}
	}
}

int main() {
	//read input && init

	// ifstream infile;
	ofstream outfile;
	// infile.open("D:\\in.txt");
	outfile.open("D:\\out.txt");

	/*for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++)
			infile >> graph[i][j];
	}

	 for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++)
			std::cout << graph[i][j];
		std::cout << "\n";
	} */ //test if file is read
	std::cout << "\n"; 
	std::cout << "       _|_|_|_|                        _|      _|_|_|_|  _|                                \n";
	std::cout << "      _|          _|_|_|    _|_|_|  _|_|_|_|  _|        _|    _|_|    _|      _|      _|  \n";
	std::cout << "     _|_|_|    _|    _|  _|_|        _|      _|_|_|    _|  _|    _|  _|      _|      _|  \n";
	std::cout << "    _|        _|    _|      _|_|    _|      _|        _|  _|    _|    _|  _|  _|  _|    \n";
	std::cout << "   _|_|_|_|    _|_|_|  _|_|_|        _|_|  _|        _|    _|_|        _|      _|      \n";
	std::cout << "\n"; 
	std::cout << "EastFlow Console, ver 7/21.\n";
	std::cout << "Just Right-Click:\n";

	for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++) {
			scanf("%c", &graph[i][j]);
		}
		getchar();
	} 

	std::cout << "\nCalculatng, please wait...\n";

	toNum['R'] = 0;
	toNum['B'] = 1;
	toNum['G'] = 2;
	toNum['Y'] = 3;

	//find solution
	for (int i = 0; i < MAX_N && !flag; i++) {
		for (int j = 0; j < MAX_N && !flag; j++) {
			if (graph[i][j] != 'W') {
				color[i][j] = 1;
				dfs(i, j, graph[i][j]);
			}
		}
	}

	if (!flag) printf("\nSolution Not Found.\n");
	else 
	{
		printf("\n");
		printf("=============================================\n");
		printf("FINISHED! Load in UI now.\n");
		printf("=============================================\n");
		printf("\n");
		printf("\n");
	}
	 for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++) {
			outfile << grap[i][j];
		}
		outfile << "\n";
	} 
	outfile.close();
	system("CLIP < D:\\out.txt");
	return 0;
}


