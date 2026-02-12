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

typedef unsigned char uchar;
static uchar in_strat[E][2u];
typedef unsigned short ushort;
static ushort indep_sets[E_1][NCP], active_sets[E_1][NCP];
static ushort indep_cnts[E_1], active_cnts[E_1];
static ushort used_set[E], tmp_set[E_1], used_cnt;
typedef unsigned long long ullong;
static ullong strat, max_strat;
typedef long double ldouble;

static void make_in_strat(const bool diagne)
{
  if (!memset(in_strat, 0, sizeof(in_strat)))
    exit(EXIT_FAILURE);
  if (!memset(indep_sets, 0, sizeof(indep_sets)))
    exit(EXIT_FAILURE);
  if (!memset(active_sets, 0, sizeof(active_sets)))
    exit(EXIT_FAILURE);
  if (!memset(indep_cnts, 0, sizeof(indep_cnts)))
    exit(EXIT_FAILURE);
  if (!memset(active_cnts, 0, sizeof(active_cnts)))
    exit(EXIT_FAILURE);
  if (!memset(used_set, 0, sizeof(used_set)))
    exit(EXIT_FAILURE);
  if (!memset(tmp_set, 0, sizeof(tmp_set)))
    exit(EXIT_FAILURE);
  used_cnt = ushort(0u);
  strat = 0ull;

  ushort i = ushort(0u);
  if (diagne) {
    // diagonals: (1,N); (2,N),(1,N-1); (3,N),(2,N-1),(1,N-2); ...
    for (uchar r = uchar(0u); r < N_1; ++r) {
      for (uchar l = uchar(0u); l <= r; ++l) {
        in_strat[i][R] = r - l;
        in_strat[i][C] = N_1 - l;
        ++i;
      }
    }
  }
  else {
    // row-cyclic
    for (uchar r = uchar(0u); r < N_1; ++r) {
      for (uchar c = uchar(r + 1u); c < N; ++c) {
        in_strat[i][R] = r;
        in_strat[i][C] = c;
        ++i;
      }
    }
  }

  for (i = ushort(0u); i < E_1; ++i)
    for (ushort j = ushort(i + 1u); j < E; ++j)
      if ((in_strat[i][R] != in_strat[j][R]) && (in_strat[i][R] != in_strat[j][C]) && (in_strat[i][C] != in_strat[j][R]) && (in_strat[i][C] != in_strat[j][C]))
        indep_sets[i][indep_cnts[i]++] = j;
}

