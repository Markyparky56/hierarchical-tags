#include "HashedStringMap.h"
#include <string.h>

HashedStringEntry* HashedStringEntry_Create(uint32_t inKey, const char* inString)
{
  HashedStringEntry* newEntry = (HashedStringEntry*)malloc(sizeof(HashedStringEntry));
  if (newEntry)
  {
    newEntry->Key = inKey;
    newEntry->String = inString;
    newEntry->StringLength = inString ? strlen(inString) : 0;
    newEntry->Next = NULL;

    return newEntry;
  }

  return NULL;
}

HashedStringEntry* HashedStringEntry_GetEnd(HashedStringEntry* headOfList)
{
  if (headOfList)
  {
    HashedStringEntry* endPtr = headOfList;
    while (headOfList->Next)
    {
      endPtr = headOfList->Next;
    }
    return endPtr;
  }
  return NULL;
}

void HashedStringEntry_Append(HashedStringEntry* parent, HashedStringEntry* newEntry)
{
  if (parent && newEntry)
  {
    if (parent->Next)
    {
      // Special-case, insert in middle of list
      HashedStringEntry* temp = parent->Next;
      parent->Next = newEntry;
      newEntry->Next = temp;
    }
    else
    {
      parent->Next = newEntry;
    }
  }
}

void HashedStringEntry_Cleanup(HashedStringEntry* entry)
{
  if (entry)
  {
    HashedStringEntry* next = entry->Next;
    free(entry);
    if (next)
    {
      HashedStringEntry_Cleanup(next);
    }
  }
}

HashedStringMap* HashedStringMap_Create(uint32_t initialSize)
{
  return NULL;
}

HashedStringEntry* HashedStringMap_Insert(
  HashedStringMap* inMap,
  HashedString* hashedString,
  const char* inString,
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  const char* inLCaseString,
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  uint32_t stringLength
)
{
  return NULL;
}

HashedStringEntry* HashedStringMap_Find(
  HashedStringMap* inMap,
  HashedString* hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  , HashedStringCaseSensitivity sensitivity
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
)
{
  return NULL;
}
