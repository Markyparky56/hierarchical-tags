#ifndef HASHEDSTRING_H
#define HASHEDSTRING_H

#include <stdint.h>

#ifndef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
#define HASHEDSTRING_ALLOW_CASE_INSENSITIVE 1
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE

#ifndef HASHEDSTRING_USE_32BIT
typedef uint64_t hsHash_t;
#else
typedef uint32_t hsHash_t;
#endif

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
  hsHash_t Hash;

#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  // Hash of lower-cased string
  hsHash_t CommonHash;
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
};

HashedString HashedString_Create(const char* inString);

#endif // HASHEDSTRING_H
