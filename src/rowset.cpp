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
#include <chrono>
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

#if (!defined(N))
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

typedef unsigned char uchar;
static struct pivot { uchar r, c; } in_strat[E];
typedef unsigned short ushort;
static ushort indep_sets[E_1][NCP], active_sets[E_1][NCP];
static ushort indep_cnts[E_1], active_cnts[E_1];
static ushort used_set[E], tmp_set[E], used_cnt, max_used_cnt;
typedef unsigned long long ullong;
static ullong btrack;
typedef long double ldouble;

static bool make_in_strat()
{
#ifndef NDEBUG
  std::cerr << "Clearing memory... " << std::flush;
#endif /* !NDEBUG */
  if (!memset(in_strat, 0, sizeof(in_strat)))
    return false;
  if (!memset(indep_sets, 0, sizeof(indep_sets)))
    return false;
  if (!memset(active_sets, 0, sizeof(active_sets)))
    return false;
  if (!memset(indep_cnts, 0, sizeof(indep_cnts)))
    return false;
  if (!memset(active_cnts, 0, sizeof(active_cnts)))
    return false;
  if (!memset(used_set, 0, sizeof(used_set)))
    return false;
  if (!memset(tmp_set, 0, sizeof(tmp_set)))
    return false;
  max_used_cnt = used_cnt = 0u;
  btrack = 0ull;
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

#ifndef NDEBUG
  std::cerr << "Generating pivots... " << std::flush;
#endif /* !NDEBUG */
  ushort i = 0u;
  for (uchar r = 0u; r < N_1; ++r) {
    for (uchar c = r + 1u; c < N; ++c) {
      pivot &pvt = in_strat[i++];
      pvt.r = r;
      pvt.c = c;
    }
  }
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

#ifndef NDEBUG
  std::cerr << "Building forward-looking independent sets... " << std::flush;
#endif /* !NDEBUG */
  for (i = 0u; i < E_1; ++i)
    for (ushort j = (i + 1u); j < E; ++j)
      if ((in_strat[i].r != in_strat[j].r) && (in_strat[i].r != in_strat[j].c) && (in_strat[i].c != in_strat[j].r) && (in_strat[i].c != in_strat[j].c))
        indep_sets[i][indep_cnts[i]++] = j;
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

#ifndef NDEBUG
  std::cerr << "Asserting monotonically non-increasing cardinalities... " << std::flush;
#endif /* !NDEBUG */
  for (i = 1u; i < E_1; ++i)
    if (indep_cnts[i] > indep_cnts[i - 1u]) {
#ifndef NDEBUG
      std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
      return false;
    }
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

  return true;
}

static bool print_gv()
{
#ifndef NDEBUG
  std::cerr << "Describing the graph as a Graphviz file... " << std::flush;
#endif /* !NDEBUG */
  std::ostringstream gv_filename;
  gv_filename << "rowset_" << N << ".gv";
  std::ofstream gv(gv_filename.str(), (std::ios::out | std::ios::trunc));
  if (!gv) {
#ifndef NDEBUG
    std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
    return false;
  }
  gv << "strict graph rowset {" << std::endl << "\tmargin=0" << std::endl;
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
#ifndef GRAPHVIZ_IXBASE
#define GRAPHVIZ_IXBASE (ushort)1u
#endif /* !GRAPHVIZ_IXBASE */
  for (ushort i = 0u; i < E; ++i) {
    pivot &pvt = in_strat[i];
    gv << "\t\t" << (i + GRAPHVIZ_IXBASE) << " [label=\"" << (i + GRAPHVIZ_IXBASE) << "=(" << (pvt.r + GRAPHVIZ_IXBASE)  << ',' << (pvt.c + GRAPHVIZ_IXBASE) << ")\"]" << std::endl;
  }
  gv << "\t}" << std::endl;
  for (ushort i = 0u; i < E_1; ++i)
    for (ushort j = (i + 1u); j < E; ++j)
      if ((in_strat[i].r == in_strat[j].r) || (in_strat[i].r == in_strat[j].c) || (in_strat[i].c == in_strat[j].r) || (in_strat[i].c == in_strat[j].c))
        gv << "\t" << (i + GRAPHVIZ_IXBASE) << " -- " << (j + GRAPHVIZ_IXBASE) << std::endl;
  gv << "}" << std::endl;
  gv.close();
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

// use '-DGRAPHVIZ_PREFIX=""' if circo is in the PATH, or '-DGRAPHVIZ_PREFIX="graphviz_prefix/"' if it is not
#ifdef GRAPHVIZ_PREFIX
#ifndef NDEBUG
  std::cerr << "Plotting the graph... " << std::flush;
#endif /* !NDEBUG */
  std::ostringstream circo_call;
  circo_call << GRAPHVIZ_PREFIX << "circo -v -Tpdf -orowset_" << N << ".pdf -x rowset_" << N << ".gv > rowset_" << N << ".log 2>&1";
  if (system(circo_call.str().c_str())) {
#ifndef NDEBUG
    std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
    return false;
  }
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */
#endif /* GRAPHVIZ_PREFIX */

#ifndef NDEBUG
  std::cerr << "Storing the forward-looking independent sets... " << std::flush;
#endif /* !NDEBUG */
  std::ostringstream txt_filename;
  txt_filename << "rowset_" << N << ".txt";
  std::ofstream txt(txt_filename.str(), (std::ios::out | std::ios::trunc));
  if (!txt) {
#ifndef NDEBUG
    std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
    return false;
  }
  for (ushort i = 0u; i < E_1; ++i) {
    txt << std::setw(maxw) << (i + GRAPHVIZ_IXBASE) << "@(" << std::setw(w) << (in_strat[i].r + GRAPHVIZ_IXBASE) << ',' << std::setw(w) << (in_strat[i].c + GRAPHVIZ_IXBASE) << ")[" << std::setw(maxw) << indep_cnts[i] << "]";
    for (ushort j = 0u; j < indep_cnts[i]; ++j) {
      const ushort k = indep_sets[i][j];
      txt << ", " << std::setw(maxw) << (k + GRAPHVIZ_IXBASE) << "=(" << std::setw(w) << (in_strat[k].r + GRAPHVIZ_IXBASE) << ',' << std::setw(w) << (in_strat[k].c + GRAPHVIZ_IXBASE) << ')';
    }
    txt << std::endl;
  }
  txt.close();
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* ?NDEBUG */
  return true;
}

