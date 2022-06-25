#include "HashedStringMap.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// Create a new HashedStringEntry given a key (hash) and the corresponding string
static HashedStringEntry* HashedStringEntry_Create(hsHash_t inKey, const char* inString)
{
  HashedStringEntry* newEntry = (HashedStringEntry*)malloc(sizeof(HashedStringEntry));
  if (newEntry)
  {
    newEntry->Key = inKey;
    newEntry->StringLength = inString ? (uint32_t)strlen(inString)+1 : 0;
    if (newEntry->StringLength > 0)
    {
      // Copy string
      newEntry->String = (char*)malloc(newEntry->StringLength * sizeof(char*));
      assert(newEntry->String);
      strcpy_s(newEntry->String, newEntry->StringLength, inString);
    }
    else
    {
      newEntry->String = NULL;
    }
    newEntry->Next = NULL;

    return newEntry;
  }

  return NULL;
}

// Get the last entry in a list of entries
static HashedStringEntry* HashedStringEntry_GetEnd(HashedStringEntry* headOfList)
{
  if (headOfList)
  {
    HashedStringEntry* endPtr = headOfList;
    while (headOfList->Next)
    {
      endPtr = headOfList->Next;
    }
    assert(endPtr->Next == NULL);
    return endPtr;
  }
  return NULL;
}

// Append a new entry to an existing entry. If that pre-existing entry already has a valid next pointer, we move that to the newEntry
static void HashedStringEntry_Append(HashedStringEntry* parent, HashedStringEntry* newEntry)
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

// Find end of list, set as next
static void HashedStringEntry_AppendEnd(HashedStringEntry* entry, HashedStringEntry* newEntry)
{
  if (entry && newEntry)
  {
    HashedStringEntry* end = HashedStringEntry_GetEnd(entry);
    assert(end);
    end->Next = newEntry;
  }
}

// free a chain of entries
static void HashedStringEntry_Cleanup(HashedStringEntry* entry)
{
  if (entry)
  {
    HashedStringEntry* next = entry->Next;
    free(entry->String);
    free(entry);
    if (next)
    {
      HashedStringEntry_Cleanup(next);
    }
  }
}

static uint32_t HashedStringMap_GetBucketIndex(HashedStringMap* inMap, const hsHash_t hash)
{
  assert(inMap);
  return hash % inMap->NumBuckets;
}

// Get head entry in bucket
static HashedStringEntry* HashedStringMap_GetBucket(HashedStringMap* inMap, const uint32_t index)
{
  assert(inMap);
  return inMap->Buckets[index];
}

// Get pointer bucket resides in, useful if bucket is empty
static HashedStringEntry** HashedStringMap_GetBucketPtr(HashedStringMap* inMap, const uint32_t index)
{
  assert(inMap);
  return &inMap->Buckets[index];
}

static uint32_t HashedStringMap_GetGrowthTrigger(uint32_t size)
{
  // Grow when 3/4ths full
  return (uint32_t)ceilf((float)size * 0.75f);
}

HashedStringMap* HashedStringMap_Create(uint32_t initialSize)
{
  assert(initialSize > 0);

  HashedStringMap* newMap = (HashedStringMap*)malloc(sizeof(HashedStringMap));
  if (newMap)
  {
    newMap->NumBuckets = initialSize;
    newMap->NumElements = 0;
    newMap->GrowthTrigger = HashedStringMap_GetGrowthTrigger(initialSize);

    // Allocate array of empty (NULL) buckets
    newMap->Buckets = (HashedStringEntry**)malloc(initialSize * sizeof(HashedStringEntry*));
    assert(newMap->Buckets);
    for (uint32_t b = 0; b < initialSize; ++b)
    {
      newMap->Buckets[b] = NULL;
    }
  }
  return newMap;
}

void HashedStringMap_Cleanup(HashedStringMap* inMap)
{
  if (inMap)
  {
    for (uint32_t i = 0; i < inMap->NumBuckets; ++i)
    {
      HashedStringEntry* bucket = inMap->Buckets[i];
      if (bucket)
      {
        HashedStringEntry_Cleanup(bucket);
      }
    }
    free(inMap->Buckets);
    free(inMap);
  }  
}

