cmake_minimum_required (VERSION 3.5)
project(myhtml4jnative)

set(myhtml4jnative_VERSION_MAJOR 0)
set(myhtml4jnative_VERSION_MINOR 1)
set(myhtml4jnative_VERSION_PATCH 0)
set(myhtml4jnative_VERSION_AS_STRING "${myhtml4jnative_VERSION_MAJOR}.${myhtml4jnative_VERSION_MINOR}.${myhtml4jnative_VERSION_PATCH}")

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(test)
