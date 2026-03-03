// Send-Receive Communication
#include "defs.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

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

#ifndef K
#define K unsigned char
#endif /* K */

// # of pivots in a parallel step
#ifdef P
#error P defined
#else /* P = N / 2 */
#define P (N >> 1u)
#endif /* ?P */

// # of parallel steps in a sweep
#ifdef S
#error S defined
#else /* S = N - 1 */
#define S (N - 1u)
#endif /* ?S */

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
  K r, c;
} A[S][P], B[S][P];

static struct comm
{
  unsigned dst : 31; // a rank to send to
  unsigned col : 1; // left or right block-column
  unsigned src : 31; // a rank to receive from
  unsigned swp : 1; // a logical swap needed after MPI_Sendrecv_replace
} X[S][P], Y[S][P];

static size_t min_cost = ~0ull;

static inline bool operator<(const pivot &a, const pivot &b)
{
  return ((a.r < b.r) || ((a.r == b.r) && (a.c < b.c)));
}

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

static size_t gen_comm()
{
  size_t w = 0u;
  for (unsigned i = 0u; i < S; ++i) {
    const unsigned j = ((i + 1u) % S);
    for (unsigned k = 0u; k < P; ++k) {
      for (unsigned l = 0u; l < P; ++l) {
        if ((A[i][k].r == A[j][l].r) || (A[i][k].r == A[j][l].c) || (A[i][k].c == A[j][l].r) || (A[i][k].c == A[j][l].c))
          X[i][k].dst = l;
        if ((A[j][k].r == A[i][l].r) || (A[j][k].r == A[i][l].c) || (A[j][k].c == A[i][l].r) || (A[j][k].c == A[i][l].c))
          X[i][k].src = l;
        if (A[i][k].r == A[j][k].r) {
          X[i][k].col = 1u;
          X[i][k].swp = 0u;
        }
        else if (A[i][k].r == A[j][k].c) {
          X[i][k].col = 1u;
          X[i][k].swp = 1u;
        }
        else if (A[i][k].c == A[j][k].r) {
          X[i][k].col = 0u;
          X[i][k].swp = 1u;
        }
        else if (A[i][k].c == A[j][k].c) {
          X[i][k].col = 0u;
          X[i][k].swp = 0u;
        }
        else // should never happen
          exit(EXIT_FAILURE);
      }
      w += (W[X[i][k].src][k] + W[k][X[i][k].dst]);
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
        if (accept_perm(A[k], A[l]))
          f += find_perms(l + 1u);
      } while (std::prev_permutation(&(A[l][0u]), &(A[l][P])));
    }
    else {
      const unsigned k = (S - 1u);
      if (accept_perm(A[k], A[0u])) {
        f = 1u;
        const size_t cost = gen_comm();
#ifndef NDEBUG
        std::cout << cost << ':' << std::endl;
#endif /* !NDEBUG */
        if (cost < min_cost) {
#ifdef NDEBUG
        std::cout << cost << ':' << std::endl;
#endif /* NDEBUG */
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
          (void)memcpy(B, A, sizeof(A));
          (void)memcpy(Y, X, sizeof(X));
          min_cost = cost;
        }
      }
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
  const size_t s = find_perms(0u);
  std::cout << "#~strats=" << s << std::endl;
  std::cout << "min_cost=" << min_cost << std::endl;
  return EXIT_SUCCESS;
}
