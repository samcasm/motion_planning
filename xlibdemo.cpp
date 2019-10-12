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

#define PI 3.14159265

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


void createTriangles(Point point1, Point point2, Point point3, int gridLength, int type)
{
  if (type == 0) {
    point1 = fix_neg_points(point1);
    point2 = fix_neg_points(point2);
    point3 = fix_neg_points(point3);
  }
  /* type == 1 for obstacles(red), else 0(yellow) */ 
  XDrawLine(display_ptr, win, type == 1? gc_red: gc_yellow, point1.x * win_width / gridLength, point1.y * win_height / gridLength,
            point2.x * win_width / gridLength, point2.y * win_height / gridLength);

  XDrawLine(display_ptr, win, type == 1? gc_red: gc_yellow, point2.x * win_width / gridLength, point2.y * win_height / gridLength,
            point3.x * win_width / gridLength, point3.y * win_height / gridLength);

  XDrawLine(display_ptr, win, type == 1? gc_red: gc_yellow, point3.x * win_width / gridLength, point3.y * win_height / gridLength,
            point1.x * win_width / gridLength, point1.y * win_height / gridLength);
}

void createRobot(short x0,short y0,short x1,short y1,short x2,short y2, int gridSize, int cellSize)
{
  
  short point1_x = (x0 * win_width) / (gridSize * cellSize);
  short point1_y = (y0 * win_height) / (gridSize * cellSize);
  short point2_x = (x1 * win_width) / (gridSize * cellSize);
  short point2_y = (y1 * win_height) / (gridSize * cellSize);
  short point3_x = (x2 * win_width) / (gridSize * cellSize);
  short point3_y = (y2 * win_height) / (gridSize * cellSize);
  
  XPoint foo[] = {{point1_x,point1_y},{point2_x,point2_y},{point3_x,point3_y}};
  int npoints = sizeof(foo)/sizeof(XPoint);
  XFillPolygon(display_ptr, win, gc_yellow, foo , npoints, Convex, CoordModeOrigin);
}

float degreeToRadian(int deg){

  return deg * PI / 180.0;
}

Cell convertPointToCell(int x, int y, int deg){
    int var1 = round(x/5);
    int var2 = round(y/5);
    int var3 = round(deg/10);

    struct Cell currcell = {var1, var2, var3};
    return currcell;
}

Point rotate_trans_Point(Point P, int x, int y, int deg){
  float rotx = (P.x * cos(degreeToRadian(deg))) - (P.y * sin(degreeToRadian(deg)));
  float roty = (P.x * sin(degreeToRadian(deg))) + (P.y * cos(degreeToRadian(deg)));

  /*
  cout << P.x * cos(degreeToRadian(deg)) << "  "  << P.y * sin(degreeToRadian(deg)) << "\n";
  cout << P.x * sin(degreeToRadian(deg)) << "  "  << P.y * cos(degreeToRadian(deg)) << "\n";
  cout << P.x << " " << P.y << " " <<  deg << " actual points";
  cout << rotx << " " << roty << " " << "rotated points";
  cout << resx << " " << resy <<" finally\n" ;
  */
  
  float x1 = x * 5;
  float y1 = y * 5;


  int resx = round(rotx) + x1;
  int resy = round(roty) + y1;
  
  struct Point newpoint = {resx, resy};
  return newpoint;
}

