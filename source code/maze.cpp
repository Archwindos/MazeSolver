#include "maze.h"
using namespace std::chrono;

Maze::Maze(int height, int length) {
    size_length = length;
    size_height = height;
    initMap();
}

Maze::~Maze() {}
void Maze::printmap()
{

    for (int i = 0; i < size_height + 2; i++)
    {
        for (int j = 0; j < size_length + 2;j++)
        {
            std::cout << map[i][j].type;
        }
        std::cout << endl;
    }
}

void Maze::initMap()
{
    int borderLength = size_length + 1;
    int borderHeight = size_height + 1;
    //初始化vector大小，避免频繁动态分配
    vector<Lattice> temp(borderLength+1);
    map.resize(borderHeight+1, temp);
    //为避免做边界判断，最外面加一圈墙
    for (int i = 0; i <= borderHeight; i++) {
        map[i][0].coordinate.col = i;
        map[i][0].coordinate.row = 0;
        map[i][0].isSearched = true;
        map[i][0].previous = NULL;
        map[i][0].type = '#';

        map[i][borderLength].coordinate.col = i;
        map[i][borderLength].coordinate.row = borderLength;
        map[i][borderLength].isSearched = true;
        map[i][borderLength].previous = NULL;
        map[i][borderLength].type = '#';
    }
    for (int j = 0; j <= borderLength; j++) {
        map[0][j].coordinate.col = 0;
        map[0][j].coordinate.row = j;
        map[0][j].isSearched = true;
        map[0][j].previous = NULL;
        map[0][j].type = '#';

        map[borderHeight][j].coordinate.col = borderHeight;
        map[borderHeight][j].coordinate.row = j;
        map[borderHeight][j].isSearched = true;
        map[borderHeight][j].previous = NULL;
        map[borderHeight][j].type = '#';
    }

    //一般的地图初始化，索引从1开始
    for (int i = 1; i <= size_height; i++) {
        for (int j = 1; j <= size_length; j++) {
            //coordinate是定死的
            map[i][j].coordinate.col = j;
            map[i][j].coordinate.row = i;

            map[i][j].isSearched = false;
            map[i][j].previous = NULL;
            map[i][j].type = '0';
        }
    }

    isMapSet = false;
    curAlgorithm = BFS;
}

int Maze::setMap(char** inputMap)
{
    startPoint.col = startPoint.row = -1;
    endPoint.col = endPoint.row = -1;

    for (int i = 1; i <= size_height; i++) {
        for (int j = 1; j <= size_length; j++) {
            map[i][j].isSearched = false;
            map[i][j].previous = NULL;
            map[i][j].type = inputMap[i-1][j-1];

            if (map[i][j].type == 's') {
                if (startPoint.col != -1) return -1;
                startPoint = map[i][j].coordinate;
                cout << startPoint.col << startPoint.row << endl;
            }
            if (map[i][j].type == 'e') {
                if (endPoint.col != -1) return -1;
                endPoint = map[i][j].coordinate;
            }
            if (map[i][j].type == '#') {
                map[i][j].isSearched = true;
            }
        }
    }

    initAST();
    searchTime = 0;
    stack<Point> intstk;
    stackDFS.swap(intstk);
    queue<Point> intque;
    queueBFS.swap(intque);
    isMapSet = true;
    return true;
}

int Maze::setAlgorithm(searchAlgorithm alg)
{
    curAlgorithm = alg;
    return 1;
}

int Maze::clearSearch()
{
    if (isMapSet == false) return -1;

    for (int i = 1; i <= size_height; i++) {
        for (int j = 1; j <= size_length; j++) {
            map[i][j].isSearched = false;
            map[i][j].previous = NULL;

            if (map[i][j].type == '#') {
                map[i][j].isSearched = true;
            }
            else {
                map[i][j].type = '0';
            }
        }
    }
    map[startPoint.row][startPoint.col].type = 's';
    map[endPoint.row][endPoint.col].type = 'e';
    initAST();
    searchTime = 0;
    stack<Point> intstk;
    stackDFS.swap(intstk);
    queue<Point> intque;
    queueBFS.swap(intque);

    return true;
}

int Maze::isWayFind()
{
    if (isMapSet == false) return -1;

    if (map[endPoint.row][endPoint.col].previous == NULL) return false;
    else return true;
}

