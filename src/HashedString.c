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

#ifndef XXHASH_SEED
// Random prime
#define XXHASH_SEED 50177
#endif // XXHASH_SEED

uint32_t HashString(const char* inString, size_t strLength)
{
#if HASHEDSTRING_USE_CITYHASH
  return CityHash32(inString, strLength);
#else
  return XXH32(inString, strLength, XXHASH_SEED);
#endif
}

// Copy and convert at the same time
void StringToLowerCase(const char* srcString, char* dstString, size_t strLength)
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

  const size_t strLength = strlen(inString);
  hStr.Hash = HashString(inString, strLength);

#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  uint32_t lcaseHash = -1;
  if (strLength < 1024)
  {
    // Reasonable sized buffer
    char lcaseString[1024];
    StringToLowerCase(inString, lcaseString, strLength);
    lcaseHash = HashString(lcaseString, strLength);
  }
  else
  {
    // Scary long string requires dynamic alloc
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
