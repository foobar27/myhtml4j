#pragma once

#include "jni.h"

jlong newContext(JNIEnv *env, jclass callbackClass);
void deleteContext(JNIEnv *env, jlong context);

// TODO parse flags
void parseUTF8(JNIEnv *env, jstring s, jobject callback);
