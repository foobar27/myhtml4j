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

#include <cinttypes>
#include <sstream>
#include <string>

#include <jni.h>

#include "myjni.h"

jstring stringToJni(JNIEnv *env, const std::string & t);

jstring charArrayToJni(JNIEnv *env, const char* t);

struct JObject {
    jobject id() const {
        return m_id;
    }
    JObject(jobject id)
        : m_id(id)
    {}
private:
    jobject m_id;
};

struct JMethodBase {
    JMethodBase(jmethodID id)
        : m_id(id)
    {}

    jmethodID id() const {
        return m_id;
    }

private:
    jmethodID m_id;
};

// TODO also type by Arguments?
template<class ReturnType>
struct JMethod;

template<>
struct JMethod<void> : JMethodBase {

    JMethod(jmethodID id)
        : JMethodBase(id)
    {}

    template<class... Args>
    void operator()(JNIEnv *env, JObject object, Args... args) const {
        env->CallVoidMethod(object.id(), id(), args...); // TODO transform args
    }

private:
};

struct JClass {

    JClass(jclass id);
    jclass id() const;

    static JClass load(JNIEnv* env, const char * name) {
        auto id = env->FindClass(name);
        if (!id) {
            throw 0; // TODO
        }
        return {id};
    }

    JClass globalRef(JNIEnv* env) {
        return { (jclass) env->NewGlobalRef(m_id) };
    }

    template<class ReturnType>
    JMethod<ReturnType> getMethod(JNIEnv *env, const std::string & name, const std::string & signature) {
        jmethodID id = env->GetMethodID(
                    m_id,
                    name.c_str(),
                    signature.c_str());
        return { id };
    }

private:
    jclass m_id;
};
