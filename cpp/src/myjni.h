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
