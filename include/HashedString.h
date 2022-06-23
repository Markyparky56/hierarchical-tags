#ifndef HASHEDSTRING_H
#define HASHEDSTRING_H

#include <stdint.h>

#ifndef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
#define HASHEDSTRING_ALLOW_CASE_INSENSITIVE 1
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE

typedef enum HashedStringCaseSensitivity HashedStringCaseSensitivity;
enum HashedStringCaseSensitivity
{
  HSCS_Sensitive,
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  HSCS_Insensitive
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
};


typedef struct HashedString HashedString;

struct HashedString
{
  // Unique identifier for this string
  uint32_t Hash;

#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  // Hash of lower-cased string
  uint32_t CommonHash;
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
};

HashedString CreateHashedString(const char* inString);

#endif // HASHEDSTRING_H
