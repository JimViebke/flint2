/* 
    Copyright (C) 2022 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fft_small.h"
#include "machine_vectors.h"

/************************** inverse butterfly *********************************
    2*a0 =      (b0 + b1)
    2*a1 = w^-1*(b0 - b1)
    W := -w^-1
*/
#define RADIX_2_REVERSE_PARAM(V, Q, j) \
    V W = V##_set_d((UNLIKELY((j) == 0)) ? -Q->w2s[0] : Q->w2s[(j)^(n_next_pow2m1(j)>>1)]); \
    V n    = V##_set_d(Q->p); \
    V ninv = V##_set_d(Q->pinv);

#define RADIX_2_REVERSE_MOTH(V, X0, X1) \
{ \
    V x0, x1, y0, y1; \
    x0 = V##_load(X0); \
    x1 = V##_load(X1); \
    y0 = V##_add(x0, x1); \
    y1 = V##_sub(x1, x0); \
    y0 = V##_reduce_to_pm1n(y0, n, ninv); \
    y1 = V##_mulmod2(y1, W, n, ninv); \
    V##_store(X0, y0); \
    V##_store(X1, y1); \
}

/****************** inverse butterfly with truncation ************************/

#define DEFINE_IT(nn, zz, ff) \
static void CAT4(radix_2_moth_inv_trunc_block, nn, zz, ff)( \
    const sd_fft_ctx_t Q, \
    double* X0, double* X1, \
    ulong j) \
{ \
    int l = 2; \
    flint_printf("function l = %d, n = %d, z = %d, f = %d", l, nn, zz, ff); \
    if (1 <= zz && zz <= 2 && nn <= zz && 1 <= nn+ff && nn+ff <= l) \
    { \
        flint_printf(" is not implemented\n"); \
    } \
    else \
    { \
        flint_printf(" does not exist and should not be called\n"); \
    } \
    fflush(stdout); \
    flint_abort(); \
}

DEFINE_IT(0,1,0)
DEFINE_IT(0,2,0)
DEFINE_IT(2,1,0)
DEFINE_IT(2,1,1)
DEFINE_IT(2,2,0)
DEFINE_IT(2,2,1)
#undef DEFINE_IT


/* {x0, x1} = {2*x0 - w*x1, x0 - w*x1} */
static void radix_2_moth_inv_trunc_block_1_2_1(
    const sd_fft_ctx_t Q,
    double* X0, double* X1,
    ulong j)
{
    vec4d n    = vec4d_set_d(Q->p);
    vec4d ninv = vec4d_set_d(Q->pinv);
    vec4d w = vec4d_set_d(Q->w2s[j]);
    vec4d c = vec4d_set_d(2.0);
    ulong i = 0; do {
        vec4d u0, u1;
        u0 = vec4d_load(X0 + i);
        u1 = vec4d_load(X1 + i);
        u0 = vec4d_reduce_to_pm1n(u0, n, ninv);
        u1 = vec4d_mulmod2(u1, w, n, ninv);
        vec4d_store(X0 + i, vec4d_fmsub(c, u0, u1));
        vec4d_store(X1 + i, vec4d_sub(u0, u1));
    } while (i += 4, i < BLK_SZ);
    FLINT_ASSERT(i == BLK_SZ);
}

/* {x0} = {2*x0 - w*x1} */
static void radix_2_moth_inv_trunc_block_1_2_0(
    const sd_fft_ctx_t Q,
    double* X0, double* X1,
    ulong j)
{
    vec4d n    = vec4d_set_d(Q->p);
    vec4d ninv = vec4d_set_d(Q->pinv);
    vec4d w = vec4d_set_d(Q->w2s[j]);
    vec4d c = vec4d_set_d(2.0);
    ulong i = 0; do {
        vec4d u0, u1;
        u0 = vec4d_load(X0 + i);
        u1 = vec4d_load(X1 + i);
        u0 = vec4d_reduce_to_pm1n(u0, n, ninv);
        u1 = vec4d_mulmod2(u1, w, n, ninv);
        vec4d_store(X0 + i, vec4d_fmsub(c, u0, u1));
    } while (i += 4, i < BLK_SZ);
    FLINT_ASSERT(i == BLK_SZ);
}

