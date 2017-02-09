/*
 * This file is part of the myhtml4j distribution.
 *   (https://github.com/foobar27/myhtml4j)
 * Copyright (c) 2017 Sebastien Wagener.
 *
 * myhtml4j is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * myhtml4j is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
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

JNIEXPORT MYHTML4JNATIVE_EXPORT jstring JNICALL Java_com_github_foobar27_myhtml4j_Native_html2textUTF8
  (JNIEnv *, jclass, jlong, jstring);

}
