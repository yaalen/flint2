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

   Copyright (C) 2010 Sebastian Pancratz

******************************************************************************/

#include <mpir.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_poly.h"

void 
_fmpz_poly_evaluate_horner_mpq(mpq_t res, 
                               const fmpz * f, long len, const mpq_t a)
{
    if (len == 0L)
        mpq_set_si(res, 0, 1);
    else if (len == 1L)
    {
        fmpz_get_mpz(mpq_numref(res), f);
        mpz_set_ui(mpq_denref(res), 1);
    }
    else
    {
        mpq_t s, t;
        fmpz * c = (fmpz *) f + (len - 1L);
        mpq_init(s);
        mpq_init(t);
        fmpz_get_mpz(mpq_numref(res), c);
        mpz_set_ui(mpq_denref(res), 1);
        do {
            fmpz_get_mpz(mpq_numref(s), --c);
            mpq_mul(t, res, a);
            mpq_add(res, s, t);
        } while (c != f);
        mpq_clear(s);
        mpq_clear(t);
    }
}

void 
fmpz_poly_evaluate_horner_mpq(mpq_t res, const fmpz_poly_t f, const mpq_t a)
{
    if (res == a)
    {
        mpq_t t;
        mpq_init(t);
        _fmpz_poly_evaluate_horner_mpq(t, f->coeffs, f->length, a);
        mpq_swap(res, t);
        mpq_clear(t);
    }
    else
        _fmpz_poly_evaluate_horner_mpq(res, f->coeffs, f->length, a);
}

