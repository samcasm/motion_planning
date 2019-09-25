#ifndef HELPERS_H
#define HELPERS_H
extern int gridSize;
struct Point
{
    int x, y;
};
struct Triangle{
    Point x, y, z;
};
struct Cell{
    int x,y,z;
};
struct queueNode{
    Cell pt;
    int dist;
};

bool pointOnSegment(Point a, Point b, Point c);
int orientation(Point a, Point b, Point c);
bool doIntersect(Point a, Point b, Point x, Point y);
bool pointInTriangle(Point x, Point a, Point b, Point c);
int computeNewX_Y(int x_y, int deg, char selection);
bool isCollidingWithBoundary(Point a, Point b, Point c, int gridSize);
bool isCollidingWithObstacle(Point x, Point y, Point z, Triangle *obstacles, int noOfObstacles);
int BFS(int grid[100][100][36], int x, int y, int z);
#endif