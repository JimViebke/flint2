/*
    Copyright (C) 2015 Anubhav Srivastava
    Copyright (C) 2015 Elena Sergeicheva

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz_poly.h"
#include "fmpz_poly_mat.h"
#include "ulong_extras.h"

int main(void)
{
    fmpz_poly_mat_t A, B, C;
    fmpz_poly_mat_t window1, window2;
    slong i;
    FLINT_TEST_INIT(state);

    flint_printf("concat_vertical....");
    fflush(stdout);

    

    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        slong r1, r2, c1, bits;

        r1 = n_randint(state, 10);
        r2 = n_randint(state, 10);
        c1 = n_randint(state, 10);
        bits = 1 + n_randint(state, 20);

        fmpz_poly_mat_init(A, r1, c1);
        fmpz_poly_mat_init(B, r2, c1);
        fmpz_poly_mat_init(C, (r1 + r2), c1);

        fmpz_poly_mat_randtest(A, state, n_randint(state, 10) + 1, bits);
        fmpz_poly_mat_randtest(B, state, n_randint(state, 10) + 1, bits);

        fmpz_poly_mat_randtest(C, state, n_randint(state, 10) + 1, bits);

        fmpz_poly_mat_concat_vertical(C, A, B);
        
        fmpz_poly_mat_window_init(window1, C, 0, 0, r1, c1);
        fmpz_poly_mat_window_init(window2, C, r1, 0, (r1 + r2), c1);


        if (!(fmpz_poly_mat_equal(window1, A) && fmpz_poly_mat_equal(window2, B)))
        {
            flint_printf("FAIL: results not equal\n");
            fflush(stdout);
            flint_abort();
        }
        
        fmpz_poly_mat_clear(A);
        fmpz_poly_mat_clear(B);
        fmpz_poly_mat_clear(C);

        fmpz_poly_mat_window_clear(window1);
        fmpz_poly_mat_window_clear(window2);
    }

    FLINT_TEST_CLEANUP(state);
    
    flint_printf("PASS\n");
    return 0;
}
