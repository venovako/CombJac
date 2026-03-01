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

#include <cstdlib>
#include <fstream>
#include <iomanip>

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
#else
static const std::streamsize w = std::streamsize(10);
static const std::streamsize v = std::streamsize(10);
#endif /* ?M */

int main(int argc, char* argv[])
{
  const KN (*const tbl1)[2u] = (const KN (*)[2u])AN;
  KM (*const tbl2)[2u] = (KM (*)[2u])AM;

  /* (SW,NE); (SE,NW) */
  for (unsigned p = 0u; p < SN; ++p) {
    const unsigned q = (p << 1u);
    for (unsigned a = 0u; a < PN; ++a) {
      const unsigned b = (a << 1u);
      const unsigned k = (p * PN + a);
      const unsigned l = (q * PM + b);
      const unsigned m = ((q + 1u) * PM + b);
      const unsigned i = tbl1[k][0u];
      const unsigned j = tbl1[k][1u];
      const unsigned i2 = (i << 1u);
      const unsigned j2 = (j << 1u);
      /* SW */
      tbl2[l][0u] = (i2 + 1u);
      tbl2[l][1u] = j2;
      /* NE */
      tbl2[l + 1u][0u] = i2;
      tbl2[l + 1u][1u] = (j2 + 1u);
      /* SE */
      tbl2[m][0u] = (i2 + 1u);
      tbl2[m][1u] = (j2 + 1u);
      /* NW */
      tbl2[m + 1u][0u] = i2;
      tbl2[m + 1u][1u] = j2;
    }
  }
  /* super-diag */
  const unsigned d = ((M - 2u) * PM);
  for (unsigned i = 0u; i < PM; ++i) {
    const unsigned j = ((PM - 1u) - i);
    tbl2[d + j][1u] = ((tbl2[d + j][0u] = (i << 1u)) + 1u);
  }

  std::ofstream hpp(HM, (std::ios::out | std::ios::trunc));
  if (!hpp)
    return EXIT_FAILURE;
  hpp << KK << ' ' << AK << '[' << std::setw(v) << SM << "][" << std::setw(v) << PM << "][2] =" << std::endl << '{' << std::endl;

  for (unsigned s = 0u; s < SM; ++s) {
    hpp << "  {";
    for (unsigned p = 0u; p < PM; ++p) {
      hpp << '{' << std::setw(w) << unsigned(AM[s][p][0u]) << ',' << std::setw(w) << unsigned(AM[s][p][1u]) << '}';
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
