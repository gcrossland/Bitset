/** @file */
/* -----------------------------------------------------------------------------
   Bitset Library
   © Geoff Crossland 2013-2015
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
  pub explicit Bitset (size_t width);
  prv Bitset (size_t size, bool);
  pub Bitset (const Bitset &) = default;
  pub Bitset &operator= (const Bitset &) = default;
  pub Bitset (Bitset &&) = default;
  pub Bitset &operator= (Bitset &&) = default;

  pub void ensureWidth (size_t width);
  prv void ensureWidthForWord (size_t wordI);
  prv bool wordIsWithinWidth (size_t wordI) const noexcept;
  pub void setExistingBit (size_t i) noexcept;
  pub void setBit (size_t i);
  pub void clearExistingBit (size_t i) noexcept;
  pub void clearBit (size_t i);
  pub bool getExistingBit (size_t i) const noexcept;
  pub bool getBit (size_t i) const noexcept;
  prv template<typename _OutOfRangeResult, typename _ReadOp> size_t getNextBit (size_t i, const _OutOfRangeResult &outOfRangeResult, const _ReadOp &readOp) const noexcept;
  pub size_t getNextSetBit (size_t i) const noexcept;
  pub size_t getNextClearBit (size_t i) const noexcept;
  pub void clear () noexcept;
  pub bool empty () const noexcept;
  pub void compact ();

  prv template<typename _MergeOp> static void op (
    const core::string<word> &i0, const core::string<word> &i1, size_t iSize,
    core::string<word> &r_o, _MergeOp mergeOp
  );
  prv template<typename _MergeOp, typename _RemainderOp> static void op (
    const core::string<word> &i0, size_t i0Size, const core::string<word> &i1, size_t i1Size,
    core::string<word> &r_o, _MergeOp mergeOp, _RemainderOp remainderOp
  );
  prv static void orOp (const core::string<word> &i0, size_t i0Size, const core::string<word> &i1, size_t i1Size, core::string<word> &r_o);
  prv static void andOp (const core::string<word> &i0, const core::string<word> &i1, size_t iSize, core::string<word> &r_o);
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
  pub bool operator== (const Bitset &r) const;
  pub bool operator!= (const Bitset &r) const;
};

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}

#endif
