// Send-Receive Communication
#include "defs.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#ifndef V
#include <csetjmp>
static jmp_buf jmp;
#endif /* !V */

#if !defined(N)
#error N not defined
#elif (N < 4u)
#error N < 4
#elif (N > 100u)
#error N > 100
#elif (N & 1u)
#error N odd
#elif (N <= 10u)
static const std::streamsize v = std::streamsize(1);
#else /* N > 10 */
static const std::streamsize v = std::streamsize(2);
#endif /* ?N */

// # of pivots in a parallel step
#ifdef P
#error P defined
#else /* P = N / 2 */
#define P (N >> 1u)
#endif /* ?P */

#if (P <= 10u)
#ifdef REPLACE
static const std::streamsize t = std::streamsize(2);
#else /* !REPLACE */
static const std::streamsize t = std::streamsize(1);
#endif /* ?REPLACE */
#else /* P > 10 */
#ifdef REPLACE
static const std::streamsize t = std::streamsize(3);
#else /* !REPLACE */
static const std::streamsize t = std::streamsize(2);
#endif /* ?REPLACE */
#endif /* ?P */

// # of parallel steps in a sweep
#ifdef S
#error S defined
#else /* S = N - 1 */
#define S (N - 1u)
#endif /* ?S */

#ifdef DST
#error DST defined
#else /* !DST */
#define DST 0u
#endif /* ?DST */

#ifdef SRC
#error SRC defined
#else /* !SRC */
#define SRC 1u
#endif /* ?SRC */

static const
#include H

/* W should be in the matix format:
{
  { w11, w12, ..., w1P },
  ...
  { wP1, wP2, ..., wPP }
};
where wIJ is the cost of sending a unit of data from I to J
(similar to the NUMA distances of `numactl -H`)
*/
static const unsigned W[P][P] =
#ifdef V
#include V
#else /* !V */
  {};
#endif /* ?V */

static struct pivot
{
  unsigned char r, c;
} A[S][P], B[S][P];

static inline bool operator<(const pivot &a, const pivot &b)
{
  return ((a.r < b.r) || ((a.r == b.r) && (a.c < b.c)));
}

static size_t min_cost = ~0ull;

#ifdef REPLACE
static signed char X[S][P][2u];
static signed char Y[S][P][2u];

static inline int s1r1(const pivot &a, const pivot &b)
{
  if (a.r == b.r)
    return 1;
  if (a.c == b.c)
    return 2;
  if (a.r == b.c)
    return -1;
  if (a.c == b.r)
    return -2;
  return 0;
}

static inline bool accept_perm(const pivot a[], const pivot b[])
{
  for (unsigned i = 0u; i < P; ++i)
    if (!s1r1(a[i], b[i]))
      return false;
  return true;
}
#else /* !REPLACE */
static signed char X[S][P][2u][2u];
static signed char Y[S][P][2u][2u];
#endif /* ?REPLACE */

static size_t gen_comm()
{
  size_t w = 0u;
  for (unsigned i = 0u; i < S; ++i) {
    const unsigned j = ((i + 1u) % S);
    for (unsigned k = 0u; k < P; ++k) {
#ifdef REPLACE
      bool r = false;
      for (unsigned l = 0u; l < P; ++l) {
        if (l == k)
          continue;
        else if ((A[i][k].r == A[j][l].r) || (A[i][k].r == A[j][l].c)) {
          r = true;
          X[i][k][DST] = -int(l) - 1;
          break;
        }
        else if ((A[i][k].c == A[j][l].r) || (A[i][k].c == A[j][l].c)) {
          X[i][k][DST] = int(l);
          break;
        }
      }
      for (unsigned l = 0u; l < P; ++l) {
        if (l == k)
          continue;
        else if ((A[j][k].r == A[i][l].r) || (A[j][k].r == A[i][l].c)) {
          X[i][k][SRC] = (r ? int(l) : (-int(l) - 1));
          break;
        }
        else if ((A[j][k].c == A[i][l].r) || (A[j][k].c == A[i][l].c)) {
          X[i][k][SRC] = (r ? (-int(l) - 1) : int(l));
          break;
        }
      }
      if (X[i][k][SRC] < 0)
        w += W[-(X[i][k][SRC] + 1)][k];
      else
        w += W[X[i][k][SRC]][k];
      if (X[i][k][DST] < 0)
        w += W[k][-(X[i][k][DST] + 1)];
      else
        w += W[k][X[i][k][DST]];
#else /* !REPLACE */
      for (unsigned l = 0u; l < P; ++l) {
        if ((A[i][k].r == A[j][l].r) || (A[i][k].r == A[j][l].c))
          X[i][k][R][DST] = l;
        if ((A[i][k].c == A[j][l].r) || (A[i][k].c == A[j][l].c))
          X[i][k][C][DST] = l;
        if ((A[j][k].r == A[i][l].r) || (A[j][k].r == A[i][l].c))
          X[i][k][R][SRC] = l;
        if ((A[j][k].c == A[i][l].r) || (A[j][k].c == A[i][l].c))
          X[i][k][C][SRC] = l;
      }
      w += W[X[i][k][R][SRC]][k];
      w += W[k][X[i][k][R][DST]];
      w += W[X[i][k][C][SRC]][k];
      w += W[k][X[i][k][C][DST]];
#endif /* ?REPLACE */
    }
  }
  return w;
}

