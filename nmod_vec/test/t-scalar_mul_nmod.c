/*
    Copyright (C) 2009 William Hart

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
#include "nmod_vec.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, result;
    FLINT_TEST_INIT(state);
    

    flint_printf("scalar_mul_nmod....");
    fflush(stdout);

    {
        nmod_t mod;
        nmod_init(&mod, 2);
        _nmod_vec_scalar_mul_nmod(NULL, NULL, 0, 1, mod);
    }

    /* Check (a + b)*c == a*c + b*c */
    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        slong len = n_randint(state, 100) + 1;
        mp_limb_t n = n_randtest_not_zero(state);
        mp_limb_t c = n_randint(state, n);
        nmod_t mod;

        mp_ptr vec = _nmod_vec_init(len);
        mp_ptr vec2 = _nmod_vec_init(len);
        mp_ptr vec3 = _nmod_vec_init(len);

        nmod_init(&mod, n);

        _nmod_vec_randtest(vec, state, len, mod);
        _nmod_vec_randtest(vec2, state, len, mod);

        _nmod_vec_add(vec3, vec, vec2, len, mod);
        _nmod_vec_scalar_mul_nmod(vec3, vec3, len, c, mod);

        _nmod_vec_scalar_mul_nmod(vec, vec, len, c, mod);
        _nmod_vec_scalar_mul_nmod(vec2, vec2, len, c, mod);
        _nmod_vec_add(vec, vec, vec2, len, mod);

        result = _nmod_vec_equal(vec, vec3, len);
        if (!result)
        {
            flint_printf("FAIL:\n");
            flint_printf("len = %wd, n = %wd\n", len, n);
            fflush(stdout);
            flint_abort();
        }

        _nmod_vec_clear(vec);
        _nmod_vec_clear(vec2);
        _nmod_vec_clear(vec3);
    }

    FLINT_TEST_CLEANUP(state);
    
    flint_printf("PASS\n");
    return 0;
}
