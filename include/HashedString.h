#ifndef HASHEDSTRING_H
#define HASHEDSTRING_H

#include <stdint.h>
#include <stdbool.h>

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

typedef struct HashedString HashedString_t;

#ifndef HASHEDSTRING_NO_SHORTTYPEDEFS
typedef HashedString_t HString;
#endif

struct HashedString
{
  // Unique identifier for this string
  hsHash_t Hash;

#if HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  // Hash of lower-cased string
  hsHash_t CommonHash;
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
};

HashedString_t HashedString_Create(const char* inString);
const char* HashedString_GetString(const HashedString_t* inHashedString);

// Compare, case-sensitive
bool HashedString_Compare(const HashedString_t* lhs, const HashedString_t* rhs);
// Compare given sensitivity
bool HashedString_Compare_WithSensitivity(const HashedString_t* lhs, const HashedString_t* rhs, const HashedStringCaseSensitivity sensitivity);


#endif // HASHEDSTRING_H
