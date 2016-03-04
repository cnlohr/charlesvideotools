//Copyright 2011-2014 <>< Charles Lohr - Licensed under the MIT/x11 or New BSD Licenses.  You choose.
//Except the one part that is copied from xosc (as noted)

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xinerama.h>
#include <string.h>

const char * const default_font = "-misc-fixed-medium-r-semicondensed--*-*-*-*-c-*-*-*";

int width, height, xpos;
Display * disply;
int screens;
Window window;
int depth;
//int lineheight = 3;
GC gc;
GC mask_gc;
GC mask_gc_back;

int x, y, w, h;

//This function is copied from xosc and is under the LGPL
static void
stay_on_top(Display * dpy, Window win)
{
  Atom gnome, net_wm, type;
  int format;
  unsigned long nitems, bytesafter;
  unsigned char *args = NULL;
  Window root = DefaultRootWindow(dpy);

  gnome = XInternAtom(dpy, "_WIN_SUPPORTING_WM_CHECK", False);
  net_wm = XInternAtom(dpy, "_NET_SUPPORTED", False);

  /*
   * gnome-compilant 
   * tested with icewm + WindowMaker 
   */
  if (Success == XGetWindowProperty
      (dpy, root, gnome, 0, (65536 / sizeof(long)), False,
       AnyPropertyType, &type, &format, &nitems, &bytesafter, &args) &&
      nitems > 0) {
    /*
     * FIXME: check capabilities 
     */
    XClientMessageEvent xev;
    Atom gnome_layer = XInternAtom(dpy, "_WIN_LAYER", False);

    memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.window = win;
    xev.message_type = gnome_layer;
    xev.format = 32;
    xev.data.l[0] = 6 /* WIN_LAYER_ONTOP */ ;

    XSendEvent(dpy, DefaultRootWindow(dpy), False, SubstructureNotifyMask,
               (XEvent *) & xev);
    XFree(args);
  }
  /*
   * netwm compliant.
   * tested with kde 
   */
  else if (Success == XGetWindowProperty
           (dpy, root, net_wm, 0, (65536 / sizeof(long)), False,
            AnyPropertyType, &type, &format, &nitems, &bytesafter, &args)
           && nitems > 0) {
    XEvent e;
    Atom net_wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom net_wm_top = XInternAtom(dpy, "_NET_WM_STATE_STAYS_ON_TOP", False);

    memset(&e, 0, sizeof(e));
    e.xclient.type = ClientMessage;
    e.xclient.message_type = net_wm_state;
    e.xclient.display = dpy;
    e.xclient.window = win;
    e.xclient.format = 32;
    e.xclient.data.l[0] = 1 /* _NET_WM_STATE_ADD */ ;
    e.xclient.data.l[1] = net_wm_top;
    e.xclient.data.l[2] = 0l;
    e.xclient.data.l[3] = 0l;
    e.xclient.data.l[4] = 0l;

    XSendEvent(dpy, DefaultRootWindow(dpy), False,
               SubstructureRedirectMask, &e);
    XFree(args);
  }
  XRaiseWindow(dpy, win);
}


