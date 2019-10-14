/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib 
   sudo apt install g++
   sudo apt install libx11-dev
   install c/c++ intellisense extension 
*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "helpers.h"
#include <iostream>
#include <complex>
#include <bits/stdc++.h>
using namespace std;


Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = (char *)"Example Window";
char *icon_name_string = (char *)"Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

/* custom xlib functions */

void createGrid(int gridLength, int cellSize)
{
  for (int i = 0; i <= gridLength; i = i + cellSize)
  {
    XDrawLine(display_ptr, win, gc, 0, i * win_height / gridLength,
              win_width, i * win_height / gridLength);
    XDrawLine(display_ptr, win, gc, i * win_width / gridLength, 0,
              i * win_width / gridLength, win_height);
  }
}

Point fix_neg_points(Point p){
  struct Point newp;
  if (p.x <= 0 && p.y <= 0){
    newp = {0,0};
  }else if (p.x < 0 and p.y > 0){
    newp = {0, p.y};
  }else if (p.x > 0 and p.y < 0){
    newp = {p.x, 0};
  }else{
    newp = {p.x, p.y};
  }
  return newp;
}


void createTriangles(Point point1, Point point2, Point point3, int gridLength)
{
  // if (type == 0) {
  //   point1 = fix_neg_points(point1);
  //   point2 = fix_neg_points(point2);
  //   point3 = fix_neg_points(point3);
  // }
  /* type == 1 for obstacles(red), else 0(yellow) */ 
  XDrawLine(display_ptr, win, gc_red, point1.x * win_width / gridLength, point1.y * win_height / gridLength,
            point2.x * win_width / gridLength, point2.y * win_height / gridLength);

  XDrawLine(display_ptr, win, gc_red, point2.x * win_width / gridLength, point2.y * win_height / gridLength,
            point3.x * win_width / gridLength, point3.y * win_height / gridLength);

  XDrawLine(display_ptr, win, gc_red, point3.x * win_width / gridLength, point3.y * win_height / gridLength,
            point1.x * win_width / gridLength, point1.y * win_height / gridLength);
}

void createTriangles1(floatPoint point1, floatPoint point2, floatPoint point3, int gridLength)
{
  
  XDrawLine(display_ptr, win, gc_red, point1.x * win_width / gridLength, point1.y * win_height / gridLength,
            point2.x * win_width / gridLength, point2.y * win_height / gridLength);

  XDrawLine(display_ptr, win, gc_red, point2.x * win_width / gridLength, point2.y * win_height / gridLength,
            point3.x * win_width / gridLength, point3.y * win_height / gridLength);

  XDrawLine(display_ptr, win, gc_red, point3.x * win_width / gridLength, point3.y * win_height / gridLength,
            point1.x * win_width / gridLength, point1.y * win_height / gridLength);
}

void createRobot(short x0,short y0,short x1,short y1,short x2,short y2, int startx, int starty, int gridSize, int cellSize)
{
  
  short point1_x = ((x0 + startx) * win_width) / (gridSize * cellSize);
  short point1_y = ((y0 + starty) * win_height) / (gridSize * cellSize);
  short point2_x = ((x1 + startx) * win_width) / (gridSize * cellSize);
  short point2_y = ((y1 + starty) * win_height) / (gridSize * cellSize);
  short point3_x = ((x2 + startx) * win_width) / (gridSize * cellSize);
  short point3_y = ((y2 + starty) * win_height) / (gridSize * cellSize);
  
  XPoint foo[] = {{point1_x,point1_y},{point2_x,point2_y},{point3_x,point3_y}};
  int npoints = sizeof(foo)/sizeof(XPoint);
  XFillPolygon(display_ptr, win, gc_yellow, foo , npoints, Convex, CoordModeOrigin);
}

double degreeToRadian(double deg){

  return deg * M_PI / 180.0;
}

Cell convertPointToCell(int x, int y, int deg, int cellSize){
    int var1 = x/cellSize;
    int var2 = y/cellSize;
    int var3 = deg/10;

    struct Cell currcell = {var1, var2, var3};
    return currcell;
}

Point rotate_trans_Point(Point P, int cellSize,  int x, int y, int deg){
  float cos_d = round(cos(degreeToRadian(deg)) * 1000.0) / 1000.0;
  float sin_d = round(sin(degreeToRadian(deg)) * 1000.0) / 1000.0;

  float rotx = (P.x * cos_d) - (P.y * sin_d);
  float roty = (P.x * sin_d) + (P.y * cos_d);

  
  float x1 = x * cellSize;
  float y1 = y * cellSize;


  int resx = round(rotx) + x1;
  int resy = round(roty) + y1;
  
  struct Point newpoint = {resx, resy};
  return newpoint;
}

