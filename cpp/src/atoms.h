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

#include <cstdint>
#include <unordered_map>
#include <string>
#include <jni.h>

struct AttributeKey {
    int32_t id;
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
