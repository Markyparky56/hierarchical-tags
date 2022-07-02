#include "HashedStringMap.h"
#include <stdio.h>

int main(int argc, const char** argv)
{
  HString myFirstString = HashedString_Create("MyFirstString");
  const char* myStringReturned = HashedString_GetString(&myFirstString);

  printf(myStringReturned);

  return 0;
}
