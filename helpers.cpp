#include <iostream>
#include <math.h>
#include "helpers.h"
#include <string.h>
#include <queue>
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


bool isPointOutOfBounds(Point a, int gridLength){
  if (a.x < 0 || a.x > gridLength || a.y < 0 || a.y > gridLength){
    return true;
  }
  return false;
}

bool isCollidingWithBoundary(Point a, Point b, Point c, int gridLength){
  if (isPointOutOfBounds(a, gridLength) || isPointOutOfBounds(b, gridLength) || isPointOutOfBounds(c, gridLength)){
    return true;
  }
  return false; 
}

bool isCollidingWithObstacle(Point x, Point y, Point z, vector<Triangle> obstacles, int noOfObstacles){
  struct Triangle triangle1 = {x, y, z};
  for (int i=0; i<noOfObstacles; i++){
    Triangle obstacle =  obstacles[i];
    // if (triangle1.x.x > 445 && triangle1.x.y < 453){
    //   cout << obstacle.x.x << obstacle.x.y;
    // } 
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

bool isValid(int grid[][5][4],int x, int y, int z, int gridLength, int degrees){

    int size1 = gridLength;
    int size2 = gridLength;
    int size3 = degrees;
    if (x < size1 && x >= 0 && y < size2 && y >= 0 && z < size3 && z >= 0 && grid[x][y][z] == 1) {
          return true;    
    }
    return false;
}

queueNode BFS(int grid[][5][4], Cell src, Cell dest, int gridSize, int degrees, int cellSize){

    if(!isValid(grid, src.x, src.y, src.z, gridSize, degrees ) || !isValid(grid, dest.x, dest.y, dest.z ,gridSize, degrees)){
      struct queueNode node = {src, -1};
      return node;
    }
    
    int d1[6] = {1, -1, 0, 0, 0, 0};    
    int d2[6] = {0, 0, 1, -1, 0, 0};    
    int d3[6] = {0, 0, 0, 0, 1, -1};    
    
    bool visited[gridSize][gridSize][degrees];
    memset(visited, false, sizeof(visited) ); 

    visited[src.x][src.y][src.z] = true;

    std::queue<queueNode> q;

    struct Cell sourceCell = src;
    struct queueNode s = {src, 0};
    s.pathVector.push_back(sourceCell);

    q.push(s);

    while (!q.empty()){
        queueNode curr = q.front(); 
        Cell pt = curr.pt; 
  
        // If we have reached the destination cell, 
        // we are done 
        if (pt.x == dest.x && pt.y == dest.y && pt.z == dest.z) {
            /*for (int i=0; i<curr.pathVector.size(); i++){
              cout << curr.pathVector[i].x << curr.pathVector[i].y << curr.pathVector[i].z <<" \n";
            }*/
            return curr; 
        }
  
        // Otherwise dequeue the front cell in the queue 
        // and enqueue its adjacent cells 
        q.pop(); 
        for (int i = 0; i < (sizeof(d1)/sizeof(int)); i++) 
        { 
            int row = pt.x + d1[i]; 
            int col = pt.y + d2[i]; 
            int deg = pt.z + d3[i];

              
            // if adjacent cell is valid, has path and 
            // not visited yet, enqueue it. 
            if (isValid(grid, row, col, deg, gridSize, degrees) && grid[row][col][deg] &&  
               !visited[row][col][deg]) 
            { 
                // mark cell as visited and enqueue it 
                visited[row][col][deg] = true; 
                struct Cell currentCell = {row, col, deg};
                struct queueNode Adjcell = { {row, col, deg}, 
                                      curr.dist + 1 };
                Adjcell.pathVector.push_back(currentCell);
                Adjcell.pathVector.insert(Adjcell.pathVector.end(), curr.pathVector.begin(), curr.pathVector.end()); 
                q.push(Adjcell); 
            }   

        } 
    }
    cout << "Could not find a path\n";

  }