int main(int argc, char *argv[])
{
	XGCValues xgcv;
	memset( &xgcv, 0, sizeof( xgcv ) );
	xgcv.graphics_exposures = False;

	system( "killall -o 1s boxer" );
	if( argc != 5 )
	{
		fprintf( stderr, "Error: expects four parameters ./boxer [x] [y] [w] [h]\n" );
		return -1;
	}

	x = atoi( argv[1] );
	y = atoi( argv[2] );
	w = atoi( argv[3] );
	h = atoi( argv[4] );

	int event_basep, error_basep, a, b;
	int xinerama_screen;
	XineramaScreenInfo *screeninfo = NULL;

	if (setlocale(LC_ALL, "") == NULL || !XSupportsLocale())
		fprintf(stderr, "Locale not available, expect problems with fonts.\n");

	char * dpy = getenv("DISPLAY");

	Display * display = XOpenDisplay(dpy);
	if (!display) {
		fprintf( stderr, "Error: Cannot open display.\n" );
		return -1;
	}
	int screen = XDefaultScreen(display);

	if (!XShapeQueryExtension(display, &event_basep, &error_basep))
	{
    		fprintf( stderr, "X-Server does not support shape extension" );
		return -1;
	}

 	Visual * visual = DefaultVisual(display, screen);
	depth = DefaultDepth(display, screen);

	if (XineramaQueryExtension(display, &a, &b ) &&
		(screeninfo = XineramaQueryScreens(display, &screens)) &&
		XineramaIsActive(display) && xinerama_screen >= 0 &&
		xinerama_screen < screens ) {
		width = screeninfo[xinerama_screen].width;
		height = screeninfo[xinerama_screen].height;
		xpos = screeninfo[xinerama_screen].x_org;
	} else
	{
		width = XDisplayWidth(display, screen);
		height = XDisplayHeight(display, screen);
		xpos = 0;
	}
	if (screeninfo)
	XFree(screeninfo);


	XSetWindowAttributes setwinattr;
	setwinattr.override_redirect = 1;

	window = XCreateWindow(display, XRootWindow(display, screen),
		0, 0, width, height,
		0, depth, CopyFromParent, visual, CWOverrideRedirect, &setwinattr);

	XSelectInput(display, window, ExposureMask);

	//printf( "Window: %p (%d x %d @ %d)\n", window, width, height, depth );
	XStoreName(display, window, "xzbar");

	int mask_bitmap  = XCreatePixmap(display, window, width, height, 1);

	int line_bitmap = XCreatePixmap(display, window, width, height, depth);

	gc = XCreateGC(display, window, GCGraphicsExposures, &xgcv);
	mask_gc = XCreateGC( display, mask_bitmap, GCGraphicsExposures, &xgcv);
	mask_gc_back = XCreateGC( display, mask_bitmap, GCGraphicsExposures, &xgcv);

	XSetBackground(display, gc, WhitePixel(display, screen));

	XSetForeground(display, mask_gc_back, BlackPixel(display, screen));
	XSetBackground(display, mask_gc_back, WhitePixel(display, screen));

	XSetForeground(display, mask_gc, WhitePixel(display, screen));
	XSetBackground(display, mask_gc, BlackPixel(display, screen));

	stay_on_top( display, window );


	XMapRaised(display, window);

	char **missing;
	int nmissing;
	char *defstr;
	_XOC * fontset = XCreateFontSet( display, default_font, &missing, &nmissing, &defstr);

	while(1)
	{
		XEvent e;
		int i;

		XSetForeground(display, gc, 0);
		XFillRectangle(display, mask_bitmap, mask_gc_back, 0, 0, width, height);
		XSetForeground(display, gc, 0xFF00FF);

/*
		for (i = 0; i < 100; i++ ) {
			XRectangle t;
			t.x = rand()%1000;
			t.y = 100;
			t.width = 5;
			t.height = 20;
			XFillRectangles(display, mask_bitmap, mask_gc, &t, 1);
			XFillRectangles(display, line_bitmap, gc, &t, 1);
		}
*/

/*
		XRectangle t;
		t.x = x;
		t.y = y;
		t.width = w;
		t.height = h;
*/
		int addx = x + w;
		int addy = y + h;
		int ox = x;
		int oy = y;

		if( ox > 0 )
			XDrawLine( display, mask_bitmap, mask_gc, ox, addy, ox, oy );
		else
			ox = 0;

		if( oy > 0 )
			XDrawLine( display, mask_bitmap, mask_gc, ox, oy, addx, oy );
		else
			oy = 0;

		XDrawLine( display, mask_bitmap, mask_gc, addx, oy, addx, addy );
		XDrawLine( display, mask_bitmap, mask_gc, addx, addy, ox, addy );

		if( ox > 0 )
			XDrawLine( display, line_bitmap, gc, ox, addy, ox, oy );
		else
			ox = 0;

		if( oy > 0 )
			XDrawLine( display, line_bitmap, gc, ox, oy, addx, oy );
		else
			oy = 0;
	

		XDrawLine( display, line_bitmap, gc, addx, oy, addx, addy );
		XDrawLine( display, line_bitmap, gc, addx, addy, ox, addx );

	//	XDrawRectangles(display, mask_bitmap, mask_gc, &t, 1);
	//	XDrawRectangles(display, line_bitmap, gc, &t, 1);


		//Combine the mask again.
		XShapeCombineMask(display, window, ShapeBounding, 0, 0, mask_bitmap, ShapeSet);

//		XNextEvent(display, &e);

//		if (e.type == Expose) {
			//printf( "Expose\n" );
			XCopyArea(display, line_bitmap, window, gc, 0, 0, width, height, 0, 0);
//		}

		XFlush(display);
		usleep(100000);
	}
}
