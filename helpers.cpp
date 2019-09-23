#include <iostream>
#include <math.h>
#include "helpers.h"
using namespace std;

/* custom functions */

/* check if point b lies on the segment ab */
bool pointOnSegment(Point a, Point b, Point c){
  if(b.x <= max(a.x, c.x) && b.x >= min(a.x, c.x) && b.y <= max(a.y, c.y) && b.y >= min(a.y, c.y)){
    return true;
  }
  return false;
}

/* check the orientation of the points a, b, c
  returns -> 0 if the points are colinear
          -> 1 if clockwise
          -> 2 if anti-clockwise */
int orientation(Point a, Point b, Point c){
  /* using the determinants formula */
  int res = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);

  if(res == 0) return 0;

  if(res > 0) return 1;

  return 2;

}

/* Primitives */
/* intersection function */
bool doIntersect(Point a, Point b, Point x, Point y){

  /* Case 1: when (a,b,x) and (a,b,y) have different orientations and 
             when (x,y,a) and (x,y,b) have different orientation */
  int or1 = orientation(a,b,x);
  int or2 = orientation(a,b,y);
  int or3 = orientation(x,y,a);
  int or4 = orientation(x,y,b);

  if (or1 != or2 && or3 != or4){
    return true;
  }

  /*Case 2: when points a,b,x are colinear and point x lies on segment ab */
  if((or1 == 0 && pointOnSegment(a,x,b)) or (or2 == 0 && pointOnSegment(a,y,b)) or (or3 == 0 && pointOnSegment(x,a,y)) or (or4 && pointOnSegment(x,b,y))){
    return true;
  }

  return false;

}

bool pointInTriangle(Point x, Point a, Point b, Point c){
    
    
    int or1 = orientation(x, a, b);
    int or2 = orientation(x, b, c);
    int or3 = orientation(x, c, a);

    return or1 == or2 ? (or2 == or3 ? true : false) : false ;
}

int computeNewX_Y(int x_y, int deg, char selection){
  if (selection == 'x'){
    return ( ((x_y * 5) * cos(deg)) - ((x_y * 5) * sin(deg)) ) + (x_y  * 5);
  }
  return ( ((x_y * 5) * sin(deg)) + ((x_y) * cos(deg)) ) + (x_y * 5);
}

bool isCollidingWithObstacle(Point x, Point y, Point z, Triangle *obstacles, int noOfObstacles){
  struct Triangle triangle1 = {x, y, z};
  for (int i=0; i<noOfObstacles; i++){
    Triangle obstacle =  obstacles[i];

    if (doIntersect(triangle1.x, triangle1.y, obstacle.x, obstacle.y)) return true;
    if (doIntersect(triangle1.x, triangle1.y, obstacle.x, obstacle.z)) return true;
    if (doIntersect(triangle1.x, triangle1.y, obstacle.y, obstacle.z)) return true;
    if (doIntersect(triangle1.x, triangle1.z, obstacle.x, obstacle.y)) return true;
    if (doIntersect(triangle1.x, triangle1.z, obstacle.x, obstacle.z)) return true;
    if (doIntersect(triangle1.x, triangle1.z, obstacle.y, obstacle.z)) return true;
    if (doIntersect(triangle1.y, triangle1.z, obstacle.x, obstacle.y)) return true;
    if (doIntersect(triangle1.y, triangle1.z, obstacle.x, obstacle.z)) return true;
    if (doIntersect(triangle1.y, triangle1.z, obstacle.y, obstacle.z)) return true;

    int inTriangle = true;

    inTriangle = inTriangle && pointInTriangle(triangle1.x, obstacle.x, obstacle.y, obstacle.z);
    inTriangle = inTriangle && pointInTriangle(triangle1.y, obstacle.x, obstacle.y, obstacle.z);
    inTriangle = inTriangle && pointInTriangle(triangle1.z, obstacle.x, obstacle.y, obstacle.z);

    if (inTriangle == true) return true;

    inTriangle = true;

    inTriangle = inTriangle && pointInTriangle(obstacle.x, triangle1.x, triangle1.y, triangle1.z);
    inTriangle = inTriangle && pointInTriangle(obstacle.y, triangle1.x, triangle1.y, triangle1.z);
    inTriangle = inTriangle && pointInTriangle(obstacle.z, triangle1.x, triangle1.y, triangle1.z);

    if (inTriangle == true) return true;

  } 
  return false;
}