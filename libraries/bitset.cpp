#include "bitset.hpp"

namespace bitset {

const core::Version VERSION{LIB_MAJ, LIB_MIN}; DEPENDENCIES;

using core::getLowestSetBit;
using core::string;
using std::move;
using std::min;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
DC();

constexpr size_t Bitset::BITS;
constexpr Bitset::word Bitset::ONE;
constexpr size_t Bitset::NON_INDEX;

Bitset::Bitset () {
}

Bitset::Bitset (size_t width) : b((width + (BITS - 1)) / BITS) {
  ensureWidth(width);
}

Bitset::Bitset (size_t size, bool) : b(size) {
  b.append_any(size);
}

void Bitset::ensureWidth (size_t width) {
  if (width != 0) {
    ensureWidthForWord((width - 1) / BITS);
  }
}

void Bitset::ensureWidthForWord (size_t wordI) {
  size_t bSize = b.size();
  if (wordI >= bSize) {
    b.append(wordI + 1 - bSize, 0);
  }
}

bool Bitset::wordIsWithinWidth (size_t wordI) const noexcept {
  return wordI < b.size();
}

void Bitset::setExistingBit (size_t i) noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  DPRE(wordIsWithinWidth(wordI));
  b[wordI] |= ONE << bitI;
}

void Bitset::setBit (size_t i) {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  ensureWidthForWord(wordI);
  b[wordI] |= ONE << bitI;
}

void Bitset::clearExistingBit (size_t i) noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  DPRE(wordIsWithinWidth(wordI));
  b[wordI] &= ~(ONE << bitI);
}

void Bitset::clearBit (size_t i) {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  if (wordIsWithinWidth(wordI)) {
    b[wordI] &= ~(ONE << bitI);
  }
}

bool Bitset::getExistingBit (size_t i) const noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  DPRE(wordIsWithinWidth(wordI));
  return (b[wordI] >> bitI) & 0b1;
}

bool Bitset::getBit (size_t i) const noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  return wordIsWithinWidth(wordI) ? (b[wordI] >> bitI) & 0b1 : 0;
}

