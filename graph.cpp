#include <iostream>
#include <sstream>
#include <string>
#include "graph.h"

#define WIDTH 1024
#define HEIGHT 480
#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define ABS(A) (((A) < 0) ? (-A) : (A))
#define LIM 0.15

Environment::Environment(System& s): syst(s)
{
	findlen();
	draw = DrawOrig;
	dpy = XOpenDisplay(NULL);                 				
	scr = DefaultScreen(dpy);           
	int depth = DefaultDepth(dpy, scr);	
	cmap = DefaultColormap(dpy, scr);	
	XGCValues gval;  
	XFontStruct* fnptr;
	XSetWindowAttributes attr;      
	attr.override_redirect = False;
	attr.background_pixel = WhitePixel(dpy, scr);
	attr.event_mask = (KeyPressMask | ExposureMask );	       				
	XEvent event;                   				   				
	win = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, WIDTH, HEIGHT, 1, depth, InputOutput, CopyFromParent, (CWOverrideRedirect | CWBackPixel),  &attr);
	gc[0] = XCreateGC(dpy, win, 0, 0);
	gc[1] = XCreateGC(dpy, win, 0, 0);
	gc[2] = XCreateGC(dpy, win, 0, 0);
	gc[3] = XCreateGC(dpy, win, 0, 0);
	gc[4] = XCreateGC(dpy, win, 0, 0);
	XSetForeground(dpy, gc[0], 0x000000);
	XSetForeground(dpy, gc[1], 0xFFFFFF);
	XSetForeground(dpy, gc[2], 0x000000);
	XSetForeground(dpy, gc[3], 0xFF0000);
	XSetForeground(dpy, gc[4], 0x00FFFF);
	if((fnptr = XLoadQueryFont(dpy, "9x15")) != NULL)
		XSetFont(dpy, gc[0], fnptr->fid); //8122129 15855887
	unsigned long gmask = (GCLineWidth);
	gval.line_width = 2;
	XChangeGC(dpy, gc[0], gmask, &gval);
	XChangeGC(dpy, gc[3], gmask, &gval);
	XChangeGC(dpy, gc[4], gmask, &gval);
	//gmask = (GCLineStyle);
	//gval.line_style = LineOnOffDash; 
	//XChangeGC(dpy, gc[4], gmask, &gval);
	unsigned long emask = (ExposureMask | KeyPressMask);
	XSizeHints hint;  
	hint.min_width = 256; hint.min_height = 256;
	hint.flags = PMinSize;
	XSetNormalHints(dpy, win, &hint);
	XFreeFont(dpy, fnptr);
	XSelectInput(dpy, win, emask); 
	XMapWindow(dpy, win);
	XFlush(dpy);
}

