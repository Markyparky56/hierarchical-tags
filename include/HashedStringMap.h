#ifndef HASHEDSTRINGMAP_H
#define HASHEDSTRINGMAP_h

#include "HashedString.h"
#include <stdbool.h>

// Default Map Growth Ratio
#define GoldenRatio (1.618033988749894f)

typedef struct HashedStringEntry HashedStringEntry;

// Key-Value intrusive-linked list container
struct HashedStringEntry
{
  // Corresponding Hash
  uint32_t Key;
  // Corresponding String
  const char* String;
  uint32_t StringLength;

  // Pointer to next HashedString in this bucket
  HashedStringEntry* Next;
};

typedef struct HashedStringMap HashedStringMap;
struct HashedStringMap
{
  // How many buckets we have, used to modulo key to find index
  uint32_t NumBuckets;
  // How many elements we have spread across all buckets
  uint32_t NumElements;
  // When NumElements equals this value we grow the number of buckets, reallocate, and redistribute the map's contents
  uint32_t GrowthTrigger;

  HashedStringEntry* buckets;
};

HashedStringMap* HashedStringMap_Create(uint32_t initialSize);
HashedStringEntry* HashedStringMap_Insert(
  HashedStringMap* inMap, 
  HashedString* hashedString, 
  const char* inString, 
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  const char* inLCaseString, 
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE 
  uint32_t stringLength
);
HashedStringEntry* HashedStringMap_Find(
  HashedStringMap* inMap, 
  HashedString* hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  , HashedStringCaseSensitivity sensitivity
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
);


#endif // HASHEDSTRINGMAP_H
