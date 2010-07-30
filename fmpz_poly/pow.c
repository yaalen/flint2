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
#include "fmpz_vec.h"
#include "fmpz_poly.h"

void _fmpz_poly_pow_binexp(fmpz * res, const fmpz * poly, long len, long e)
{
    /* We need a couple of copies of the input data to allow aliasing in the */
    /* multiplication methods, call these A (= res), B, and C.  Two of them  */
    /* will be occupied by the current result and the current power of the   */
    /* base, giving rise to six positions pos for (A, B, C):                 */
    /*   0 - (res, base, empty)                                              */
    /*   1 - (res, empty, base)                                              */
    /*   2 - (base, res, empty)                                              */
    /*   3 - (base, empty, res)                                              */
    /*   4 - (empty, res, base)                                              */
    /*   5 - (empty, base, res)                                              */
    /* We keep track of the length of the power of the base and the current  */
    /* result in lenB and lenR.                                              */
    const long alloc = e * (len - 1) + 1;
    fmpz * A = res;
    fmpz * B = _fmpz_vec_init(2 * alloc);
    fmpz * C = B + alloc;
    _fmpz_vec_copy(B, poly, len);
    fmpz_set_ui(A, 1UL);
    long lenB = len;
    long lenR = 1;
    int pos = 0;
    
    while (1)
    {
        if (e & 1)
        {
            switch (pos)
            {
                case 0: _fmpz_poly_mul(C, B, lenB, A, lenR); pos = 5; break;
                case 1: _fmpz_poly_mul(B, C, lenB, A, lenR); pos = 4; break;
                case 2: _fmpz_poly_mul(C, A, lenB, B, lenR); pos = 3; break;
                case 3: _fmpz_poly_mul(B, A, lenB, C, lenR); pos = 2; break;
                case 4: _fmpz_poly_mul(A, C, lenB, B, lenR); pos = 1; break;
                case 5: _fmpz_poly_mul(A, B, lenB, C, lenR); pos = 0; break;
            }
            lenR += lenB - 1;
        }
        
        e >>= 1;
        if (e == 0)
            break;
        
        switch (pos)
        {
            case 0: _fmpz_poly_mul(C, B, lenB, B, lenB); pos = 1; break;
            case 1: _fmpz_poly_mul(B, C, lenB, C, lenB); pos = 0; break;
            case 2: _fmpz_poly_mul(C, A, lenB, A, lenB); pos = 4; break;
            case 3: _fmpz_poly_mul(B, A, lenB, A, lenB); pos = 5; break;
            case 4: _fmpz_poly_mul(A, C, lenB, C, lenB); pos = 2; break;
            case 5: _fmpz_poly_mul(A, B, lenB, B, lenB); pos = 3; break;
        }
        lenB += lenB - 1;
    }
    
    if (pos == 2 | pos == 4)
        _fmpz_vec_swap(A, B, lenR);
    if (pos == 3 | pos == 5)
        _fmpz_vec_swap(A, C, lenR);
    
    _fmpz_vec_clear(B, alloc);
}

void _fmpz_poly_pow_small(fmpz * res, const fmpz * poly, long len, long e)
{
    switch (e)
    {
        case 0:
            fmpz_set_ui(res, 1UL);
            break;
        case 1:
            if (res != poly)
                _fmpz_vec_copy(res, poly, len);
            break;
        case 2:
            if (res != poly)
                _fmpz_poly_mul(res, poly, len, poly, len);
            else
            {
                long alloc = 2 * len - 1;
                fmpz * copy = _fmpz_vec_init(alloc);
                _fmpz_poly_mul(copy, poly, len, poly, len);
                _fmpz_vec_swap(res, copy, alloc);
                _fmpz_vec_clear(copy, alloc);
            }
            break;
        case 3:
            if (res != poly)
            {
                long alloc = 2 * len - 1;
                fmpz * copy = _fmpz_vec_init(alloc);
                _fmpz_poly_mul(copy, poly, len, poly, len);
                _fmpz_poly_mul(res, copy, alloc, poly, len);
                _fmpz_vec_clear(copy, alloc);
            }
            else
            {
                long square = 2 * len - 1;
                long alloc  = square + len;
                fmpz * copy = _fmpz_vec_init(alloc);
                _fmpz_vec_copy(copy, poly, len);
                _fmpz_poly_mul(copy + len, poly, len, poly, len);
                _fmpz_poly_mul(res, copy + len, square, copy, len);
                _fmpz_vec_clear(copy, alloc);
            }
            break;
        case 4:
        {
            long alloc = 2 * len - 1;
            fmpz * copy = _fmpz_vec_init(alloc);
            _fmpz_poly_mul(copy, poly, len, poly, len);
            _fmpz_poly_mul(res, copy, alloc, copy, alloc);
            _fmpz_vec_clear(copy, alloc);
            break;
        }
    }
}

void _fmpz_poly_pow(fmpz * res, const fmpz * poly, long len, long e)
{
    if (e < 5)
        _fmpz_poly_pow_small(res, poly, len, e);
    else
        _fmpz_poly_pow_binexp(res, poly, len, e);
}

void fmpz_poly_pow(fmpz_poly_t res, const fmpz_poly_t poly, long e)
{
    if (poly->length == 0)
    {
        fmpz_poly_zero(res);
        return;
    }
    if (e == 0)
    {
        fmpz_poly_fit_length(res, 1);
        _fmpz_poly_set_length(res, 1);
        fmpz_set_ui(res->coeffs, 1UL);
        return;
    }
    
    const long len  = poly->length;
    const long rlen = e * (len - 1) + 1;
    
    fmpz_poly_fit_length(res, rlen);
    _fmpz_poly_set_length(res, rlen);
    
    _fmpz_poly_pow(res->coeffs, poly->coeffs, len, e);
}

