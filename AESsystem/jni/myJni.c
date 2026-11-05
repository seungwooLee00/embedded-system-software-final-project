#include "myJni.h"

JNIEXPORT jint JNICALL
Java_com_example_aessystem_DeleteService_getKeyFromSwitch (JNIEnv *env, jobject this) {
  return fpgaReadNumber ();
}

JNIEXPORT jboolean JNICALL 
Java_com_example_aessystem_DeleteService_deleteKeyValue (JNIEnv *env, jobject this, jint key) {
  jboolean ret;
  if (deleteKey (key)) {
    ret = JNI_TRUE;
    fpgaSetLcdSecond ("data deleted");
  }
  else {
    ret = JNI_FALSE;
    fpgaSetLcdSecond ("no such key");
  }
  fpgaCountWithBlock (3);
  fpgaSetLcdSecond (EMPTY_LINE);
  fpgaSetFnd (0);
  fpgaSetDot (-1);
  
  return ret;
}

JNIEXPORT jstring JNICALL 
Java_com_example_aessystem_InsertService_getValueFromSwitch (JNIEnv *env, jobject this) {
  char *str = fpgaReadText ();
  if (str == NULL) return NULL;

  jstring ret = (*env)->NewStringUTF(env, str);
  free (str);
  return ret;
}

JNIEXPORT jint JNICALL 
Java_com_example_aessystem_InsertService_encode (JNIEnv *env, jobject this, jstring _plain) {
  char *plain = (*env)->GetStringUTFChars(env, _plain, NULL);
  char *cipher = doCipher (plain);
  int key = generateKey (cipher);
  LOGV("key: %d", key);

  fpgaSetFnd (key);
  fpgaCountWithBlock (3);
  fpgaSetFnd (0);
  fpgaSetDot (-1);
  fpgaSetLcdSecond (EMPTY_LINE);

  insertKeyValue (key, cipher);
  free (cipher);
  
  return key;
}

JNIEXPORT void JNICALL 
Java_com_example_aessystem_MainActivity_clearDevice (JNIEnv *env, jclass cls) {
  fpgaClearDevice ();
}

JNIEXPORT void JNICALL 
Java_com_example_aessystem_MainActivity_displayModeOnLcd (JNIEnv *env, jobject this, jint mode) {
  jclass cls = (*env)->GetObjectClass(env, this);
  jmethodID mid = (*env)->GetMethodID(env, cls, "getModeMsg", "(I)Ljava/lang/String;");
  jstring msg = (*env)->CallObjectMethod(env, this, mid, mode);
  char *buf = (*env)->GetStringUTFChars(env, msg, NULL);
  fpgaSetLcdFirst (buf);
  fpgaSetLcdSecond (EMPTY_LINE);
  (*env)->ReleaseStringUTFChars(env, msg, buf);
}

JNIEXPORT void JNICALL 
Java_com_example_aessystem_MainActivity_createHashTable (JNIEnv *env, jobject this) {
  createHashTable ();
}

JNIEXPORT void JNICALL 
Java_com_example_aessystem_MainActivity_destroyHashTable (JNIEnv *env, jobject this) {
  destroyHashTable ();
}

JNIEXPORT jint JNICALL 
Java_com_example_aessystem_SearchService_getKeyFromSwitch (JNIEnv *env, jobject this) {
  return fpgaReadNumber ();
}

JNIEXPORT jstring JNICALL 
Java_com_example_aessystem_SearchService_getKeyValue (JNIEnv *env, jobject this, jint key) {
  char *value = searchKey (key);
  jstring ret = (*env)->NewStringUTF(env, value);
  free (value);
  fpgaSetFnd (0);
  return ret;
}

JNIEXPORT jstring JNICALL 
Java_com_example_aessystem_SearchService_decode (JNIEnv *env, jobject this, jint key) {
  char *cipher = searchKey (key);
  char *plain;
  int i;
  jstring ret;

  if (cipher == NULL) {
    ret = NULL;
    fpgaSetLcdSecond ("no such key");
    
  }
  else {
    plain = doReverseCipher (cipher);
    for (i=0; i<16; i++) {
      if (plain[i] == '$') plain[i] = ' ';
    }
    ret = (*env)->NewStringUTF(env, plain);
    fpgaSetLcdSecond (plain);
  }

  fpgaCountWithBlock (3);
  fpgaSetFnd (0);
  fpgaSetDot (-1);
  fpgaSetLcdSecond (EMPTY_LINE);

  free (cipher);
  free (plain);

  return ret;
}
