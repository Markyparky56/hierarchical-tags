#include "HashedString.h"
#include "HashedStringMap.h"

#include <stdbool.h>
#include <stdalign.h>
#include <stdlib.h>
#include <string.h>
#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
#include <ctype.h>
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE

#ifdef HASHEDSTRING_USE_CITYHASH
#include "city.h"
#else
#include "xxhash.h"
#endif // HASHEDSTRING_USE_CITYHASH

#ifndef HASHEDSTRING_MAP_INITIALSIZE
#define HASHEDSTRING_MAP_INITIALSIZE 16
#endif

static bool bCreatedHashedStringMapSingleton = false;
alignas(HashedStringMap) static uint8_t HashedStringMapSingletonData[sizeof(HashedStringMap)];

static HashedStringMap* GetHashedStringMap()
{
  HashedStringMap* hashedStringMapSingleton = (HashedStringMap*)HashedStringMapSingletonData;

  if (bCreatedHashedStringMapSingleton)
  {
    return hashedStringMapSingleton;
  }
  else
  {
    HashedStringMap_Init(hashedStringMapSingleton, HASHEDSTRING_MAP_INITIALSIZE);
    bCreatedHashedStringMapSingleton = true;
    return hashedStringMapSingleton;
  }
}

static HashedStringMap* GetHashedStringMapUnchecked()
{
  HashedStringMap* hashedStringMapSingleton = (HashedStringMap*)HashedStringMapSingletonData;
  return hashedStringMapSingleton;
}

static hsHash_t HashString(const char* inString, size_t strLength)
{
// NOTE: Some of these functions accept a seed param, should that be exposed/used to improve hashing?

#ifdef HASHEDSTRING_USE_32BIT
#if HASHEDSTRING_USE_CITYHASH
  return CityHash32(inString, strLength);
#else
  return XXH32(inString, strLength, 0);
#endif // HASHEDSTRING_USE_CITYHASH
#else
#if HASHEDSTRING_USE_CITYHASH
  return CityHash64(inString, strLength);
#else
  return XXH3_64bits(inString, strLength);
#endif // HASHEDSTRING_USE_CITYHASH
#endif // HASHEDSTRING_USE_32BIT
}

// Copy and convert at the same time
static void StringToLowerCase(const char* restrict srcString, char* restrict dstString, size_t strLength)
{
  for (int i = 0; i < strLength; ++i)
  {
    *dstString = tolower(*srcString);
    srcString++;
    dstString++;
  }
}

HashedString HashedString_Create(const char* inString)
{
  HashedString hStr;

  if (inString == NULL)
  {
    hStr.Hash = 0;
    hStr.CommonHash = 0;
    return hStr;
  }

  const size_t strLength = strlen(inString)+1;
  hStr.Hash = HashString(inString, strLength);

#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  hsHash_t lCaseHash = 0;
  if (strLength < 256)
  {
    // Reasonable sized buffer
    char lCaseString[256];
    StringToLowerCase(inString, lCaseString, strLength);
    lCaseHash = HashString(lCaseString, strLength);

    // Add to map for later look-up
    HashedStringMap* stringMap = GetHashedStringMap();
    HashedStringMap_FindOrAdd(stringMap, &hStr, (uint32_t)strLength, inString, lCaseString, NULL);
  }
  else
  {
    // long string requires dynamic alloc
    char* lCaseString = (char*)malloc(strLength);
    if (lCaseString != NULL)
    {
      StringToLowerCase(inString, lCaseString, strLength);
    }
    lCaseHash = HashString(lCaseString, strLength);

    // Add to map for later look-up
    HashedStringMap* stringMap = GetHashedStringMap();
    HashedStringMap_FindOrAdd(stringMap, &hStr, (uint32_t)strLength, inString, lCaseString, NULL);

    free(lCaseString);
  }
  hStr.CommonHash = lCaseHash;
#else
  // Add to map for later look-up
  HashedStringMap* stringMap = GetHashedStringMap();
  HashedStringMap_FindOrAdd(stringMap, &hStr, strLength, inString);
#endif

  return hStr;
}
