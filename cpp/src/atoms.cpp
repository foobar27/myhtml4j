#include "atoms.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include "attribute_keys.h"
#pragma GCC diagnostic pop
#include "myjni.h"
#include <cstring>

uint32_t attribute_key_to_id(const char* s, unsigned int len) {
    auto result = AttributeKeyLookup::in_word_set(s, len);
    if (result) {
        return result->id;
    }
    return -1;
}

AttributeKey AttributeKeyCache::get(const char* c_name) {
    using namespace std;
    auto length = strlen(c_name);
    auto id = attribute_key_to_id(c_name, length);
    if (id >= 0) {
        return {id, nullptr};
    } else {
        string name {c_name, length};
        auto name_it = m_cache.find(name);
        if (name_it != m_cache.end()) {
            return {id, name_it->second};
        } else {
            auto js = ToJniType<std::string>::toJni(m_env, name);
            m_cache[name] = js;
            return {id, js};
        }
    }
}
