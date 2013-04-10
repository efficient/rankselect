/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -* -*/

#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "shared.h"

const int kWordSize = 64;
const int kBasicBlockSize = 512;
const int kBasicBlockBits = 9;
const int kBasicBlockMask = kBasicBlockSize - 1;
const int kWordCountPerBasicBlock = kBasicBlockSize / kWordSize;

class Bitmap {
public:
    Bitmap() { pCount_ = 0; }
    
    virtual uint64 rank(uint64 pos) = 0;
	virtual uint64 select(uint64 rank) = 0;
    
    uint64 pCount() { return pCount_; }
    
protected:
    uint64 pCount_;
};

class BitmapPoppy: public Bitmap {
public:
    BitmapPoppy(uint64* bits, uint64 nbits);
    ~BitmapPoppy() {}
    
    uint64 rank(uint64 pos);
    uint64 select(uint64 rank);
    
private:
    uint64* bits_;
    uint64  nbits_;
    
    uint64* l2Entries_;
    uint64  l2EntryCount_;
    uint64* l1Entries_;
    uint64  l1EntryCount_;
    uint64  basicBlockCount_;
    
    uint32* loc_[1 << 16];
    uint32  locCount_[1 << 16];

    static const int kLocFreq = 8192;
    static const int kLocFreqMask = 8191;
    static const int kL2EntryCountPerL1Entry = 1 << 21;
    static const int kBasicBlockCountPerL1Entry = 1 << 23;
};

#endif /* _BITMAP_H_ */