/* {x0, x1} = {2*x0, x0} */
static void radix_2_moth_inv_trunc_block_1_1_1(
    const sd_fft_ctx_t Q,
    double* X0, double* X1,
    ulong j)
{
    vec4d n    = vec4d_set_d(Q->p);
    vec4d ninv = vec4d_set_d(Q->pinv);
    ulong i = 0; do {
        vec4d u0;
        u0 = vec4d_load(X0 + i);
        u0 = vec4d_reduce_to_pm1n(u0, n, ninv);
        vec4d_store(X0 + i, vec4d_add(u0, u0));
        vec4d_store(X1 + i, u0);
    } while (i += 4, i < BLK_SZ);
    FLINT_ASSERT(i == BLK_SZ);
}

/* {x0} = {2*x0} */
static void radix_2_moth_inv_trunc_block_1_1_0(
    const sd_fft_ctx_t Q,
    double* X0, double* X1,
    ulong j)
{
    vec4d n    = vec4d_set_d(Q->p);
    vec4d ninv = vec4d_set_d(Q->pinv);
    ulong i = 0; do {
        vec4d u0;
        u0 = vec4d_load(X0 + i);
        u0 = vec4d_reduce_to_pm1n(u0, n, ninv);
        vec4d_store(X0 + i, vec4d_add(u0, u0));
    } while (i += 4, i < BLK_SZ);
    FLINT_ASSERT(i == BLK_SZ);
}

/* {x0} = {(x0 + w*x1)/2} */
static void radix_2_moth_inv_trunc_block_0_2_1(
    const sd_fft_ctx_t Q,
    double* X0, double* X1,
    ulong j)
{
    vec4d n    = vec4d_set_d(Q->p);
    vec4d ninv = vec4d_set_d(Q->pinv);
    vec4d w = vec4d_set_d(Q->w2s[j]);
    vec4d c = vec4d_set_d(vec1d_fnmadd(0.5, Q->p, 0.5));
    ulong i = 0; do {
        vec4d u0, u1;
        u0 = vec4d_load(X0 + i);
        u1 = vec4d_load(X1 + i);
        u1 = vec4d_mulmod2(u1, w, n, ninv);
        u0 = vec4d_mulmod2(vec4d_add(u0, u1), c, n, ninv);
        vec4d_store(X0 + i, u0);
    } while (i += 4, i < BLK_SZ);
    FLINT_ASSERT(i == BLK_SZ);
}

/* {x0} = {(x0)/2} */
static void radix_2_moth_inv_trunc_block_0_1_1(
    const sd_fft_ctx_t Q,
    double* X0, double* X1,
    ulong j)
{
    vec4d n    = vec4d_set_d(Q->p);
    vec4d ninv = vec4d_set_d(Q->pinv);
    vec4d c = vec4d_set_d(vec1d_fnmadd(0.5, Q->p, 0.5));
    ulong i = 0; do {
        vec4d u0;
        u0 = vec4d_load(X0 + i);
        u0 = vec4d_mulmod2(u0, c, n, ninv);
        vec4d_store(X0 + i, u0);
    } while (i += 4, i < BLK_SZ);
    FLINT_ASSERT(i == BLK_SZ);
}

/************************* inverse butterfly **********************************
    4*a0 =            (b0 + b1) +        (b2 + b3)
    4*a1 =       w^-1*(b0 - b1) - i*w^-1*(b2 - b3)
    4*a2 = w^-2*(     (b0 + b1) -        (b2 + b3))
    4*a3 = w^-2*(w^-1*(b0 - b1) + i*w^-1*(b2 - b3))
    W  := -w^-1
    W2 := -w^-2
    IW := i*w^-1
*/
#define RADIX_4_REVERSE_PARAM(V, Q, j, jm, j_can_be_0) \
    FLINT_ASSERT((j) == 0 || (jm) == ((j)^(n_next_pow2m1(j)>>1))); \
    V W  = V##_set_d(((j_can_be_0) && UNLIKELY((j) == 0)) ? -Q->w2s[0] : Q->w2s[2*(jm)+1]); \
    V W2 = V##_set_d(((j_can_be_0) && UNLIKELY((j) == 0)) ? -Q->w2s[0] : Q->w2s[(jm)]); \
    V IW = V##_set_d(((j_can_be_0) && UNLIKELY((j) == 0)) ?  Q->w2s[1] : Q->w2s[2*(jm)]); \
    V n    = V##_set_d(Q->p); \
    V ninv = V##_set_d(Q->pinv); \

