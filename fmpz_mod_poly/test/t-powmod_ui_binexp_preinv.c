/*
    Copyright (C) 2009 William Hart
    Copyright (C) 2011 Fredrik Johansson
    Copyright (C) 2012 Lina Kulakova
    Copyright (C) 2013 Martin Lee

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#undef ulong
#define ulong ulongxx/* interferes with system includes */

#include <stdlib.h>
#include <stdio.h>

#undef ulong

#include <gmp.h>

#define ulong mp_limb_t

#include "flint.h"
#include "fmpz_vec.h"
#include "fmpz_mod_poly.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, result;
    fmpz_mod_ctx_t ctx;
    FLINT_TEST_INIT(state);

    flint_printf("powmod_ui_binexp_preinv....");
    fflush(stdout);

    fmpz_mod_ctx_init_ui(ctx, 2);

    /* Aliasing of res and a */
    for (i = 0; i < 50 * flint_test_multiplier(); i++)
    {
        fmpz_mod_poly_t a, res, f, finv;
        fmpz_t p;
        ulong exp;

        fmpz_init(p);
        fmpz_set_ui(p, n_randtest_prime(state, 0));
        fmpz_mod_ctx_set_modulus(ctx, p);
        exp = n_randint(state, 50);

        fmpz_mod_poly_init(a, ctx);
        fmpz_mod_poly_init(f, ctx);
        fmpz_mod_poly_init(finv, ctx);
        fmpz_mod_poly_init(res, ctx);

        fmpz_mod_poly_randtest(a, state, n_randint(state, 50), ctx);
        fmpz_mod_poly_randtest_not_zero(f, state, n_randint(state, 50) + 1, ctx);

        fmpz_mod_poly_reverse (finv, f, f->length, ctx);
        fmpz_mod_poly_inv_series_newton (finv, finv, f->length, ctx);

        fmpz_mod_poly_powmod_ui_binexp_preinv(res, a, exp, f, finv, ctx);
        fmpz_mod_poly_powmod_ui_binexp_preinv(a, a, exp, f, finv, ctx);

        result = (fmpz_mod_poly_equal(res, a, ctx));
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("a:\n"); fmpz_mod_poly_print(a, ctx), flint_printf("\n\n");
            flint_printf("f:\n"); fmpz_mod_poly_print(f, ctx), flint_printf("\n\n");
            flint_printf("res:\n"); fmpz_mod_poly_print(res, ctx), flint_printf("\n\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_clear(p);
        fmpz_mod_poly_clear(a, ctx);
        fmpz_mod_poly_clear(f, ctx);
        fmpz_mod_poly_clear(finv, ctx);
        fmpz_mod_poly_clear(res, ctx);
    }

    /* Aliasing of res and f */
    for (i = 0; i < 50 * flint_test_multiplier(); i++)
    {
        fmpz_mod_poly_t a, res, f, finv;
        fmpz_t p;
        ulong exp;

        fmpz_init(p);
        fmpz_set_ui(p, n_randtest_prime(state, 0));
        fmpz_mod_ctx_set_modulus(ctx, p);
        exp = n_randint(state, 50);

        fmpz_mod_poly_init(a, ctx);
        fmpz_mod_poly_init(f, ctx);
        fmpz_mod_poly_init(finv, ctx);
        fmpz_mod_poly_init(res, ctx);

        fmpz_mod_poly_randtest(a, state, n_randint(state, 50), ctx);
        fmpz_mod_poly_randtest_not_zero(f, state, n_randint(state, 50) + 1, ctx);

        fmpz_mod_poly_reverse (finv, f, f->length, ctx);
        fmpz_mod_poly_inv_series_newton (finv, finv, f->length, ctx);

        fmpz_mod_poly_powmod_ui_binexp_preinv(res, a, exp, f, finv, ctx);
        fmpz_mod_poly_powmod_ui_binexp_preinv(f, a, exp, f, finv, ctx);

        result = (fmpz_mod_poly_equal(res, f, ctx));
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("a:\n"); fmpz_mod_poly_print(a, ctx), flint_printf("\n\n");
            flint_printf("f:\n"); fmpz_mod_poly_print(f, ctx), flint_printf("\n\n");
            flint_printf("res:\n"); fmpz_mod_poly_print(res, ctx), flint_printf("\n\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_clear(p);
        fmpz_mod_poly_clear(a, ctx);
        fmpz_mod_poly_clear(f, ctx);
        fmpz_mod_poly_clear(finv, ctx);
        fmpz_mod_poly_clear(res, ctx);
    }

    /* Aliasing of res and finv */
    for (i = 0; i < 50 * flint_test_multiplier(); i++)
    {
        fmpz_mod_poly_t a, res, f, finv;
        fmpz_t p;
        ulong exp;

        fmpz_init(p);
        fmpz_set_ui(p, n_randtest_prime(state, 0));
        fmpz_mod_ctx_set_modulus(ctx, p);
        exp = n_randint(state, 50);

        fmpz_mod_poly_init(a, ctx);
        fmpz_mod_poly_init(f, ctx);
        fmpz_mod_poly_init(finv, ctx);
        fmpz_mod_poly_init(res, ctx);

        fmpz_mod_poly_randtest(a, state, n_randint(state, 50), ctx);
        fmpz_mod_poly_randtest_not_zero(f, state, n_randint(state, 50) + 1, ctx);


        fmpz_mod_poly_reverse (finv, f, f->length, ctx);
        fmpz_mod_poly_inv_series_newton (finv, finv, f->length, ctx);

        fmpz_mod_poly_powmod_ui_binexp_preinv(res, a, exp, f, finv, ctx);
        fmpz_mod_poly_powmod_ui_binexp_preinv(finv, a, exp, f, finv, ctx);

        result = (fmpz_mod_poly_equal(res, finv, ctx));
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("a:\n"); fmpz_mod_poly_print(a, ctx), flint_printf("\n\n");
            flint_printf("f:\n"); fmpz_mod_poly_print(f, ctx), flint_printf("\n\n");
            flint_printf("finv:\n"); fmpz_mod_poly_print(finv, ctx), flint_printf("\n\n");
            flint_printf("res:\n"); fmpz_mod_poly_print(res, ctx), flint_printf("\n\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_clear(p);
        fmpz_mod_poly_clear(a, ctx);
        fmpz_mod_poly_clear(f, ctx);
        fmpz_mod_poly_clear(finv, ctx);
        fmpz_mod_poly_clear(res, ctx);
    }

    /* No aliasing */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpz_mod_poly_t a, res1, res2, f, finv;
        fmpz_t p;
        ulong exp;
        int j;

        fmpz_init(p);
        fmpz_set_ui(p, n_randtest_prime(state, 0));
        fmpz_mod_ctx_set_modulus(ctx, p);
        exp = n_randint(state, 50);

        fmpz_mod_poly_init(a, ctx);
        fmpz_mod_poly_init(f, ctx);
        fmpz_mod_poly_init(finv, ctx);
        fmpz_mod_poly_init(res1, ctx);
        fmpz_mod_poly_init(res2, ctx);

        fmpz_mod_poly_randtest(a, state, n_randint(state, 50), ctx);
        fmpz_mod_poly_randtest_not_zero(f, state, n_randint(state, 50) + 1, ctx);

        fmpz_mod_poly_reverse (finv, f, f->length, ctx);
        fmpz_mod_poly_inv_series_newton (finv, finv, f->length, ctx);

        fmpz_mod_poly_powmod_ui_binexp_preinv(res1, a, exp, f, finv, ctx);

        fmpz_mod_poly_zero(res2, ctx);
        if (fmpz_mod_poly_length(f, ctx) > 1)
            fmpz_mod_poly_set_coeff_ui(res2, 0, 1, ctx);
        for (j = 1; j <= exp; j++)
            fmpz_mod_poly_mulmod(res2, res2, a, f, ctx);

        result = (fmpz_mod_poly_equal(res1, res2, ctx));
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("a:\n"); fmpz_mod_poly_print(a, ctx), flint_printf("\n\n");
            flint_printf("f:\n"); fmpz_mod_poly_print(f, ctx), flint_printf("\n\n");
            flint_printf("finv:\n"); fmpz_mod_poly_print(finv, ctx), flint_printf("\n\n");
            flint_printf("res1:\n"); fmpz_mod_poly_print(res1, ctx), flint_printf("\n\n");
            flint_printf("res2:\n"); fmpz_mod_poly_print(res2, ctx), flint_printf("\n\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_clear(p);
        fmpz_mod_poly_clear(a, ctx);
        fmpz_mod_poly_clear(f, ctx);
        fmpz_mod_poly_clear(res1, ctx);
        fmpz_mod_poly_clear(res2, ctx);
        fmpz_mod_poly_clear(finv, ctx);
    }

    /* Check that a^(b+c) = a^b * a^c */
    for (i = 0; i < 50 * flint_test_multiplier(); i++)
    {
        fmpz_mod_poly_t a, res1, res2, res3, res4, f, finv;
        fmpz_t p;
        ulong exp1, exp2, exp3;

        fmpz_init(p);
        fmpz_set_ui(p, n_randtest_prime(state, 0));
        fmpz_mod_ctx_set_modulus(ctx, p);

        exp1 = n_randint(state, 50);
        exp2 = n_randint(state, 50);

        fmpz_mod_poly_init(a, ctx);
        fmpz_mod_poly_init(f, ctx);
        fmpz_mod_poly_init(finv, ctx);
        fmpz_mod_poly_init(res1, ctx);
        fmpz_mod_poly_init(res2, ctx);
        fmpz_mod_poly_init(res3, ctx);
        fmpz_mod_poly_init(res4, ctx);

        fmpz_mod_poly_randtest(a, state, n_randint(state, 50), ctx);
        fmpz_mod_poly_randtest_not_zero(f, state, n_randint(state, 50) + 1, ctx);

        fmpz_mod_poly_reverse (finv, f, f->length, ctx);
        fmpz_mod_poly_inv_series_newton (finv, finv, f->length, ctx);

        fmpz_mod_poly_powmod_ui_binexp_preinv(res1, a, exp1, f, finv, ctx);
        fmpz_mod_poly_powmod_ui_binexp_preinv(res2, a, exp2, f, finv, ctx);
        fmpz_mod_poly_mulmod(res4, res1, res2, f, ctx);

        exp3= exp1+exp2;
        fmpz_mod_poly_powmod_ui_binexp_preinv(res3, a, exp3, f, finv, ctx);

        result = (fmpz_mod_poly_equal(res4, res3, ctx));
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("a:\n"); fmpz_mod_poly_print(a, ctx), flint_printf("\n\n");
            flint_printf("f:\n"); fmpz_mod_poly_print(f, ctx), flint_printf("\n\n");
            flint_printf("res3:\n"); fmpz_mod_poly_print(res3, ctx), flint_printf("\n\n");
            flint_printf("res4:\n"); fmpz_mod_poly_print(res4, ctx), flint_printf("\n\n");
            fflush(stdout);
            flint_abort();
        }

        fmpz_clear(p);
        fmpz_mod_poly_clear(a, ctx);
        fmpz_mod_poly_clear(f, ctx);
        fmpz_mod_poly_clear(finv, ctx);
        fmpz_mod_poly_clear(res1, ctx);
        fmpz_mod_poly_clear(res2, ctx);
        fmpz_mod_poly_clear(res3, ctx);
        fmpz_mod_poly_clear(res4, ctx);
    }

    fmpz_mod_ctx_clear(ctx);
    FLINT_TEST_CLEANUP(state);
    
    flint_printf("PASS\n");
    return 0;
}