template<typename _OutOfRangeResult, typename _ReadOp> size_t Bitset::getNextBit (size_t i, const _OutOfRangeResult &outOfRangeResult, const _ReadOp &readOp) const noexcept {
  size_t wordI = i / BITS;
  size_t bitI = i % BITS;

  if (!wordIsWithinWidth(wordI)) {
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

void Bitset::clear () noexcept {
  b.clear();
}

bool Bitset::empty () const noexcept {
  for (const word &w : b) {
    if (w != 0) {
      return false;
    }
  }
  return true;
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

template<typename _MergeOp> void Bitset::op (
  const string<word> &i0, const string<word> &i1, size_t iSize,
  string<word> &r_o, _MergeOp mergeOp
) {
  DPRE(r_o.size() >= iSize, "r_o must have size at least that of the smaller of the inputs");

  for (size_t i = 0; i != iSize; ++i) {
    r_o[i] = mergeOp(i0[i], i1[i]);
  }
}

template<typename _MergeOp, typename _RemainderOp> void Bitset::op (
  const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size,
  string<word> &r_o, _MergeOp mergeOp, _RemainderOp remainderOp
) {
  DPRE(i0Size >= i1Size);
  DPRE(r_o.size() >= i0Size, "r_o must have size at least that of the bigger of the inputs");

  op(i0, i1, i1Size, r_o, mergeOp);
  for (size_t i = i1Size; i != i0Size; ++i) {
    r_o[i] = remainderOp(i0[i]);
  }
}

void Bitset::orOp (const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size, string<word> &r_o) {
  DPRE(i0Size >= i1Size);
  DPRE(r_o.size() == i0Size);
  Bitset::op(i0, i0Size, i1, i1Size, r_o, [] (word v0, word v1) -> word {
    return v0 | v1;
  }, [] (word o) -> word {
    return o;
  });
}

void Bitset::andOp (const string<word> &i0, const string<word> &i1, size_t iSize, string<word> &r_o) {
  DPRE(r_o.size() == iSize);
  Bitset::op(i0, i1, iSize, r_o, [] (word v0, word v1) -> word {
    return v0 & v1;
  });
}

void Bitset::andNotOp (const string<word> &i0, size_t i0Size, const string<word> &i1, size_t i1Size, string<word> &r_o) {
  DPRE(i0Size >= i1Size);
  DPRE(r_o.size() == i0Size);
  Bitset::op(i0, i0Size, i1, i1Size, r_o, [] (word v0, word v1) -> word {
    return v0 & ~v1;
  }, [] (word o) -> word {
    return o;
  });
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

  if (lSize < rSize) {
    Bitset o(move(r));

    Bitset::orOp(o.b, rSize, l.b, lSize, o.b);

    return o;
  } else {
    Bitset o(move(l));

    Bitset::orOp(o.b, lSize, r.b, rSize, o.b);

    return o;
  }
}

Bitset operator| (Bitset &&l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  if (l.b.capacity() < rSize) {
    DA(lSize < rSize);
    Bitset o(rSize, false);

    Bitset::orOp(r.b, rSize, l.b, lSize, o.b);

    return o;
  } else {
    Bitset o(move(l));
    const string<Bitset::word> *i0 = &o.b;
    size_t i0Size = lSize;
    const string<Bitset::word> *i1 = &r.b;
    size_t i1Size = rSize;
    if (lSize < rSize) {
      i0 = &r.b;
      i0Size = rSize;
      i1 = &o.b;
      i1Size = lSize;
      o.b.append_any(rSize - lSize);
    }

    Bitset::orOp(*i0, i0Size, *i1, i1Size, o.b);

    return o;
  }
}

Bitset operator| (const Bitset &l, Bitset &&r) {
  return move(r) | l;
}

Bitset operator| (const Bitset &l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  const string<Bitset::word> *i0 = &l.b;
  size_t i0Size = lSize;
  const string<Bitset::word> *i1 = &r.b;
  size_t i1Size = rSize;
  if (lSize < rSize) {
    i0 = &r.b;
    i0Size = rSize;
    i1 = &l.b;
    i1Size = lSize;
  }
  Bitset o(i0Size, false);

  Bitset::orOp(*i0, i0Size, *i1, i1Size, o.b);

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
  size_t oSize;
  if (lSize < rSize) {
    oSize = lSize;
  } else {
    oSize = rSize;
    o.b.resize_any(oSize);
  }

  Bitset::andOp(o.b, r.b, oSize, o.b);

  return o;
}

Bitset operator& (const Bitset &l, Bitset &&r) {
  return move(r) & l;
}

Bitset operator& (const Bitset &l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  size_t oSize = min(lSize, rSize);
  Bitset o(oSize, false);

  Bitset::andOp(l.b, r.b, oSize, o.b);

  return o;
}

Bitset &Bitset::andNot (const Bitset &r) {
  *this = andNot(move(*this), r);
  return *this;
}

Bitset Bitset::andNot (Bitset &&l, const Bitset &r) {
  size_t lSize = l.b.size();
  size_t rSize = r.b.size();

  size_t oSize = min(lSize, rSize);
  Bitset o(move(l));

  Bitset::andNotOp(o.b, lSize, r.b, oSize, o.b);

  return o;
}

bool Bitset::operator== (const Bitset &r) const {
  size_t lSize = b.size();
  size_t rSize = r.b.size();
  size_t oSize;
  const string<word> *b;
  if (lSize > rSize) {
    oSize = rSize;
    b = &this->b;
  } else {
    oSize = lSize;
    b = &r.b;
  }

  if (this->b.compare(0, oSize, r.b, 0, oSize) != 0) {
    return false;
  }
  for (const word *i = b->data() + oSize, *end = b->data() + b->size(); i != end ; ++i) {
    if (*i != 0) {
      return false;
    }
  }
  return true;
}

bool Bitset::operator!= (const Bitset &r) const {
  return !(*this == r);
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}
