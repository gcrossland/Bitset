#include "header.hpp"
#include <vector>
#include <algorithm>

using std::fill;
using std::copy;
using std::vector;
using bitset::Bitset;
using std::move;
using core::check;
using std::all_of;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
DC();

int main (int argc, char *argv[]) {
  /*std::shared_ptr<core::debug::Stream> errs(new core::debug::Stream());
  DOPEN(, errs);
  bitset::DOPEN(, errs);*/

  testBitsets();

  return 0;
}

struct Rep {
  static const size_t valueSize = 96;

  bool value[valueSize];

  pub Rep () {
    fill(value, value + valueSize, 0);
  }

  pub Rep (const Rep &o) {
    copy(o.value, o.value + valueSize, value);
  }

  pub Rep &operator= (const Rep &o) {
    if (this == &o) {
      return *this;
    }

    copy(o.value, o.value + valueSize, value);
    return *this;
  }

  pub bool empty () const {
    return all_of(value, value + valueSize, [] (const bool &o) -> bool {
      return !o;
    });
  }
};

static const iu indexMap[] = {
  0, 1, 30, 31, 32, 33, 62, 63, 64, 65, 94, 95
};

void createBitsetsImpl (
  iu nextBitId, iu maxBitIds, Rep &r_rep, iu bitIndexBegin, iu bitIndexEnd, vector<Rep> &r_reps
) {
  if (nextBitId == maxBitIds) {
    r_reps.emplace_back(r_rep);
    return;
  }
  if (bitIndexBegin == bitIndexEnd) {
    return;
  }

  for (iu i = bitIndexBegin; i != bitIndexEnd; ++i) {
    r_rep.value[indexMap[i]] = 1;
    createBitsetsImpl(nextBitId + 1, maxBitIds, r_rep, i + 1, bitIndexEnd, r_reps);
    r_rep.value[indexMap[i]] = 0;
  }
}

vector<Rep> createBitsets (iu cardinalityBegin, iu cardinalityEnd) {
  vector<Rep> reps;

  Rep rep;
  for (iu i = cardinalityBegin; i != cardinalityEnd; ++i) {
    createBitsetsImpl(0, i, rep, 0, sizeof(indexMap) / sizeof(*indexMap), reps);
  }

  return reps;
}