int Maze::showWay(stack<Point>& output)
{
    if (isMapSet == false) return -1;

    Lattice* curLattice = &map[endPoint.row][endPoint.col];
    if (curLattice->previous == NULL) return false;

    while (curLattice != NULL) {
        Point toOutside;
        toOutside.col = curLattice->coordinate.col - 1;
        toOutside.row = curLattice->coordinate.row - 1;
        output.push(toOutside);
        curLattice = curLattice->previous;
    }

    return true;
}

int Maze::search()
{
    if (isMapSet == false) return -1;

    if (curAlgorithm == BFS) {
        return search_BFS();
    }
    else if (curAlgorithm == DFS) {
        return search_DFS();
    }
    else if (curAlgorithm == DBFS){
        return search_DBFS();
    }
    else if (curAlgorithm == AST) {
        return search_AST();
    }

    return -1;
}

int Maze::searchOneStep(queue<Point>& newSearchedPoint)
{
    if (isMapSet == false) return -1;

    if (curAlgorithm == BFS) {
        return searchOneStep_BFS(newSearchedPoint);
    }
    else if (curAlgorithm == DFS) {
        return searchOneStep_DFS(newSearchedPoint);
    }
    else if(curAlgorithm == DBFS){
        return searchOneStep_DBFS(newSearchedPoint);
    }
    else if (curAlgorithm == AST) {
        return searchOneStep_AST(newSearchedPoint);
    }
    return -1;
}

void Maze::searchFourCC(std::queue<Point> &queue, Point cur, std::queue<Point> &output){
    Point toSearch;
    toSearch.col = cur.col - 1;
    toSearch.row = cur.row;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
    toSearch.col = cur.col + 1;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
    toSearch.col = cur.col;
    toSearch.row = cur.row - 1;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
    toSearch.row = cur.row + 1;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
}

void Maze::searchFourCC(std::stack<Point> &stack, Point cur, std::queue<Point> &output){
    Point toSearch;
    toSearch.col = cur.col - 1;
    toSearch.row = cur.row;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        stack.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
    toSearch.col = cur.col + 1;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        stack.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
    toSearch.col = cur.col;
    toSearch.row = cur.row - 1;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        stack.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
    toSearch.row = cur.row + 1;
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        stack.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
    }
}

void Maze::searchFourCC_DBFS(std::queue<Point> &queue, Point cur, std::queue<Point> &output){
    Point toSearch;
    toSearch.col = cur.col - 1;
    toSearch.row = cur.row;

    char curType = map[cur.row][cur.col].type;
    char opposite;
    if(curType == 's') opposite = 'e';
    else opposite = 's';

    if(map[toSearch.row][toSearch.col].type == opposite){
        DBFSConnect(cur, toSearch);
        return;
    }
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
        map[toSearch.row][toSearch.col].type = curType;
    }
    toSearch.col = cur.col + 1;
    if(map[toSearch.row][toSearch.col].type == opposite){
        DBFSConnect(cur, toSearch);
        return;
    }
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
        map[toSearch.row][toSearch.col].type = curType;
    }
    toSearch.col = cur.col;
    toSearch.row = cur.row - 1;
    if(map[toSearch.row][toSearch.col].type == opposite){
        DBFSConnect(cur, toSearch);
        return;
    }
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
        map[toSearch.row][toSearch.col].type = curType;
    }
    toSearch.row = cur.row + 1;
    if(map[toSearch.row][toSearch.col].type == opposite){
        DBFSConnect(cur, toSearch);
        return;
    }
    if (map[toSearch.row][toSearch.col].isSearched == false) {
        queue.push(Point(toSearch.row, toSearch.col));
        map[toSearch.row][toSearch.col].isSearched = true;
        map[toSearch.row][toSearch.col].previous = &map[cur.row][cur.col];
        output.push(Point(toSearch.row-1, toSearch.col-1));
        map[toSearch.row][toSearch.col].type = 'p';
        map[toSearch.row][toSearch.col].type = curType;
    }
}

float Maze::search_BFS()
{
    auto start = steady_clock::now();

    queueBFS.push(startPoint);
    map[startPoint.row][startPoint.col].isSearched = true;
    while (queueBFS.empty() == false) {
        //赋值，出队
        Point cur = queueBFS.front();
        queueBFS.pop();
        queue<Point> output;
        searchFourCC(queueBFS, cur, output);
        //是否搜到了出口
        if (isWayFind() == true) break;
    }

    auto end = steady_clock::now();
    auto last = duration_cast<microseconds>(end - start);
    return last.count();
}

