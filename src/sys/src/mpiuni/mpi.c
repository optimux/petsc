#ifndef lint
static char vcid[] = "$Id: mpi.c,v 1.17 1996/03/21 20:55:21 balay Exp balay $";
#endif

#include <signal.h> 
#include "petsc.h"               /*I   "petsc.h"   I*/
#include <stdio.h>
#include <unistd.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#include "pinclude/petscfix.h" 
#define MPI_SUCCESS 0
void * MPIUNI_TMP   = 0;
double MPIUNI_DUMMY = 0;
int    MPIUNI_DATASIZE[5] = { sizeof(int),sizeof(float),sizeof(double),
                              2*sizeof(double),sizeof(char)};
double MPI_Wtime()
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

/*     Fortran versions of several routines */

#if defined(__cplusplus)
extern "C" {
#endif

/******mpi_init*******/
void  mpi_init(int *ierr)
{
  *ierr = MPI_SUCCESS;
}

void  mpi_init_(int *ierr)
{
  *ierr = MPI_SUCCESS;
}

void  mpi_init__(int *ierr)
{
  *ierr = MPI_SUCCESS;
}

void  MPI_INIT(int *ierr)
{
  *ierr = MPI_SUCCESS;
}

/******mpi_comm_size*******/
void mpi_comm_size(MPI_Comm *comm,int *size,int *ierr) 
{
  *size = 1;
  *ierr = 0;
}

void mpi_comm_size_(MPI_Comm *comm,int *size,int *ierr) 
{
  *size = 1;
  *ierr = 0;
}

void mpi_comm_size__(MPI_Comm *comm,int *size,int *ierr) 
{
  *size = 1;
  *ierr = 0;
}

void MPI_COMM_SIZE(MPI_Comm *comm,int *size,int *ierr) 
{
  *size = 1;
  *ierr = 0;
}

/******mpi_comm_rank*******/
void mpi_comm_rank(MPI_Comm *comm,int *rank,int *ierr)
{
  *rank=0;
  *ierr=MPI_SUCCESS;
}

void mpi_comm_rank_(MPI_Comm *comm,int *rank,int *ierr)
{
  *rank=0;
  *ierr=MPI_SUCCESS;
}

void mpi_comm_rank__(MPI_Comm *comm,int *rank,int *ierr)
{
  *rank=0;
  *ierr=MPI_SUCCESS;
}

void MPI_COMM_RANK(MPI_Comm *comm,int *rank,int *ierr)
{
  *rank=0;
  *ierr=MPI_SUCCESS;
}

/******mpi_wtick*******/
double mpi_wtick() 
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

double mpi_wtick_() 
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

double mpi_wtick__() 
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

double MPI_WTICK() 
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

/*******mpi_wtime******/
double mpi_wtime()
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

double mpi_wtime_()
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

double mpi_wtime__()
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

double MPI_WTIME()
{
  fprintf(stderr,"MPI_Wtime: use PetscGetTime instead.\n");
  return 0.0;
}

/*******mpi_abort******/
void mpi_abort(MPI_Comm *comm,int *errorcode,int *ierr) 
{
  PetscError(__LINE__,__DIR__,__FILE__,*errorcode,"[0] Aborting program!");
  exit(*errorcode); 
  *ierr = MPI_SUCCESS;
}

void mpi_abort_(MPI_Comm *comm,int *errorcode,int *ierr) 
{
  PetscError(__LINE__,__DIR__,__FILE__,*errorcode,"[0] Aborting program!");
  exit(*errorcode);
  *ierr = MPI_SUCCESS;
}

void mpi_abort__(MPI_Comm *comm,int *errorcode,int *ierr) 
{
  PetscError(__LINE__,__DIR__,__FILE__,*errorcode,"[0] Aborting program!");
  exit(*errorcode);
  *ierr = MPI_SUCCESS;
}

void MPI_ABORT(MPI_Comm *comm,int *errorcode,int *ierr) 
{
  PetscError(__LINE__,__DIR__,__FILE__,*errorcode,"[0] Aborting program!");
  exit(*errorcode);
  *ierr = MPI_SUCCESS;
}
/*******mpi_allreduce******/
void mpi_allreduce(void *sendbuf,void *recvbuf,int *count,int *datatype,
                   int *op,int *comm,int *ierr) 
{
  PetscMemcpy( recvbuf, sendbuf, (*count)*MPIUNI_DATASIZE[*datatype]);
  *ierr = MPI_SUCCESS;
} 
void mpi_allreduce_(void *sendbuf,void *recvbuf,int *count,int *datatype,
                   int *op,int *comm,int *ierr) 
{
  PetscMemcpy( recvbuf, sendbuf, (*count)*MPIUNI_DATASIZE[*datatype]);
  *ierr = MPI_SUCCESS;
} 
void mpi_allreduce__(void *sendbuf,void *recvbuf,int *count,int *datatype,
                   int *op,int *comm,int *ierr) 
{
  PetscMemcpy( recvbuf, sendbuf, (*count)*MPIUNI_DATASIZE[*datatype]);
  *ierr = MPI_SUCCESS;
} 
void MPI_ALLREDUCE(void *sendbuf,void *recvbuf,int *count,int *datatype,
                   int *op,int *comm,int *ierr) 
{
  PetscMemcpy( recvbuf, sendbuf, (*count)*MPIUNI_DATASIZE[*datatype]);
  *ierr = MPI_SUCCESS;
} 
#if defined(__cplusplus)
}
#endif


