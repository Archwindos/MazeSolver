#ifndef MAZE_H
#define MAZE_H
#pragma once

#include <stack>
#include <queue>
#include <vector>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <QDebug>
#include <chrono>
using namespace std;

enum searchAlgorithm {
    DFS,
    BFS,
    AST,
    DBFS,
    IDAST
};

struct Point {
    int row;
    int col;
    Point(int r = -1, int c = -1) :row(r), col(c) {};
};//点坐标

struct Lattice {
    Point coordinate;
    char type=0;
    //'0'表示自由空间，'#'表示墙体，'s'表示起点，'e'表示终点，'@'表示找到的路，'.'表示已经搜索的格
    bool isSearched;
    struct Lattice* previous;
};//地图格子

struct ASTPoint{
    int row;
    int col;
    int g;
    int f;
    ASTPoint(int r = -1, int c = -1, int gx = 0, int fx = 0) :row(r), col(c), g(gx), f(fx) {};
    ASTPoint(Point a, int gx = 0, int fx = 0) :row(a.row), col(a.col), g(gx), f(fx) {};
};


class Maze {
    // 动态分配二维字符数组内存
protected:
    int size_length;
    int size_height;
    float searchTime;
    Point startPoint;
    Point endPoint;
    searchAlgorithm curAlgorithm;
    vector<vector<Lattice>> map;

    bool isMapSet;

    void initMap();//初始化map数组

    void searchFourCC(queue<Point>&, Point, std::queue<Point>&);//CC, connected components，搜索四连通域

    void searchFourCC(stack<Point>&, Point, std::queue<Point>&);

    void searchFourCC(std::vector<ASTPoint>&, ASTPoint, std::queue<Point>&);

    void searchFourCC_DBFS(std::queue<Point>&, Point , std::queue<Point>&);

    float search_BFS();

    bool searchOneStep_BFS(queue<Point>&);

    float search_DFS();

    bool searchOneStep_DFS(queue<Point>&);

    float search_AST();

    int heuristic_AST (Point, Point);

    bool searchOneStep_AST(queue<Point>&);

    float search_DBFS();

    bool searchOneStep_DBFS(queue<Point>&);

    void DBFSConnect(Point , Point );

    float search_IDAST();

    bool searchOneStep_IDAST(queue<Point>&);

public:
    Maze(int Size_X, int Size_Y);

    ~Maze();

    int getLength() const {
        return size_length;
    }

    int getHeight() const {
        return size_height;
    }

    searchAlgorithm getAlgorithm() {
        return curAlgorithm;
    }

    int setMap(char** inputMap);//从外部输入地图

    int setAlgorithm(searchAlgorithm);

    int clearSearch();//清除搜索相关的信息，但保留地图结构

    int isWayFind();

    int showWay(stack<Point>&);//返回路径

    int search();//返回计算时间

    int searchOneStep(queue<Point>&);//用于展示搜索过程，返回是否完成搜索

    void printmap();
};


#endif // MAZE_H
