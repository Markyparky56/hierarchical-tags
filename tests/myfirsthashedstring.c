#include "HashedStringMap.h"
#include <stdio.h>

int main(int argc, const char** argv)
{
  HString myFirstString = HashedString_Create("MyFirstString");
  const char* myStringReturned = HashedString_GetString(&myFirstString);

  printf("%s\n", myStringReturned);

  HString mySecondString = HashedString_Create("MySecondString");
  const char* mySecondStringReturned = HashedString_GetString(&mySecondString);

  printf("%s\n", mySecondStringReturned);

  HString myFirstStringButLowercase = HashedString_Create("myfirststring");

  printf("Comparing myFirstString with mySecondString: %d\n", HashedString_Compare(&myFirstString, &mySecondString));

  printf("Comparing myFirstString with myFirstStringButLowercase: %d\n", HashedString_Compare_WithSensitivity(&myFirstString, &myFirstStringButLowercase, HSCS_Insensitive));

  return 0;
}