#define RADIX_4_REVERSE_PARAM_SURE(V, Q, j, jm, j_is_0) \
    FLINT_ASSERT((j) == 0 || (jm) == ((j)^(n_next_pow2m1(j)>>1))); \
    V W  = V##_set_d((j_is_0) ? -Q->w2s[0] : Q->w2s[2*(jm)+1]); \
    V W2 = V##_set_d((j_is_0) ? -Q->w2s[0] : Q->w2s[(jm)]); \
    V IW = V##_set_d((j_is_0) ?  Q->w2s[1] : Q->w2s[2*(jm)]); \
    V n    = V##_set_d(Q->p); \
    V ninv = V##_set_d(Q->pinv); \

#define RADIX_4_REVERSE_MOTH(V, X0, X1, X2, X3) \
{ \
    V x0, x1, x2, x3, y0, y1, y2, y3; \
    x0 = V##_load(X0); \
    x1 = V##_load(X1); \
    x2 = V##_load(X2); \
    x3 = V##_load(X3); \
    y0 = V##_add(x0, x1); \
    y1 = V##_add(x2, x3); \
    y2 = V##_sub(x0, x1); \
    y3 = V##_sub(x3, x2); \
    y2 = V##_mulmod2(y2, W, n, ninv); \
    y3 = V##_mulmod2(y3, IW, n, ninv); \
    x0 = V##_add(y0, y1); \
    x1 = V##_sub(y3, y2); \
    V##_store(X1, x1); \
    x2 = V##_sub(y1, y0); \
    x3 = V##_add(y3, y2); \
    x0 = V##_reduce_to_pm1n(x0, n, ninv); \
    x2 = V##_mulmod2(x2, W2, n, ninv); \
    x3 = V##_mulmod2(x3, W2, n, ninv); \
    V##_store(X0, x0); \
    V##_store(X2, x2); \
    V##_store(X3, x3); \
}

/* second parameter is the bool j == 0 */

/*
    These functions are disabled because the ifft expects input in
    slightly-worse-than-bit-reversed order as in basecase_4.
*/
#if 0
/* length 1 */
FLINT_FORCE_INLINE void
sd_ifft_basecase_0_0(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm)
{
    FLINT_ASSERT(0 == (j == 0));
}

FLINT_FORCE_INLINE void
sd_ifft_basecase_0_1(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm)
{
    FLINT_ASSERT(1 == (j == 0));
}

/* length 2 */
FLINT_FORCE_INLINE void
sd_ifft_basecase_1_0(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm)
{
    RADIX_2_REVERSE_PARAM(vec1d, Q, j)
    RADIX_2_REVERSE_MOTH(vec1d, X+0, X+1);
}

FLINT_FORCE_INLINE void
sd_ifft_basecase_1_1(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm)
{
    RADIX_2_REVERSE_PARAM(vec1d, Q, j)
    RADIX_2_REVERSE_MOTH(vec1d, X+0, X+1);
}

/* length 4 */
FLINT_FORCE_INLINE void
sd_ifft_basecase_2_0(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm)
{
    RADIX_4_REVERSE_PARAM_SURE(vec1d, Q, j, jm, 0)
    RADIX_4_REVERSE_MOTH(vec1d, X+0, X+1, X+2, X+3);
}

FLINT_FORCE_INLINE void
sd_ifft_basecase_2_1(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm)
{
    RADIX_4_REVERSE_PARAM_SURE(vec1d, Q, j, jm, 1)
    RADIX_4_REVERSE_MOTH(vec1d, X+0, X+1, X+2, X+3);
}
#endif

