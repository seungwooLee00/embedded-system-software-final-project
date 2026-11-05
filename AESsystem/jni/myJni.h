#include <jni.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <semaphore.h>

#include "android/log.h"

#define LOG_TAG "MyTag" 
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

#define EMPTY_LINE "                "
 
char* fpgaReadText (void);
int fpgaReadNumber (void);
void fpgaSetLcdFirst (char *msg);
void fpgaSetLcdSecond (char *msg);
void fpgaSetFnd (int value);
void fpgaSetDot (int value);
void fpgaCountNonBlock (int sec);
void fpgaCountWithBlock (int sec);
void fpgaClearDevice (void);

char* doCipher (char *);
char* doReverseCipher (char *);

void createHashTable (void);
void destroyHashTable (void);
int insertKeyValue (int key, char *value); 
char* searchKey (int key);
int deleteKey (int key);

JNIEXPORT jboolean JNICALL Java_com_example_aessystem_DeleteService_deleteKeyValue
  (JNIEnv *, jobject, jint);

JNIEXPORT jstring JNICALL Java_com_example_aessystem_InsertService_getValueFromSwitch
  (JNIEnv *, jobject);

JNIEXPORT jint JNICALL Java_com_example_aessystem_InsertService_encode
  (JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL Java_com_example_aessystem_MainActivity_clearDevice
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_com_example_aessystem_MainActivity_displayModeOnLcd
  (JNIEnv *, jobject, jint);

JNIEXPORT void JNICALL Java_com_example_aessystem_MainActivity_createHashTable
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_com_example_aessystem_MainActivity_destroyHashTable
  (JNIEnv *, jobject);

JNIEXPORT jint JNICALL Java_com_example_aessystem_SearchService_getKeyFromSwitch
  (JNIEnv *, jobject);

JNIEXPORT jstring JNICALL Java_com_example_aessystem_SearchService_decode
  (JNIEnv *, jobject, jint);

JNIEXPORT void JNICALL Java_com_example_aessystem_Semaphore_initSema
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_com_example_aessystem_Semaphore_postSema
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_com_example_aessystem_Semaphore_waitSema
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_com_example_aessystem_Semaphore_delSema
  (JNIEnv *, jclass);