static bool next_pivot()
{
#ifndef NDEBUG
  if (max_used_cnt < used_cnt) {
    max_used_cnt = used_cnt;
    std::cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << std::flush
              << "Used " << std::setw(maxw) << max_used_cnt << " out of " <<  std::setw(maxw) << E << " pivots..."
              << std::flush;
  }
#endif /* !NDEBUG */
  const div_t qr = div(int(used_cnt), int(P));
  if (qr.rem) {
    const ushort prev_ix = used_set[used_cnt - 1u];
    const ushort needed = P_1 - ushort(qr.rem);
    if (needed) {
      const ushort P_pix = (P - ushort(qr.rem)) << 1u;
      const ushort prev_cnt = active_cnts[prev_ix];
      ushort j, prev_cnt_ = prev_cnt - needed;

      for (j = 0u; j < prev_cnt_; ++j) {
        const ushort my_ix = active_sets[prev_ix][j];
        if ((N - in_strat[my_ix].r) < P_pix)
          break;
        const ushort mync_cnt = indep_cnts[my_ix];
        if (mync_cnt < needed)
          break; // only if |indep_cnts[j]| monotonically non-increasing w.r.t. j, else continue;
      }
      if (j < prev_cnt_)
        prev_cnt_ = j;

      const ushort *const prev_end = &(active_sets[prev_ix][prev_cnt]);
      for (ushort i = 0u; i < prev_cnt_; ++i) {
        const ushort my_ix = active_sets[prev_ix][i];
        const ushort mync_cnt = indep_cnts[my_ix];
        const ushort *const prev_beg = &(active_sets[prev_ix][i + 1u]);
        const ushort *const mync_beg = &(indep_sets[my_ix][0u]);
        const ushort *const mync_end = &(indep_sets[my_ix][mync_cnt]);
        ushort *const my_dst = &(active_sets[my_ix][0u]);
        active_cnts[my_ix] = ushort(std::set_intersection(prev_beg, prev_end, mync_beg, mync_end, my_dst) - my_dst);
        if (active_cnts[my_ix] >= needed) {
          used_set[used_cnt++] = my_ix;
          if (next_pivot())
            return true;
          --used_cnt;
          ++btrack;
        }
        else
          ++btrack;
      }
    }
    else {
      used_set[used_cnt++] = active_sets[prev_ix][0u];
      if (next_pivot())
        return true;
      --used_cnt;
      ++btrack;
    }
  }
  else if (used_cnt == E)
    return true;
  else { // new step head
    const ushort ix = ushort(qr.quot);
    if (ix) {
      std::copy(used_set, used_set + used_cnt, tmp_set);
      std::sort(tmp_set, tmp_set + used_cnt);
    }
    if (indep_cnts[ix] < P_1)
      return false;
    ushort *const dst = &(active_sets[ix][0u]);
    if ((active_cnts[ix] = ushort(std::set_difference(&(indep_sets[ix][0u]), &(indep_sets[ix][indep_cnts[ix]]), tmp_set, tmp_set + used_cnt, dst) - dst)) < P_1)
      return false;
    used_set[used_cnt++] = ix;
    if (next_pivot())
      return true;
    --used_cnt;
    ++btrack;
  }
  return false;
}

