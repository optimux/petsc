/* $Id: viewer.h,v 1.10 1995/06/21 22:31:42 bsmith Exp curfman $ */

#if !defined(__VIEWER_PACKAGE)
#define __VIEWER_PACKAGE

#include "petsc.h"

typedef struct _Viewer*      Viewer;
#define VIEWER_COOKIE        PETSC_COOKIE+1
#define MATLAB_VIEWER        0
#define FILE_VIEWER          1
#define FILES_VIEWER         2

#define FILE_FORMAT_DEFAULT  0
#define FILE_FORMAT_MATLAB   1
#define FILE_FORMAT_IMPL     2
#define FILE_FORMAT_INFO     3

extern int ViewerFileOpen(char*,Viewer *);
extern int ViewerFileOpenSync(char*,MPI_Comm,Viewer *);
extern int ViewerFileSetFormat(Viewer,int,char *);
extern int ViewerMatlabOpen(char*,int,Viewer *);
extern int ViewerDestroy(Viewer);

extern Viewer STDOUT_VIEWER;  
extern Viewer STDERR_VIEWER;
extern Viewer SYNC_STDOUT_VIEWER;

#endif
