/* HPhi  -  Quantum Lattice Model Simulator */
/* Copyright (C) 2015 Takahiro Misawa, Kazuyoshi Yoshimi, Mitsuaki Kawamura, Youhei Yamaji, Synge Todo, Naoki Kawashima */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

//Define Mode for mltply
// complex version

#ifdef MPI
#include "mpi.h"
#endif
#include "Common.h"
#include "mltply.h"
#include "bitcalc.h"
#include "wrapperMPI.h"

void GC_child_general_hopp_MPIdouble(int itrans, struct BindStruct *X, 
  double complex *tmp_v0, double complex *tmp_v1)
{
#ifdef MPI
  int bit1, bit2, mybit1, mybit2, ierr, dest, bitdiff, sgn;
  unsigned long int idim_max_buf, j;
  MPI_Status status;
  double complex trans, dmv, dam_pr;

  bit1 = (int)X->Def.Tpow[2 * X->Def.EDGeneralTransfer[itrans][0] - 2 
                            + X->Def.EDGeneralTransfer[itrans][1]];
  bit2 = (int)X->Def.Tpow[2 * X->Def.EDGeneralTransfer[itrans][2] - 2
                            + X->Def.EDGeneralTransfer[itrans][3]];
  bitdiff = abs(bit1 - bit2);

  mybit1 = myrank & bit1;
  mybit2 = myrank & bit2;

  dest = myrank ^ (bit1 + bit2);
  SgnBit((unsigned long int)(dest & bitdiff), &sgn); // Fermion sign

  if(mybit1 == 0 && mybit2 == bit2){
    trans = (double)sgn * X->Def.EDParaGeneralTransfer[itrans];
  }
  else if(mybit1 == bit1 && mybit2 == 0) {
    trans = (double)sgn * conj(X->Def.EDParaGeneralTransfer[itrans]);
  }
  else return;

  ierr = MPI_Sendrecv(&X->Check.idim_max, 1, MPI_UNSIGNED_LONG, dest, 0,
    &idim_max_buf, 1, MPI_UNSIGNED_LONG, dest, 0, MPI_COMM_WORLD, &status);
  ierr = MPI_Sendrecv(tmp_v1, X->Check.idim_max, MPI_DOUBLE_COMPLEX, dest, 0,
    v1buf, idim_max_buf, MPI_DOUBLE_COMPLEX, dest, 0, MPI_COMM_WORLD, &status);

  dam_pr = 0.0;
  for (j = 0; j < idim_max_buf; j++) {
    dmv = trans * v1buf[j];
    if (X->Large.mode == M_MLTPLY) tmp_v0[j] += dmv;
    dam_pr += tmp_v1[j] * dmv;
  }

  X->Large.prdct += dam_pr;
  
#endif
}

void GC_child_general_hopp_MPIsingle(int itrans, struct BindStruct *X,
  double complex *tmp_v0, double complex *tmp_v1)
{
#ifdef MPI
  int bit2, mybit1, mybit2, ierr, dest, bit2diff, sgn;
  unsigned long int idim_max_buf, j, bit1, bit1check, bit1diff, ioff;
  MPI_Status status;
  double complex trans, dmv, dam_pr;
  /*
   Prepare index in the inter PE
  */
  bit2 = (int)X->Def.Tpow[2 * X->Def.EDGeneralTransfer[itrans][2] - 2
    + X->Def.EDGeneralTransfer[itrans][3]];
  bit2diff = bit2 - 1;

  mybit2 = myrank & bit2;

  dest = myrank ^ bit2;
  SgnBit((unsigned long int)(dest & bit2diff), &sgn); // Fermion sign

#ifdef MPI
  ierr = MPI_Sendrecv(&X->Check.idim_max, 1, MPI_UNSIGNED_LONG, dest, 0,
    &idim_max_buf, 1, MPI_UNSIGNED_LONG, dest, 0, MPI_COMM_WORLD, &status);
  ierr = MPI_Sendrecv(tmp_v1, X->Check.idim_max, MPI_DOUBLE_COMPLEX, dest, 0,
    v1buf, idim_max_buf, MPI_DOUBLE_COMPLEX, dest, 0, MPI_COMM_WORLD, &status);
#endif

  /*
   Index in the intra PE
  */
  bit1 = X->Def.Tpow[2 * X->Def.EDGeneralTransfer[itrans][0] - 2
    + X->Def.EDGeneralTransfer[itrans][1]];
  
  if (mybit2 == bit2) {
    trans = (double)sgn * X->Def.EDParaGeneralTransfer[itrans];
    bit1check = 0;
  }
  else if (mybit2 == 0) {
    trans = (double)sgn * conj(X->Def.EDParaGeneralTransfer[itrans]);
    bit1check = bit1;
  }
  else return;

  bit1diff = X->Def.Tpow[2 * X->Def.Nsite - 1] * 2 - bit1;

  dam_pr = 0.0;
  for (j = 0; j < idim_max_buf; j++) {

    mybit1 = j & bit1;

    if (mybit1 == bit1check) {

      SgnBit(j & bit1diff, &sgn);
      ioff = j ^ bit1;

      dmv = (double)sgn * trans * v1buf[j];
      if (X->Large.mode == M_MLTPLY) tmp_v0[ioff] += dmv;
      dam_pr += tmp_v1[j] * dmv;
    }
  }

  X->Large.prdct += dam_pr;

#endif
}

