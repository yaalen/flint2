/*============================================================================

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

===============================================================================*/
/****************************************************************************

   Copyright (C) 2008, 2009 William Hart
   
*****************************************************************************/

#include <mpir.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_poly.h"

void _fmpz_poly_add(fmpz * res, const fmpz * poly1, long len1, const fmpz * poly2, long len2)
{
	long longer = FLINT_MAX(len1, len2);
	long shorter = FLINT_MIN(len1, len2);

   long i;
   for (i = 0; i < shorter; i++) // add up to the length of the shorter poly
      fmpz_add(res + i, poly1 + i, poly2 + i);   
   
   if (poly1 != res) // copy any remaining coefficients from poly1
      for (i = shorter; i < len1; i++)
         fmpz_set(res + i, poly1 + i);

   if (poly2 != res) // copy any remaining coefficients from poly2
      for (i = shorter; i < len2; i++)
         fmpz_set(res + i, poly2 + i);
}

void fmpz_poly_add(fmpz_poly_t res, const fmpz_poly_t poly1, const fmpz_poly_t poly2)
{
   long longer = FLINT_MAX(poly1->length, poly2->length);

   fmpz_poly_fit_length(res, longer);
	
   _fmpz_poly_add(res->coeffs, poly1->coeffs, poly1->length, poly2->coeffs, poly2->length);
    
   _fmpz_poly_set_length(res, longer);
   _fmpz_poly_normalise(res); // there may have been cancellation
}
