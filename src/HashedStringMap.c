#include "HashedStringMap.h"
#include <string.h>
#include <assert.h>
#include <math.h>

// Create a new HashedStringEntry given a key (hash) and the corresponding string
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

// Get the last entry in a list of entries
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

// Append a new entry to an existing entry. If that pre-existing entry already has a valid next pointer, we move that to the newEntry
void HashedStringEntry_Append(HashedStringEntry* parent, HashedStringEntry* newEntry)
{
  if (parent && newEntry)
  {
    if (parent->Next)
    {
      // Special-case, insert in middle of list
      HashedStringEntry* temp = parent->Next;
      parent->Next = newEntry;

      // Don't want to handle reparenting newEntry's children
      assert(newEntry->Next == NULL);
      newEntry->Next = temp;
    }
    else
    {
      parent->Next = newEntry;
    }
  }
}

// free a chain of entries
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
  HashedStringMap* newMap = (HashedStringMap*)malloc(sizeof(HashedStringMap));
  if (newMap)
  {
    newMap->NumBuckets = initialSize;
    newMap->NumElements = 0;
    newMap->GrowthTrigger = (uint32_t)ceilf((float)initialSize * 0.75f);

    // Allocate array of empty buckets
    newMap->Buckets = (HashedStringEntry**)malloc(initialSize * sizeof(HashedStringEntry*));
  }
  return newMap;
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
  if (inMap && hashedString)
  {
    // TODO
  }
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
  if (inMap && hashedString)
  {
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
    const uint32_t hash = sensitivity == HSCS_Sensitive ? hashedString->Hash : hashedString->CommonHash;
#else
    const uint32_t hash = hashedString->Hash;
#endif

    // Get bucket index
    const uint32_t index = hash % inMap->NumBuckets;
    HashedStringEntry* entry = inMap->Buckets[index];
    if (entry)
    {
      // Keep traversing list until entry->Key == hash or we run out of entries
      while (entry->Key != hash && entry->Next)
      {
        entry = entry->Next;
      }
      if (entry)
      {
        return entry;
      }
    }
  }
  return NULL;
}