/* length 16 */
#define DEFINE_IT(j_is_0) \
FLINT_FORCE_INLINE void sd_ifft_basecase_4_##j_is_0(\
    const sd_fft_ctx_t Q, double* X, ulong j, ulong jm) \
{ \
    vec4d n    = vec4d_set_d(Q->p); \
    vec4d ninv = vec4d_set_d(Q->pinv); \
    vec4d W, W2, IW, u, v; \
    vec4d x0, x1, x2, x3, y0, y1, y2, y3; \
    FLINT_ASSERT(j_is_0 == (j == 0)); \
    FLINT_ASSERT(j == 0 || jm == (j^(n_next_pow2m1(j)>>1))); \
 \
    x0 = vec4d_load(X + 0); \
    x1 = vec4d_load(X + 4); \
    x2 = vec4d_load(X + 8); \
    x3 = vec4d_load(X + 12); \
 \
    if (j_is_0) \
    { \
        W  = vec4d_set_d4(-Q->w2s[0], Q->w2s[3], Q->w2s[7], Q->w2s[5]); \
        IW = vec4d_set_d4( Q->w2s[1], Q->w2s[2], Q->w2s[6], Q->w2s[4]); \
        W2 = vec4d_set_d4(-Q->w2s[0], Q->w2s[1], Q->w2s[3], Q->w2s[2]); \
    } \
    else \
    { \
        W2 = vec4d_load_aligned(Q->w2s + 4*jm); /* a b c d */ \
        W2 = vec4d_permute_3_2_1_0(W2);         /* d c b a */ \
        u = vec4d_load_aligned(Q->w2s + 8*jm);  /* 0 1 2 3 */ \
        v = vec4d_load_aligned(Q->w2s + 8*jm+4);/* 4 5 6 7 */ \
        W  = vec4d_unpackhi(u, v);              /* 1 5 3 7 */ \
        IW = vec4d_unpacklo(u, v);              /* 0 4 2 6 */ \
        W  = vec4d_permute_3_1_2_0(W);          /* 7 5 3 1 */ \
        IW = vec4d_permute_3_1_2_0(IW);         /* 6 4 2 0 */ \
    } \
 \
    y0 = vec4d_add(x0, x1); \
    y1 = vec4d_add(x2, x3); \
    y2 = vec4d_sub(x0, x1); \
    y3 = vec4d_sub(x3, x2); \
    y2 = vec4d_mulmod2(y2, W, n, ninv); \
    y3 = vec4d_mulmod2(y3, IW, n, ninv); \
    x0 = vec4d_add(y0, y1); \
    x1 = vec4d_sub(y3, y2); \
    x2 = vec4d_sub(y1, y0); \
    x3 = vec4d_add(y3, y2); \
    x0 = vec4d_reduce_to_pm1n(x0, n, ninv); \
    x2 = vec4d_mulmod2(x2, W2, n, ninv); \
    x3 = vec4d_mulmod2(x3, W2, n, ninv); \
 \
    VEC4D_TRANSPOSE(x0, x1, x2, x3, x0, x1, x2, x3); \
 \
    W  = vec4d_set_d(j_is_0 ? -Q->w2s[0] : Q->w2s[2*jm+1]); \
    IW = vec4d_set_d(j_is_0 ?  Q->w2s[1] : Q->w2s[2*jm+0]); \
    W2 = vec4d_set_d(j_is_0 ? -Q->w2s[0] : Q->w2s[jm]); \
    y0 = vec4d_add(x0, x1); \
    y1 = vec4d_add(x2, x3); \
    y2 = vec4d_sub(x0, x1); \
    y3 = vec4d_sub(x3, x2); \
    y2 = vec4d_mulmod2(y2, W, n, ninv); \
    y3 = vec4d_mulmod2(y3, IW, n, ninv); \
    x0 = vec4d_add(y0, y1); \
    x1 = vec4d_sub(y3, y2); \
    x2 = vec4d_sub(y1, y0); \
    x3 = vec4d_add(y3, y2); \
    x0 = vec4d_reduce_to_pm1n(x0, n, ninv); \
    x2 = vec4d_mulmod2(x2, W2, n, ninv); \
    x3 = vec4d_mulmod2(x3, W2, n, ninv); \
    vec4d_store(X+0, x0); \
    vec4d_store(X+4, x1); \
    vec4d_store(X+8, x2); \
    vec4d_store(X+12, x3); \
}