static void print_gv()
{
  std::ostringstream gv_filename;
  gv_filename << "diagne_" << N << ".gv";
  std::ofstream gv(gv_filename.str(), (std::ios::out | std::ios::trunc));
  if (!gv)
    exit(EXIT_FAILURE);

  gv << "strict graph diagne {" << std::endl << "\tmargin=0" << std::endl;
#ifndef GRAPHVIZ_SIZE
#define GRAPHVIZ_SIZE 4.25
#endif /* !GRAPHVIZ_SIZE */
  gv << "\tsize=" << GRAPHVIZ_SIZE << std::endl;
#ifndef GRAPHVIZ_FONTSIZE
#define GRAPHVIZ_FONTSIZE 96
#endif /* !GRAPHVIZ_FONTSIZE */
  gv << "\tnode [fontsize=" << GRAPHVIZ_FONTSIZE << ']' << std::endl;
#ifndef GRAPHVIZ_EDGECOLOR
#define GRAPHVIZ_EDGECOLOR "dimgray"
#endif /* !GRAPHVIZ_EDGECOLOR */
  gv << "\tedge [color=" << GRAPHVIZ_EDGECOLOR << ']' << std::endl;
  gv << "\t{" << std::endl;
  for (ushort i = ushort(0u); i < E; ++i)
    gv << "\t\t" << (i + IXBASE) << " [label=\"" << (i + IXBASE) << "=(" << (in_strat[i][R] + IXBASE) << ',' << (in_strat[i][C] + IXBASE) << ")\"]" << std::endl;
  gv << "\t}" << std::endl;
  for (ushort i = ushort(0u); i < E_1; ++i)
    for (ushort j = ushort(i + 1u); j < E; ++j)
      if ((in_strat[i][R] == in_strat[j][R]) || (in_strat[i][R] == in_strat[j][C]) || (in_strat[i][C] == in_strat[j][R]) || (in_strat[i][C] == in_strat[j][C]))
        gv << "\t" << (i + IXBASE) << " -- " << (j + IXBASE) << std::endl;
  gv << "}" << std::endl;
  gv.close();

  // use '-DGRAPHVIZ_PREFIX=""' if circo is in the PATH, or '-DGRAPHVIZ_PREFIX="graphviz_prefix/"' if it is not
#ifdef GRAPHVIZ_PREFIX
  std::ostringstream circo_call;
  circo_call << GRAPHVIZ_PREFIX << "circo -v -Tpdf -odiagne_" << N << ".pdf -x diagne_" << N << ".gv > diagne_" << N << ".log 2>&1";
  if (system(circo_call.str().c_str()))
    exit(EXIT_FAILURE);
#endif /* GRAPHVIZ_PREFIX */

  std::ostringstream txt_filename;
  txt_filename << "diagne_" << N << ".txt";
  std::ofstream txt(txt_filename.str(), (std::ios::out | std::ios::trunc));
  if (!txt)
    exit(EXIT_FAILURE);

  for (ushort i = ushort(0u); i < E_1; ++i) {
    txt << std::setw(maxw) << (i + IXBASE) << "@(" << std::setw(w) << (in_strat[i][R] + IXBASE) << ',' << std::setw(w) << (in_strat[i][C] + IXBASE) << ")[" << std::setw(maxw) << indep_cnts[i] << "]";
    for (ushort j = ushort(0u); j < indep_cnts[i]; ++j) {
      const ushort k = indep_sets[i][j];
      txt << ", " << std::setw(maxw) << (k + IXBASE) << "=(" << std::setw(w) << (in_strat[k][R] + IXBASE) << ',' << std::setw(w) << (in_strat[k][C] + IXBASE) << ')';
    }
    txt << std::endl;
  }
  txt.close();
}

static void print_hdr()
{
  std::ostringstream hdr_filename;
  hdr_filename << "diagne_" << N << '-' << strat << ".hdr";
  std::ofstream hdr(hdr_filename.str(), (std::ios::out | std::ios::trunc));
  if (!hdr)
    exit(EXIT_FAILURE);
  
  hdr << "unsigned char cDG" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "][2] =" << std::endl
      << '{' << std::endl;

  ushort i = ushort(0u);
  for (uchar s = uchar(0u); s < S; ++s) {
    hdr << "  {";
    for (uchar p = uchar(0u); p < P; ++p) {
      const ushort j = used_set[i++];
      hdr << '{' << std::setw(w) << ushort(in_strat[j][R]) << ',' << std::setw(w) << ushort(in_strat[j][C]) << '}';
      if (p < P_1)
        hdr << ',';
    }
    hdr << '}';
    if (s < S_1)
      hdr << ',';
    hdr << std::endl;
  }

  hdr << "};" << std::endl << std::endl;

  hdr << "unsigned char DGc" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "][2] =" << std::endl
      << '{' << std::endl;

  i = E;
  for (uchar s = S; s; ) {
    hdr << "  {";
    for (uchar p = P; p; ) {
      const ushort j = used_set[--i];
      hdr << '{' << std::setw(w) << ushort(in_strat[j][R]) << ',' << std::setw(w) << ushort(in_strat[j][C]) << '}';
      if (--p)
        hdr << ',';
    }
    hdr << '}';
    if (--s)
      hdr << ',';
    hdr << std::endl;
  }

  hdr << "};" << std::endl;
  hdr.close();
}

