#ifndef lint
static char vcid[] = "$Id: xinit.c,v 1.5 1994/07/31 16:02:17 gropp Exp $";
#endif

#include <stdio.h>
#include "tools.h"
#include "xtools/ximpl.h"

/* 
   This file contains routines to open an X window display and window
   This consists of a number of routines that set the various
   fields in the XBaseWindow structure, which is passed to 
   all of these routines.

   Note that if you use the default visual and colormap, then you
   can use these routines with any X toolkit that will give you the
   Window id of the window that it is managing.  Use that instead of the
   call to XBCreateWindow .  Similarly for the Display.
 */

/*@
  XBWinCreate - Create an XBWin structure that can be used by all other XB
  routines
 @*/
XBWindow XBWinCreate()
{
struct XBiWindow *w;

w = NEW(struct XBiWindow);
CHKPTRN(w);

/* Initialize the structure */
w->disp      = 0;
w->screen    = 0;
w->win       = 0;
w->gc.set    = 0;
w->vis       = 0;
w->numcolors = 0;
w->maxcolors = 0;
w->cmap      = 0;
w->foreground= 0;
w->background= 0;
w->x         = 0;
w->y         = 0;
w->w         = 0;
w->h         = 0;
w->drw       = 0;
return w;
}

/*@
  XBWinDestroy - Recover an XBWindow structure

  Input parameter:
. w  - XB window structure to destroy.  
@*/
void XBWinDestroy( w )
XBWindow w;
{
/* Should try to recover X resources ... */
FREE( w );
}

/*@
  XBOpenDisplay - Open a display
  
  Input Parameters:
  XBWin        - pointer to base window structure
  display_name - either null ("") or of the form "host:0"
@*/
int XBOpenDisplay( XBWin, display_name )
XBWindow XBWin;
char     *display_name;
{
if (display_name && display_name[0] == 0)
    display_name = 0;
XBWin->disp = XOpenDisplay( display_name );

if (!XBWin->disp) 
    return ERR_CAN_NOT_OPEN_DISPLAY;

/* Set the default screen */
XBWin->screen = DefaultScreen( XBWin->disp );

/* ? should this set defaults? */
return ERR_NONE;
}

/* @ 
    XBSetVisual - set the visual class for a window and colormap

    Input Parameters:
.   nc - number of colors.  Use the maximum of the visual if
    nc == 0.  Use nc = 2 for black and white displays.
 @ */
int XBSetVisual( XBWin, q_default_visual, cmap, nc )
XBWindow XBWin;
int      q_default_visual;
Colormap cmap;
int      nc;
{
if (q_default_visual) {
    XBWin->vis    = DefaultVisual( XBWin->disp, XBWin->screen );
    XBWin->depth  = DefaultDepth(XBWin->disp,XBWin->screen);
    if (!cmap)
        XBWin->cmap  = DefaultColormap( XBWin->disp, XBWin->screen );
    else
        XBWin->cmap  = cmap;
    }
else {
    /* Try to match to some popular types */
    XVisualInfo vinfo;
    if (XMatchVisualInfo( XBWin->disp, XBWin->screen, 24, DirectColor,
			 &vinfo)) {
	XBWin->vis    = vinfo.visual;
	XBWin->depth  = 24;
	}
    else if (XMatchVisualInfo( XBWin->disp, XBWin->screen, 8, PseudoColor,
			 &vinfo)) {
	XBWin->vis    = vinfo.visual;
	XBWin->depth  = 8;
	}
    else if (XMatchVisualInfo( XBWin->disp, XBWin->screen,
			 DefaultDepth(XBWin->disp,XBWin->screen), PseudoColor,
			 &vinfo)) {
	XBWin->vis    = vinfo.visual;
	XBWin->depth  = DefaultDepth(XBWin->disp,XBWin->screen);
	}
    else {
	XBWin->vis    = DefaultVisual( XBWin->disp, XBWin->screen );
	XBWin->depth  = DefaultDepth(XBWin->disp,XBWin->screen);
	}
    /* There are other types; perhaps this routine should accept a 
       "hint" on which types to look for. */
    XBWin->cmap = (Colormap) 0;
    }

/* reset the number of colors from info on the display, the colormap */
XBInitColors( XBWin, cmap, nc );
return ERR_NONE;
}

