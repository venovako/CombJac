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

static void make_in_strat()
{
#ifndef NDEBUG
  std::cerr << "Clearing memory... " << std::flush;
#endif /* !NDEBUG */
  (void)memset(in_strat, 0, sizeof(in_strat));
  (void)memset(indep_sets, 0, sizeof(indep_sets));
  (void)memset(active_sets, 0, sizeof(active_sets));
  (void)memset(indep_cnts, 0, sizeof(indep_cnts));
  (void)memset(active_cnts, 0, sizeof(active_cnts));
  (void)memset(used_set, 0, sizeof(used_set));
  (void)memset(tmp_set, 0, sizeof(tmp_set));
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
  std::cerr << "Describing the graph as a Graphviz file... " << std::flush;
#endif /* !NDEBUG */
  std::ostringstream gv_filename;
  gv_filename << "rowset_" << N << ".gv";
  std::ofstream gv(gv_filename.str(), (std::ios::out | std::ios::trunc));
  gv << "strict graph rowset {" << std::endl;
#ifdef GRAPHVIZ_SIZE
  gv << "\tsize=" << GRAPHVIZ_SIZE << std::endl;
#else /* !GRAPHVIZ_SIZE */
  // TODO: gv << "\toverlap=scale" << std::endl;
#endif /* ?GRAPHVIZ_SIZE */
  gv << "\tedge [color=gray]" << std::endl;
  gv << "\t{" << std::endl;
  for (ushort j = 0u; j < i; ++j) {
    pivot &pvt = in_strat[j];
    gv << "\t\t" << j << " [label=\"" << j << "=(" << ushort(pvt.r) << ',' << ushort(pvt.c) << ")\"]" << std::endl;
  }
  gv << "\t}" << std::endl;
  for (ushort j = 0u; j < i; ++j)
    for (ushort k = (j + 1u); k < i; ++k)
      if ((in_strat[j].r == in_strat[k].r) || (in_strat[j].r == in_strat[k].c) || (in_strat[j].c == in_strat[k].r) || (in_strat[j].c == in_strat[k].c))
        gv << "\t" << j << " -- " << k << std::endl;
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
  i = (ushort)system(circo_call.str().c_str());
#ifndef NDEBUG
  std::cerr << (i ? "error!" : "done.") << std::endl;
#endif /* !NDEBUG */
#endif /* GRAPHVIZ_PREFIX */

#ifndef NDEBUG
  std::cerr << "Building forward-looking independent sets... " << std::flush;
#endif /* !NDEBUG */
  for (i = 0u; i < E_1; ++i)
    for (ushort j = i + 1u; j < E; ++j)
      if ((in_strat[i].r != in_strat[j].r) && (in_strat[i].r != in_strat[j].c) && (in_strat[i].c != in_strat[j].r) && (in_strat[i].c != in_strat[j].c))
        indep_sets[i][indep_cnts[i]++] = j;
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

#ifndef NDEBUG
  std::cerr << "Asserting monotonically non-increasing cardinalities... " << std::flush;
#endif /* !NDEBUG */
  for (i = 1u; i < E_1; ++i)
    if (indep_cnts[i] > indep_cnts[i - 1u])
      exit(EXIT_FAILURE);
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* !NDEBUG */

#ifndef NDEBUG
  std::cerr << "Storing the forward-looking independent sets... " << std::flush;
#endif /* !NDEBUG */
  std::ostringstream txt_filename;
  txt_filename << "rowset_" << N << ".txt";
  std::ofstream txt(txt_filename.str(), (std::ios::out | std::ios::trunc));
  for (i = 0u; i < E_1; ++i) {
    txt << std::setw(maxw) << i << "@(" << std::setw(w) << ushort(in_strat[i].r) << ',' << std::setw(w) << ushort(in_strat[i].c) << ")[" << std::setw(maxw) << indep_cnts[i] << "]";
    for (ushort j = 0u; j < indep_cnts[i]; ++j) {
      const ushort k = indep_sets[i][j];
      txt << ", " << std::setw(maxw) << k << "@(" << std::setw(w) << ushort(in_strat[k].r) << ',' << std::setw(w) << ushort(in_strat[k].c) << ')';
    }
    txt << std::endl;
  }
  txt.close();
#ifndef NDEBUG
  std::cerr << "done." << std::endl;
#endif /* ?NDEBUG */
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

static void print_hdr()
{
  std::ostringstream hdr_filename;
  hdr_filename << "rowset_" << N << ".hdr";
  std::ofstream hdr(hdr_filename.str(), (std::ios::out | std::ios::trunc));
  
  hdr << "unsigned char rowcyc" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
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

  hdr << "};" << std::endl;
  hdr.close();
}

static void print_idx()
{
  std::ostringstream idx_filename;
  idx_filename << "rowset_" << N << ".idx";
  std::ofstream idx(idx_filename.str(), (std::ios::out | std::ios::trunc));
 
  idx << "unsigned char rowidx" << std::setfill('0') << std::setw(3) << N << std::setfill(' ')
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

  idx << "};" << std::endl;
  idx.close();
}

int main(int argc, char *argv[])
{
  if (argc != 1) {
    std::cerr << argv[0] << std::endl;
    return EXIT_FAILURE;
  }

  make_in_strat();

#ifndef NDEBUG
  std::cerr << "                                 " << std::flush;
#endif /* !NDEBUG */
  const bool ok = next_pivot();
#ifndef NDEBUG
  std::cerr << (ok ? " done." : " error!") << std::endl;
#endif /* !NDEBUG */
  if (!ok)
    return EXIT_FAILURE;

  print_hdr();
  print_idx();
  std::cout << "# of failed attempts = " << btrack << std::endl;
  return EXIT_SUCCESS;
}
