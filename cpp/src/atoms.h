#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <jni.h>

struct AttributeKey {
    uint32_t id;
    jstring name;
};

struct AttributeKeyCache {

    AttributeKeyCache(JNIEnv* env)
        : m_env(env)
    {}

    AttributeKey get(const char* name);

private:
    JNIEnv* m_env;
    std::unordered_map<std::string, jstring> m_cache;
};