/* @
   XBSetGC - set the GC structure in the base window
 @ */
XBSetGC( XBWin, fg )
XBWindow XBWin;
PixVal   fg;
{
XGCValues       gcvalues;       /* window graphics context values */

/* Set the graphics contexts */
/* create a gc for the ROP_SET operation (writing the fg value to a pixel) */
/* (do this with function GXcopy; GXset will automatically write 1) */
gcvalues.function   = GXcopy;
gcvalues.foreground = fg;
XBWin->gc.cur_pix   = fg;
XBWin->gc.set = XCreateGC( XBWin->disp, RootWindow(XBWin->disp,XBWin->screen),
                              GCFunction | GCForeground, &gcvalues );
return ERR_NONE;
}

/*@
   XBOpenWindow - Open the window data structure 

   Note that before a window can be opened, the Visual class and the
   (initial) colormap should be set.
    Opening a window comes in two parts:
    Creating most of the local window structure, and
    actually creating and mapping the window.
    This allows us to defer the choice of the window size until
    the various tools have been set (note that many tools have
    minimum sizes determined by things like the current font size).

    This needs to know a few more things inorder to co-exist with
    other tools and applications.  In particular, it needs to know
    whether to use the default visual or another window's colormap.
 @*/
int XBOpenWindow( XBWin )
XBWindow XBWin;
{
return ERR_NONE;
}

/*
    Actually display a window at [x,y] with sizes (w,h)
    If w and/or h are 0, use the sizes in the fields of XBWin
    (which may have been set by, for example, XBSetWindowSize)
 */
int XBDisplayWindow( XBWin, label, x, y, w, h, backgnd_pixel )
XBWindow XBWin;
char     *label;
int      w, h, x, y;
PixVal   backgnd_pixel;
{
unsigned int    wavail, havail;
XSizeHints      size_hints;
int             q_user_pos;
XWindowAttributes       in_window_attributes;
XSetWindowAttributes    window_attributes;
int                     depth, border_width;
unsigned long           wmask;

/* get the available widths */
wavail              = DisplayWidth(  XBWin->disp, XBWin->screen );
havail              = DisplayHeight( XBWin->disp, XBWin->screen );

if (w <= 0 || h <= 0) return ERR_ILLEGAL_SIZE;

if (w > wavail)
    w   = wavail;
if (h > havail)
    h   = havail;

/* Find out if the user specified the position */
q_user_pos  = (x >= 0) && (y >= 0);

border_width   = 0;
if (x < 0) x   = 0;
if (y < 0) y   = 0;
x   = (x + w > wavail) ? wavail - w : x;
y   = (y + h > havail) ? havail - h : y;

/* We need XCreateWindow since we may need an visual other than
   the default one */
XGetWindowAttributes( XBWin->disp, RootWindow(XBWin->disp,XBWin->screen),
                      &in_window_attributes );
window_attributes.background_pixmap = None;
window_attributes.background_pixel  = backgnd_pixel;
/* No border for now */
window_attributes.border_pixmap     = None;
/* 
window_attributes.border_pixel      = border_pixel; 
 */
window_attributes.bit_gravity       = in_window_attributes.bit_gravity;
window_attributes.win_gravity       = in_window_attributes.win_gravity;
        /* Backing store is too slow in color systems */
window_attributes.backing_store     = 0;
window_attributes.backing_pixel     = backgnd_pixel;
window_attributes.save_under        = 1;
window_attributes.event_mask        = 0;
window_attributes.do_not_propagate_mask = 0;
window_attributes.override_redirect = 0;
window_attributes.colormap          = XBWin->cmap;
/* None for cursor does NOT mean none, it means Parent's cursor */
window_attributes.cursor            = None; 
wmask   = CWBackPixmap | CWBackPixel | CWBorderPixmap | CWBitGravity |
          CWWinGravity | CWBackingStore | CWBackingPixel | CWOverrideRedirect |
          CWSaveUnder  | CWEventMask    | CWDontPropagate |
          CWCursor     | CWColormap ;
/* depth should really be the depth of the visual in use */
depth       = DefaultDepth( XBWin->disp, XBWin->screen );
XBWin->win  = XCreateWindow( XBWin->disp, 
			     RootWindow(XBWin->disp,XBWin->screen),
                             x, y, w, h, border_width,
                             depth, InputOutput, XBWin->vis,
                             wmask, &window_attributes );

if (!XBWin->win) 
    return ERR_CAN_NOT_OPEN_WINDOW;

/* set resize hints (prohibit?) */
size_hints.x            = x;
size_hints.y            = y;
size_hints.min_width    = 4*border_width;
size_hints.min_height   = 4*border_width;
size_hints.width        = w;
size_hints.height       = h;
if (q_user_pos)
    size_hints.flags        = USPosition | USSize | PMinSize; /*  | PAspect; */
else
    size_hints.flags        = PPosition  | PSize  | PMinSize; /*  | PAspect; */

/* Set the standard properties */
XSetStandardProperties( XBWin->disp, XBWin->win, label, label, 0,
                        (char **)0, 0, &size_hints );

/* make the window visible */
XSelectInput( XBWin->disp, XBWin->win, ExposureMask | StructureNotifyMask );
XMapWindow( XBWin->disp, XBWin->win );

/* some window systems are cruel and interfere with the placement of
   windows.  We wait here for the window to be created or to die */
if (XB_wait_map( XBWin, (void (*)())0 )) {
    XBWin->win    = (Window)0;
    return 0;
    }
/* Initial values for the upper left corner */
XBWin->x = 0;
XBWin->y = 0;
return 0;
}



