/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "helpers.h"
#include <iostream>
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
char *win_name_string = (char*)"Example Window";
char *icon_name_string = (char*)"Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;


/* custom xlib functions */

void createGrid(int gridLength, int cellSize ){
    for (int i=0; i<=gridLength; i = i+cellSize)
      {
            XDrawLine(display_ptr, win, gc, 0, i * win_height/gridLength,
                win_width, i * win_height/gridLength);
            XDrawLine(display_ptr, win, gc, i * win_width/gridLength, 0,
                i * win_width/gridLength, win_height);

      }
}

void createObstacles(Point point1, Point point2, Point point3){
    XDrawLine(display_ptr, win, gc_red, point1.x, point1.y,
                point2.x, point2.y);

    XDrawLine(display_ptr, win, gc_red, point2.x, point2.y,
                point3.x, point3.y);
    
    XDrawLine(display_ptr, win, gc_red, point3.x, point3.y,
                point1.x, point1.y);
}

void createRobot(Point point1, Point point2, Point point3){
    XDrawLine(display_ptr, win, gc_yellow, point1.x, point1.y,
                point2.x, point2.y);

    XDrawLine(display_ptr, win, gc_yellow, point2.x, point2.y,
                point3.x, point3.y);
    
    XDrawLine(display_ptr, win, gc_yellow, point3.x, point3.y,
                point1.x, point1.y);
}

int computeNewX_Y(Point point, int deg, char selection){
  if (selection == 'x'){
    return ( ((point.x * 5) * cos(deg)) - ((point.y * 5) * sin(deg)) ) + (point.x  * 5);
  }
  return ( ((point.x * 5) * sin(deg)) + ((point.y) * cos(deg)) ) + (point.y * 5);
}



int main(int argc, char **argv)
{
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

  class_hints->res_name = (char*)"x_use_example";
  class_hints->res_class = (char*)"examples";

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
  XSetLineAttributes(display_ptr, gc, 4, LineSolid, CapRound, JoinRound);
  /* and three other graphics contexts, to draw in yellow and red and grey*/
  gc_yellow = XCreateGC(display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow, 6, LineSolid, CapRound, JoinRound);
  if (XAllocNamedColor(display_ptr, color_map, "yellow",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color yellow\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
  gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
  XSetLineAttributes(display_ptr, gc_red, 6, LineSolid, CapRound, JoinRound);
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

      createGrid(500, 5);
      Point point1 = {100, 100};
      Point point2 = {100, 130};
      Point point3 = {150, 110};
      createObstacles(point1, point2, point3);
      
      Point point1R = {200, 200};
      Point point2R = {200, 230};
      Point point3R = {250, 210};
      createRobot(point1R, point2R, point3R);

      /* TESTS
      struct Point e1 = {10,60};
      struct Point c1 = {0,0};
      struct Point c2 = {20,0};
      struct Point c3 = {10,30};
      pointInTriangle(e1, c1,c2,c3)? cout << "Inside\n" : cout << "Not inside\n";
      
      struct Point p1 = {1, 1}, q1 = {10, 1}; 
      struct Point p2 = {1, 2}, q2 = {10, 2}; 
    
      doIntersect(p1, q1, p2, q2)? cout << "Yes\n": cout << "No\n"; 
    
      p1 = {10, 0}, q1 = {0, 10}; 
      p2 = {0, 0}, q2 = {10, 10}; 
      doIntersect(p1, q1, p2, q2)? cout << "Yes\n": cout << "No\n"; 
      p1 = {-5, -5}, q1 = {0, 0}; 
      p2 = {1, 1}, q2 = {10, 10}; 
      doIntersect(p1, q1, p2, q2)? cout << "Yes\n": cout << "No\n"; 

      */
      int freeSpace[100][100][36]; 
      struct Point origin1 = {-6,-3}, origin2 = {-6,3}, origin3 = {10, 0};
      for (int i= 0; i<100; i++){
        for (int j=0; j<100; j++ ){
          for (int k=0; k<36; k++){
            int deg = 10 * k;
            int new_x1 = computeNewX_Y(origin1, deg, 'x');
            int new_y1 = computeNewX_Y(origin1, deg, 'y');

            int new_x2 = computeNewX_Y(origin2, deg, 'x');
            int new_y2 = computeNewX_Y(origin2, deg, 'y');

            int new_x3 = computeNewX_Y(origin3, deg, 'x');
            int new_y3 = computeNewX_Y(origin3, deg, 'y');

          }
        }
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
        if (report.xbutton.button == Button1)
          XFillArc(display_ptr, win, gc_red,
                   x - win_height / 40, y - win_height / 40,
                   win_height / 20, win_height / 20, 0, 360 * 64);
        else
          XFillArc(display_ptr, win, gc_yellow,
                   x - win_height / 40, y - win_height / 40,
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
