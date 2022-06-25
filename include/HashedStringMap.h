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
  hsHash_t Key;
  // Corresponding String
  char* String;
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

  HashedStringEntry** Buckets;
};

HashedStringMap* HashedStringMap_Create(uint32_t initialSize);
void HashedStringMap_Init(HashedStringMap* inMap, uint32_t initialSize);
void HashedStringMap_Cleanup(HashedStringMap* inMap);
HashedStringEntry* HashedStringMap_FindOrAdd(
  HashedStringMap* inMap, 
  HashedString* hashedString, 
  const uint32_t stringLength,
  const char* inString, 
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  const char* inLCaseString, 
  HashedStringEntry** outLCaseEntry
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE 
);
HashedStringEntry* HashedStringMap_Find(
  HashedStringMap* inMap, 
  HashedString* hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  , HashedStringCaseSensitivity sensitivity
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
);
const char* HashedStringMap_GetString(HashedStringMap* inMap, HashedString* hashedString);

#endif // HASHEDSTRINGMAP_H