DEFINE_IT(0)
DEFINE_IT(1)
#undef DEFINE_IT


/* use with N = M-2 and M >= 6 */
#define EXTEND_BASECASE(N, M, j_is_0) \
void CAT3(sd_ifft_basecase, M, j_is_0)(const sd_fft_ctx_t Q, double* X, ulong j, ulong jm) \
{ \
    ulong l = n_pow2(M - 2); \
    FLINT_ASSERT(j_is_0 == (j == 0)); \
    FLINT_ASSERT(j == 0 || jm == (j^(n_next_pow2m1(j)>>1))); \
    CAT3(sd_ifft_basecase, N, j_is_0)(Q, X+0*l, 4*j+0, j_is_0 ? 0 : 4*jm+3); \
    CAT3(sd_ifft_basecase, N, 0     )(Q, X+1*l, 4*j+1, j_is_0 ? 1 : 4*jm+2); \
    CAT3(sd_ifft_basecase, N, 0     )(Q, X+2*l, 4*j+2, j_is_0 ? 3 : 4*jm+1); \
    CAT3(sd_ifft_basecase, N, 0     )(Q, X+3*l, 4*j+3, j_is_0 ? 2 : 4*jm+0); \
    { \
        RADIX_4_REVERSE_PARAM_SURE(vec8d, Q, j, jm, j_is_0) \
        ulong i = 0; do { \
            RADIX_4_REVERSE_MOTH(vec8d, X+0*l+i, X+1*l+i, X+2*l+i, X+3*l+i) \
        } while (i += 8, i < l); \
        FLINT_ASSERT(i == l); \
    } \
}

EXTEND_BASECASE(4, 6, 0)
EXTEND_BASECASE(4, 6, 1)
EXTEND_BASECASE(6, 8, 0)
EXTEND_BASECASE(6, 8, 1)
#undef EXTEND_BASECASE

/* parameter 1: j can be zero */
void sd_ifft_base_1(const sd_fft_ctx_t Q, ulong I, ulong j)
{
    ulong jm = j^(n_next_pow2m1(j)>>1);
    double* x = sd_fft_ctx_blk_index(Q, I);
    if (j == 0)
        sd_ifft_basecase_8_1(Q, x, j, jm);
    else
        sd_ifft_basecase_8_0(Q, x, j, jm);
}

/* parameter 0: j cannot be zero */
void sd_ifft_base_0(const sd_fft_ctx_t Q, ulong I, ulong j)
{
    ulong jm = j^(n_next_pow2m1(j)>>1);
    double* x = sd_fft_ctx_blk_index(Q, I);
    FLINT_ASSERT(j != 0);
    sd_ifft_basecase_8_0(Q, x, j, jm);
}

/***************** inverse butterfy with truncation **************************/

#define DEFINE_IT(nn, zz, ff) \
static int CAT4(radix_4_moth_inv_trunc_block, nn, zz, ff)( \
    const sd_fft_ctx_t Q, \
    double* X0, double* X1, double* X2, double* X3, \
    ulong j, \
    ulong jm) \
{ \
    int l = 4; \
    flint_printf("function l = %d, n = %d, z = %d, f = %d", l, nn, zz, ff); \
    if (1 <= zz && zz <= 4 && nn <= zz && 1 <= nn+ff && nn+ff <= l) \
    { \
        flint_printf(" is not implemented\n"); \
        fflush(stdout); \
    } \
    else \
    { \
        flint_printf(" does not exist and should not be called\n"); \
        fflush(stdout); \
        flint_abort(); \
    } \
    return 0; \
}

