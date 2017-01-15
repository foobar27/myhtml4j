#pragma once

#include "jni.h"
#include "myhtml4jnative_export.h"

extern "C" {
/*
 * Class:     com_github_foobar27_myhtml4j_Native
 * Method:    newContext
 * Signature: (Ljava/lang/Class;)J
 */
JNIEXPORT MYHTML4JNATIVE_EXPORT jlong JNICALL Java_com_github_foobar27_myhtml4j_Native_newContext
  (JNIEnv *, jclass, jclass);

/*
 * Class:     com_github_foobar27_myhtml4j_Native
 * Method:    deleteContext
 * Signature: (J)V
 */
JNIEXPORT MYHTML4JNATIVE_EXPORT void JNICALL Java_com_github_foobar27_myhtml4j_Native_deleteContext
  (JNIEnv *, jclass, jlong);


/*
 * Class:     com_github_foobar27_myhtml4j_Native
 * Method:    parseUTF8
 * Signature: (JLjava/lang/String;Lcom/github/foobar27/myhtml4j/Native/NativeCallBack;)V
 */
JNIEXPORT MYHTML4JNATIVE_EXPORT void JNICALL Java_com_github_foobar27_myhtml4j_Native_parseUTF8
  (JNIEnv *, jclass, jlong, jstring, jobject);
}
