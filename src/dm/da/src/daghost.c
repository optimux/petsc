#ifndef lint
static char vcid[] = "$Id: daghost.c,v 1.8 1997/01/06 20:30:32 balay Exp bsmith $";
#endif
 
/*
  Code for manipulating distributed regular arrays in parallel.
*/

#include "src/da/daimpl.h"    /*I   "da.h"   I*/

#undef __FUNC__  
#define __FUNC__ "DAGetGhostCorners" /* ADIC Ignore */
/*@
    DAGetGhostCorners - Returns the global (x,y,z) indices of the lower left
    corner of the local region, including ghost points.

   Input Parameter:
.  da - the distributed array

   Output Parameters:
.  x,y,z - the corner indices
$    y and z are optional (used for 2D and 3D problems)
.  m,n,p - widths in the corresponding directions
$    n and p are optional (used for 2D and 3D problems)

   Note:
   Any of y, z, n, and p should be set to PETSC_NULL if not needed.

.keywords: distributed array, get, ghost, corners, nodes, local indices

.seealso: DAGetCorners()
@*/
int DAGetGhostCorners(DA da,int *x,int *y,int *z,int *m, int *n, int *p)
{
  int w;

  PetscValidHeaderSpecific(da,DA_COOKIE);
  /* since the xs, xe ... have all been multiplied by the number of degrees 
     of freedom per cell, w = da->w, we divide that out before returning.*/
  w = da->w;  
  if (x) *x = da->Xs/w; if (m) *m = (da->Xe - da->Xs)/w;
  if (y) *y = da->Ys;   if (n) *n = (da->Ye - da->Ys);
  if (z) *z = da->Zs;   if (p) *p = (da->Ze - da->Zs); 
  return 0;
}

