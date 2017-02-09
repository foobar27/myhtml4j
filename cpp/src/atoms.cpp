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
#include "atoms.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include "attribute_keys.h"
#pragma GCC diagnostic pop
#include "myjni.h"
#include <cstring>

int32_t attribute_key_to_id(const char* s, unsigned int len) {
    auto result = AttributeKeyLookup::in_word_set(s, len);
    if (result) {
        return result->id;
    }
    return -1;
}

AttributeKey AttributeKeyCache::get(const char* c_name) {
    using namespace std;
    if (!c_name) {
        c_name = "";
    }
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
            auto js = stringToJni(m_env, name);
            m_cache[name] = js;
            return {id, js};
        }
    }
}
