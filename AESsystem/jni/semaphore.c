#include "myJni.h"

static sem_t sem;

JNIEXPORT void JNICALL
Java_com_example_aessystem_Semaphore_initSema (JNIEnv *env, jclass this) {
  if (sem_init (&sem, 0, 1) != 0) {
    LOGV("fail to initialize semaphore");
    return ;
  }
  LOGV("success to initialize semaphore");
}

JNIEXPORT void JNICALL 
Java_com_example_aessystem_Semaphore_postSema (JNIEnv *env, jclass this) {
  if (sem_post (&sem) != 0) {
    LOGV("fail to post semaphore");
  }
  LOGV("semaphore posted");
}

JNIEXPORT void JNICALL
Java_com_example_aessystem_Semaphore_waitSema (JNIEnv *env, jclass this) {
  if (sem_wait (&sem) != 0) {
    LOGV("fail to wait semaphore");
  }
  LOGV("semaphore waited");
}

JNIEXPORT void JNICALL
Java_com_example_aessystem_Semaphore_delSema (JNIEnv *env, jclass this) {
  if (sem_destroy (&sem) != 0) {
    LOGV ("fail to destroy semaphore");
  }
  LOGV("semaphore destroyed");
}