static void print_ftn()
{
  if (N >= 128u)
    return;

  std::ostringstream ftn_filename;
  ftn_filename << "diagne_" << N << '-' << strat << ".ftn";
  std::ofstream ftn(ftn_filename.str(), (std::ios::out | std::ios::trunc));
  if (!ftn)
    exit(EXIT_FAILURE);

  ftn << "integer(kind=int8), parameter :: cDG" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << "(2," << std::setw(3) << P << ',' << std::setw(3) << S << ") = reshape([&" << std::endl;

  ushort i = ushort(0u);
  for (uchar s = uchar(0u); s < S; ++s) {
    ftn << "     ";
    for (uchar p = uchar(0u); p < P; ++p) {
      const ushort j = used_set[i++];
      ftn << std::setw(w) << (in_strat[j][R] + IXBASE) << ',' << std::setw(w) << (in_strat[j][C] + IXBASE) << (((s < S_1) || (p < P_1)) ? ',' : ' ');
    }
    ftn << '&' << std::endl;
  }

  ftn << "], [2," << std::setw(3) << P << ',' << std::setw(3) << S << "])" << std::endl << std::endl;

  ftn << "integer(kind=int8), parameter :: DGc" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << "(2," << std::setw(3) << P << ',' << std::setw(3) << S << ") = reshape([&" << std::endl;

  i = E;
  for (uchar s = S; s; --s) {
    ftn << "     ";
    for (uchar p = P; p; --p) {
      const ushort j = used_set[--i];
      ftn << std::setw(w) << (in_strat[j][R] + IXBASE) << ',' << std::setw(w) << (in_strat[j][C] + IXBASE) << (((s > 1u) || (p > 1u)) ? ',' : ' ');
    }
    ftn << '&' << std::endl;
  }

  ftn << "], [2," << std::setw(3) << P << ',' << std::setw(3) << S << "])" << std::endl;
  ftn.close();
}

static void print_idx()
{
  std::ostringstream idx_filename;
  idx_filename << "diagne_" << N << '-' << strat << ".idx";
  std::ofstream idx(idx_filename.str(), (std::ios::out | std::ios::trunc));
  if (!idx)
    exit(EXIT_FAILURE);

  idx << "unsigned char iDG" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "] =" << std::endl
      << '{' << std::endl;

  ushort i = ushort(0u);
  for (uchar s = uchar(0u); s < S; ++s) {
    idx << "  {";
    for (uchar p = uchar(0u); p < P; ++p) {
      idx << std::setw(maxw) << used_set[i];
      if (p < P_1)
        idx << ',';
      ++i;
    }
    idx << '}';
    if (s < S_1)
      idx << ',';
    idx << std::endl;
  }

  idx << "};" << std::endl << std::endl;

  idx << "unsigned char DGi" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "] =" << std::endl
      << '{' << std::endl;

  i = E;
  for (uchar s = S; s; ) {
    idx << "  {";
    for (uchar p = P; p; ) {
      idx << std::setw(maxw) << used_set[--i];
      if (--p)
        idx << ',';
    }
    idx << '}';
    if (--s)
      idx << ',';
    idx << std::endl;
  }

  idx << "};" << std::endl;
  idx.close();
}

