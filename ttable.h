#ifndef CHDL_TTABLE_H
#define CHDL_TTABLE_H

#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <random>
#include <string>

namespace chdl {

node TruthTable(const std::vector<node> &in, const char *tt,
                std::map<std::set<int>, node> &cache);

template <unsigned N>
node TruthTable(const bvec<N> &in, const char *tt,
                std::map<std::set<int>, node> &cache)
{
  using namespace std;

  vector<node> v(N);
  for (unsigned i = 0; i < N; ++i) v[i] = in[i];

  return TruthTable(v, tt, cache);
}

template <unsigned A>
node LLRom(const bvec<A> &a, const std::vector<bool> &contents,
            std::map<std::set<int>, node> &cache)
{
  char tt[1ul<<A];
  unsigned sz(contents.size() > (1ul<<A) ? (1ul<<A) : contents.size());

  for (unsigned i = 0; i < sz; ++i) {
    if (contents[i]) tt[i] = '1';
    else             tt[i] = '0';
  }

  for (unsigned i = sz; i < (1ul<<A); ++i) {
    tt[i] = 'x';
  }

  return TruthTable(a, tt, cache);
}

template <unsigned A, unsigned N, typename T>
  bvec<N> LLRom(const bvec<A> &a, const std::vector<T> &init) 
{
  using namespace std;

  bvec<N> q;
  map<set<int>, node> cache;

  for (unsigned i = 0; i < N; ++i) {
    vector<bool> tt;
    for (unsigned j = 0; j < init.size(); ++j)
      tt.push_back((init[j] >> i) & 1);
    q[i] = LLRom(a, tt, cache);
  }

  return q;  
}

template <unsigned A, unsigned N>
  bvec<N> LLRom(const bvec<A> &a, std::string filename)
{
  using namespace std;

  ifstream in(filename);
  vector<vector<bool> > contents(N);

  while (!!in) {
    vector<bool> row;
    char c;
    while (!!in && (c = in.get()) != '\n') {
      unsigned cval = (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : (c - '0');
      row.push_back(cval & 8);
      row.push_back(cval & 4);
      row.push_back(cval & 2);
      row.push_back(cval & 1);
    }

    if (!in) break;
    
    for (unsigned i = 0; i < N; ++i)
      contents[i].push_back(row[row.size() - 1 - i]);
  }

  map<set<int>, node> cache;

  bvec<N> out;
  for (unsigned i = 0; i < N; ++i)
    out[i] = LLRom(a, contents[i], cache);
  return out;
}

}

#endif