void testBitsets () {
  vector<Rep> reps = createBitsets(0, 3);
  {
    vector<Rep> r2 = createBitsets(10, 13);
    reps.insert(reps.end(), r2.begin(), r2.end());
  }
  vector<Bitset> bitsets;
  for (Rep &rep : reps) {
    Bitset bitset;
    check(bitset.empty());
    for (iu i = 0; i != Rep::valueSize; ++i) {
      if (rep.value[i]) {
        bitset.setBit(i);
      }
    }
    check(rep.empty(), bitset.empty());
    bitsets.emplace_back(move(bitset));

    Bitset bitset2(Rep::valueSize);
    check(bitset2.empty());
    for (iu i = 0; i != Rep::valueSize; ++i) {
      if (rep.value[i]) {
        bitset2.setExistingBit(i);
      }
    }
    check(rep.empty(), bitset2.empty());
    check(bitsets.back(), bitset2);
  }
  bitsets[0].setBit(32);
  check(!bitsets[0].empty());
  bitsets[0].clear();
  check(bitsets[0].empty());
  bitsets[1].setBit(Rep::valueSize);
  bitsets[1].clearBit(Rep::valueSize);

  for (iu j = 0; j != reps.size(); ++j) {
    Rep &rep = reps[j];
    Bitset &bitset = bitsets[j];

    for (iu i = 0; i != Rep::valueSize; ++i) {
      check(rep.value[i], bitset.getBit(i));
    }
    check(0, bitset.getBit(Rep::valueSize));
    check(0, bitset.getBit(Rep::valueSize * 200));

    bool withinWidth = false;
    for (iu i = Rep::valueSize - 1; i != static_cast<iu>(0) - 1; --i) {
      if (rep.value[i]) {
        withinWidth = true;
      }
      if (withinWidth) {
        check(rep.value[i], bitset.getExistingBit(i));
      }
    }

    size_t lastBit = 0;
    for (iu i = 0; i != Rep::valueSize; ++i) {
      if (rep.value[i]) {
        for (iu j = lastBit; j <= i; ++j) {
          check(i, bitset.getNextSetBit(j));
        }
        lastBit = bitset.getNextSetBit(lastBit);
        check(i, lastBit);
        ++lastBit;
      }
    }
    check(Bitset::nonIndex, bitset.getNextSetBit(lastBit));
    check(Bitset::nonIndex, bitset.getNextSetBit(Rep::valueSize * 200));

    lastBit = 0;
    for (iu i = 0; i != Rep::valueSize; ++i) {
      if (!rep.value[i]) {
        for (iu j = lastBit; j <= i; ++j) {
          check(i, bitset.getNextClearBit(j));
        }
        lastBit = bitset.getNextClearBit(lastBit);
        check(i, lastBit);
        ++lastBit;
      }
    }
    check(bitset.getNextClearBit(lastBit) >= lastBit);
    check(bitset.getNextClearBit(lastBit) <= Rep::valueSize);
    check(Rep::valueSize * 200, bitset.getNextClearBit(Rep::valueSize * 200));

    for (bool assumingWithinWidth : {false, true}) {
      Bitset bitset2 = bitset;
      bool exhausted = (bitset2.getNextSetBit(0) == Bitset::nonIndex);
      for (iu i = 0; i != Rep::valueSize; ++i) {
        if (rep.value[i]) {
          check(false, exhausted);
          if (assumingWithinWidth) {
            bitset2.clearExistingBit(i);
          } else {
            bitset2.clearBit(i);
          }
          if (bitset2.getNextSetBit(i + 1) == Bitset::nonIndex) {
            exhausted = true;
          }
        }
      }
      check(true, exhausted);
    }
  }

  auto checkOr = [] (const Rep &r0, const Rep &r1, const Bitset &res) {
    for (iu i = 0; i != Rep::valueSize; ++i) {
      check(r0.value[i] | r1.value[i], res.getBit(i));
    }
  };
  auto checkAnd = [] (const Rep &r0, const Rep &r1, const Bitset &res) {
    for (iu i = 0; i != Rep::valueSize; ++i) {
      check(r0.value[i] & r1.value[i], res.getBit(i));
    }
  };
  auto checkAndNot = [] (const Rep &r0, const Rep &r1, const Bitset &res) {
    for (iu i = 0; i != Rep::valueSize; ++i) {
      check(r0.value[i] & ~r1.value[i], res.getBit(i));
    }
  };
  for (iu j = 0; j != reps.size(); ++j) {
    Rep &rep0 = reps[j];
    Bitset &bitset0 = bitsets[j];
    for (iu k = 0; k != reps.size(); ++k) {
      Rep &rep1 = reps[k];
      Bitset &bitset1 = bitsets[k];

      {
        Bitset b = bitset0;
        b |= bitset1;
        checkOr(rep0, rep1, b);
      }
      {
        Bitset b = bitset0;
        b |= Bitset(bitset1);
        checkOr(rep0, rep1, b);
      }
      checkOr(rep0, rep1, bitset0 | bitset1);
      checkOr(rep0, rep1, Bitset(bitset0) | bitset1);
      checkOr(rep0, rep1, bitset0 | Bitset(bitset1));
      checkOr(rep0, rep1, Bitset(bitset0) | Bitset(bitset1));

      {
        Bitset b = bitset0;
        b &= bitset1;
        checkAnd(rep0, rep1, b);
      }
      {
        Bitset b = bitset0;
        b &= Bitset(bitset1);
        checkAnd(rep0, rep1, b);
      }
      checkAnd(rep0, rep1, bitset0 & bitset1);
      checkAnd(rep0, rep1, Bitset(bitset0) & bitset1);
      checkAnd(rep0, rep1, bitset0 & Bitset(bitset1));
      checkAnd(rep0, rep1, Bitset(bitset0) & Bitset(bitset1));

      {
        Bitset b = bitset0;
        b.andNot(bitset1);
        checkAndNot(rep0, rep1, b);
      }
      {
        Bitset b = bitset0;
        b.andNot(Bitset(bitset1));
        checkAndNot(rep0, rep1, b);
      }
      checkAndNot(rep0, rep1, Bitset::andNot(bitset0, bitset1));
      checkAndNot(rep0, rep1, Bitset::andNot(Bitset(bitset0), bitset1));
      checkAndNot(rep0, rep1, Bitset::andNot(bitset0, Bitset(bitset1)));
      checkAndNot(rep0, rep1, Bitset::andNot(Bitset(bitset0), Bitset(bitset1)));

      check(j == k, bitset0 == bitset1);
    }
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
