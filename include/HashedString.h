#ifndef HASHEDSTRING_H
#define HASHEDSTRING_H

#include <stdint.h>

#ifndef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
#define HASHEDSTRING_ALLOW_CASE_INSENSITIVE 1
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE

typedef struct
{
  // Unique identifier for this string
  uint32_t Hash;

#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  // Hash of lower-cased string
  uint32_t CommonHash;
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
} HashedString;

HashedString CreateHashedString(const char* inString);

#endif // HASHEDSTRING_H