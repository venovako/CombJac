#include "defs.hpp"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

static
#include HN
static KM AM[SM][PM][2u];

#if (M <= 10u)
static const std::streamsize w = std::streamsize(1);
static const std::streamsize v = std::streamsize(3);
#elif (M <= 100u)
static const std::streamsize w = std::streamsize(2);
static const std::streamsize v = std::streamsize(3);
#elif (M <= 1000u)
static const std::streamsize w = std::streamsize(3);
static const std::streamsize v = std::streamsize(3);
#elif (M <= 10000u)
static const std::streamsize w = std::streamsize(4);
static const std::streamsize v = std::streamsize(4);
#elif (M <= 100000u)
static const std::streamsize w = std::streamsize(5);
static const std::streamsize v = std::streamsize(5);
#elif (M <= 1000000u)
static const std::streamsize w = std::streamsize(6);
static const std::streamsize v = std::streamsize(6);
#elif (M <= 10000000u)
static const std::streamsize w = std::streamsize(7);
static const std::streamsize v = std::streamsize(7);
#elif (M <= 100000000u)
static const std::streamsize w = std::streamsize(8);
static const std::streamsize v = std::streamsize(8);
#elif (M <= 1000000000u)
static const std::streamsize w = std::streamsize(9);
static const std::streamsize v = std::streamsize(9);
#else /* M > 1000000000 */
static const std::streamsize w = std::streamsize(10);
static const std::streamsize v = std::streamsize(10);
#endif /* ?M */

int main(int argc, char* argv[])
{
  if (argc != 1) {
    std::cerr << *argv << std::endl;
    return EXIT_FAILURE;
  }

  /* (SW,NE); (SE,NW) */
#ifdef _OPENMP
#pragma omp parallel for default(none) shared(AN,AM)
#endif /* _OPENMP */
  for (unsigned p = 0u; p < SN; ++p) {
    const unsigned q = (p << 1u);
    const unsigned q1 = (q + 1u);
    for (unsigned a = 0u; a < PN; ++a) {
      const unsigned b = (a << 1u);
      const unsigned b1 = (b + 1u);
      const unsigned i = AN[p][a][R];
      const unsigned j = AN[p][a][C];
      const unsigned i2 = (i << 1u);
      const unsigned j2 = (j << 1u);
      /* SW */
      AM[q][b][R] = (i2 + 1u);
      AM[q][b][C] = j2;
      /* NE */
      AM[q][b1][R] = i2;
      AM[q][b1][C] = (j2 + 1u);
      /* SE */
      AM[q1][b][R] = (i2 + 1u);
      AM[q1][b][C] = (j2 + 1u);
      /* NW */
      AM[q1][b1][R] = i2;
      AM[q1][b1][C] = j2;
    }
  }
  /* super-diag */
  const unsigned d = (M - 2u);
#ifdef _OPENMP
#pragma omp parallel for default(none) shared(AM,d)
#endif /* _OPENMP */
  for (unsigned i = 0u; i < PM; ++i) {
    const unsigned j = ((PM - 1u) - i);
    AM[d][j][C] = ((AM[d][j][R] = (i << 1u)) + 1u);
  }

  std::ofstream hpp(HM, (std::ios::out | std::ios::trunc));
  if (!hpp) {
    std::cerr << HM << std::endl;
    return EXIT_FAILURE;
  }
  hpp << KK << ' ' << AK << '[' << std::setw(v) << SM << "][" << std::setw(v) << PM << "][2] =" << std::endl << '{' << std::endl;

  for (unsigned s = 0u; s < SM; ++s) {
    hpp << "  {";
    for (unsigned p = 0u; p < PM; ++p) {
      hpp << '{' << std::setw(w) << unsigned(AM[s][p][R]) << ',' << std::setw(w) << unsigned(AM[s][p][C]) << '}';
      if (p < (PM - 1u))
        hpp << ',';
    }
    hpp << '}';
    if (s < (SM - 1u))
      hpp << ',';
    hpp << std::endl;
  }

  hpp << "};" << std::endl;
  hpp.close();
  return EXIT_SUCCESS;
}
