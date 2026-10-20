// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_RadixSort_H
#define GledCore_RadixSort_H

#include <bit>
#include <numeric>
#include <type_traits>
#include <vector>

namespace gled {

// Stable LSB radix sort giving ranks; from mkFit's radix_sort, after Opcode's (Pierre Terdiman).
template <typename V, typename R>
class RadixSort
{
public:
  static_assert(std::is_same<V, unsigned short>() || std::is_same<V, unsigned int>());
  // Ranks are 32-bit: 16-bit ones would wrap above 65535 values.
  static_assert(std::is_same<R, unsigned int>());
  // The passes take the bytes of each value least significant first.
  static_assert(std::endian::native == std::endian::little);

  typedef unsigned char ubyte_t;
  static constexpr R c_NBytes = sizeof(V);

  // Fills ranks with indices into values, in ascending order of values.
  void Sort(const std::vector<V>& values, std::vector<R>& ranks)
  {
    if (values.empty()) { ranks.clear(); return; }
    R histos[c_NBytes * 256] = { 0 };
    histo_loop(values, histos);
    radix_loop(values, histos, ranks);
  }

private:
  std::vector<R> mRanks2;

  void histo_loop(const std::vector<V>& values, R* histos)
  {
    const ubyte_t *p  = (const ubyte_t*) values.data();
    const ubyte_t *pe = p + values.size() * c_NBytes;
    while (p != pe)
    {
      for (R j = 0; j < c_NBytes; ++j)
        histos[(j << 8) + *p++]++;
    }
  }

  void radix_loop(const std::vector<V>& values, R* histos, std::vector<R>& ranks)
  {
    const R nb = values.size();
    R* link[256];
    ranks.resize(nb);
    mRanks2.resize(nb);
    bool ranks_are_invalid = true;
    // Pass j sorts by byte j, LSB first (little-endian).
    for (R j = 0; j < c_NBytes; ++j)
    {
      R *cur_count = &histos[j << 8];
      const ubyte_t *input_bytes = (const ubyte_t*) values.data() + j;
      // All values share this byte: the pass would not change the order.
      if (cur_count[input_bytes[0]] == nb) continue;

      link[0] = mRanks2.data();
      for (R i = 1; i < 256; ++i)
        link[i] = link[i - 1] + cur_count[i - 1];

      if (ranks_are_invalid)
      {
        for (R i = 0; i < nb; ++i)
          *link[input_bytes[i * c_NBytes]]++ = i;
        ranks_are_invalid = false;
      }
      else
      {
        for (R k = 0; k < nb; ++k)
        {
          R id = ranks[k];
          *link[input_bytes[id * c_NBytes]]++ = id;
        }
      }
      ranks.swap(mRanks2);
    }
    if (ranks_are_invalid)
      std::iota(ranks.begin(), ranks.end(), 0);
  }
};

} // endnamespace gled

#endif
