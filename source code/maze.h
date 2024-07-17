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
    BiBFS,
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

struct astPoint
{
    int x, y; // 栅格行列
    astPoint(int x, int y) :x(x), y(y) {}; // 参数列表初始化
    double distance(astPoint& p)        // 求距离
    {
       // return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)); // 欧几里得距离
        return abs(x-p.x)+abs(y-p.y);
    }
    void setval(Point point)
    {
        x = point.col;
        y = point.row;
    }
};//Astar定义地图上的点

struct Node
{
    astPoint point; // 栅格点
    double g, h, f;// 代价值,f总价值,g到起点的代价值，h到终点的估计代价（启发式函数）
    Node* parent;// 父节点指针
    Node(astPoint point, double g, double h, Node* parent = nullptr) :point(point), g(g), h(h), f(g + h), parent(parent)
    {}
};// Astar定义节点

struct NodeCompare {
    bool operator()(Node* n1, Node* n2) {
        return (n1->f) < (n2->f); // 表示升序排列
    }
};//Astar 自定义Node*排序规则


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

    astPoint start{ 0,0 };//Astar 起点
    astPoint goal{ 0,0 };//Astar 终点

    vector<Node*> openlist; //Astar openlist 表示待搜索的节点
    vector<Node*> closelist;//Astar closelist表示已搜索的节点

    queue<Point> queueBFS;
    stack<Point> stackDFS;

    bool isMapSet;

    void initMap();//初始化map数组

    void searchFourCC(queue<Point>&, Point, std::queue<Point>&);//CC, connected components，搜索四连通域

    void searchFourCC(stack<Point> &, Point, std::queue<Point>&);//CC, connected components

    void searchFourCC_Bi(queue<Point> &, Point, std::queue<Point>&);

    float search_BFS();

    bool searchOneStep_BFS(queue<Point>&);

    float search_DFS();

    bool searchOneStep_DFS(queue<Point>&);

    float search_AST();

    bool searchOneStep_AST(queue<Point>&);

    float search_BiBFS();

    bool searchOneStep_BiBFS(queue<Point>&);

    void BiBFSConnect(Point , Point );

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

    void ast_pointset(Point startPoint, Point endPoint);//Astar 起点，终点坐标

    void printmap();

    void initAST();
};


#endif // MAZE_H
