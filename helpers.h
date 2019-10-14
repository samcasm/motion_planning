#ifndef HELPERS_H
#define HELPERS_H
#include <vector>
using namespace std;

extern int gridSize;
struct Point
{
    int x, y;
};
struct floatPoint{
    float x,y;
};

struct Triangle{
    Point x, y, z;
};
struct Triangle1{
    floatPoint x, y, z;
};
struct Cell{
    int x,y,z;
};
struct queueNode{
    Cell pt;
    int dist;
    std::vector<Cell> pathVector;
};


bool pointOnSegment(floatPoint a, floatPoint b, floatPoint c);
int orientation(floatPoint a, floatPoint b, floatPoint c);
bool doIntersect(floatPoint a, floatPoint b, floatPoint x, floatPoint y);
bool pointInTriangle(floatPoint x, floatPoint a, floatPoint b, floatPoint c);
bool isCollidingWithBoundary(floatPoint a, floatPoint b, floatPoint c, int gridSize);
bool isCollidingWithObstacle(floatPoint x, floatPoint y, floatPoint z, vector<Triangle1> obstacles, int noOfObstacles);
queueNode BFS(int grid[100][100][36], Cell src, Cell dest, int gridSize, int deg, int cellSize);
#endif