floatPoint rotate_trans_Point1(Point P, int cellSize,  int x, int y, int deg){
  float cos_d = round(cos(degreeToRadian(deg*10)) * 1000.0) / 1000.0;
  float sin_d = round(sin(degreeToRadian(deg*10)) * 1000.0) / 1000.0;

  float rotx = (P.x * cos_d) - (P.y * sin_d);
  float roty = (P.x * sin_d) + (P.y * cos_d);
  
  float x1 = x * cellSize;
  float y1 = y * cellSize;


  float resx = rotx + x1;
  float resy = roty + y1;
  
  struct floatPoint newpoint = {resx, resy};
  return newpoint;
}

int main(int argc, char **argv)
{
  /********************************                    read the input file                        ***********************************/
  FILE *inputfile;
  int startx, starty, startphi, targetx, targety, targetphi;
  int i, finished, number_obst;
  float obstx[3][30], obsty[3][30];
  short vx[3], vy[3];

  if (argc != 2){
    cout << "need filename as command line argument. \n"; fflush(stdout);
    exit(0);
  }
  inputfile = fopen(argv[1], "r");
  if (fscanf(inputfile, "V (%hu,%hu) (%hu,%hu) (%hu, %hu)\n", &(vx[0]), &(vy[0]), &(vx[1]), &(vy[1]), &(vx[2]), &(vy[2])) != 6){
    cout << "error in first line.\n"; fflush(stdout);
    exit(0);
  }
  if (fscanf(inputfile, "S (%d,%d) %d\n", &startx, &starty, &startphi) != 3){
    cout << "error in the third line. \n"; fflush(stdout);
    exit(0);
  }
  if (fscanf(inputfile, "T (%d,%d) %d\n", &targetx, &targety, &targetphi) != 3){
    cout << "error in the third line. \n"; fflush(stdout);
    exit(0);
  }
  i=0; finished = 0;
  while (i<30 && !finished){
    if(fscanf(inputfile, "O (%f,%f) (%f,%f) (%f,%f)\n", &(obstx[0][i]), &(obsty[0][i]), &(obstx[1][i]), &(obsty[1][i]), &(obstx[2][i]), &(obsty[2][i])) != 6) {
      finished = 1;
    }
    else i += 1;
  }
  number_obst = i;
  cout << "found " << i <<  " obstacles. so far ok\n";

  int gridSize = 100;
  int cellSize = 5;
  int degrees = 36;

  int freeSpace[100][100][36];
  struct Point origin1 = {vx[0], vy[0]}, origin2 = {vx[1], vy[1]}, origin3 = {vx[2], vy[2]};
  struct floatPoint temp, temp1, temp2, temp3;
  /*obstacles*/
  std::vector<Triangle1> obstacles;
  
  for (int i=0; i<number_obst; i++){
    struct floatPoint obst_v1 = {obstx[0][i], obsty[0][i]}, obst_v2 = {obstx[1][i], obsty[1][i]}, obst_v3 = {obstx[2][i], obsty[2][i]};
    struct Triangle1 obstacle_tri = {obst_v1, obst_v2, obst_v3};
    obstacles.push_back(obstacle_tri);
  }
  
  int noOfObstacles = number_obst;
  
  for (int i = 0; i < gridSize; i++)
  {
    for (int j = 0; j < gridSize; j++)
    {
      for (int k = 0; k < degrees; k++)
      {
        // change as per the degrees

        /* compute projection of rotation and translation */
        temp1 = rotate_trans_Point1(origin1, cellSize, j, i, k);

        temp2 = rotate_trans_Point1(origin2, cellSize, j ,i, k);

        temp3 = rotate_trans_Point1(origin3, cellSize, j ,i, k);
        
        /* check bounding condition */
        if (isCollidingWithBoundary(temp1, temp2, temp3, gridSize * cellSize))
        {
          freeSpace[j][i][k] = 0;
        }
        /* check obstacle collision */
        else if (isCollidingWithObstacle(temp1, temp2, temp3, obstacles, noOfObstacles))
        { 
          // if (i == 3 && j == 2 && k == 2){
          // cout << "\n" << temp1.x << " " << temp1.y << " " << temp2.x << " " << temp2.y << " " << temp3.x << " " << temp3.y << " colliding points\n";

          // }
          
          freeSpace[j][i][k] = 0;
        }
        else{
          freeSpace[j][i][k] = 1;
        }
        


        /*cout << freeSpace[i][j][k] << "   " ;*/

      }
    }
  }

  cout << freeSpace[2][3][2] << "< ---";
  
  // floatPoint resres = rotate_trans_Point1(origin1, cellSize, 1, 1, 90);
  // cout << origin1.x << " " << origin1.y ;
  // cout << "\n" << resres.x << " " << resres.y << "this is the final showdown\n";

  struct Cell src = convertPointToCell(startx, starty, startphi, cellSize);
  struct Cell dest = convertPointToCell(targetx, targety, targetphi, cellSize);

  cout << src.x << " " << src.y << " " << src.z << "the source\n";
  cout << dest.x << " " << dest.y << " " << dest.z << "the destination\n";
  cout << freeSpace[src.x][src.y][src.z] << " " << freeSpace[dest.x][dest.y][dest.z] << "\n";
  
  int resultsize = 0;
  queueNode result = BFS(freeSpace, src, dest, gridSize, degrees, cellSize);
  if (result.dist != -1){
    resultsize = result.pathVector.size();
    for (int i=0; i< resultsize; i++){
      cout << result.pathVector[i].x << " " << result.pathVector[i].y << " " << result.pathVector[i].z;
    }
  }else{
    cout << "invalid input";
  }
  

  /* opening display: basic connection to X Server */
  if ((display_ptr = XOpenDisplay(display_name)) == NULL)
  {
    printf("Could not open display. \n");
    exit(-1);
  }
  printf("Connected to X server  %s\n", XDisplayName(display_name));
  screen_num = DefaultScreen(display_ptr);
  screen_ptr = DefaultScreenOfDisplay(display_ptr);
  color_map = XDefaultColormap(display_ptr, screen_num);
  display_width = DisplayWidth(display_ptr, screen_num);
  display_height = DisplayHeight(display_ptr, screen_num);

  printf("Width %d, Height %d, Screen Number %d\n",
         display_width, display_height, screen_num);
  /* creating the window */
  border_width = 10;
  win_x = 0;
  win_y = 0;
  win_width = display_width / 2;
  win_height = (int)(win_width / 1.7); /*rectangular window*/

  win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
                            win_x, win_y, win_width, win_height, border_width,
                            BlackPixel(display_ptr, screen_num),
                            WhitePixel(display_ptr, screen_num));
  /* now try to put it on screen, this needs cooperation of window manager */
  size_hints = XAllocSizeHints();
  wm_hints = XAllocWMHints();
  class_hints = XAllocClassHint();
  if (size_hints == NULL || wm_hints == NULL || class_hints == NULL)
  {
    printf("Error allocating memory for hints. \n");
    exit(-1);
  }

  size_hints->flags = PPosition | PSize | PMinSize;
  size_hints->min_width = 60;
  size_hints->min_height = 60;

  XStringListToTextProperty(&win_name_string, 1, &win_name);
  XStringListToTextProperty(&icon_name_string, 1, &icon_name);

  wm_hints->flags = StateHint | InputHint;
  wm_hints->initial_state = NormalState;
  wm_hints->input = False;

  class_hints->res_name = (char *)"x_use_example";
  class_hints->res_class = (char *)"examples";

  XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc,
                   size_hints, wm_hints, class_hints);

  /* what events do we want to receive */
  XSelectInput(display_ptr, win,
               ExposureMask | StructureNotifyMask | ButtonPressMask);

  /* finally: put window on screen */
  XMapWindow(display_ptr, win);

  XFlush(display_ptr);

  /* create graphics context, so that we may draw in this window */
  gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
  XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
  XSetLineAttributes(display_ptr, gc, 1, LineSolid, CapRound, JoinRound);
  /* and three other graphics contexts, to draw in yellow and red and grey*/
  gc_yellow = XCreateGC(display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow, 2, LineSolid, CapRound, JoinRound);
  if (XAllocNamedColor(display_ptr, color_map, "yellow",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color yellow\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
  gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
  XSetLineAttributes(display_ptr, gc_red, 2, LineSolid, CapRound, JoinRound);
  if (XAllocNamedColor(display_ptr, color_map, "red",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color red\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
  gc_grey = XCreateGC(display_ptr, win, valuemask, &gc_grey_values);
  if (XAllocNamedColor(display_ptr, color_map, "light grey",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color grey\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_grey, tmp_color1.pixel);

  /* and now it starts: the event loop */
  while (1)
  {
    XNextEvent(display_ptr, &report);
    switch (report.type)
    {
    case Expose:
    {
      /* (re-)draw the example figure. This event happens
			 each time some part ofthe window gets exposed (becomes visible) */

      for (int i=0; i<number_obst; i++){
        short point1_x = (obstx[0][i] * win_width) / (gridSize * cellSize);
        short point1_y = (obsty[0][i] * win_height) / (gridSize * cellSize);
        short point2_x = (obstx[1][i] * win_width) / (gridSize * cellSize);
        short point2_y = (obsty[1][i] * win_height) / (gridSize * cellSize);
        short point3_x = (obstx[2][i] * win_width) / (gridSize * cellSize);
        short point3_y = (obsty[2][i] * win_height) / (gridSize * cellSize);
        XPoint foo[] = {{point1_x,point1_y},{point2_x,point2_y},{point3_x,point3_y}};
        int npoints = sizeof(foo)/sizeof(XPoint);

        XFillPolygon(display_ptr, win, gc_red, foo , npoints, Convex, CoordModeOrigin);
      }
      
      createGrid(gridSize * cellSize, cellSize);

      createRobot(vx[0],vy[0],vx[1], vy[1], vx[2], vy[2], startx, starty, gridSize, cellSize);

      //  for (int i = 0; i < gridSize; i++)
      //   {
      //     for (int j = 0; j < gridSize; j++)
      //     {
      //       for (int k = 0; k < degrees; k++)
      //       {
      //         if( freeSpace[i][j][k] == 0){
      //           floatPoint p1 = rotate_trans_Point1(origin1, cellSize, j, i, k );
      //           floatPoint p2 = rotate_trans_Point1(origin2, cellSize, j, i, k );
      //           floatPoint p3 = rotate_trans_Point1(origin3, cellSize, j, i, k );
      //           createTriangles1(p1,p2,p3, gridSize*cellSize);

      //         }

      //       }
      //     }
      //   }

      // if(freeSpace[2][3][2] == 7){
      //   floatPoint p1 = rotate_trans_Point1(origin1, cellSize, 3, 2, 2 );
      //   floatPoint p2 = rotate_trans_Point1(origin2, cellSize, 3, 2, 2 );
      //   floatPoint p3 = rotate_trans_Point1(origin3, cellSize, 3, 2, 2 );
      //   cout << "nooooo" ;
      //   createTriangles1(p1,p2,p3, gridSize*cellSize);
      // }

      for (int i = 0; i < resultsize; i++){
        floatPoint a1 = rotate_trans_Point1(origin1, cellSize, result.pathVector[i].x, result.pathVector[i].y, result.pathVector[i].z);
        floatPoint a2 = rotate_trans_Point1(origin2, cellSize, result.pathVector[i].x, result.pathVector[i].y, result.pathVector[i].z);
        floatPoint a3 = rotate_trans_Point1(origin3, cellSize, result.pathVector[i].x, result.pathVector[i].y, result.pathVector[i].z);

        createTriangles1(a1, a2, a3, gridSize*cellSize);
      }

      
      /*              
      XDrawLine(display_ptr, win, gc_red, win_width / 4, 2 * win_height / 3,
                3 * win_width / 4, 2 * win_height / 3);
      XFillArc(display_ptr, win, gc_grey, win_width / 2 - win_height / 6,
               win_height / 3,
               win_height / 3, win_height / 3, 0, 360 * 64);
      XDrawArc(display_ptr, win, gc_yellow, win_width / 4, win_height / 3,
               win_height / 6, win_height / 3, 90 * 64, 180 * 64);
     */
    }
    break;
    case ConfigureNotify:
      /* This event happens when the user changes the size of the window*/
      win_width = report.xconfigure.width;
      win_height = report.xconfigure.height;
      break;
    case ButtonPress:
      /* This event happens when the user pushes a mouse button. I draw
			a circle to show the point where it happened, but do not save 
			the position; so when the next redraw event comes, these circles
		disappear again. */
      {
        int x, y;
        x = report.xbutton.x;
        y = report.xbutton.y;

        short point1_x = (startx * win_width) / (gridSize * cellSize);
        short point1_y = (starty * win_height) / (gridSize * cellSize);
        short point2_x = (targetx * win_width) / (gridSize * cellSize);
        short point2_y = (targety * win_height) / (gridSize * cellSize);

        
          XFillArc(display_ptr, win, gc_grey,
                   point1_x, point1_y,
                   win_height / 50, win_height / 50, 0, 360 * 64);
          
        
          XFillArc(display_ptr, win, gc_grey,
                   point2_x, point2_y,
                   win_height / 50, win_height / 50, 0, 360 * 64);
      }
      break;
    default:
      /* this is a catch-all for other events; it does not do anything.
			 One could look at the report type to see what the event was */
      break;
    }
  }
  exit(0);
}