static void HashedStringMap_GrowAndRebuild(HashedStringMap* inMap)
{
  assert(inMap);
  const uint32_t numBuckets = inMap->NumBuckets;
  const uint32_t newNumBuckets = (uint32_t)ceilf((float)numBuckets * GoldenRatio);
  assert(newNumBuckets > 0);

  // Allocate new buckets array, set all to NULL initially
  HashedStringEntry** newBuckets = (HashedStringEntry**)malloc(newNumBuckets * sizeof(HashedStringEntry*));
  assert(newBuckets);
  for (uint32_t b = 0; b < newNumBuckets; ++b)
  {
    newBuckets[b] = NULL;
  }

  // Swap buckets pointers
  HashedStringEntry** oldBuckets = inMap->Buckets;
  inMap->Buckets = newBuckets;
  inMap->NumBuckets = newNumBuckets;

  // Update growth trigger
  inMap->GrowthTrigger = HashedStringMap_GetGrowthTrigger(newNumBuckets);

  // For each existing bucket, insert each entry into new buckets
  for (uint32_t b = 0; b < numBuckets; ++b)
  {
    HashedStringEntry* oldBucket = oldBuckets[b];
    if (oldBucket)
    {
      HashedStringEntry* entryToMove = oldBucket;
      HashedStringEntry* nextInBucket = NULL;
      do
      {
        // Unlink entryToMove, next preserved in nextInBucket
        nextInBucket = entryToMove->Next;
        entryToMove->Next = NULL;

        // Find new bucket index
        const uint32_t bucketIndex = HashedStringMap_GetBucketIndex(inMap, entryToMove->Key);
        HashedStringEntry* newBucket = HashedStringMap_GetBucket(inMap, bucketIndex);

        // If valid, bucket already has something in it, append
        if (newBucket)
        {
          HashedStringEntry_AppendEnd(newBucket, entryToMove);
        }
        else // Make this entry the new head
        {
          HashedStringEntry** newBucketPtr = HashedStringMap_GetBucketPtr(inMap, bucketIndex);
          *newBucketPtr = entryToMove;
        }

        // Copy nextInBucket to entryToMove for next iteration
        entryToMove = nextInBucket;
      } while (nextInBucket);
    }
  }

  // Free old buckets array
  free(oldBuckets);
}

static HashedStringEntry* HashedStringMap_AddInternal(
  HashedStringMap* inMap,
  const hsHash_t hash,
  const uint32_t stringLength,
  const char* inString
)
{
  assert(inMap);

  // Make new entry
  HashedStringEntry* newEntry = HashedStringEntry_Create(hash, inString);
  assert(newEntry);

  // Find bucket
  const uint32_t bucketIndex = HashedStringMap_GetBucketIndex(inMap, hash);
  HashedStringEntry* entry = HashedStringMap_GetBucket(inMap, bucketIndex);
  if (entry)
  {
    // Bucket already in use, find end and append
    HashedStringEntry_AppendEnd(entry, newEntry);
  }
  else // Bucket is empty, set as head of bucket
  {
    HashedStringEntry** bucketPtr = HashedStringMap_GetBucketPtr(inMap, bucketIndex);
    assert(bucketPtr);
    *bucketPtr = newEntry;
  }

  // Increment elements, check if we need to grow the map
  if (++(inMap->NumElements) >= inMap->GrowthTrigger)
  {
    HashedStringMap_GrowAndRebuild(inMap);
  }

  return newEntry;
}

HashedStringEntry* HashedStringMap_FindOrAdd(
  HashedStringMap* inMap,
  HashedString* hashedString,
  const uint32_t stringLength,
  const char* inString,
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  const char* inLCaseString,
  HashedStringEntry** outLCaseEntry
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
)
{
  if (inMap && hashedString)
  {
    HashedStringEntry* outEntry;

    // Try find case-sensitive entry
    HashedStringEntry* existingEntry = HashedStringMap_Find(inMap, hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
      , HSCS_Sensitive
#endif
    );
    // If one doesn't exist, add it
    if (!existingEntry)
    {
      const hsHash_t hash = hashedString->Hash;
      outEntry = HashedStringMap_AddInternal(inMap, hash, stringLength, inString);      
    }
    else
    {
      outEntry = existingEntry;
    }

#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
    // Try find case-insensitive entry
    HashedStringEntry* existingLCaseEntry = HashedStringMap_Find(inMap, hashedString, HSCS_Insensitive);
    if (!existingLCaseEntry)
    {
      const hsHash_t hash = hashedString->CommonHash;
      HashedStringEntry* lCaseEntry = HashedStringMap_AddInternal(inMap, hash, stringLength, inLCaseString);
      if (outLCaseEntry)
      {
        *outLCaseEntry = lCaseEntry;
      }
    }
    else if (outLCaseEntry)
    {
      *outLCaseEntry = existingLCaseEntry;
    }
#endif
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
    const hsHash_t hash = sensitivity == HSCS_Sensitive ? hashedString->Hash : hashedString->CommonHash;
#else
    const hsHash_t hash = hashedString->Hash;
#endif

    // Get bucket index
    const uint32_t bucketIndex = HashedStringMap_GetBucketIndex(inMap, hash);
    HashedStringEntry* entry = HashedStringMap_GetBucket(inMap, bucketIndex);
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

const char* HashedStringMap_GetString(HashedStringMap* inMap, HashedString* hashedString)
{
  if (inMap && hashedString)
  {
    HashedStringEntry* entry = HashedStringMap_Find(inMap, hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
      , HSCS_Sensitive
#endif
    );
    if (entry)
    {
      return entry->String;
    }
  }
  return NULL;
}
