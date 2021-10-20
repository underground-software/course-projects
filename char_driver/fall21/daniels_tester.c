#include <stdio.h>
#include <stdlib.h>

// Loads a bullet into the chamber, returns result from driver
int reload(FILE* driver);


// Shoots the current bullet from the chamber, returns result from driver
int shoot(FILE* driver);


// Check ammunition in chamber, return number of bullets or -2 on EOF error, -1 on driver not found error
int ammo(FILE* driver);


// Test loading 3 bullets, returns 0 if successful
int testReload(FILE* driver);


// Test shooting 3 bullets, returns 0 if successful
int testShoot(FILE* driver);


// Test usage with normal revolver conditions, returns 0 if successful
int testNormal(FILE* driver);


// Test usage with edge cases, returns 0 if successful
int testEdge(FILE* driver);

int main(){
  const char* address = "/dev/revolver";
  FILE * driver;
  driver = fopen(address, "r");
  if(driver == NULL){
    printf("Error in accessing file.\n");
    exit(1);
  }

  int result = 0;

  result = testNormal(driver);
  if(result != 0){
    printf("Error in normal conditions test.\n");
    exit(1);
  }

  result = testEdge(driver);
  if(result != 0){
    printf("Error in edge conditions test.\n");
    exit(1);
  }

  printf("Success in all tests.\n");
  fclose(driver);
  return 0;
}

int ammo(FILE* driver){
  if(driver == NULL)
    return -1;
  char c;
  c = fgetc(driver);
  switch(c){
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  default:
    return -2;
  }
  return -3; // Should never execute
}


int shoot(FILE* driver){
  if(driver == NULL)
    return -1;

  int result = fputc('s', driver);
  if(result == EOF)
    return -2;

  return 0;
}


int reload(FILE* driver){
  if(driver == NULL)
    return -1;

  int result = fputc('l', driver);
  if(result == EOF)
    return -2;

  return 0;
}


int testReload(FILE* driver){
  if(driver == NULL)
    return -1;

  int result = 0;
  for(int i = 0; i < 3; i++){
    result = reload(driver);
    if(result != 0)
      return result;
  }
  return 0;
}


int testShoot(FILE* driver){
  if(driver == NULL)
    return -1;

  int result = 0;
  for(int i = 0; i < 3; i++){
    result = shoot(driver);
    if(result != 0)
      return result;
  }
  return 0;
}


int testNormal(FILE* driver){
  if(driver == NULL)
    return -1;

  int result = 0;

  result = ammo(driver);
  if(result != 0)
    return -2;

  result = testReload(driver);
  if(result != 0)
    return result;

  result = ammo(driver);
  if(result != 3)
    return -2;

  result = testShoot(driver);
  if(result != 0)
    return result;

  result = ammo(driver);
  if(result != 0)
    return -2;

  return 0;
}


int testEdge(FILE* driver){
  if(driver == NULL)
    return -1;

  int result = 0;

  for(int i = 0; i < 6; i++){
    result = reload(driver);
    if(result != 0)
      return -2;
  }

  result = reload(driver);
  if(result == 0)
    return -2;

  for(int i = 0; i < 6; i++){
    result = shoot(driver);
    if(result != 0)
      return -2;
  }

  result = shoot(driver);
  if(result == 0)
    return -2;

  return 0;
}
