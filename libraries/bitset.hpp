/** @file */
/* -----------------------------------------------------------------------------
   Bitset Library
   © Geoff Crossland 2013, 2014
----------------------------------------------------------------------------- */
#ifndef BITSET_ALREADYINCLUDED
#define BITSET_ALREADYINCLUDED

#include <core.hpp>

namespace bitset {

extern const core::Version VERSION;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
extern DC();

class Bitset {
  prv typedef iu word;
  prv static constexpr size_t BITS = core::numeric_limits<word>::bits;
  prv static constexpr word ONE = 1;
  pub static constexpr size_t NON_INDEX = core::numeric_limits<size_t>::max();

  prv core::string<word> b;

  pub Bitset ();
  pub Bitset (size_t capacity);
  pub Bitset (const Bitset &) = default;
  pub Bitset &operator= (const Bitset &) = default;
  pub Bitset (Bitset &&) = default;
  pub Bitset &operator= (Bitset &&) = default;

  prv void ensureSize (size_t wordI);
  prv bool isWithinSize (size_t wordI) const noexcept;
  // XXXX just have references to pseudo-bools? iterators?
  // XXXX variants to set/clear ranges?
  pub void setBit (size_t i);
  pub void clearBit (size_t i);
  pub bool getBit (size_t i) const noexcept;
  prv template<typename _OutOfRangeResult, typename _ReadOp> size_t getNextBit (size_t i, const _OutOfRangeResult &outOfRangeResult, const _ReadOp &readOp) const noexcept;
  pub size_t getNextSetBit (size_t i) const noexcept;
  pub size_t getNextClearBit (size_t i) const noexcept;
  pub void compact ();

  prv template<typename _MergeOp> static size_t op (
    const core::string<word> &i0, size_t i0Size, const core::string<word> &i1, size_t i1Size,
    core::string<word> &r_o, _MergeOp mergeOp
  );
  prv template<typename _MergeOp, typename _RemainderOp> static size_t op (
    const core::string<word> &i0, size_t i0Size, const core::string<word> &i1, size_t i1Size,
    core::string<word> &r_o, _MergeOp mergeOp, _RemainderOp remainderOp
  );
  prv static void orOp (const core::string<word> &i0, size_t i0Size, const core::string<word> &i1, size_t i1Size, core::string<word> &r_o);
  prv static void andOp (const core::string<word> &i0, size_t i0Size, const core::string<word> &i1, size_t i1Size, core::string<word> &r_o);
  pub Bitset &operator|= (Bitset &&r);
  pub Bitset &operator|= (const Bitset &r);
  friend Bitset operator| (Bitset &&l, Bitset &&r);
  friend Bitset operator| (Bitset &&l, const Bitset &r);
  friend Bitset operator| (const Bitset &l, Bitset &&r);
  friend Bitset operator| (const Bitset &l, const Bitset &r);
  pub Bitset &operator&= (Bitset &&r);
  pub Bitset &operator&= (const Bitset &r);
  friend Bitset operator& (Bitset &&l, Bitset &&r);
  friend Bitset operator& (Bitset &&l, const Bitset &r);
  friend Bitset operator& (const Bitset &l, Bitset &&r);
  friend Bitset operator& (const Bitset &l, const Bitset &r);
};

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}

#endif
