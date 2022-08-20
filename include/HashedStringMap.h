#ifndef HASHEDSTRINGMAP_H
#define HASHEDSTRINGMAP_h

#include "HashedString.h"
#include <stdbool.h>

// Default Map Growth Ratio
#define GoldenRatio (1.618033988749894f)

// Key-Value intrusive-linked list container
typedef struct HashedStringEntry HashedStringEntry_t;
struct HashedStringEntry
{
  // Corresponding Hash
  hsHash_t Key;
  // Corresponding String
  char* String;
  uint32_t StringLength;

  // Pointer to next HashedString in this bucket
  struct HashedStringEntry* Next;
};

typedef struct HashedStringMap HashedStringMap_t;
struct HashedStringMap
{
  // How many buckets we have, used to modulo key to find index
  uint32_t NumBuckets;
  // How many elements we have spread across all buckets
  uint32_t NumElements;
  // When NumElements equals this value we grow the number of buckets, reallocate, and redistribute the map's contents
  uint32_t GrowthTrigger;

  struct HashedStringEntry** Buckets;
};

HashedStringMap_t* HashedStringMap_Create(uint32_t initialSize);
void HashedStringMap_Init(HashedStringMap_t* inMap, uint32_t initialSize);
void HashedStringMap_Cleanup(HashedStringMap_t* inMap);
HashedStringEntry_t* HashedStringMap_FindOrAdd(
  HashedStringMap_t* inMap,
  HashedString_t* hashedString,
  const char* inString, 
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  const char* inLCaseString, 
  HashedStringEntry_t** outLCaseEntry
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE 
);
HashedStringEntry_t* HashedStringMap_Find(
  HashedStringMap_t* inMap,
  const HashedString_t* hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  , HashedStringCaseSensitivity sensitivity
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
);
const char* HashedStringMap_GetString(HashedStringMap_t* inMap, const HashedString_t* hashedString);

#endif // HASHEDSTRINGMAP_H