static size_t find_perms(const unsigned l)
{
  size_t f = 0u;
  if (l) {
    if (l < S) {
      const unsigned k = (l - 1u);
      do {
#ifdef REPLACE
        if (accept_perm(A[k], A[l]))
#endif /* REPLACE */
          f += find_perms(l + 1u);
      } while (std::prev_permutation(&(A[l][0u]), &(A[l][P])));
    }
    else {
      const unsigned k = (S - 1u);
#ifdef REPLACE
      if (accept_perm(A[k], A[0u])) {
#endif /* REPLACE */
        f = 1u;
        const size_t cost = gen_comm();
        if (cost < min_cost) {
          std::cout << '{' << std::endl;
          for (unsigned s = 0u; s < S; ++s) {
            std::cout << "  {";
            for (unsigned p = 0u; p < P; ++p) {
              std::cout << '{' << std::setw(v) << unsigned(A[s][p].r) << ',' << std::setw(v) << unsigned(A[s][p].c) << '}';
              if (p < (P - 1u))
                std::cout << ',';
            }
            std::cout << '}';
            if (s < (S - 1u))
              std::cout << ',';
            std::cout << std::endl;
          }
          std::cout << "};" << std::endl;
          std::cout << cost << ": {" << std::endl;
          for (unsigned s = 0u; s < S; ++s) {
            std::cout << "  {";
            for (unsigned p = 0u; p < P; ++p) {
#ifdef REPLACE
              std::cout << '{' << std::setw(t) << int(X[s][p][DST]) << ',' << std::setw(t) << int(X[s][p][SRC]) << '}';
#else /* !REPLACE */
              std::cout << "{{" << std::setw(t) << int(X[s][p][R][DST]) << ',' << std::setw(t) << int(X[s][p][R][SRC]) << "},{" << std::setw(t) << int(X[s][p][C][DST]) << ',' << std::setw(t) << int(X[s][p][C][SRC]) << "}}";
#endif /* ?REPLACE */
              if (p < (P - 1u))
                std::cout << ',';
            }
            std::cout << '}';
            if (s < (S - 1u))
              std::cout << ',';
            std::cout << std::endl;
          }
          std::cout << "};" << std::endl << std::endl << std::flush;
          (void)memcpy(B, A, sizeof(A));
          (void)memcpy(Y, X, sizeof(X));
          min_cost = cost;
        }
#ifndef V
        longjmp(jmp, 1);
#endif /* !V */
#ifdef REPLACE
      }
#endif /* REPLACE */
    }
  }
  else {
    do {
      f += find_perms(1u);
    } while (std::prev_permutation(&(A[0u][0u]), &(A[0u][P])));
  }
  return f;
}

int main(int argc, char* argv[])
{
  if (argc != 1) {
    std::cerr << *argv << std::endl;
    return EXIT_FAILURE;
  }
  (void)memcpy(A, T, sizeof(T));
#ifdef V
  const size_t s = find_perms(0u);
  std::cout << "#~strats=" << s << std::endl;
  std::cout << "min_cost=" << min_cost << std::endl;
#else /* !V */
  if (!setjmp(jmp))
    (void)find_perms(0u);
#endif /* ?V */
  return EXIT_SUCCESS;
}