static void print_asy()
{
  static const ldouble zero = 0.0L;
  static const ldouble one = 1.0L;
  static const ldouble one_n = one / ldouble(N);
  static const ldouble one_2n = one / ldouble(N << 1u);
#ifndef ASYMPTOTE_FONTSIZE
#define ASYMPTOTE_FONTSIZE 10u
#endif /* !ASYMPTOTE_FONTSIZE */
  static const ushort size_pt = N * ((ASYMPTOTE_FONTSIZE * 3u) >> 1u);

  // draw images
  ushort i = ushort(0u);
  for (uchar s = uchar(0u); s <= S; ++s) {
    std::ostringstream asy_filename;
    asy_filename << "diagne_" << N << '-' << strat << '_' << std::setfill('0') << std::setw(w) << (s + IXBASE) << std::setfill(' ') << ".asy";
    std::ofstream asy(asy_filename.str(), (std::ios_base::out | std::ios_base::trunc));
    if (!asy)
      exit(EXIT_FAILURE);

    asy << std::fixed << std::setprecision(17);

    // header
    if (unsigned(ASYMPTOTE_FONTSIZE) != 12u)
      asy << "import fontsize;" << std::endl << "defaultpen(fontsize(" << ASYMPTOTE_FONTSIZE << "pt));" << std::endl;
    if (s < S)
      asy << "texpreamble(\"\\usepackage[fixed]{fontawesome5}\");" << std::endl;
    asy << std::endl;
    asy << "size(" << size_pt << "pt);" << std::endl;
    asy << std::endl;
    asy << "draw(unitsquare);" << std::endl;
    asy << std::endl;

    // y isolines
    for (uchar j = N_1; j; --j) {
      const ldouble y = j * one_n;
      asy << "draw((" << zero << ',' << y << ")--(" << one << ',' << y << "));" << std::endl;
    }
    asy << std::endl;

    // x isolines
    for (uchar j = N_1; j; --j) {
      const ldouble x = j * one_n;
      asy << "draw((" << x << ',' << zero << ")--(" << x << ',' << one << "));" << std::endl;
    }
    asy << std::endl;

    // diagonal boxes
    // D---C
    // |   |
    // A---B
    for (uchar j = uchar(0u); j < N; ++j) {
      const uchar j1 = uchar(j + 1u);
      const uchar n_j1 = uchar(N - j1);
      const ldouble a_x = one_n * j;
      const ldouble a_y = one_n * n_j1;
      const ldouble b_x = a_x + one_n;
      const ldouble b_y = a_y;
      const ldouble c_x = b_x;
      const ldouble c_y = b_y + one_n;
      const ldouble d_x = a_x;
      const ldouble d_y = c_y;
      const ldouble l_x = a_x + one_2n;
      const ldouble l_y = a_y + one_2n;
      asy << "filldraw((" <<
        a_x << ',' << a_y << ")--(" <<
        b_x << ',' << b_y << ")--(" <<
        c_x << ',' << c_y << ")--(" <<
        d_x << ',' << d_y << ")--cycle,mediumgray);" << std::endl;
      asy << "label(\"\\boldmath$" << (j + IXBASE) << "$\",(" << l_x << ',' << l_y << "),white);" << std::endl;
    }

    if (s < S) {
      // step positions
      for (uchar j = uchar(0u); j < P; ++j) {
        const ushort k = used_set[i++];
        const ldouble p = ldouble(in_strat[k][R]);
        const ldouble q = ldouble(in_strat[k][C]);
        ldouble l_x = __builtin_fmal(one_n, q, one_2n);
        ldouble l_y = one - __builtin_fmal(one_n, p, one_2n);
        asy << std::endl;
        asy << "label(\"\\faChessRook\",(" << l_x << ',' << l_y << "));" << std::endl;
        l_x = __builtin_fmal(one_n, p, one_2n);
        l_y = one - __builtin_fmal(one_n, q, one_2n);
        asy << "label(\"\\faChessRook\",(" << l_x << ',' << l_y << "),lightgray);" << std::endl;
      }
    }
    else {
      i = ushort(0u);
      for (uchar j = uchar(0u); j < S; ++j) {
        for (uchar k = uchar(0u); k < P; ++k) {
          const ushort l = used_set[i++];
          const ldouble p = ldouble(in_strat[l][R]);
          const ldouble q = ldouble(in_strat[l][C]);
          ldouble l_x = __builtin_fmal(one_n, q, one_2n);
          ldouble l_y = one - __builtin_fmal(one_n, p, one_2n);
          asy << std::endl;
          asy << "label(\"$" << (j + IXBASE) << "$\",(" << l_x << ',' << l_y << "));" << std::endl;
          l_x = __builtin_fmal(one_n, p, one_2n);
          l_y = one - __builtin_fmal(one_n, q, one_2n);
          asy << "label(\"$" << (j + IXBASE) << "$\",(" << l_x << ',' << l_y << "),lightgray);" << std::endl;
        }
      }
    }

    asy.close();
    // use '-DASYMPTOTE_PREFIX=""' if asy is in the PATH, or '-DASYMPTOTE_PREFIX="asymptote_prefix/"' if it is not
#ifdef ASYMPTOTE_PREFIX
    std::ostringstream asy_call;
    asy_call << ASYMPTOTE_PREFIX << "asy -v -nobatchView -f pdf -tex pdflatex " << asy_filename.str() << " >> diagne_" << N << '-' << strat << ".log 2>&1";
    if (system(asy_call.str().c_str()))
      exit(EXIT_FAILURE);
#endif /* ASYMPTOTE_PREFIX */
  }

  // use '-DPDFTK_PREFIX=""' if pdftk is in the PATH, or '-DPDFTK_PREFIX="pdftk_prefix/"' if it is not
#ifdef ASYMPTOTE_PREFIX
#ifdef PDFTK_PREFIX
  std::ostringstream pdftk_call;
  pdftk_call << PDFTK_PREFIX << "pdftk";
  for (uchar s = uchar(0u); s <= S; ++s)
    pdftk_call << " diagne_" << N << '-' << strat << '_' << std::setfill('0') << std::setw(w) << (s + IXBASE) << std::setfill(' ') << ".pdf";
  pdftk_call << " cat output diagne_" << N << '-' << strat << ".pdf verbose dont_ask >> diagne_" << N << '-' << strat << ".log 2>&1";
  if (system(pdftk_call.str().c_str()))
    exit(EXIT_FAILURE);
#endif /* PDFTK_PREFIX */
#endif /* ASYMPTOTE_PREFIX */
}

