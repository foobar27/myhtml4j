#include "atoms.cpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include "attribute_keys.h"
#pragma GCC diagnostic pop

AttributeKey AttributeKeyCache::get(const char* c_name) {
  return baseGet<AttributeKey, AttributeKeyLookup>(*this, c_name);
}

