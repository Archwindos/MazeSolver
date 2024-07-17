
#include<string>
#include<fstream>
#include <iostream>
#include<fileprocess.h>
#include<limits>
using namespace std;
int saveMap(char** map,int row,int col, string filepath)//txt文件格式：第一行存入校验码，第二行行和列，后面存入迷宫
{
    ofstream fout;                                      //将设计的迷宫存入txt
    //string filepath = ".//dataset//" + filepath;
    fout.open(filepath);
    if (!fout.is_open()) {
        std::cerr << "Unable to open file"+filepath;
        return -1;
    }
    fout << "C++migongxianren" << endl;
    fout << row << " " << col << endl;
    for (int i = 0; i<row; i++)
    {
        for (int j = 0; j<col; j++)
        {
            fout << map[i][j];

        }
        fout << endl;
    }
    
    fout.close();
    return 1;

}

int readMap(char**& map, int &row,int &col,std::string filepath)//将迷宫取出，存到**map指向的二维数组。
{
    std::ifstream fin;
    //std::string filepath = ".//dataset//" + filepath;
    fin.open(filepath);
    if (!fin.is_open()) {
        std::cerr << "Unable to open file" << filepath << std::endl;
        return -1;
    }
    
    string mima;//校验码，看是否打开迷宫文件
    getline(fin, mima);
    if (mima != "C++migongxianren")
    {
        cout << "打开错误文件" << endl;
        return -1;
    }
    fin >> row;
    fin >> col;
    string buffer;
    getline(fin,buffer);
    map = new char* [row];
    for (int i = 0; i < row; ++i) {
        map[i] = new char[col+1]; 
    }
    for (int i = 0; i < row; i++)
    {
        
       
        if (!std::getline(fin, buffer)) {
            // 处理文件结束或读取错误  
            std::cerr << "Failed to read line " << i + 1 << std::endl;
            // 释放已分配的内存并返回错误  
            for (int j = 0; j < i; j++) {
                delete[] map[j];
            }
            delete[] map;
            return -1;
        }
        for (int j = 0; j < col; j++)
        {
            if (buffer.size() != col) {
                cerr << "Line " << i + 1 << " does not have the expected length." << endl;
                return -1;
            }
            map[i][j] = buffer[j]; // 直接从buffer中复制字符  
        }
        map[i][col] = '\0';// 添加字符串终止符  
    }
    fin.close();
    return 1;
}

//int main()
//{
//    char mapa[3][4] = { {'#','#','#','#'}, {'e','#','#','#'}, {'2','#','#','#'} };
//    char* map_ptrs[3];
//    for (int i = 0; i < 3; i++) {
//        map_ptrs[i] = mapa[i]; // 将 mapa 的每一行的地址赋给 map_ptrs  
//    }
//    char** map = map_ptrs;
//    saveMap(map, 3, 4, "text.txt");
//    
//    int row = 0;
//    int col = 0;
//    if (readMap(map,row,col, "text.txt") == 1) {
//        for (int i = 0; i<row; i++)
//        {
//            for (int j = 0; j<col; j++)
//            {
//                std::cout << map[i][j];
//            }
//            std::cout << std::endl;
//        }
//
//        // 释放动态分配的内存  
//        for (int i = 0; map[i] != nullptr; i++) {
//            delete[] map[i];
//        }
//        delete[] map;
//    }
//    return 0;
//}
