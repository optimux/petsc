#ifndef lint
static char vcid[] = "$Id: dsflush.c,v 1.9 1997/01/06 20:26:34 balay Exp bsmith $";
#endif
/*
       Provides the calling sequences for all the basic Draw routines.
*/
#include "src/draw/drawimpl.h"  /*I "draw.h" I*/

#undef __FUNC__  
#define __FUNC__ "DrawSyncFlush" /* ADIC Ignore */
/*@
   DrawSyncFlush - Flushes graphical output. This waits until all 
   processors have arrived and flushed, then does a global flush.
   This is usually done to change the frame for double buffered graphics.

  Input Parameters:
.  draw - the drawing context

.keywords: draw, sync, flush

@*/
int DrawSyncFlush(Draw draw)
{
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  if (draw->type == DRAW_NULLWINDOW) return 0;
  if (draw->ops.syncflush) return (*draw->ops.syncflush)(draw);
  return 0;
}
