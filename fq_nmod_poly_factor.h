/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2013 Mike Hansen

******************************************************************************/

#ifndef FQ_NMOD_POLY_FACTOR_H
#define FQ_NMOD_POLY_FACTOR_H

static __inline__ int FQ_NMOD_POLY_ITERATED_FROBENIUS_CUTOFF(const fq_nmod_ctx_t ctx, slong length)
{
    int result;
    fmpz_t q;
    fmpz_init(q);
    fq_nmod_ctx_order(q, ctx);
    if ( 2 * fmpz_sizeinbase(q, 2) < 3 * (n_sqrt(length) + 1))
        result = 1;
    else
        result = 0;
    fmpz_clear(q);
    return result;
}

#ifdef T
#undef T
#endif

#define T fq_nmod
#define CAP_T FQ_NMOD
#include "fq_poly_factor_templates.h"
#undef CAP_T
#undef T

#endif