/* 
   There should also be a routine that gets this data from a widget, so
   that this structure can be used with a widget set 
 */

/*@C
  XBGetArgs - Get the X related arguments (geometry for now)

  Input Parameters:
. Argc - pointer to argument count
. argv - argument vector
. flag - 1 if the "found" arguments should be stripped from the argument list

  Output Parameters:
. px,py - position of window
. pw,ph - width and height of window

  Note:
  The output parameters are unchanged if they are not set.  The form of the
  argument is
$ -geometry width x height + xoffset + yoffset
$ with no spaces.  For example
$ -geometry 400x400
$ for a 400 x 400 window, and
$ -geometry +100+200
$ for a window located at (100,200).
 @*/
void XBGetArgs( Argc, argv, flag, px, py, pw, ph )
int  *Argc, flag;
char **argv;
int  *px, *py, *pw, *ph;
{
char         val[128];
int          vallen;
int          st, xx, yy;
unsigned int ww, hh;

vallen = 128;
if (SYArgGetString( Argc, argv, flag, "-geometry", val, vallen )) {
    /* value is of form wxh+x+y */
    st  = XParseGeometry( val, &xx, &yy, &ww, &hh );
    if (st & XValue)        *px = xx;
    if (st & YValue)        *py = yy;
    if (st & WidthValue)    *pw = (int)ww;
    if (st & HeightValue)   *ph = (int)hh;
    }
}

/*@C
  XBGetArgsDisplay - Get the X display name from the argument list

  Input Parameters:
. Argc - pointer to argument count
. argv - argument vector
. flag - 1 if the "found" arguments should be stripped from the argument list
. dlen - length of dname

  Output Parameters:
. dname - Name of display

  Note:
  The output parameter is unchanged if they are not set.  The form of the
  argument is
$ -display name
 @*/
void XBGetArgsDisplay( Argc, argv, flag, dlen, dname )
int  *Argc, flag;
char **argv, *dname;
int  dlen;
{
SYArgGetString( Argc, argv, flag, "-display", dname, dlen );
}


int XBiQuickWindow( mywindow, host, name, x, y, nx, ny, nc )
XBWindow mywindow;
char     *host, *name;
int      x, y, nx, ny, nc;
{
if (XBOpenDisplay( mywindow, host )) {
    fprintf( stderr, "Could not open display\n" );
    return 1;
    }
if (XBSetVisual( mywindow, 1, (Colormap)0, nc )) {
    fprintf( stderr, "Could not set visual to default\n" );
    return 1;
    }
if (XBOpenWindow( mywindow )) {
    fprintf( stderr, "Could not open the window\n" );
    return 1;
    }
if (XBDisplayWindow( mywindow, name, x, y, nx, ny, (PixVal)0 )) {
    fprintf( stderr, "Could not display window\n" );
    return 1;
    }
XBSetGC( mywindow, mywindow->cmapping[1] );
XBClearWindow(mywindow,0,0,mywindow->w,mywindow->h);
return 0;
}