bool Maze::searchOneStep_BFS(queue<Point>& output)
{
    //第一次把入口放进来
    if (map[startPoint.row][startPoint.col].isSearched == false) {
        queueBFS.push(startPoint);
        map[startPoint.row][startPoint.col].isSearched = true;
    }
    if(queueBFS.empty()) return true;
    Point cur = queueBFS.front();
    queueBFS.pop();
    searchFourCC(queueBFS, cur, output);
    return false;
}

float Maze::search_DFS()
{
    auto start = steady_clock::now();

    stackDFS.push(startPoint);
    map[startPoint.row][startPoint.col].isSearched = true;
    while (stackDFS.empty() == false) {
        //赋值，出栈
        Point cur = stackDFS.top();
        stackDFS.pop();
        queue<Point> output;
        searchFourCC(stackDFS, cur, output);
        //是否搜到了出口
        if (isWayFind() == true) break;
    }

    auto end = steady_clock::now();
    auto last = duration_cast<microseconds>(end - start);
    return last.count();
}

bool Maze::searchOneStep_DFS(queue<Point>& output)
{
    //第一次把入口放进来
    if (map[startPoint.row][startPoint.col].isSearched == false) {
        stackDFS.push(startPoint);
        map[startPoint.row][startPoint.col].isSearched = true;
    }
    if(stackDFS.empty()) return true;
    Point cur = stackDFS.top();
    stackDFS.pop();
    searchFourCC(stackDFS, cur, output);

    return false;
}

void Maze::DBFSConnect(Point tail1, Point tail2){
    Lattice* positiveTail;
    Lattice* negetiveTail;
    Lattice* pre;
    if(map[tail1.row][tail1.col].type == 's'){
        positiveTail = &map[tail1.row][tail1.col];
        negetiveTail = &map[tail2.row][tail2.col];
    }else{
        positiveTail = &map[tail2.row][tail2.col];
        negetiveTail = &map[tail1.row][tail1.col];
    }
    pre = negetiveTail->previous;

    while(pre != NULL){
        negetiveTail->previous = positiveTail;
        positiveTail = negetiveTail;
        negetiveTail = pre;
        pre = negetiveTail->previous;
    }
    negetiveTail->previous = positiveTail;
}

float Maze::search_DBFS()
{
    auto start = steady_clock::now();

    queue<Point> queueDBFS;
    queueDBFS.push(startPoint);
    map[startPoint.row][startPoint.col].isSearched = true;
    queue<Point> queueDBFSInverse;
    queueDBFSInverse.push(endPoint);
    map[endPoint.row][endPoint.col].isSearched = true;

    while (queueDBFS.empty() == false || queueDBFSInverse.empty() == false) {
        queue<Point> output;
        Point cur = queueDBFS.front();
        queueDBFS.pop();
        searchFourCC_DBFS(queueDBFS, cur, output);
        if (isWayFind() == true) break;

        cur = queueDBFSInverse.front();
        queueDBFSInverse.pop();
        searchFourCC_DBFS(queueDBFSInverse, cur, output);
        if (isWayFind() == true) break;
    }

    auto end = steady_clock::now();
    auto last = duration_cast<microseconds>(end - start);
    return last.count();
}

bool Maze::searchOneStep_DBFS(queue<Point>& output){
    //第一次把入口放进来
    static queue<Point> queueDBFS;
    static queue<Point> queueDBFSInverse;
    if (map[startPoint.row][startPoint.col].isSearched == false) {
        queueDBFS = queue<Point>();
        queueDBFSInverse = queue<Point>();
        queueDBFS.push(startPoint);
        map[startPoint.row][startPoint.col].isSearched = true;
        queueDBFSInverse.push(endPoint);
        map[endPoint.row][endPoint.col].isSearched = true;
    }
    if(queueDBFS.empty() && queueDBFSInverse.empty()) return true;

    Point cur = queueDBFS.front();
    queueDBFS.pop();
    searchFourCC_DBFS(queueDBFS, cur, output);
    if (isWayFind() == true) return false;

    cur = queueDBFSInverse.front();
    queueDBFSInverse.pop();
    searchFourCC_DBFS(queueDBFSInverse, cur, output);

    return false;
}


