/* 
   This should not be included in users code.

  Includes definition of structure for seqential double precision vectors

  These are shared by dvec1.c dvec2.c dvec3.c bvec1.c bvec2.c 
  pvectors/pvec.c pvectors/pbvec.c 
*/

#ifndef __DVECIMPL 
#define __DVECIMPL

#include "private/vecimpl.h"

typedef struct { 
  VECHEADER
} Vec_Seq;

EXTERN PetscErrorCode VecMDot_Seq(Vec,PetscInt,const Vec[],PetscScalar *);
EXTERN PetscErrorCode VecMTDot_Seq(Vec,PetscInt,const Vec[],PetscScalar *);
EXTERN PetscErrorCode VecMin_Seq(Vec,PetscInt*,PetscReal *);
EXTERN PetscErrorCode VecSet_Seq(Vec,PetscScalar);
EXTERN PetscErrorCode VecSetRandom_Seq(Vec,PetscRandom);
EXTERN PetscErrorCode VecMAXPY_Seq(Vec,PetscInt,const PetscScalar *,Vec *);
EXTERN PetscErrorCode VecAYPX_Seq(Vec,PetscScalar,Vec);
EXTERN PetscErrorCode VecWAXPY_Seq(Vec,PetscScalar,Vec,Vec);
EXTERN PetscErrorCode VecAXPBYPCZ_Seq(Vec,PetscScalar,PetscScalar,PetscScalar,Vec,Vec);
EXTERN PetscErrorCode VecPointwiseMult_Seq(Vec,Vec,Vec);
EXTERN PetscErrorCode VecPointwiseMax_Seq(Vec,Vec,Vec);
EXTERN PetscErrorCode VecPointwiseMaxAbs_Seq(Vec,Vec,Vec);
EXTERN PetscErrorCode VecPointwiseMin_Seq(Vec,Vec,Vec);
EXTERN PetscErrorCode VecPointwiseDivide_Seq(Vec,Vec,Vec);
EXTERN PetscErrorCode VecMaxPointwiseDivide_Seq(Vec,Vec,PetscReal*);
EXTERN PetscErrorCode VecGetArray_Seq(Vec,PetscScalar *[]);
EXTERN PetscErrorCode VecRestoreArray_Seq(Vec,PetscScalar *[]);
EXTERN PetscErrorCode VecPlaceArray_Seq(Vec,const PetscScalar *);
EXTERN PetscErrorCode VecResetArray_Seq(Vec);
EXTERN PetscErrorCode VecReplaceArray_Seq(Vec,const PetscScalar *);
EXTERN PetscErrorCode VecGetSize_Seq(Vec,PetscInt *);
EXTERN PetscErrorCode VecDot_Seq(Vec,Vec,PetscScalar *);
EXTERN PetscErrorCode VecTDot_Seq(Vec,Vec,PetscScalar *);
EXTERN PetscErrorCode VecScale_Seq(Vec,PetscScalar);
EXTERN PetscErrorCode VecCopy_Seq(Vec,Vec);
EXTERN PetscErrorCode VecSwap_Seq(Vec,Vec);
EXTERN PetscErrorCode VecAXPY_Seq(Vec,PetscScalar,Vec);
EXTERN PetscErrorCode VecAXPBY_Seq(Vec,PetscScalar,PetscScalar,Vec);
EXTERN PetscErrorCode VecMax_Seq(Vec,PetscInt*,PetscReal *);
EXTERN PetscErrorCode VecDuplicate_Seq(Vec,Vec *);
EXTERN PetscErrorCode VecSetOption_Seq(Vec,VecOption,PetscTruth);
EXTERN PetscErrorCode VecConjugate_Seq(Vec);
EXTERN PetscErrorCode VecNorm_Seq(Vec,NormType,PetscReal*);

#if defined(PETSC_HAVE_CUDA)
  #define CHKERRCUDA(err) if (err != CUBLAS_STATUS_SUCCESS) SETERRQ1(PETSC_COMM_SELF,PETSC_ERR_LIB,"CUDA error %d",err)

  #undef __FUNCT__
  #define __FUNCT__ "VecCUDACopyToGPU"
  PETSC_STATIC_INLINE PetscErrorCode VecCUDACopyToGPU(Vec v)
  {
    Vec_Seq        *vs = (Vec_Seq*)v->data;
    PetscInt       one = 1, cn = v->map->n;
    PetscErrorCode ierr;

    PetscFunctionBegin;
    if (vs->valid_GPU_array == UNALLOCATED){
      /*if this is the first time we're copying to the GPU then we allocate memory first */
      ierr = cublasAlloc(cn,sizeof(PetscScalar),(void **)&vs->GPUarray);CHKERRCUDA(ierr);
    }
    ierr = cublasSetVector(cn,sizeof(PetscScalar),vs->array,one,vs->GPUarray,one);CHKERRCUDA(ierr);
    vs->valid_GPU_array = SAME;
    PetscFunctionReturn(0);
  }

  #undef __FUNCT__
  #define __FUNCT__ "VecCUDACopyFromGPU"
PETSC_STATIC_INLINE PetscErrorCode VecCUDACopyFromGPU(Vec v)
  {
    Vec_Seq        *vs = (Vec_Seq*)v->data;
    PetscInt       one = 1, cn = v->map->n;
    PetscErrorCode ierr;

    PetscFunctionBegin;
    ierr = cublasGetVector(cn,sizeof(PetscScalar),vs->GPUarray,one,vs->array,one);CHKERRCUDA(ierr);
    vs->valid_GPU_array = SAME;
    PetscFunctionReturn(0);
  }
  #endif
  
  #endif