static bool next_pivot()
{
  // the index of the current step
  const ushort six = ushort(used_cnt / P);
  // the pivot index within the current step
  const ushort pix = ushort(used_cnt % P);
  if (pix) { // not a head of the step
    ushort i = ushort(used_cnt - 1u);
    // the previous pivot's index
    const ushort prev_ix = used_set[i];
    const ushort prev_cnt = active_cnts[prev_ix];
    // how many more pivots to complete the step, not counting the one to be tried here
    const ushort needed = ushort(P_1 - pix);
    if (needed) { // not the last pivot in the step
      const ushort *const prev_end = &(active_sets[prev_ix][prev_cnt]);
      for (i = ushort(0u); i < prev_cnt; /**/) {
        // the pivot index to try in this iteration
        const ushort my_ix = active_sets[prev_ix][i];
        // i := i+1 = i', loop if not enough pivots remaining
        if ((prev_cnt - ++i) < needed)
          continue;
        const ushort mync_cnt = indep_cnts[my_ix];
        if (mync_cnt < needed)
          continue;
        // the tail of the previous active set, beyond my_ix
        const ushort *const prev_beg = &(active_sets[prev_ix][i]);
        // this pivot's partial independent set
        const ushort *const mync_beg = indep_sets[my_ix];
        const ushort *const mync_end = &(indep_sets[my_ix][mync_cnt]);
        // this pivot's active set to be built
        ushort *const my_dst = active_sets[my_ix];
        // active_cnts[my_ix] <= min(mync_cnt, prev_cnt-i')
        active_cnts[my_ix] = ushort(std::set_intersection(prev_beg, prev_end, mync_beg, mync_end, my_dst) - my_dst);
        // try my_ix if there are enough pivots available in the new active set
        if (active_cnts[my_ix] >= needed) {
          used_set[used_cnt++] = my_ix;
          if (next_pivot())
            return true;
          --used_cnt;
        }
      }
    }
    else { // the last pivot in the step
      for (i = ushort(0u); i < prev_cnt; ++i) {
        const ushort my_ix = active_sets[prev_ix][i];
        used_set[used_cnt++] = my_ix;
        if (next_pivot())
          return true;
        --used_cnt;
      }
    }
  }
  else if (used_cnt == E) {
    print_hdr();
    print_ftn();
    print_idx();
    print_asy();
    return (++strat == max_strat);
  }
  else { // new step head
    ushort i = ushort(0u);
    if (six) { // not the initial step, i.e., used_cnt > 0
      std::copy(used_set, used_set + used_cnt, tmp_set);
      std::sort(tmp_set, tmp_set + used_cnt);
      for (ushort j = ushort(0u); j < used_cnt; ++j) {
        if (i < tmp_set[j])
          break;
        if (i == tmp_set[j])
          ++i;
      }
    }
    if (indep_cnts[i] < P_1)
      return false;
    ushort *const dst = active_sets[i];
    // indep_sets[i] - used_set
    active_cnts[i] = ushort(std::set_difference(indep_sets[i], &(indep_sets[i][indep_cnts[i]]), tmp_set, tmp_set + used_cnt, dst) - dst);
    if (active_cnts[i] < P_1)
      return false;
    used_set[used_cnt++] = i;
    if (next_pivot())
      return true;
    --used_cnt;
  }
  return false;
}

int main(int argc, char *argv[])
{
  if ((argc < 1) || (argc > 2)) {
    std::cerr << argv[0] << " [max_strat]" << std::endl;
    return EXIT_FAILURE;
  }
  const long long m = ((argc == 1) ? 0ll : atoll(argv[1]));
  max_strat = ullong((m < 0ll) ? -(m + 1ll) : m);
  make_in_strat(m >= 0ll);
  print_gv();
  return (next_pivot() ? EXIT_SUCCESS : EXIT_FAILURE);
}