int main(int argc, char **argv)
{
  /********************************                    read the input file                        ***********************************/
  FILE *inputfile;
  int startx, starty, startphi, targetx, targety, targetphi;
  int i, finished, number_obst;
  short obstx[3][30], obsty[3][30];
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
    if(fscanf(inputfile, "O (%hu,%hu) (%hu,%hu) (%hu,%hu)\n", &(obstx[0][i]), &(obsty[0][i]), &(obstx[1][i]), &(obsty[1][i]), &(obstx[2][i]), &(obsty[2][i])) != 6) {
      finished = 1;
    }
    else i += 1;
  }
  number_obst = i;
  cout << "found " << i <<  " obstacles. so far ok\n";

  int gridSize = 5;
  int cellSize = 2;
  int degrees = 4;

  int freeSpace[5][5][4];
  struct Point origin = {0, 0}, origin1 = {vx[0], vy[0]}, origin2 = {vx[1], vy[1]}, origin3 = {vx[2], vy[2]};
  struct Point temp, temp1, temp2, temp3;
  /*obstacles*/
  std::vector<Triangle> obstacles;
  
  for (int i=0; i<number_obst; i++){
    struct Point obst_v1 = {obstx[0][i], obsty[0][i]}, obst_v2 = {obstx[1][i], obsty[1][i]}, obst_v3 = {obstx[2][i], obsty[2][i]};
    struct Triangle obstacle_tri = {obst_v1, obst_v2, obst_v3};
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
        int deg = 45 * k;

        /* compute projection of rotation and translation */
        temp1 = rotate_trans_Point(origin1, j, i, deg);

        temp2 = rotate_trans_Point(origin2,j ,i, deg);

        temp3 = rotate_trans_Point(origin3, j ,i, deg);
        
        /* check bounding condition */
        if (isCollidingWithBoundary(temp1, temp2, temp3, gridSize * cellSize))
        {
          freeSpace[i][j][k] = 0;
        }
        /* check obstacle collision */
        else if (isCollidingWithObstacle(temp1, temp2, temp3, obstacles, noOfObstacles))
        { 
          freeSpace[i][j][k] = 0;
        }
        else{
          freeSpace[i][j][k] = 1;
        }
          

        /*cout << freeSpace[i][j][k] << "   " ;*/

      }
    }
  }

  Point res = rotate_trans_Point(origin1, 2,2, 10);
  Point res1 = rotate_trans_Point(origin1, 2,2, 20);
  Point res2 = rotate_trans_Point(origin1, 2,2, 30);
  Point res3 = rotate_trans_Point(origin1, 2,2, 40);
  Point res4 = rotate_trans_Point(origin1, 2,2, 50);

  cout << res.x << " " << res.y << " \n";
  cout << res1.x << " " << res1.y << " \n";
  cout << res2.x << " " << res2.y << " \n";
  cout << res3.x << " " << res3.y << " \n";
  cout << res4.x << " " << res4.y << " \n";

  struct Cell src = convertPointToCell(startx, starty, startphi);
  struct Cell dest = convertPointToCell(targetx, targety, targetphi);

  cout << src.x << " " << src.y << " " << src.z << "the source\n";
  cout << dest.x << " " << dest.y << " " << dest.z << "the destination\n";
  cout << freeSpace[src.x][src.y][src.z] << " " << freeSpace[dest.x][dest.y][dest.z] << "\n";
  
  // int result = BFS(freeSpace, src, dest, gridSize, degrees, cellSize);
  // cout << result << "the result \n";



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
      XPoint foo1[] = {{vx[0],vy[0]},{vx[1],vy[1]},{vx[2],vy[2]}};
      int npoints1 = sizeof(foo1)/sizeof(XPoint);

      XFillPolygon(display_ptr, win, gc_yellow, foo1 , npoints1, Convex, CoordModeOrigin);
      createGrid(gridSize * cellSize, cellSize);

      createRobot(vx[0],vy[0],vx[1], vy[1], vx[2], vy[2], gridSize, cellSize);
      /*

      for (int i=0; i<noOfObstacles; i++){
        createTriangles(obstacles[i].x, obstacles[i].y, obstacles[i].z, gridSize * cellSize, 1);
      }

      createTriangles(origin1, origin2, origin3, gridSize * cellSize, 0);

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

        
          XFillArc(display_ptr, win, gc_yellow,
                   point1_x, point1_y,
                   win_height / 20, win_height / 20, 0, 360 * 64);
          
        
          XFillArc(display_ptr, win, gc_yellow,
                   point2_x, point2_y,
                   win_height / 20, win_height / 20, 0, 360 * 64);
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