DEFINE_IT(0,1,0)
DEFINE_IT(0,1,1)
DEFINE_IT(0,2,0)
DEFINE_IT(0,2,1)
DEFINE_IT(0,3,0)
DEFINE_IT(0,3,1)
DEFINE_IT(0,4,0)
DEFINE_IT(0,4,1)
DEFINE_IT(1,1,0)
DEFINE_IT(1,1,1)
DEFINE_IT(1,2,0)
DEFINE_IT(1,2,1)
DEFINE_IT(1,3,0)
DEFINE_IT(1,3,1)
DEFINE_IT(1,4,0)
DEFINE_IT(1,4,1)
DEFINE_IT(2,1,0)
DEFINE_IT(2,1,1)
DEFINE_IT(2,2,0)
DEFINE_IT(2,2,1)
DEFINE_IT(2,3,0)
DEFINE_IT(2,3,1)
DEFINE_IT(2,4,0)
DEFINE_IT(2,4,1)
DEFINE_IT(3,1,0)
DEFINE_IT(3,1,1)
DEFINE_IT(3,2,0)
DEFINE_IT(3,2,1)
DEFINE_IT(3,3,0)
DEFINE_IT(3,3,1)
DEFINE_IT(3,4,0)
DEFINE_IT(3,4,1)
DEFINE_IT(4,1,0)
DEFINE_IT(4,1,1)
DEFINE_IT(4,2,0)
DEFINE_IT(4,2,1)
DEFINE_IT(4,3,0)
DEFINE_IT(4,3,1)
DEFINE_IT(4,4,0)
DEFINE_IT(4,4,1)
#undef DEFINE_IT

/************************ the recursive stuff ********************************/

void sd_ifft_main_block(
    const sd_fft_ctx_t Q,
    ulong I, /* starting index */
    ulong S, /* stride */
    ulong k, /* BLK_SZ transforms each of length 2^k */
    ulong j)
{
    if (k > 2)
    {
        ulong k1 = k/2;
        ulong k2 = k - k1;

        /* row ffts */
        ulong l1 = n_pow2(k1);
        ulong b = 0; do {
            sd_ifft_main_block(Q, I + (b<<k2)*S, S, k2, (j<<k1) + b);
        } while (b++, b < l1);

        /* column ffts */
        ulong l2 = n_pow2(k2);
        ulong a = 0; do {
            sd_ifft_main_block(Q, I + a*S, S<<k2, k1, j);
        } while (a++, a < l2);

        return;
    }

    ulong jm = j^(n_next_pow2m1(j)>>1);

    if (k == 2)
    {
        double* X0 = sd_fft_ctx_blk_index(Q, I + S*0);
        double* X1 = sd_fft_ctx_blk_index(Q, I + S*1);
        double* X2 = sd_fft_ctx_blk_index(Q, I + S*2);
        double* X3 = sd_fft_ctx_blk_index(Q, I + S*3);
        RADIX_4_REVERSE_PARAM(vec8d, Q, j, jm, 1)
        ulong i = 0; do {
            RADIX_4_REVERSE_MOTH(vec8d, X0+i, X1+i, X2+i, X3+i);
        } while(i += 8, i < BLK_SZ);
    }
    else if (k == 1)
    {
        double* X0 = sd_fft_ctx_blk_index(Q, I + S*0);
        double* X1 = sd_fft_ctx_blk_index(Q, I + S*1);
        RADIX_2_REVERSE_PARAM(vec8d, Q, j)
        ulong i = 0; do {
            RADIX_2_REVERSE_MOTH(vec8d, X0+i, X1+i);
        } while (i += 8, i < BLK_SZ);
    }
}

void sd_ifft_main(
    const sd_fft_ctx_t Q,
    ulong I, /* starting index */
    ulong S, /* stride */
    ulong k, /* transform length BLK_SZ*2^k */
    ulong j)
{
    if (k > 2)
    {
        ulong k1 = k/2;
        ulong k2 = k - k1;

        ulong l1 = n_pow2(k1);
        ulong b = 0; do {
            sd_ifft_main(Q, I + b*(S<<k2), S, k2, (j<<k1) + b);
        } while (b++, b < l1);

        ulong l2 = n_pow2(k2);
        ulong a = 0; do {
            sd_ifft_main_block(Q, I + a*S, S<<k2, k1, j);
        } while (a++, a < l2);

        return;
    }

    if (k == 2)
    {
        /* k1 = 2; k2 = 0 */
        sd_ifft_base_1(Q, I+S*0, 4*j+0);
        sd_ifft_base_0(Q, I+S*1, 4*j+1);
        sd_ifft_base_0(Q, I+S*2, 4*j+2);
        sd_ifft_base_0(Q, I+S*3, 4*j+3);
        sd_ifft_main_block(Q, I, S, 2, j);
    }
    else if (k == 1)
    {
        /* k1 = 1; k2 = 0 */
        sd_ifft_base_1(Q, I+S*0, 2*j+0);
        sd_ifft_base_0(Q, I+S*1, 2*j+1);
        sd_ifft_main_block(Q, I, S, 1, j);
    }
    else
    {
        sd_ifft_base_1(Q, I, j);
    }
}

