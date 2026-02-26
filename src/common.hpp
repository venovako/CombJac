#ifndef COMMON_HPP
#define COMMON_HPP

#ifdef _WIN32
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif /* !_CRT_NONSTDC_NO_DEPRECATE */
#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#endif /* !_CRT_NONSTDC_NO_WARNINGS */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif /* !_CRT_SECURE_NO_DEPRECATE */
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif /* !_CRT_SECURE_NO_WARNINGS */
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif /* !_USE_MATH_DEFINES */
#else /* !_WIN32 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* !_GNU_SOURCE */
#endif /* ?_WIN32 */

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef MIN_N
#error MIN_N defined
#else /* N >= 2 */
#define MIN_N 2u
#endif /* ?MIN_N */

#ifdef MAX_N
#error MAX_N defined
#else /* N bounded from above by... */
#ifdef _WIN32
#define MAX_N 212u
#else /* !_WIN32 */
#define MAX_N 254u
#endif /* ?_WIN32 */
#endif /* ?MAX_N */

#if !defined(N)
#error N not defined
#elif ((N) < (MIN_N))
#error N < MIN_N
#elif ((N) > (MAX_N))
#error N > MAX_N
#elif ((N) & 1u)
#error N odd
#elif ((N) <= 4u)
static const std::streamsize maxw = std::streamsize(1);
#elif ((N) <= 7u)
static const std::streamsize maxw = std::streamsize(2);
#elif ((N) <= 43u)
static const std::streamsize maxw = std::streamsize(3);
#elif ((N) <= 70u)
static const std::streamsize maxw = std::streamsize(4);
#else /* N > 71 */
static const std::streamsize maxw = std::streamsize(5);
#endif /* ?N */

#if ((N) <= 10u)
static const std::streamsize w = std::streamsize(1);
#elif ((N) <= 100u)
static const std::streamsize w = std::streamsize(2);
#else /* N <= 1000u */
static const std::streamsize w = std::streamsize(3);
#endif /* ?N */

// # of pivots in a parallel step
#ifdef P
#error P defined
#else /* P = N / 2 */
#define P ((N) >> 1u)
#endif /* ?P */

// # of parallel steps in a sweep
#ifdef S
#error S defined
#else /* S = N - 1 */
#define S ((N) - 1u)
#endif /* ?S */

// # of matrix entries in the strictly upper/lower triangle 
#ifdef E
#error E defined
#else /* E = P * S */
#define E ((P) * (S))
#endif /* ?E */

#ifdef N_1
#error N_1 defined
#else /* N_1 = N - 1 */
#define N_1 ((N) - 1u)
#endif /* ?N_1 */

#ifdef S_1
#error S_1 defined
#else /* S_1 = S - 1 */
#define S_1 ((S) - 1u)
#endif /* ?S_1 */

#ifdef P_1
#error P_1 defined
#else /* P_1 = P - 1 */
#define P_1 ((P) - 1u)
#endif /* ?P_1 */

#ifdef E_1
#error E_1 defined
#else /* E_1 = E - 1 */
#define E_1 ((E) - 1u)
#endif /* ?E_1 */

// # of the pivots not colliding with a given one
#ifdef NCP
#error NCP defined
#else /* NCP = E - ((N - 1) * 2 - 1) */
#define NCP ((E) - (((N_1) << 1u) - 1u))
#endif /* ?NCP */

// index base (0 or 1) for the printouts
#ifndef IXBASE
#define IXBASE ushort(1u)
#endif /* !IXBASE */

#ifdef R
#error R defined
#else /* !R */
#define R 0u
#endif /* ?R */

#ifdef C
#error C defined
#else /* !C */
#define C 1u
#endif /* ?C */

#ifndef GRAPHVIZ_SIZE
#define GRAPHVIZ_SIZE 4.25
#endif /* !GRAPHVIZ_SIZE */

#ifndef GRAPHVIZ_FONTSIZE
#define GRAPHVIZ_FONTSIZE 96
#endif /* !GRAPHVIZ_FONTSIZE */

#ifndef GRAPHVIZ_EDGECOLOR
#define GRAPHVIZ_EDGECOLOR "dimgray"
#endif /* !GRAPHVIZ_EDGECOLOR */

#ifndef ASYMPTOTE_FONTSIZE
#define ASYMPTOTE_FONTSIZE 10u
#endif /* !ASYMPTOTE_FONTSIZE */

typedef unsigned char uchar;
static uchar in_strat[E][2u];
typedef unsigned short ushort;
static ushort indep_sets[E_1][NCP], active_sets[E_1][NCP];
static ushort indep_cnts[E_1], active_cnts[E_1];
static ushort used_set[E], tmp_set[E_1], used_cnt;
typedef unsigned long long ullong;
typedef long double ldouble;

#endif /* !COMMON_HPP */
