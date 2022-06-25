#include "HashedString.h"

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

HashedString CreateHashedString(const char* inString)
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
  hsHash_t lcaseHash = -1;
  if (strLength < 256)
  {
    // Reasonable sized buffer
    char lcaseString[256];
    StringToLowerCase(inString, lcaseString, strLength);
    lcaseHash = HashString(lcaseString, strLength);
  }
  else
  {
    // long string requires dynamic alloc
    char* lcaseStr = (char*)malloc(strLength);
    if (lcaseStr != NULL)
    {
      StringToLowerCase(inString, lcaseStr, strLength);
    }
    lcaseHash = HashString(lcaseStr, strLength);
    free(lcaseStr);
  }
  hStr.CommonHash = lcaseHash;
#endif

  // TODO: Add string(s) to table

  return hStr;
}