/*@
   XBQuickWindow - Create an X window

   Input parameters:
.  mywindow - A pointer to an XBWindow structure that will be used to hold
              information on the window.  This should be acquired with
	      XBWinCreate.
.  host     - name of the display
.  name     - title (name) of the window
.  x,y      - coordinates of the upper left corner of the window.  If <0,
              use user-positioning (normally, the window manager will
	      ask the user where to put the window)
.  nx,ny    - width and height of the window

   Note:
   This creates a window with various defaults (visual, colormap, etc)

   A small modification to this routine would allow Black and White windows
   to be used on color displays; this would be useful for testing codes.

   Use XBWinCreate to create a valid XBWindow for this routine.
@*/
int XBQuickWindow( mywindow, host, name, x, y, nx, ny )
XBWindow mywindow;
char     *host, *name;
int      x, y, nx, ny;
{
/* Just to be careful, clear mywindow */
MEMSET( mywindow, 0, sizeof(XBWindow) );
return XBiQuickWindow( mywindow, host, name, x, y, nx, ny, 0 );
}

/* 
   And a quick version (from an already defined window) 
 */
int XBQuickWindowFromWindow( mywindow, host, win )
XBWindow mywindow;
char     *host;
Window   win;
{
Window       root;
int          d;
unsigned int ud;

if (XBOpenDisplay( mywindow, host )) {
    fprintf( stderr, "Could not open display\n" );
    return 1;
    }
if (XBSetVisual( mywindow, 1, (Colormap)0, 0 )) {
    fprintf( stderr, "Could not set visual to default\n" );
    return 1;
    }

mywindow->win = win;
XGetGeometry( mywindow->disp, mywindow->win, &root, 
              &d, &d, 
	      (unsigned int *)&mywindow->w, (unsigned int *)&mywindow->h,
              &ud, &ud );
mywindow->x = mywindow->y = 0;

XBSetGC( mywindow, mywindow->cmapping[1] );
return 0;
}

/*@
    XBFlush - Flush all X 11 requests.

    Input parameter:
.   XBWin - window

    Note:  Using an X drawing routine does not necessarily cause the
    the server to receive and draw the requests.  Use this routine
    if necessary to force the server to draw (doing so may slow down
    the program, so don't insert unnecessary XBFlush calls).

    If double-buffering is enabled, this routine copies from the buffer
    to the window before flushing the requests.  This is the appropriate
    action for animation.
@*/
void XBFlush( XBWin )
XBWindow XBWin;
{
if (XBWin->drw) {
    XCopyArea( XBWin->disp, XBWin->drw, XBWin->win, XBWin->gc.set, 0, 0, 
	       XBWin->w, XBWin->h, XBWin->x, XBWin->y );
    }
XFlush( XBWin->disp );
}

/*@
      XBSetWindowLabel - Sets new label in open window.

  Input Parameters:
.  window - Window to set label for
.  label  - Label to give window
@*/
void XBSetWindowLabel( XBwin, label )
XBWindow XBwin;
char     *label;
{
  XTextProperty prop;
  XGetWMName(XBwin->disp,XBwin->win,&prop);
  prop.value = (unsigned char *)label; prop.nitems = (long) strlen(label);
  XSetWMName(XBwin->disp,XBwin->win,&prop);
}

/*@
  XBCaptureWindowToFile - Capture a window and write it in xwd format to a file

  Input Parameters:
. XBWin - XB Window to write out
. fname - name of file

  Notes:
  This command uses XGetImage; thus, the window must be unobscured.  The
  current implementation actually uses xwd called by spawning a process.  Later
  implementations may write the file directly.

  Be sure that you have XBFlush()'ed the output; X11 does not require that
  the image be current until a flush is executed.
@*/
void XBCaptureWindowToFile( XBWin, fname )
XBWindow XBWin;
char     *fname;
{
char cmdbuf[1024];

sprintf( cmdbuf, "xwd -id %d > %s\n", XBWin->win, fname );
system( cmdbuf );
}

/*
 */
int XBWinWidth( XBWin )
XBWindow XBWin;
{
return XBWin->w;
}