void sd_ifft_trunc_block(
    const sd_fft_ctx_t Q,
    ulong I, /* starting index */
    ulong S, /* stride */
    ulong k, /* BLK_SZ transforms each of length 2^k */
    ulong j,
    ulong z, /* actual trunc is z */
    ulong n, /* actual trunc is n */
    int f)
{
    FLINT_ASSERT(f == 0 || f == 1);
    FLINT_ASSERT(n <= z);
    FLINT_ASSERT(1 <= z && z <= n_pow2(k));
    FLINT_ASSERT(1 <= n+f && n+f <= n_pow2(k));

    if (!f && z == n && n == n_pow2(k))
    {
        sd_ifft_main_block(Q, I, S, k, j);
        return;
    }

    if (k == 2)
    {
#define IT(nn, zz, ff) CAT4(radix_4_moth_inv_trunc_block, nn, zz, ff)
#define LOOKUP_IT(nn, zz, ff) tab[(ulong)(ff) + 2*((zz)-1 + 4*(nn))]
        static int (*tab[5*4*2])(const sd_fft_ctx_t, double*, double*, double*, double*, ulong, ulong) =
            {IT(0,1,0),IT(0,1,1), IT(0,2,0),IT(0,2,1), IT(0,3,0),IT(0,3,1), IT(0,4,0),IT(0,4,1),
             IT(1,1,0),IT(1,1,1), IT(1,2,0),IT(1,2,1), IT(1,3,0),IT(1,3,1), IT(1,4,0),IT(1,4,1),
             IT(2,1,0),IT(2,1,1), IT(2,2,0),IT(2,2,1), IT(2,3,0),IT(2,3,1), IT(2,4,0),IT(2,4,1),
             IT(3,1,0),IT(3,1,1), IT(3,2,0),IT(3,2,1), IT(3,3,0),IT(3,3,1), IT(3,4,0),IT(3,4,1),
             IT(4,1,0),IT(4,1,1), IT(4,2,0),IT(4,2,1), IT(4,3,0),IT(4,3,1), IT(4,4,0),IT(4,4,1)};

        ulong jm = j^(n_next_pow2m1(j)>>1);
        if (LOOKUP_IT(n,z,f)(Q, sd_fft_ctx_blk_index(Q, I+S*0),
                                sd_fft_ctx_blk_index(Q, I+S*1),
                                sd_fft_ctx_blk_index(Q, I+S*2),
                                sd_fft_ctx_blk_index(Q, I+S*3), j, jm))
        {
            return;
        }
#undef LOOKUP_IT
#undef IT
    }

    if (k > 1)
    {
        ulong k1 = k/2;
        ulong k2 = k - k1;

        ulong l2 = n_pow2(k2);
        ulong n1 = n >> k2;
        ulong n2 = n & (l2 - 1);
        ulong z1 = z >> k2;
        ulong z2 = z & (l2 - 1);
        int fp = n2 + f > 0;
        ulong z2p = n_min(l2, z);
        ulong m = n_min(n2, z2);
        ulong mp = n_max(n2, z2);

        /* complete rows */
        for (ulong b = 0; b < n1; b++)
            sd_ifft_main_block(Q, I + b*(S << k2), S, k2, (j << k1) + b);

        /* rightmost columns */
        for (ulong a = n2; a < z2p; a++)
            sd_ifft_trunc_block(Q, I + a*S, S << k2, k1, j, z1 + (a < mp), n1, fp);

        /* last partial row */
        if (fp)
            sd_ifft_trunc_block(Q, I + n1*(S << k2), S, k2, (j << k1) + n1, z2p, n2, f);

        /* leftmost columns */
        for (ulong a = 0; a < n2; a++)
            sd_ifft_trunc_block(Q, I + a*S, S << k2, k1, j, z1 + (a < m), n1 + 1, 0);

        return;
    }

    if (k == 1)
    {
#define IT(nn, zz, ff) CAT4(radix_2_moth_inv_trunc_block, nn, zz, ff)
#define LOOKUP_IT(nn, zz, ff) tab[(ulong)(ff) + 2*((zz)-1 + 2*(nn))]
        static void (*tab[3*2*2])(const sd_fft_ctx_t, double*, double*, ulong) =
            {IT(0,1,0),IT(0,1,1), IT(0,2,0),IT(0,2,1),
             IT(1,1,0),IT(1,1,1), IT(1,2,0),IT(1,2,1),
             IT(2,1,0),IT(2,1,1), IT(2,2,0),IT(2,2,1)};

        LOOKUP_IT(n,z,f)(Q, sd_fft_ctx_blk_index(Q, I+S*0),
                            sd_fft_ctx_blk_index(Q, I+S*1), j);
        return;
#undef LOOKUP_IT
#undef IT
    }
}


