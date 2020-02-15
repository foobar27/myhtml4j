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
#include "myjni.h"
#include <cstring>
#include <unordered_map>

template<typename K, typename L>
K baseGet(BaseCache & cache, const char* c_name, size_t nameLength) {
  using namespace std;
  if (!c_name) {
    c_name = "";
  }
  auto lookup = L::in_word_set(c_name, nameLength);
  if (lookup) {
    return {lookup->id, nullptr};
  } else {
    string name {c_name, nameLength};
    auto name_it = cache.m_cache.find(name);
    if (name_it != cache.m_cache.end()) {
      return {-1, name_it->second};
    } else {
      auto js = stringToJni(cache.m_env, name);
      cache.m_cache[name] = js;
      return {-1, js};
    }
  }
}

// Unfortunately gperf headers can not be included twice in the same file
// So we create a C++ file for each: atoms_attributekeys.cpp and atoms_namespaces.cpp
