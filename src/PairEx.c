/* HPhi  -  Quantum Lattice Model Simulator */
/* Copyright (C) 2015 The University of Tokyo */

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
/*-------------------------------------------------------------*/
#include "bitcalc.h"
#include "mltplyCommon.h"
#include "PairEx.h"
#include "PairExHubbard.h"
#include "PairExSpin.h"

/**
 * @file   PairEx.c
 *
 * @brief  Calculating the pair excited state generated by the pair operator\n
 * @f[ \sum_{i,j, \sigma_1, \sigma_2} h_{i,\sigma_1, j, \sigma_2} c_{i\sigma_1} c_{j\sigma_2} (a_{i\sigma_1} a_{j\sigma_2})@f]
 * , where @f$ c_{i\sigma_1} (a_{i\sigma_1})@f$ indicates a creation (anti-creation) operator at @f$i@f$-th site with @f$ \sigma_1 @f$ spin.
 *
 * @version 1.2
 * @author Kazuyoshi Yoshimi (The University of Tokyo)
 */

///
/// \brief Calculating the pair excited state by the pair operator; \n
/// @f[ \sum_{i,j, \sigma_1, \sigma_2} h_{i,\sigma_1, j, \sigma_2} c_{i\sigma_1} c_{j\sigma_2} (a_{i\sigma_1} a_{j\sigma_2})@f]
/// , where @f$ c_{i\sigma_1} (a_{i\sigma_1})@f$ indicates a creation (anti-creation) operator at @f$i@f$-th site with @f$ \sigma_1 @f$ spin.
/// Target System: Hubbard, Kondo, Spin.
/// \param X [in] define list to get and put information of calculation
/// \param tmp_v0 [in, out] Result @f$ v_0 += H v_1 @f$.
/// \param tmp_v1 [in] The wave vector @f$ v_1 @f$ to be mltiplied by the Hamiltonian @f$ H v_1 @f$.
/// \returns TRUE: Normally finished
/// \returns FALSE: Unnormally finished
/// \author Kazuyoshi Yoshimi (The University of Tokyo)
/// \version 1.2
int GetPairExcitedState
(
 struct BindStruct *X,
 int nstate, double complex **tmp_v0,
 double complex **tmp_v1
 )
{
    int iret;
    long unsigned int irght,ilft,ihfbit;

  //  i_max = X->Check.idim_max;
    if(X->Def.iFlgGeneralSpin == FALSE) {
        if (GetSplitBitByModel(X->Def.Nsite, X->Def.iCalcModel, &irght, &ilft, &ihfbit) != 0) {
            return -1;
        }
    }
    else {
        if (GetSplitBitForGeneralSpin(X->Def.Nsite, &ihfbit, X->Def.SiteToBit) != 0) {
            return -1;
        }
    }

  X->Large.i_max    =  X->Check.idim_maxOrg;
  X->Large.irght    = irght;
  X->Large.ilft     = ilft;
  X->Large.ihfbit   = ihfbit;
  X->Large.mode=M_CALCSPEC;

    switch(X->Def.iCalcModel){
  case HubbardGC:
      iret=GetPairExcitedStateHubbardGC(X, nstate, tmp_v0, tmp_v1);
    break;

  case KondoGC:
  case Hubbard:
  case Kondo:
      iret=GetPairExcitedStateHubbard(X, nstate, tmp_v0, tmp_v1);
    break;

    case Spin: // for the Sz-conserved spin system
      iret =GetPairExcitedStateSpin(X, nstate, tmp_v0, tmp_v1);
      break;

    case SpinGC:
      iret=GetPairExcitedStateSpinGC(X,nstate,tmp_v0, tmp_v1);
      break;

    default:
        iret =FALSE;
      break;
    }

    return iret;
}