Environment::~Environment()
{
	for (int i = 0; i < 5; i++)
		XFreeGC(dpy, gc[i]);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

unsigned long grad(Environment* env, double per)
{
	XColor rgb;
	if (per <= 0.25)
	{
		rgb.red = 0;
		rgb.green = 0xFFFF * per / 0.25;
		rgb.blue = 0xFFFF;
	}
	else if (per <= 0.5)
	{
		rgb.red = 0;
		rgb.green = 0xFFFF;
		rgb.blue = 0xFFFF - 0xFFFF * per / 0.5;
	}
	else if (per <= 0.75)
	{
		rgb.red = 0xFFFF * per / 0.75;
		rgb.green = 0xFFFF;
		rgb.blue = 0;
	}
	else if (per <= 1)
	{
		rgb.red = 0xFFFF;
		rgb.green = 0xFFFF - 0xFFFF * per;
		rgb.blue = 0;
	}
	else
	{
		rgb.red = 0xFFFF;
		rgb.green = 0;
		rgb.blue = 0;
	}
	XAllocColor(env->dpy, env->cmap, &rgb);
	return rgb.pixel;
}

int DrawOrig(Environment* env, XEvent* ev)
{
	env->draw = DrawOrig;
	XWindowAttributes xwa;
	XGetWindowAttributes(env->dpy, env->win, &xwa);
	int d = 40;
	int mu = MIN((xwa.width - 2 * d) / env->lenx, (xwa.height - 2 * d) / env->leny);
	int dx = (xwa.width - env->lenx * mu) / 2;
	if (env->minx < 0)
		dx += -env->minx * mu;
	int dy = (xwa.height + env->leny * mu) / 2;
	if (env->miny < 0)
		dy += env->miny * mu;		
	for (int i = 0; i < env->syst.getRN(); i++)
	{
		double x1 = env->syst.getR()[i]->getFirst().getX();
		double y1 = -env->syst.getR()[i]->getFirst().getY();
		double x2 = env->syst.getR()[i]->getSecond().getX();
		double y2 = -env->syst.getR()[i]->getSecond().getY();
		XDrawLine(env->dpy, env->win, env->gc[0], x1 * mu + dx, y1 * mu + dy, x2 * mu + dx, y2 * mu + dy);
	}			
	for (int i = 0; i < env->syst.getNN(); i++)
	{
		int x = env->syst.getN()[i].getX() * mu + dx;
		int y = dy - env->syst.getN()[i].getY() * mu;
		if (env->syst.getN()[i].getlx() && env->syst.getN()[i].getly())
			DrawFix1(env->dpy, env->win, env->gc, x, y);
		else if (env->syst.getN()[i].getlx())
			DrawFix2(env->dpy, env->win, env->gc, x, y);
		else if (env->syst.getN()[i].getly())
			DrawFix3(env->dpy, env->win, env->gc, x, y);
		if (env->syst.getF()[i * 2][0])
			DrawArrowX(env->dpy, env->win, env->gc, x, y, env->syst.getF()[i * 2][0]);
		if (env->syst.getF()[(i * 2) + 1][0])
			DrawArrowY(env->dpy, env->win, env->gc, x, y, env->syst.getF()[(i * 2) + 1][0]);
	}
	XStoreName(env->dpy, env->win, "Model");
	return 0;
}

void info(Environment* env)
{
	for (int i = 0; i < 4; i++)
	{
		std::ostringstream os;
		os << "<= " << (LIM * (1 - i * 0.25))*100 << '%';
		std::string conv = os.str();
		XDrawString(env->dpy, env->win, env->gc[0], 30, i*15+25, conv.c_str(), conv.size());
	}
	XDrawString(env->dpy, env->win, env->gc[0], 30, 85, "= 0%", 4);
	XSetForeground(env->dpy, env->gc[4], 0xFF0000);
	XDrawLine(env->dpy, env->win, env->gc[4], 5, 20, 25, 20);
	XSetForeground(env->dpy, env->gc[4], 0xFFFF00);
	XDrawLine(env->dpy, env->win, env->gc[4], 5, 35, 25, 35);
	XSetForeground(env->dpy, env->gc[4], 0x00FF00);
	XDrawLine(env->dpy, env->win, env->gc[4], 5, 50, 25, 50);
	XSetForeground(env->dpy, env->gc[4], 0x00FFFF);
	XDrawLine(env->dpy, env->win, env->gc[4], 5, 65, 25, 65);
	XSetForeground(env->dpy, env->gc[4], 0x0000FF);
	XDrawLine(env->dpy, env->win, env->gc[4], 5, 80, 25, 80);
}

int DrawDefs(Environment* env, XEvent* ev)
{
	env->draw = DrawDefs;
	XWindowAttributes xwa;
	XGetWindowAttributes(env->dpy, env->win, &xwa);
	int d = 40;
	int mu = MIN((xwa.width - 2 * d) / env->lenx, (xwa.height - 2 * d) / env->leny);
	int dx = (xwa.width - env->lenx * mu) / 2;
	if (env->minx < 0)
		dx += -env->minx * mu;
	int dy = (xwa.height + env->leny * mu) / 2;
	if (env->miny < 0)
		dy += env->miny * mu;	
	for (int i = 0; i < env->syst.getRN(); i++)
	{
		double x1 = env->syst.getR()[i]->getFirst().getX();
		double y1 = -env->syst.getR()[i]->getFirst().getY();
		double x2 = env->syst.getR()[i]->getSecond().getX();
		double y2 = -env->syst.getR()[i]->getSecond().getY();
		x1 += env->syst.getR()[i]->getFirst().getU()[0][0];
		y1 -= env->syst.getR()[i]->getFirst().getU()[1][0];
		x2 += env->syst.getR()[i]->getSecond().getU()[0][0];
		y2 -= env->syst.getR()[i]->getSecond().getU()[1][0]; 
		double per = ABS(env->syst.getR()[i]->getd() / LIM);
		XSetForeground(env->dpy, env->gc[4], grad(env, per));
		XDrawLine(env->dpy, env->win, env->gc[4], x1 * mu + dx, y1 * mu + dy, x2 * mu + dx, y2 * mu + dy);
	}				
	for (int i = 0; i < env->syst.getNN(); i++)
	{
		double x = env->syst.getN()[i].getX() * mu + dx;
		double y = dy - env->syst.getN()[i].getY() * mu;
		if (env->syst.getN()[i].getlx() && env->syst.getN()[i].getly())
			DrawFix1(env->dpy, env->win, env->gc, x, y);
		else if (env->syst.getN()[i].getlx())
			DrawFix2(env->dpy, env->win, env->gc, x, y);
		else if (env->syst.getN()[i].getly())
			DrawFix3(env->dpy, env->win, env->gc, x, y);
	}
	XStoreName(env->dpy, env->win, "Deformations");
	info(env);
	return 0;
}

int expo(Environment* env, XEvent* ev)
{
	return env->draw(env, ev);
}

void Environment::findlen()
{
	double maxx = syst.getN()[0].getX();
	double minx = syst.getN()[0].getX();
	double maxy = syst.getN()[0].getY();
	double miny = syst.getN()[0].getY();
	for (int i = 0; i < syst.getNN(); i++)
	{
		double x = syst.getN()[i].getX() + syst.getN()[i].getU()[0][0];
		double y = syst.getN()[i].getY() + syst.getN()[i].getU()[1][0];
		if (x > maxx)
			maxx = x;
		if (x < minx)
			minx = x;
		if (y > maxy)
			maxy = y;
		if (y < miny)
			miny = y;
	}
	for (int i = 0; i < syst.getNN(); i++)
	{
		double x = syst.getN()[i].getX();
		double y = syst.getN()[i].getY();
		if (x > maxx)
			maxx = x;
		if (x < minx)
			minx = x;
		if (y > maxy)
			maxy = y;
		if (y < miny)
			miny = y;
	}
	this->minx = minx;
	this->miny = miny;
	lenx = maxx - minx;
	leny = maxy - miny;
}

int rekey(Environment* env, XEvent* ev)
{
	KeySym ks = XLookupKeysym(&(ev->xkey), 0);
	if (ks == XK_1)
	{
		XClearWindow(env->dpy, env->win);
		return DrawOrig(env, ev);
	}
	if (ks == XK_2)
	{
		XClearWindow(env->dpy, env->win);
		DrawDefs(env, ev);
		return 0;
	}
	if (ks == XK_Escape)     						
		return 'E';
	return(0);        							
}

int dispatch(Environment* env)
{
	int done = 0;                  
	XEvent event;                
	while(done == 0) 
	{                
		XNextEvent(env->dpy, &event);      
		switch(event.type) 
		{          
			case Expose: 
				expo(env, &event);  
				break;
			case KeyPress: 
				done = rekey(env, &event);   
				break;
			default: break;
		}
	}   
	return(0);
}

void DrawFix1(Display* dpy, Window win, GC* gc, int x, int y)
{
	int rad = 7;
	int len = 20;
	int n = 7;
	XDrawArc(dpy, win, gc[0], x - rad, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawLine(dpy, win, gc[0], x, y, x + len, y + len);
	XDrawLine(dpy, win, gc[0], x, y, x - len, y + len);
	XDrawLine(dpy, win, gc[0], x + len, y + len, x - len, y + len);
	for (int i = 1; i < n + 1; i++)
		XDrawLine(dpy, win, gc[2], x - len + (len * 2 / n) * i, y + len, x - len - 10 + (len * 2 / n) * i, y + len + 10);
	XFillArc(dpy, win, gc[1], x - rad, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
}

void DrawFix2(Display* dpy, Window win, GC* gc, int x, int y)
{
	int rad = 7;
	int len = 20;
	int n = 7;
	XDrawArc(dpy, win, gc[0], x - rad, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawArc(dpy, win, gc[0], x - rad, y - rad + len, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawLine(dpy, win, gc[0], x, y, x, y + len);
	XFillArc(dpy, win, gc[1], x - rad, y - rad + len, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawLine(dpy, win, gc[0], x + len, y + len + rad, x - len, y + len + rad);
	for (int i = 1; i < n + 1; i++)
		XDrawLine(dpy, win, gc[2], x - len + (len * 2 / n) * i, y + len + rad, x - len - 10 + (len * 2 / n) * i, y + len + rad + 10);
	XFillArc(dpy, win, gc[1], x - rad, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
}

void DrawFix3(Display* dpy, Window win, GC* gc, int x, int y)
{
	int rad = 7;
	int len = 20;
	int n = 7;
	XDrawArc(dpy, win, gc[0], x - rad, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawArc(dpy, win, gc[0], x - rad - len, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawLine(dpy, win, gc[0], x, y, x - len, y);
	XFillArc(dpy, win, gc[1], x - rad - len, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
	XDrawLine(dpy, win, gc[0], x - len - rad, y + len, x - len - rad, y - len);
	for (int i = 1; i < n + 2; i++)
		XDrawLine(dpy, win, gc[2], x - len - rad, y + len - (len * 2 / n) * i, x - len - rad - 10, y + len + 10 - (len * 2 / n) * i);
	XFillArc(dpy, win, gc[1], x - rad, y - rad, rad * 2, rad * 2, 0 * 64, 360 * 64);
}

void DrawArrowY(Display* dpy, Window win, GC* gc, int x, int y, int s)
{
	int len = (s > 0) ? 30 : -30;
	XDrawLine(dpy, win, gc[3], x, y, x, y + len);
	XDrawLine(dpy, win, gc[3], x, y, x + 5, y + len / 3);
	XDrawLine(dpy, win, gc[3], x, y, x - 5, y + len / 3);
}

void DrawArrowX(Display* dpy, Window win, GC* gc, int x, int y, int s)
{
	int len = (s < 0) ? 30 : -30;
	XDrawLine(dpy, win, gc[3], x, y, x + len, y);
	XDrawLine(dpy, win, gc[3], x, y, x + len / 3, y + 5);
	XDrawLine(dpy, win, gc[3], x, y, x + len / 3, y -5);
}




