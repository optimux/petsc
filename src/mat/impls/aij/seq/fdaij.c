
#include <../src/mat/impls/aij/seq/aij.h>
#include <../src/mat/impls/baij/seq/baij.h>
                              
/*
    This routine is shared by SeqAIJ and SeqBAIJ matrices, 
    since it operators only on the nonzero structure of the elements or blocks.
*/
#undef __FUNCT__
#define __FUNCT__ "MatFDColoringCreate_SeqXAIJ"
PetscErrorCode MatFDColoringCreate_SeqXAIJ(Mat mat,ISColoring iscoloring,MatFDColoring c)
{
  PetscErrorCode ierr;
  PetscInt       bs,nz,bcols,nis=iscoloring->n;
  PetscBool      isBAIJ;     
  PetscReal      mem;

  PetscFunctionBegin;
  ierr = MatGetBlockSize(mat,&bs);CHKERRQ(ierr);
  ierr = PetscObjectTypeCompare((PetscObject)mat,MATSEQBAIJ,&isBAIJ);CHKERRQ(ierr);
  if (isBAIJ) { 
    Mat_SeqBAIJ *spA = (Mat_SeqBAIJ*)mat->data;
    nz    = spA->nz;
  } else {
    Mat_SeqAIJ  *spA = (Mat_SeqAIJ*)mat->data;
    nz    = spA->nz;
    bs    = 1; /* only bs=1 is supported for SeqAIJ matrix */
  } 
  c->M       = mat->rmap->N/bs;   /* set total rows, columns and local rows */
  c->N       = mat->cmap->N/bs;
  c->m       = mat->rmap->N/bs;
  c->rstart  = 0;
  c->ncolors = nis;

  /* set default brows and bcols for speedup inserting the dense matrix into sparse Jacobian; 
     bcols is chosen s.t. dy-array takes 50% of memory space as mat */
  mem = nz*(sizeof(PetscScalar) + sizeof(PetscInt)) + 3*c->m*sizeof(PetscInt);
  bcols = (PetscInt)(0.5*mem /(c->m*sizeof(PetscScalar)));
  if (bcols > nis) bcols = nis;
  c->brows = 1000/bcols;
  c->bcols = bcols;
  c->ctype = IS_COLORING_GHOSTED;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "MatFDColoringSetUp_SeqXAIJ"
PetscErrorCode MatFDColoringSetUp_SeqXAIJ(Mat mat,ISColoring iscoloring,MatFDColoring c)
{
  PetscErrorCode ierr;
  PetscInt       i,n,nrows,N,j,k,m,ncols,col,nis=iscoloring->n,*rowhit,bs,bs2,*spidx,nz;
  const PetscInt *is,*row,*ci,*cj;
  IS             *isa;
  PetscBool      isBAIJ;     
  PetscScalar    *A_val,**valaddrhit;
  MatEntry       *Jentry,*Jentry_new;
  PetscInt       *color_start,nz_new,row_end,*row_start,*nrows_new;
  PetscInt       bcols=c->bcols;

  PetscFunctionBegin;
  ierr = ISColoringGetIS(iscoloring,PETSC_IGNORE,&isa);CHKERRQ(ierr);

  ierr = MatGetBlockSize(mat,&bs);CHKERRQ(ierr);
  ierr = PetscObjectTypeCompare((PetscObject)mat,MATSEQBAIJ,&isBAIJ);CHKERRQ(ierr);
  if (isBAIJ) { 
    Mat_SeqBAIJ *spA = (Mat_SeqBAIJ*)mat->data;
    A_val = spA->a;
    nz    = spA->nz;
  } else {
    Mat_SeqAIJ  *spA = (Mat_SeqAIJ*)mat->data;
    A_val = spA->a;
    nz    = spA->nz;
    bs    = 1; /* only bs=1 is supported for SeqAIJ matrix */
  } 

  N         = mat->cmap->N/bs;
  ierr       = PetscMalloc(nis*sizeof(PetscInt),&c->ncolumns);CHKERRQ(ierr);
  ierr       = PetscMalloc(nis*sizeof(PetscInt*),&c->columns);CHKERRQ(ierr);
  ierr       = PetscMalloc(nis*sizeof(PetscInt),&c->nrows);CHKERRQ(ierr);
  ierr       = PetscLogObjectMemory((PetscObject)c,3*nis*sizeof(PetscInt));CHKERRQ(ierr);

  ierr       = PetscMalloc(nz*sizeof(MatEntry),&Jentry);CHKERRQ(ierr);
  ierr       = PetscLogObjectMemory((PetscObject)c,nz*sizeof(MatEntry));CHKERRQ(ierr);
  c->matentry = Jentry;

  if (isBAIJ) {
    ierr = MatGetColumnIJ_SeqBAIJ_Color(mat,0,PETSC_FALSE,PETSC_FALSE,&ncols,&ci,&cj,&spidx,NULL);CHKERRQ(ierr);
  } else {
    ierr = MatGetColumnIJ_SeqAIJ_Color(mat,0,PETSC_FALSE,PETSC_FALSE,&ncols,&ci,&cj,&spidx,NULL);CHKERRQ(ierr);
  }
 
  ierr = PetscMalloc3(c->m,PetscInt,&rowhit,c->m,PetscScalar*,&valaddrhit,nis+1,PetscInt,&color_start);CHKERRQ(ierr);
  ierr = PetscMemzero(rowhit,c->m*sizeof(PetscInt));CHKERRQ(ierr);

  nz = 0;
  for (i=0; i<nis; i++) { /* loop over colors */
    color_start[i] = nz;
    
    ierr = ISGetLocalSize(isa[i],&n);CHKERRQ(ierr);
    ierr = ISGetIndices(isa[i],&is);CHKERRQ(ierr);

    c->ncolumns[i] = n;
    if (n) {
      ierr = PetscMalloc(n*sizeof(PetscInt),&c->columns[i]);CHKERRQ(ierr);
      ierr = PetscLogObjectMemory((PetscObject)c,n*sizeof(PetscInt));CHKERRQ(ierr);
      ierr = PetscMemcpy(c->columns[i],is,n*sizeof(PetscInt));CHKERRQ(ierr);
    } else {
      c->columns[i] = 0;
    }

    /* fast, crude version requires O(N*N) work */
    bs2   = bs*bs;
    nrows = 0;
    for (j=0; j<n; j++) {  /* loop over columns */
      col    = is[j];
      row    = cj + ci[col];
      m      = ci[col+1] - ci[col];  
      nrows += m;
      for (k=0; k<m; k++) {  /* loop over columns marking them in rowhit */
        rowhit[*row]       = col + 1;
        valaddrhit[*row++] = &A_val[bs2*spidx[ci[col] + k]]; 
      }
    }
    c->nrows[i] = nrows; /* total num of rows for this color */
   
    for (j=0; j<N; j++) { /* loop over rows */
      if (rowhit[j]) {
        Jentry[nz].row     = j;              /* local row index */
        Jentry[nz].col     = rowhit[j] - 1;  /* local column index */
        Jentry[nz].valaddr = valaddrhit[j];  /* address of mat value for this entry */ 
        nz++;
        rowhit[j] = 0.0;                     /* zero rowhit for reuse */
      }
    } 
    ierr = ISRestoreIndices(isa[i],&is);CHKERRQ(ierr);
  }
  color_start[nis] = nz;

  // ---------- reorder Jentry ------------
  if (!isBAIJ && bcols > 1) {
    PetscInt nbcols=0,brows=c->brows;

    m = mat->rmap->n;
    if (brows < 1) brows = m;

    ierr = PetscMalloc(nz*sizeof(MatEntry),&Jentry_new);CHKERRQ(ierr);
    ierr = PetscMalloc(bcols*sizeof(PetscInt),&row_start);CHKERRQ(ierr); 
    ierr = PetscMalloc(nis*sizeof(PetscInt),&nrows_new);CHKERRQ(ierr);

    nz_new  = 0;
    for (i=0; i<nis; i+=bcols) { /* loop over colors */
      if (i + bcols > nis) bcols = nis - i;
    
      row_end = brows;
      if (row_end > m) row_end = m;
      for (j=0; j<bcols; j++) row_start[j] = 0;
      while (row_end <= m) { /* loop over block rows */
        for (j=0; j<bcols; j++) {       /* loop over block columns */
          nrows = c->nrows[i+j];
          for (nz=color_start[i+j]; nz<color_start[i+j+1]; nz++) { /* for each Jentry */
            if (row_start[j] >= nrows) break;
            if (Jentry[nz].row >= row_end) {
              color_start[i+j] = nz;
              break;
            } else {
              Jentry_new[nz_new].row     = Jentry[nz].row + j*m; /* index in dy-array */
              Jentry_new[nz_new].col     = Jentry[nz].col; 
              Jentry_new[nz_new].valaddr = Jentry[nz].valaddr;  
              nz_new++;
              row_start[j]++;
            }
          }
        }
        if (row_end == m) break;
        row_end += brows;
        if (row_end > m) row_end = m;
      }
      nrows_new[nbcols++] = nz_new;
    }
    for (i=nbcols-1; i>0; i--) nrows_new[i] -= nrows_new[i-1];
    ierr = PetscFree(c->nrows);CHKERRQ(ierr);
    c->nrows = nrows_new;
   
    ierr = PetscFree(Jentry);CHKERRQ(ierr);
    c->matentry = Jentry_new;
    ierr = PetscMalloc(c->bcols*mat->rmap->n*sizeof(PetscScalar),&c->dy);CHKERRQ(ierr);
    ierr = PetscFree(row_start);CHKERRQ(ierr);
  }
  //---------------------------------------

  if (isBAIJ) {
    ierr = MatRestoreColumnIJ_SeqBAIJ_Color(mat,0,PETSC_FALSE,PETSC_FALSE,&ncols,&ci,&cj,&spidx,NULL);CHKERRQ(ierr);
    ierr = PetscMalloc(bs*mat->rmap->n*sizeof(PetscScalar),&c->dy);CHKERRQ(ierr);
  } else {
    ierr = MatRestoreColumnIJ_SeqAIJ_Color(mat,0,PETSC_FALSE,PETSC_FALSE,&ncols,&ci,&cj,&spidx,NULL);CHKERRQ(ierr);
  }
  ierr = PetscFree3(rowhit,valaddrhit,color_start);CHKERRQ(ierr);
  ierr = ISColoringRestoreIS(iscoloring,&isa);CHKERRQ(ierr);

  ierr = VecCreateGhost(PetscObjectComm((PetscObject)mat),mat->rmap->n,PETSC_DETERMINE,0,NULL,&c->vscale);CHKERRQ(ierr); 
  PetscFunctionReturn(0);
}
