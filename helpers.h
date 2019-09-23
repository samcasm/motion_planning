#ifndef HELPERS_H
#define HELPERS_H
struct Point
{
    int x, y;
};
struct Triangle{
    Point x, y, z;
};

bool pointOnSegment(Point a, Point b, Point c);
int orientation(Point a, Point b, Point c);
bool doIntersect(Point a, Point b, Point x, Point y);
bool pointInTriangle(Point x, Point a, Point b, Point c);
int computeNewX_Y(int x_y, int deg, char selection);
bool isCollidingWithObstacle(Point x, Point y, Point z, Triangle *obstacles, int noOfObstacles);
#endif