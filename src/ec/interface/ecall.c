#ifndef lint
static char vcid[] = "$Id: ecall.c,v 1.2 1997/01/22 18:46:10 bsmith Exp bsmith $";
#endif

#include "petsc.h"
#include "src/ec/ecimpl.h"          /*I   "ec.h"   I*/

extern int ECCreate_Lapack(EC);

#undef __FUNC__  
#define __FUNC__ "ECRegisterAll" /* ADIC Ignore */
/*@C
  ECRegisterAll - Registers all of the preconditioners in the EC package.

  Adding new methods:
  To add a new method to the registry
$   1.  Copy this routine and modify it to incorporate
$       a call to ECRegister() for the new method.  
$   2.  Modify the file "PETSCDIR/include/ec.h"
$       by appending the method's identifier as an
$       enumerator of the ECType enumeration.
$       As long as the enumerator is appended to
$       the existing list, only the ECRegisterAll()
$       routine requires recompilation.

  Restricting the choices:
  To prevent all of the methods from being registered and thus 
  save memory, copy this routine and modify it to register only 
  those methods you desire.  Make sure that the replacement routine 
  is linked before libpetscsles.a.

.keywords: EC, register, all

.seealso: ECRegister(), ECRegisterDestroy()
@*/
int ECRegisterAll()
{
  ECRegister(EC_LAPACK         , "lapack",       ECCreate_Lapack);
  return 0;
}




