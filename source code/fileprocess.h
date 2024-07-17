#pragma once
#include<maze.h>
#include<string>
#include<fstream>
#include <iostream>
using namespace std;
int saveMap(char** map,int row,int col, string filepath = "C:/Users/Lenovo/OneDrive/×ÀÃæ/1111/dataset/test.txt");
int readMap(char** &map,int &row,int &col, string filename = "C:/Users/Lenovo/OneDrive/×ÀÃæ/1111/dataset/test.txt");