static bool print_hdr()
{
#ifndef NDEBUG
  std::cerr << "Storing the strategy tables... " << std::flush;
#endif /* !NDEBUG */

  std::ostringstream hdr_filename;
  hdr_filename << "rowset_" << N << ".hdr";
  std::ofstream hdr(hdr_filename.str(), (std::ios::out | std::ios::trunc));
  if (!hdr) {
#ifndef NDEBUG
    std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
    return false;
  }
  
  hdr << "unsigned char cRC" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "][2] =" << std::endl
      << '{' << std::endl;

  ushort i = 0u;
  for (uchar s = 0u; s < S; ++s) {
    hdr << "  {";
    for (uchar p = 0u; p < P; ++p) {
      const pivot &pvt = in_strat[used_set[i]];
      hdr << '{' << std::setw(w) << ushort(pvt.r) << ',' << std::setw(w) << ushort(pvt.c) << '}';
      if (p < P_1)
        hdr << ',';
      ++i;
    }
    hdr << '}';
    if (s < S_1)
      hdr << ',';
    hdr << std::endl;
  }

  hdr << "};" << std::endl << std::endl;

  hdr << "unsigned char RCc" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "][2] =" << std::endl
      << '{' << std::endl;

  i = E;
  for (uchar s = S; s; ) {
    hdr << "  {";
    for (uchar p = P; p; ) {
      const pivot &pvt = in_strat[used_set[--i]];
      hdr << '{' << std::setw(w) << ushort(pvt.r) << ',' << std::setw(w) << ushort(pvt.c) << '}';
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

#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */
  return true;
}

static bool print_idx()
{
#ifndef NDEBUG
  std::cerr << "Storing the index tables... " << std::flush;
#endif /* !NDEBUG */

  std::ostringstream idx_filename;
  idx_filename << "rowset_" << N << ".idx";
  std::ofstream idx(idx_filename.str(), (std::ios::out | std::ios::trunc));
  if (!idx) {
#ifndef NDEBUG
    std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
    return false;
  }

  idx << "unsigned char iRC" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
      << '[' << std::setw(3) << S << "][" << std::setw(3) << P << "] =" << std::endl
      << '{' << std::endl;

  ushort i = 0u;
  for (uchar s = 0u; s < S; ++s) {
    idx << "  {";
    for (uchar p = 0u; p < P; ++p) {
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

  idx << "unsigned char RCi" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
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

#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */
  return true;
}

static bool print_asy()
{
  static const ldouble zero = 0.0L;
  static const ldouble one = 1.0L;
  static const ldouble one_n = one / ldouble(N);
  static const ldouble one_2n = one / ldouble(N << 1u);
#ifndef ASYMPTOTE_FONTSIZE
#define ASYMPTOTE_FONTSIZE 10u
#endif /* !ASYMPTOTE_FONTSIZE */
  static const ushort size_pt = N * ((ASYMPTOTE_FONTSIZE * 3u) >> 1u);

#ifndef NDEBUG
  std::cerr << "Preparing the Asymptote visualization... " << std::flush;
#endif /* !NDEBUG */

  // draw images
  ushort i = 0u;
  for (uchar s = 0u; s <= S; ++s) {
    std::ostringstream asy_filename;
    asy_filename << "rowset_" << N << '-' << std::setfill('0') << std::setw(w) << (s + GRAPHVIZ_IXBASE) << std::setfill(' ') << ".asy";
    std::ofstream asy(asy_filename.str(), (std::ios_base::out | std::ios_base::trunc));
    if (!asy) {
#ifndef NDEBUG
      std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
      return false;
    }

    asy << std::fixed << std::setprecision(17);

    // header
    if (ASYMPTOTE_FONTSIZE != 12u)
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
    for (uchar j = 0u; j < N; ++j) {
      const uchar j1 = j + 1u;
      const uchar n_j1 = N - j1;
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
      asy << "label(\"\\boldmath$" << (j + GRAPHVIZ_IXBASE) << "$\",(" << l_x << ',' << l_y << "),white);" << std::endl;
    }

    if (s < S) {
      // step positions
      for (uchar j = 0u; j < P; ++j) {
        const pivot &pvt = in_strat[used_set[i++]];
        const uchar p = pvt.r;
        const uchar q = pvt.c;
        ldouble l_x = __builtin_fmal(one_n, ldouble(q), one_2n);
        ldouble l_y = one - __builtin_fmal(one_n, ldouble(p), one_2n);
        asy << std::endl;
        asy << "label(\"\\faChessRook\",(" << l_x << ',' << l_y << "));" << std::endl;
        l_x = __builtin_fmal(one_n, ldouble(p), one_2n);
        l_y = one - __builtin_fmal(one_n, ldouble(q), one_2n);
        asy << "label(\"\\faChessRook\",(" << l_x << ',' << l_y << "),lightgray);" << std::endl;
      }
    }
    else {
      i = 0u;
      for (uchar j = 0u; j < S; ++j) {
        for (uchar k = 0u; k < P; ++k) {
          const pivot &pvt = in_strat[used_set[i++]];
          const uchar p = pvt.r;
          const uchar q = pvt.c;
          ldouble l_x = __builtin_fmal(one_n, ldouble(q), one_2n);
          ldouble l_y = one - __builtin_fmal(one_n, ldouble(p), one_2n);
          asy << std::endl;
          asy << "label(\"$" << (j + GRAPHVIZ_IXBASE) << "$\",(" << l_x << ',' << l_y << "));" << std::endl;
          l_x = __builtin_fmal(one_n, ldouble(p), one_2n);
          l_y = one - __builtin_fmal(one_n, ldouble(q), one_2n);
          asy << "label(\"$" << (j + GRAPHVIZ_IXBASE) << "$\",(" << l_x << ',' << l_y << "),lightgray);" << std::endl;
        }
      }
    }

    asy.close();
// use '-DASYMPTOTE_PREFIX=""' if asy is in the PATH, or '-DASYMPTOTE_PREFIX="asymptote_prefix/"' if it is not
#ifdef ASYMPTOTE_PREFIX
    std::ostringstream asy_call;
    asy_call << ASYMPTOTE_PREFIX << "asy -v -nobatchView -f pdf -tex pdflatex " << asy_filename.str() << " >> rowset_" << N << ".log 2>&1";
    if (system(asy_call.str().c_str())) {
#ifndef NDEBUG
      std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
      return false;
    }
#endif /* ASYMPTOTE_PREFIX */
  }

// use '-DPDFTK_PREFIX=""' if pdftk is in the PATH, or '-DPDFTK_PREFIX="pdftk_prefix/"' if it is not
#ifdef ASYMPTOTE_PREFIX
#ifdef PDFTK_PREFIX
  std::ostringstream pdftk_call;
  pdftk_call << PDFTK_PREFIX << "pdftk";
  for (uchar s = 0u; s <= S; ++s)
    pdftk_call << " rowset_" << N << '-' << std::setfill('0') << std::setw(w) << (s + GRAPHVIZ_IXBASE) << std::setfill(' ') << ".pdf";
  pdftk_call << " cat output rowset-" << N << ".pdf verbose dont_ask >> rowset_" << N << ".log 2>&1";
  if (system(pdftk_call.str().c_str())) {
#ifndef NDEBUG
    std::cerr << "error!" << std::endl;
#endif /* !NDEBUG */
    return false;
  }
#endif /* PDFTK_PREFIX */
#endif /* ASYMPTOTE_PREFIX */

#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */
  return true;
}

int main(int argc, char *argv[])
{
  if (argc != 1) {
    std::cerr << argv[0] << std::endl;
    return EXIT_FAILURE;
  }

  const auto start0 = std::chrono::high_resolution_clock::now();
  if (!make_in_strat())
    return 2;
  const auto end0 = std::chrono::high_resolution_clock::now();
  if (!print_gv())
    return 3;

#ifndef NDEBUG
  std::cerr << "                                 " << std::flush;
#endif /* !NDEBUG */
  const auto start1 = std::chrono::high_resolution_clock::now();
  const bool ok = next_pivot();
  const auto end1 = std::chrono::high_resolution_clock::now();
#ifndef NDEBUG
  std::cerr << (ok ? " done." : " error!") << std::endl;
#endif /* !NDEBUG */
  if (!ok)
    return 4;

#ifndef NDEBUG
  std::cout << "# of failed attempts = ";
#endif /* !NDEBUG */
  std::cout << btrack;
#ifdef NDEBUG
  std::cout << ", ";
#else /* !NDEBUG */
  std::cout << " in ";
#endif /* ?NDEBUG */
  const auto ms = (std::chrono::duration_cast<std::chrono::milliseconds>((end0 - start0) + (end1 - start1))).count();
  std::cout << ms;
#ifdef NDEBUG
  std::cout << ", " << std::fixed << std::setprecision(9) << (ms ? __builtin_log10l(ldouble(ms) / 1000.0L) : -3.0L);
#else /* !NDEBUG */
  std::cout << " ms";
#endif /* ?NDEBUG */
  std::cout << std::endl;

  if (!print_hdr())
    return 5;
  if (!print_idx())
    return 6;
  if (!print_asy())
    return 7;

  return EXIT_SUCCESS;
}
