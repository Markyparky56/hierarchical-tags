#include "HashedStringMap.h"
#include "StringUtil.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// Create a new HashedStringEntry given a key (hash) and the corresponding string
static HashedStringEntry_t* HashedStringEntry_Create(hsHash_t inKey, const char* inString)
{
  HashedStringEntry_t* newEntry = (HashedStringEntry_t*)malloc(sizeof(HashedStringEntry_t));
  if (newEntry)
  {
    newEntry->Key = inKey;
    const size_t stringLength = inString ? strlen(inString)+1 : 0;
    if (stringLength > 0)
    {
      // Copy string
      newEntry->String = strdup(inString);
      assert(newEntry->String);
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
static HashedStringEntry_t* HashedStringEntry_GetEnd(HashedStringEntry_t* headOfList)
{
  if (headOfList)
  {
    HashedStringEntry_t* endPtr = headOfList;
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
static void HashedStringEntry_Append(HashedStringEntry_t* parent, HashedStringEntry_t* newEntry)
{
  if (parent && newEntry)
  {
    if (parent->Next)
    {
      // Special-case, insert in middle of list
      HashedStringEntry_t* temp = parent->Next;
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
static void HashedStringEntry_AppendEnd(HashedStringEntry_t* entry, HashedStringEntry_t* newEntry)
{
  if (entry && newEntry)
  {
    HashedStringEntry_t* end = HashedStringEntry_GetEnd(entry);
    assert(end);
    end->Next = newEntry;
  }
}

// free a chain of entries
static void HashedStringEntry_Cleanup(HashedStringEntry_t* entry)
{
  if (entry)
  {
    HashedStringEntry_t* next = entry->Next;
    free(entry->String);
    free(entry);
    if (next)
    {
      HashedStringEntry_Cleanup(next);
    }
  }
}

static uint32_t HashedStringMap_GetBucketIndex(HashedStringMap_t* inMap, const hsHash_t hash)
{
  assert(inMap);
  return hash % inMap->NumBuckets;
}

// Get head entry in bucket
static HashedStringEntry_t* HashedStringMap_GetBucket(HashedStringMap_t* inMap, const uint32_t index)
{
  assert(inMap);
  return inMap->Buckets[index];
}

// Get pointer bucket resides in, useful if bucket is empty
static HashedStringEntry_t** HashedStringMap_GetBucketPtr(HashedStringMap_t* inMap, const uint32_t index)
{
  assert(inMap);
  return &inMap->Buckets[index];
}

static uint32_t HashedStringMap_GetGrowthTrigger(uint32_t size)
{
  // Grow when 3/4ths full
  // NOTE: Since we can't ensure every entry will get its own bucket, to minimise the length of internal 
  // bucket lists, we grow the map "early", spreading entries out again
  return (uint32_t)ceilf((float)size * 0.75f);
}

HashedStringMap_t* HashedStringMap_Create(uint32_t initialSize)
{
  assert(initialSize > 0);

  HashedStringMap_t* newMap = (HashedStringMap_t*)malloc(sizeof(HashedStringMap_t));
  if (newMap)
  {
    HashedStringMap_Init(newMap, initialSize);
  }
  return newMap;
}

void HashedStringMap_Init(HashedStringMap_t* inMap, uint32_t initialSize)
{
  assert(inMap);
  assert(initialSize > 0);

  inMap->NumBuckets = initialSize;
  inMap->NumElements = 0;
  inMap->GrowthTrigger = HashedStringMap_GetGrowthTrigger(initialSize);

  // Allocate array of empty (NULL) buckets
  const size_t allocSize = initialSize * sizeof(HashedStringEntry_t*);
  inMap->Buckets = (HashedStringEntry_t**)malloc(allocSize);
  if (inMap->Buckets)
  {
    memset(inMap->Buckets, 0, allocSize);
  }
}

void HashedStringMap_Cleanup(HashedStringMap_t* inMap)
{
  if (inMap)
  {
    for (uint32_t i = 0; i < inMap->NumBuckets; ++i)
    {
      HashedStringEntry_t* bucket = inMap->Buckets[i];
      if (bucket)
      {
        HashedStringEntry_Cleanup(bucket);
      }
    }
    free(inMap->Buckets);
    free(inMap);
  }  
}

static void HashedStringMap_GrowAndRebuild(HashedStringMap_t* inMap)
{
  assert(inMap);
  const uint32_t numBuckets = inMap->NumBuckets;
  const uint32_t newNumBuckets = (uint32_t)ceilf((float)numBuckets * GoldenRatio);
  assert(newNumBuckets > 0);

  // Allocate new buckets array, set all to NULL initially
  HashedStringEntry_t** newBuckets = (HashedStringEntry_t**)malloc(newNumBuckets * sizeof(HashedStringEntry_t*));
  assert(newBuckets);
  for (uint32_t b = 0; b < newNumBuckets; ++b)
  {
    newBuckets[b] = NULL;
  }

  // Swap buckets pointers
  HashedStringEntry_t** oldBuckets = inMap->Buckets;
  inMap->Buckets = newBuckets;
  inMap->NumBuckets = newNumBuckets;

  // Update growth trigger
  inMap->GrowthTrigger = HashedStringMap_GetGrowthTrigger(newNumBuckets);

  // For each existing bucket, insert each entry into new buckets
  for (uint32_t b = 0; b < numBuckets; ++b)
  {
    HashedStringEntry_t* oldBucket = oldBuckets[b];
    if (oldBucket)
    {
      HashedStringEntry_t* entryToMove = oldBucket;
      HashedStringEntry_t* nextInBucket = NULL;
      do
      {
        // Unlink entryToMove, next preserved in nextInBucket
        nextInBucket = entryToMove->Next;
        entryToMove->Next = NULL;

        // Find new bucket index
        const uint32_t bucketIndex = HashedStringMap_GetBucketIndex(inMap, entryToMove->Key);
        HashedStringEntry_t* newBucket = HashedStringMap_GetBucket(inMap, bucketIndex);

        // If valid, bucket already has something in it, append
        if (newBucket)
        {
          HashedStringEntry_AppendEnd(newBucket, entryToMove);
        }
        else // Make this entry the new head
        {
          HashedStringEntry_t** newBucketPtr = HashedStringMap_GetBucketPtr(inMap, bucketIndex);
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

static HashedStringEntry_t* HashedStringMap_AddInternal(
  HashedStringMap_t* inMap,
  const hsHash_t hash,
  const char* inString
)
{
  assert(inMap);

  // Make new entry
  HashedStringEntry_t* newEntry = HashedStringEntry_Create(hash, inString);
  assert(newEntry);

  // Find bucket
  const uint32_t bucketIndex = HashedStringMap_GetBucketIndex(inMap, hash);
  HashedStringEntry_t* entry = HashedStringMap_GetBucket(inMap, bucketIndex);
  if (entry)
  {
    // Bucket already in use, find end and append
    HashedStringEntry_AppendEnd(entry, newEntry);
  }
  else // Bucket is empty, set as head of bucket
  {
    HashedStringEntry_t** bucketPtr = HashedStringMap_GetBucketPtr(inMap, bucketIndex);
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

HashedStringEntry_t* HashedStringMap_FindOrAdd(
  HashedStringMap_t* inMap,
  HashedString_t* hashedString,
  const char* inString,
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
  const char* inLCaseString,
  HashedStringEntry_t** outLCaseEntry
#endif // HASHEDSTRING_ALLOW_CASE_INSENSITIVE
)
{
  if (inMap && hashedString)
  {
    HashedStringEntry_t* outEntry;

    // Try find case-sensitive entry
    HashedStringEntry_t* existingEntry = HashedStringMap_Find(inMap, hashedString
#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
      , HSCS_Sensitive
#endif
    );

    // If one doesn't exist, add it
    if (!existingEntry)
    {
      const hsHash_t hash = hashedString->Hash;
      outEntry = HashedStringMap_AddInternal(inMap, hash, inString);      
    }
    else
    {
      outEntry = existingEntry;
    }

#ifdef HASHEDSTRING_ALLOW_CASE_INSENSITIVE
    // Try find case-insensitive entry
    HashedStringEntry_t* existingLCaseEntry = HashedStringMap_Find(inMap, hashedString, HSCS_Insensitive);
    if (!existingLCaseEntry)
    {
      const hsHash_t hash = hashedString->CommonHash;
      HashedStringEntry_t* lCaseEntry = HashedStringMap_AddInternal(inMap, hash, inLCaseString);
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
    return outEntry;
  }
  return NULL;
}

HashedStringEntry_t* HashedStringMap_Find(
  HashedStringMap_t* inMap,
  const HashedString_t* hashedString
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
    HashedStringEntry_t* entry = HashedStringMap_GetBucket(inMap, bucketIndex);
    
    // Traverse list until entry->Key == hash or we run out of entries
    while (entry && entry->Key != hash)
    {
      entry = entry->Next;
    }
    return entry;
  }
  return NULL;
}

const char* HashedStringMap_GetString(HashedStringMap_t* inMap, const HashedString_t* hashedString)
{
  if (inMap && hashedString)
  {
    HashedStringEntry_t* entry = HashedStringMap_Find(inMap, hashedString
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