void sd_ifft_trunc(
    const sd_fft_ctx_t Q,
    ulong I, // starting index
    ulong S, // stride
    ulong k, // transform length 2^(k + LG_BLK_SZ)
    ulong j,
    ulong z,   // actual trunc is z*BLK_SZ
    ulong n,   // actual trunc is n*BLK_SZ
    int f)
{
    FLINT_ASSERT(f == 0 || f == 1);
    FLINT_ASSERT(n <= z);
    FLINT_ASSERT(1 <= BLK_SZ*z && BLK_SZ*z <= n_pow2(k+LG_BLK_SZ));
    FLINT_ASSERT(1 <= BLK_SZ*n+f && BLK_SZ*n+f <= n_pow2(k+LG_BLK_SZ));

    if (k > 2)
    {
        ulong k1 = k/2;
        ulong k2 = k - k1;

        ulong l2 = n_pow2(k2);
        ulong n1 = n >> k2;
        ulong n2 = n & (l2 - 1);
        ulong z1 = z >> k2;
        ulong z2 = z & (l2 - 1);
        int fp = n2 + f > 0;
        ulong z2p = n_min(l2, z);
        ulong m = n_min(n2, z2);
        ulong mp = n_max(n2, z2);

        /* complete rows */
        for (ulong b = 0; b < n1; b++)
            sd_ifft_main(Q, I + b*(S << k2), S, k2, (j << k1) + b);

        /* rightmost columns */
        for (ulong a = n2; a < z2p; a++)
            sd_ifft_trunc_block(Q, I + a*S, S << k2, k1, j, z1 + (a < mp), n1, fp);

        /* last partial row */
        if (fp)
            sd_ifft_trunc(Q, I + n1*(S << k2), S, k2, (j << k1) + n1, z2p, n2, f);

        /* leftmost columns */
        for (ulong a = 0; a < n2; a++)
            sd_ifft_trunc_block(Q, I + a*S, S << k2, k1, j, z1 + (a < m), n1 + 1, 0);

        return;
    }

    if (k == 2)
    {
                   sd_ifft_base_1(Q, I+S*0, 4*j+0);
        if (n > 1) sd_ifft_base_0(Q, I+S*1, 4*j+1);
        if (n > 2) sd_ifft_base_0(Q, I+S*2, 4*j+2);
        if (n > 3) sd_ifft_base_0(Q, I+S*3, 4*j+3);
        sd_ifft_trunc_block(Q, I, S, 2, j, z, n, f);
        if (f) sd_ifft_trunc(Q, I + S*n, S, 0, 4*j+n, 1, 0, f);
        
    }
    else if (k == 1)
    {
                   sd_ifft_base_1(Q, I+S*0, 2*j+0);
        if (n > 1) sd_ifft_base_0(Q, I+S*1, 2*j+1);
        sd_ifft_trunc_block(Q, I, S, 1, j, z, n, f);
        if (f) sd_ifft_trunc(Q, I + S*n, S, 0, 2*j+n, 1, 0, f);
    }
    else
    {
        FLINT_ASSERT(!f);
        sd_ifft_base_1(Q, I, j);
    }
}
