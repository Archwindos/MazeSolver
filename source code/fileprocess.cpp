
#include<string>
#include<fstream>
#include <iostream>
#include<fileprocess.h>
#include<limits>
using namespace std;
int saveMap(char** map,int row,int col, string filepath)//txt�ļ���ʽ����һ�д���У���룬�ڶ����к��У���������Թ�
{
    ofstream fout;                                      //����Ƶ��Թ�����txt
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

int readMap(char**& map, int &row,int &col,std::string filepath)//���Թ�ȡ�����浽**mapָ��Ķ�ά���顣
{
    std::ifstream fin;
    //std::string filepath = ".//dataset//" + filepath;
    fin.open(filepath);
    if (!fin.is_open()) {
        std::cerr << "Unable to open file" << filepath << std::endl;
        return -1;
    }
    
    string mima;//У���룬���Ƿ���Թ��ļ�
    getline(fin, mima);
    if (mima != "C++migongxianren")
    {
        cout << "�򿪴����ļ�" << endl;
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
            // �����ļ��������ȡ����  
            std::cerr << "Failed to read line " << i + 1 << std::endl;
            // �ͷ��ѷ�����ڴ沢���ش���  
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
            map[i][j] = buffer[j]; // ֱ�Ӵ�buffer�и����ַ�  
        }
        map[i][col] = '\0';// ����ַ�����ֹ��  
    }
    fin.close();
    return 1;
}

//int main()
//{
//    char mapa[3][4] = { {'#','#','#','#'}, {'e','#','#','#'}, {'2','#','#','#'} };
//    char* map_ptrs[3];
//    for (int i = 0; i < 3; i++) {
//        map_ptrs[i] = mapa[i]; // �� mapa ��ÿһ�еĵ�ַ���� map_ptrs  
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
//        // �ͷŶ�̬������ڴ�  
//        for (int i = 0; map[i] != nullptr; i++) {
//            delete[] map[i];
//        }
//        delete[] map;
//    }
//    return 0;
//}