int XBWinHeight( XBWin )
XBWindow XBWin;
{
return XBWin->h;
}

int XBWinX( XBWin )
XBWindow XBWin;
{
return XBWin->x;
}

int XBWinY( XBWin )
XBWindow XBWin;
{
return XBWin->y;
}

/* 
 */
PixVal XBWinForeground( XBWin )
XBWindow XBWin;
{
return XBWin->foreground;
}

void XBSetToForeground( XBWin )
XBWindow XBWin;
{
if (XBWin->gc.cur_pix != XBWin->foreground) { 
    XSetForeground( XBWin->disp, XBWin->gc.set, XBWin->foreground ); 
    XBWin->gc.cur_pix   = XBWin->foreground;
    }
}

void XBSetToBackground( XBWin )
XBWindow XBWin;
{
if (XBWin->gc.cur_pix != XBWin->background) { 
    XSetForeground( XBWin->disp, XBWin->gc.set, XBWin->background ); 
    XBWin->gc.cur_pix   = XBWin->background;
    }
}

PixVal XBGetForeground( XBWin )
XBWindow XBWin;
{
return XBWin->foreground;
}

PixVal XBGetBackground( XBWin )
XBWindow XBWin;
{
return XBWin->background;
}

/*@
   XBSetForegroundByIndex - Set the foreground color to the given value

   Input Parameters:
.  XBWin - XBWindow structure
.  icolor - Index into the pre-establised window color map.
@*/
void  XBSetForegroundByIndex( XBWin, icolor )
XBWindow XBWin;
int      icolor;
{
PixVal pixval = XBWin->cmapping[icolor];
if (XBWin->gc.cur_pix != pixval) { 
    XSetForeground( XBWin->disp, XBWin->gc.set, pixval ); 
    XBWin->gc.cur_pix   = pixval;
    }
}

PixVal XBGetPixvalByIndex( XBWin, icolor )
XBWindow XBWin;
int      icolor;
{
return XBWin->cmapping[icolor];
}

/*@
   XBSetForeground - Set the foreground color to the given pixel value

   Input Parameters:
.  XBWin - XBWindow structure
.  pixval - Pixel value 
@*/
void  XBSetForeground( XBWin, pixval )
XBWindow XBWin;
PixVal   pixval;
{
if (XBWin->gc.cur_pix != pixval) { 
    XSetForeground( XBWin->disp, XBWin->gc.set, pixval ); 
    XBWin->gc.cur_pix   = pixval;
    }
}

/*@
  XBFillRectangle - Fills a rectangle

@*/
void XBFillRectangle( XBWin, x, y, w, h )
XBWindow XBWin;
int      x, y, w, h;
{
XFillRectangle( XBWin->disp, XBDrawable(XBWin), XBWin->gc.set, x, y, w, h );
}

/*@
  XBDrawLine - Draws a line
@*/
void XBDrawLine( XBWin, x1, y1, x2, y2 )
XBWindow XBWin;
int      x1, y1, x2, y2;
{
XDrawLine( XBWin->disp, XBWin->win, XBWin->gc.set, x1, y1, x2, y2 );
}


/*
   XBWinSetSize

   Lets you change the effective size/location of a window
 */
void XBWinSetSize( XBWin, x, y, w, h )
XBWindow XBWin;
int      x, y, w, h;
{
XBWin->x = x;
XBWin->y = y;
XBWin->w = w;
XBWin->h = h;
}

void XBDrawSegments( XBWin, nsegs, segs )
XBWindow  XBWin;
int       nsegs;
XBSegment *segs;
{
XDrawSegments( XBWin->disp, XBWin->win, XBWin->gc.set, 
	       (XSegment *)segs, nsegs );
}

void XBDrawLines( XBWin, pts, npts )
XBWindow XBWin;
XBPoint  *pts;
int      npts;
{
XDrawLines( XBWin->disp, XBWin->win, XBWin->gc.set, 
	    (XPoint *)pts, npts, CoordModeOrigin );
}

/*@
      XBPixFromInteger - Looks up an entry in the win->cmapping

    Input Parameters:
     color - an integer color as listed in xtools/baseclr.h

@*/
PixVal XBPixFromInteger(win,color)
XBWindow win;
int      color; 
{
return (win)->cmapping[color];
}


