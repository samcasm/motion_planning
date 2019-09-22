#ifndef HELPERS_H
#define HELPERS_H
struct Point
{
    int x, y;
};
bool pointOnSegment(Point a, Point b, Point c);
int orientation(Point a, Point b, Point c);
bool doIntersect(Point a, Point b, Point x, Point y);
bool pointInTriangle(Point x, Point a, Point b, Point c);
#endif