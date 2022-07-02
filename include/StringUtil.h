#ifndef HASHEDSTRING_STRINGUTIL_H
#define HASHEDSTRING_STRINGUTIL_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER
// strsep shim, courtesy of https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
static inline char* strsep(char** inString, const char* sep) 
{
  if (*inString)
  {
    char* tokenStart = *inString;
    *inString = strpbrk(tokenStart, sep);
    if (*inString) 
    {
      **inString = '\0';
      (*inString)++;
    }
    return tokenStart;
  }
  return NULL;
}
#endif

// MSVC apparently has strdup already, perhaps look into a reliable way of detecting if a shim is needed?
#if defined(_MSC_VER) && __STDC_VERSION__ <= 201710L
#ifdef strdup
#undef strdup
#endif
#define strdup _strdup
#endif

//#if __STDC_VERSION__ <= 201710L
//// strdup shim, should be replaced when C23 rolls out
//// Not bothering with errono
//static inline char* strdup(const char* inString)
//{
//  const int32_t strLen = inString ? (strlen(inString) + 1) : 0;
//  if (strLen > 0)
//  {
//    // Alloc
//    char* dupedString = (char*)malloc(strLen * sizeof(char));
//    if (dupedString)
//    {
//      strcpy_s(dupedString, strLen, inString);
//      return dupedString;
//    }
//  }
//  return NULL;
//}
//#endif

// Counts *distinct* substrings
static inline int32_t CountSubStr(const char* inString, const char* subStr)
{
  if (inString && subStr)
  {
    const size_t subStrLen = strlen(subStr); // Step-size
    if (subStrLen > 0)
    {
      int32_t count = 0;
      const char* place = inString;
      while ((place = strstr(place, subStr)))
      {
        count++;
        place += subStrLen;
      }
    }
  }
  return 0;
}

static inline int32_t ExplodeString(const char* inString, const char* seperator, char** outStrings)
{
    if (inString)
    {
      const int32_t numTokens = CountSubStr(inString, seperator);
      if (numTokens > 0)
      {
        // Allocate array of pointers for substrings
        outStrings = (char**)malloc(numTokens * sizeof(char*));
        assert(outStrings);

        char* token = NULL;
        char* dupStr = strdup(inString);
        char* beginDupStr = dupStr; // Keep pointer to beginning of duped string so we can free it

        // Butcher dupStr into substrings, copy them to output array
        while ((token = strsep(&dupStr, seperator)))
        {
          // Copy token into outStrings
          *outStrings = strdup(token);
          outStrings++;
        }

        // Clean-up
        free(beginDupStr);
        return numTokens;
      }
      else // Special case, no substrings, return inString via outStrings
      {
        outStrings = (char**)malloc(sizeof(char*));
        assert(outStrings);
        *outStrings = strdup(inString);

        return 1;
      }
    }
    return 0;
}

#endif // HASHEDSTRING_STRINGUTIL_H
