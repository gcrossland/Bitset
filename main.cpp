#include "header.hpp"
#include <vector>

using std::fill;
using std::copy;
using std::vector;
using bitset::Bitset;
using std::move;
using core::check;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
DC();

int main (int argc, char *argv[]) {
  testBitsets();

  return 0;
}

struct Rep {
  static const size_t VALUE_SIZE = 96;

  bool value[VALUE_SIZE];

  pub Rep () {
    fill(value, value + VALUE_SIZE, 0);
  }

  pub Rep (const Rep &o) {
    copy(o.value, o.value + VALUE_SIZE, value);
  }

  pub Rep &operator= (const Rep &o) {
    if (this == &o) {
      return *this;
    }

    copy(o.value, o.value + VALUE_SIZE, value);
    return *this;
  }

  // XXXX
  pub bool isSane () {
    for (iu i = 0; i != VALUE_SIZE; ++i) {
      if ((i == 0 || i == 1 || i == 30 || i == 31 || i == 32 || i == 33 || i == 62 || i == 63 || i == 64 || i == 65 || i == 94 || i == 95)) {
        continue;
      }
      if (value[i] != 0) {
        return false;
      }
    }
    return true;
  }
};

static const iu INDEX_MAP[] = {
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
    r_rep.value[INDEX_MAP[i]] = 1;
    createBitsetsImpl(nextBitId + 1, maxBitIds, r_rep, i + 1, bitIndexEnd, r_reps);
    r_rep.value[INDEX_MAP[i]] = 0;
  }
}

vector<Rep> createBitsets (iu cardinalityBegin, iu cardinalityEnd) {
  vector<Rep> reps;

  Rep rep;
  for (iu i = cardinalityBegin; i != cardinalityEnd; ++i) {
    createBitsetsImpl(0, i, rep, 0, sizeof(INDEX_MAP) / sizeof(*INDEX_MAP), reps);
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
    for (iu i = 0; i != Rep::VALUE_SIZE; ++i) {
      if (rep.value[i]) {
        bitset.setBit(i);
      }
    }
    bitsets.emplace_back(move(bitset));
  }

  for (iu j = 0; j != reps.size(); ++j) {
    Rep &rep = reps[j];
    Bitset &bitset = bitsets[j];

    for (iu i = 0; i != Rep::VALUE_SIZE; ++i) {
      check(rep.value[i], bitset.getBit(i));
    }
    check(0, bitset.getBit(Rep::VALUE_SIZE));
    check(0, bitset.getBit(Rep::VALUE_SIZE * 200));

    size_t lastBit = 0;
    for (iu i = 0; i != Rep::VALUE_SIZE; ++i) {
      if (rep.value[i]) {
        lastBit = bitset.getNextSetBit(lastBit);
        check(i, lastBit);
        ++lastBit;
      }
    }
    check(Bitset::NON_INDEX, bitset.getNextSetBit(lastBit));

    lastBit = 0;
    for (iu i = 0; i != Rep::VALUE_SIZE; ++i) {
      if (!rep.value[i]) {
        lastBit = bitset.getNextClearBit(lastBit);
        check(i, lastBit);
        ++lastBit;
      }
    }
    check(bitset.getNextClearBit(lastBit) >= lastBit);
    check(bitset.getNextClearBit(lastBit) <= Rep::VALUE_SIZE);

    Bitset bitset2 = bitset;
    bool exhausted = (bitset2.getNextSetBit(0) == Bitset::NON_INDEX);
    for (iu i = 0; i != Rep::VALUE_SIZE; ++i) {
      if (rep.value[i]) {
        check(false, exhausted);
        bitset2.clearBit(i);
        if (bitset2.getNextSetBit(i + 1) == Bitset::NON_INDEX) {
          exhausted = true;
        }
      }
    }
    check(true, exhausted);
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
