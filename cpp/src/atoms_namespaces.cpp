#include "atoms.cpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#include "namespaces.h"
#pragma GCC diagnostic pop

Namespace NamespaceCache::get(const char* c_name) {
  return baseGet<Namespace, NamespaceLookup>(*this, c_name);
}
