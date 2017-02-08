#include "myjni.h"

JClass::JClass(jclass id)
    : m_id(id)
{}

jclass JClass::id() const {
    return m_id;
}

jstring stringToJni(JNIEnv *env, const std::string & t) {
    return env->NewStringUTF(t.c_str());
}

jstring charArrayToJni(JNIEnv *env, const char* t) {
    if (t) {
        return env->NewStringUTF(t);
    } else {
        return nullptr;
    }
}
