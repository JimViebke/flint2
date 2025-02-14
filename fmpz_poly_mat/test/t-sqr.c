/*
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_mat.h"
#include "fmpz_poly.h"
#include "fmpz_poly_mat.h"

int
main(void)
{
    slong i;

    FLINT_TEST_INIT(state);

    flint_printf("sqr....");
    fflush(stdout);

    /* Check evaluation homomorphism */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpz_poly_mat_t A, C;
        fmpz_mat_t a, c, d;
        fmpz_t x;
        slong m, bits, deg;

        m = n_randint(state, 20);
        deg = 1 + n_randint(state, 10);
        bits = 1 + n_randint(state, 100);

        fmpz_poly_mat_init(A, m, m);
        fmpz_poly_mat_init(C, m, m);

        fmpz_mat_init(a, m, m);
        fmpz_mat_init(c, m, m);
        fmpz_mat_init(d, m, m);

        fmpz_init(x);

        fmpz_poly_mat_randtest(A, state, deg, bits);
        fmpz_poly_mat_randtest(C, state, deg, bits);

        fmpz_poly_mat_sqr(C, A);

        fmpz_randtest(x, state, 1 + n_randint(state, 100));

        fmpz_poly_mat_evaluate_fmpz(a, A, x);
        fmpz_poly_mat_evaluate_fmpz(d, C, x);
        fmpz_mat_mul(c, a, a);

        if (!fmpz_mat_equal(c, d))
        {
            flint_printf("FAIL:\n");
            flint_printf("A:\n");
            fmpz_poly_mat_print(A, "x");
            flint_printf("C:\n");
            fmpz_poly_mat_print(C, "x");
            flint_printf("\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_poly_mat_clear(A);
        fmpz_poly_mat_clear(C);

        fmpz_mat_clear(a);
        fmpz_mat_clear(c);
        fmpz_mat_clear(d);

        fmpz_clear(x);
    }

    /* Check aliasing B and A */
    for (i = 0; i < 1000; i++)
    {
        fmpz_poly_mat_t A, B;
        slong m, bits, deg;

        m = n_randint(state, 20);
        deg = 1 + n_randint(state, 10);
        bits = 1 + n_randint(state, 100);

        fmpz_poly_mat_init(A, m, m);
        fmpz_poly_mat_init(B, m, m);

        fmpz_poly_mat_randtest(A, state, deg, bits);
        fmpz_poly_mat_randtest(B, state, deg, bits);

        fmpz_poly_mat_sqr(B, A);
        fmpz_poly_mat_sqr(A, A);

        if (!fmpz_poly_mat_equal(B, A))
        {
            flint_printf("FAIL (aliasing):\n");
            flint_printf("A:\n");
            fmpz_poly_mat_print(A, "x");
            flint_printf("B:\n");
            fmpz_poly_mat_print(B, "x");
            flint_printf("\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_poly_mat_clear(A);
        fmpz_poly_mat_clear(B);
    }

    FLINT_TEST_CLEANUP(state);
    
    flint_printf("PASS\n");
    return 0;
}
