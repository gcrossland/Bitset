#include "bitset.hpp"

namespace bitset {

const core::Version VERSION{LIB_MAJ, LIB_MIN}; DEPENDENCIES;

using core::getLowestSetBit;
using core::string;
using std::move;
using std::max;
using std::min;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
DC();

constexpr size_t Bitset::BITS;
constexpr Bitset::word Bitset::ONE;
constexpr size_t Bitset::NON_INDEX;

Bitset::Bitset () {
}

Bitset::Bitset (size_t capacity) {
  b.reserve((capacity + BITS - 1) / BITS);
}

void Bitset::ensureSize (size_t wordI) {
  size_t bSize = b.size();
  if (wordI >= bSize) {
    b.append(wordI + 1 - bSize, 0);
  }
}

bool Bitset::isWithinSize (size_t wordI) const noexcept {
  return wordI < b.size();
}

void Bitset::setBit (size_t i) {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  // XXXX make capacity ensurance user-directed? i.e. have ensureCapacityAndSetBit() to do this job? (would require getSize()...)
  ensureSize(wordI);
  b[wordI] |= ONE << bitI;
}

void Bitset::clearBit (size_t i) {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  if (isWithinSize(wordI)) {
    b[wordI] &= ~(ONE << bitI);
  }
}

bool Bitset::getBit (size_t i) const noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  return isWithinSize(wordI) ? (b[wordI] >> bitI) & 0b1 : 0;
}

template<typename _OutOfRangeResult, typename _ReadOp> size_t Bitset::getNextBit (size_t i, const _OutOfRangeResult &outOfRangeResult, const _ReadOp &readOp) const noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  if (!isWithinSize(wordI)) {
    return outOfRangeResult(i);
  }

  // Check for a bit in the remainder of the word.
  word remainder = readOp(b[wordI]) >> bitI;
  iu lowI = getLowestSetBit(remainder);
  if (lowI < BITS) {
    return i + lowI;
  }

  // Look at the following words and find the first non-zero one.
  DA(remainder == 0);
  size_t begin = wordI + 1;
  size_t end = b.size();
  for (; begin != end; ++begin) {
    remainder = readOp(b[begin]);
    if (remainder != 0) {
      // Get the bit out of it (since there is one).
      lowI = getLowestSetBit(remainder);
      DA(lowI < BITS);
      return begin * BITS + lowI;
    }
  }

  DA(end == begin);
  return outOfRangeResult(end * BITS);
}

size_t Bitset::getNextSetBit (size_t i) const noexcept {
  return this->getNextBit(i, [] (size_t i) -> size_t {
    return NON_INDEX;
  }, [] (word w) -> word {
    return w;
  });
}

size_t Bitset::getNextClearBit (size_t i) const noexcept {
  return this->getNextBit(i, [] (size_t i) -> size_t {
    return i;
  }, [] (word w) -> word {
    return ~w;
  });
}

void Bitset::compact () {
  for (size_t i = b.size() - 1; i != static_cast<size_t>(-1); --i) {
    if (b[i] != 0) {
      b.resize(i + 1);
      break;
    }
  }

  b.shrink_to_fit();
}

template<typename _MergeOp> size_t Bitset::op (
  const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size,
  string<word> &r_o, _MergeOp mergeOp
) {
  size_t end = std::min(i0Size, i1Size);
  DPRE(r_o.size() >= end, "r_o must have size at least that of the smaller of the inputs");

  for (size_t i = 0; i != end; ++i) {
    r_o[i] = mergeOp(i0[i], i1[i]);
  }

  return end;
}

template<typename _MergeOp, typename _RemainderOp> size_t Bitset::op (
  const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size,
  string<word> &r_o, _MergeOp mergeOp, _RemainderOp remainderOp
) {
  // XXXX these ops are symmetric: just require that the first one is the bigger?
  bool i0IsBigger = i0Size > i1Size;
  const string<word> &iR = i0IsBigger ? i0 : i1;
  size_t end = i0IsBigger ? i0Size : i1Size;
  DPRE(r_o.size() >= end, "r_o must have size at least that of the bigger of the inputs");

  size_t i = op(i0, i0Size, i1, i1Size, r_o, mergeOp);

  for (; i != end; ++i) {
    r_o[i] = remainderOp(iR[i]);
  }

  return end;
}

void Bitset::orOp (const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size, string<word> &r_o) {
  size_t end = Bitset::op(i0, i0Size, i1, i1Size, r_o, [] (word v0, word v1) -> word {
    return v0 | v1;
  }, [] (word o) -> word {
    return o;
  });
  r_o.resize(end);
}

void Bitset::andOp (const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size, string<word> &r_o) {
  size_t end = Bitset::op(i0, i0Size, i1, i1Size, r_o, [] (word v0, word v1) -> word {
    return v0 & v1;
  });
  r_o.resize(end);
}

Bitset &Bitset::operator|= (Bitset &&r) {
  *this = move(*this) | move(r);
  return *this;
}

Bitset &Bitset::operator|= (const Bitset &r) {
  *this = move(*this) | r;
  return *this;
}

Bitset operator| (Bitset &&l, Bitset &&r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  if (l.b.capacity() >= rSize) {
    Bitset o(move(l));
    if (lSize < rSize) {
      o.b.resize_any(rSize);
    }

    Bitset::orOp(o.b, lSize, r.b, rSize, o.b);

    return o;
  } else {
    Bitset o(move(r));

    Bitset::orOp(l.b, lSize, o.b, rSize, o.b);

    return o;
  }
}

Bitset operator| (Bitset &&l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  if (l.b.capacity() >= rSize) {
    Bitset o(move(l));
    if (lSize < rSize) {
      o.b.resize_any(rSize);
    }

    Bitset::orOp(o.b, lSize, r.b, rSize, o.b);

    return o;
  } else {
    Bitset o(rSize);
    o.b.resize_any(rSize);

    Bitset::orOp(l.b, lSize, r.b, rSize, o.b);

    return o;
  }
}

Bitset operator| (const Bitset &l, Bitset &&r) {
  return move(r) | l;
}

Bitset operator| (const Bitset &l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();
  size_t oSize = max(lSize, rSize);

  Bitset o(oSize);
  o.b.resize_any(oSize);

  Bitset::orOp(l.b, lSize, r.b, rSize, o.b);

  return o;
}

Bitset &Bitset::operator&= (Bitset &&r) {
  *this = move(*this) & move(r);
  return *this;
}

Bitset &Bitset::operator&= (const Bitset &r) {
  *this = move(*this) & r;
  return *this;
}

Bitset operator& (Bitset &&l, Bitset &&r) {
  return move(l) & r;
}

Bitset operator& (Bitset &&l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  Bitset o(move(l));

  Bitset::andOp(o.b, lSize, r.b, rSize, o.b);

  return o;
}

Bitset operator& (const Bitset &l, Bitset &&r) {
  return move(r) & l;
}

Bitset operator& (const Bitset &l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();
  size_t oSize = min(lSize, rSize);

  Bitset o(oSize);
  o.b.resize_any(oSize);

  Bitset::andOp(l.b, lSize, r.b, rSize, o.b);

  return o;
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}
