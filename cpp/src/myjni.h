#pragma once

#include <cinttypes>
#include <sstream>
#include <string>

#include <jni.h>

#include "myjni.h"

template<class T>
struct ToJniType {};

template<>
struct ToJniType<bool> {
    using type = bool;
    using jniType = jboolean;
    // ...
};

template<class T, class JniT>
struct OneToOneJniType {
    using type = T;
    using jniType = JniT;

    static jniType toJni(JNIEnv *env, const type & t) {
        return t;
    }

};

template<>
struct ToJniType<std::int8_t> : OneToOneJniType<std::int8_t, jbyte> {
    static constexpr auto signature() {
        return "B";
    }
};

template<>
struct ToJniType<std::uint16_t> : OneToOneJniType<std::uint16_t, jchar> {
    static constexpr auto signature() {
        return "C";
    }
};

template<>
struct ToJniType<std::int16_t> : OneToOneJniType<std::int16_t, jshort> {
    static constexpr auto signature() {
        return "S";
    }
};

template<>
struct ToJniType<std::int32_t> : OneToOneJniType<std::int32_t, jint> {
    static constexpr auto signature() {
        return "I";
    }
};

template<>
struct ToJniType<std::int64_t> : OneToOneJniType<std::int64_t, jlong> {
    static constexpr auto signature() {
        return "J";
    }
};

template<>
struct ToJniType<float> : OneToOneJniType<float, jfloat> {
  static constexpr auto signature() {
      return "F";
  }
};

template<>
struct ToJniType<double> : OneToOneJniType<double, jdouble> {
    static constexpr auto signature() {
        return "D";
    }
};

namespace impl {

template<class Arg, class... Args>
auto argsSignatureSS(std::stringstream & ss, Arg arg, Args... args) {
    ss << ToJniType<Arg>::signature();
    return argsSignatureSS(ss, args...);
}

template<class ReturnArg, class... Args>
auto methodSignature(ReturnArg returnArg, Args... args) {
    std::stringstream ss;
    ss << "(";
    argsSignatureSS(ss, args...);
    ss << ")" << ToJniType<ReturnArg>::signature();
    return ss.str();
}

}

template<>
struct ToJniType<std::string> {
    using type = std::string;
    using jniType = jstring;

    static jniType toJni(JNIEnv *env, const type & t) {
        return env->NewStringUTF(t.c_str());
    }

    static constexpr auto signature() {
        return "Ljava/lang/String;";
    }

};

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