float Maze::search_AST()
{
    auto start = steady_clock::now();

    while (!openlist.empty())
    {
        sort(openlist.begin(), openlist.end(), NodeCompare{});
        Node* current = *openlist.begin();
        openlist.erase(openlist.begin());
        closelist.push_back(current);
        map[current->point.y][current->point.x].isSearched = true;
        //map[current->point.x][current->point.y].type = '.';//此处直接更改了格子的类型
        if (isWayFind() == true)
        {
            openlist.clear();
            closelist.clear();
            break;
        }
        int x = current->point.x;
        int y = current->point.y;
        vector<astPoint> neighbors =
            { // 4个邻近节点的坐标
                { x - 1,y },
                { x,y - 1 }, { x,y + 1 },
                { x + 1,y }
            };

        for (auto n : neighbors)
        {
            if ((n.x >= 1 && n.x <= size_length) && (n.y >= 1 && n.y <= size_height) && map[n.y][n.x].type != '#')
            {
                bool incloselist = false;
                for (auto c : closelist)
                {
                    if (c->point.x == n.x && c->point.y == n.y)
                    {
                        incloselist = true;
                        break;
                    }
                }
                if (incloselist)
                {
                    continue;
                }

                bool inopenlist = false;
                for (auto o : openlist)
                {
                    if (o->point.x == n.x && o->point.y == n.y)
                    {
                        inopenlist = true;

                        double g = current->g + n.distance(current->point);
                        double h = n.distance(goal);
                        double f = g + h;
                        if (f < (o->f))
                        {
                            o->f = f;
                            o->parent = current;
                            map[n.y][n.x].previous = &map[current->point.y][current->point.x];

                        }
                        break;
                    }
                }
                if (!inopenlist)
                {
                    double g = current->g + n.distance(current->point);
                    double h = n.distance(goal);
                    double f = g + h;
                    openlist.push_back(new Node(n, g, h, current));
                    map[n.y][n.x].previous = &map[current->point.y][current->point.x];
                }
            }
        }

    }

    auto end = steady_clock::now();
    auto last = duration_cast<microseconds>(end - start);
    return last.count();
}

bool Maze::searchOneStep_AST(queue<Point>& newSearchedPoint)
{
    sort(openlist.begin(), openlist.end(), NodeCompare{});
    Node* current = *openlist.begin();
    openlist.erase(openlist.begin());
    closelist.push_back(current);
    map[current->point.y][current->point.x].isSearched = true;
    //map[current->point.x][current->point.y].type='.';//此处直接更改了格子的类型

    Point a(current->point.y-1,current->point.x-1);

    if(start.x!=current->point.x||start.y!=current->point.y)
        newSearchedPoint.push(a);
    if (isWayFind() == true)
    {
        openlist.clear();
        closelist.clear();

        return true;
    }
    int x = current->point.x;
    int y = current->point.y;
    vector<astPoint> neighbors =
        { // 4个邻近节点的坐标
            { x - 1,y },
            { x,y - 1 }, { x,y + 1 },
            { x + 1,y }
        };

    for (auto n : neighbors)
    {
        if ((n.x >= 1 && n.x <= size_length) && (n.y >= 1 && n.y <= size_height) && map[n.y][n.x].type != '#')
        {

            bool incloselist = false;
            for (auto c : closelist)
            {
                if (c->point.x == n.x && c->point.y == n.y)
                {
                    incloselist = true;
                    break;
                }
            }
            if (incloselist)
            {
                continue;
            }

            bool inopenlist = false;
            for (auto o : openlist)
            {
                if (o->point.x == n.x && o->point.y == n.y)
                {
                    inopenlist = true;

                    double g = current->g + n.distance(current->point);
                    double h = n.distance(goal);
                    double f = g + h;
                    if (f < (o->f))
                    {
                        o->f = f;
                        o->parent = current;
                        map[n.y][n.x].previous = &map[current->point.y][current->point.x];
                    }
                    break;
                }
            }
            if (!inopenlist)
            {
                double g = current->g + n.distance(current->point);
                double h = n.distance(goal);
                double f = g + h;
                openlist.push_back(new Node(n, g, h, current));
                map[n.y][n.x].previous = &map[current->point.y][current->point.x];
            }
        }
    }
    return false;
}

void Maze::ast_pointset(Point startPoint, Point endPoint)
{
    start.setval(startPoint);
    goal.setval(endPoint);
}
void Maze::initAST()
{
    openlist.clear();
    closelist.clear();
    ast_pointset(startPoint, endPoint);
    openlist.push_back(new Node(start, start.distance(start), start.distance(goal)));

}
