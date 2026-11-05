#include "myJni.h"

#define KEY_SIZE 9997
#define HASH_SIZE 97
#define HASHING(a) (a % HASH_SIZE)

typedef struct HashNode {
  char value[17];
  struct HashNode *next;
} HashNode;

static HashNode* hashTab[HASH_SIZE];

unsigned int
generateKey (char *value) {
  unsigned int key = 0;
  
  while (*value) {  
    key = (key << 3) + *value++;
  }
  return key % KEY_SIZE;
}

HashNode*
createNode (void) {
  HashNode *node = malloc (sizeof (HashNode));
  node->next = NULL;
  return node;
}

void
createHashTable (void) {
  int i;
  for (i=0; i<HASH_SIZE; i++) {
    hashTab[i] = NULL;
  }
}

void 
destroyHashTable (void) {
  int i;
  HashNode *ptr, *prev = NULL;

  for (i=0; i<HASH_SIZE; i++) {
    for (ptr=hashTab[i]; ptr!=NULL; ptr=ptr->next) {
      free (prev);
      prev = ptr;
    }
  }
  free (prev);
}

int 
insertKeyValue (int key, char *value) {
  HashNode *node = createNode ();
  int hkey = HASHING(key);

  node->next = hashTab[hkey];
  hashTab[hkey] = node;
  strcpy (node->value, value);
}

char* 
searchKey (int key) {
  int target = HASHING(key);
  HashNode *ptr;
  
  for (ptr=hashTab[target]; ptr!=NULL; ptr=ptr->next) {
    if (key == generateKey (ptr->value)) {
      char* value = malloc (sizeof(char) * 17);
      strncpy (value, ptr->value, 17);
      value[16] = '\0';
      return value;
    }
  }
  return NULL;
}

int 
deleteKey (int key) {
  int target = HASHING(key);
  HashNode *ptr, *prev = NULL;
  for (ptr=hashTab[target]; ptr!=NULL; ptr=ptr->next) {
    if (key == generateKey (ptr->value)) {
      if (prev == NULL) hashTab[target] = ptr->next;
      else prev->next = ptr->next;
      free (ptr);
      return 1;
    }
    else prev = ptr;
  }
  return 0;
}