#ifndef GRAPH_H
#define GRAPH_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "system.h"

typedef struct Environment
{
	Display* dpy;
	Window win;
	GC gc[5];
	Colormap cmap;
	int scr;
	double lenx;
	double leny;
	double minx;
	double miny;
	System& syst;
	Environment(System&);
	void findlen();
	int (*draw) (Environment*, XEvent*);
	~Environment();
} Environment;

int DrawOrig(Environment*, XEvent*);
int DrawDefs(Environment*, XEvent*);
void DrawFix1(Display*, Window, GC*, int, int);
void DrawFix2(Display*, Window, GC*, int, int);
void DrawFix3(Display*, Window, GC*, int, int);
void DrawArrowY(Display*, Window, GC*, int, int, int);
void DrawArrowX(Display*, Window, GC*, int, int, int);
int dispatch(Environment*);

#endif
