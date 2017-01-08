set(CMAKE_CXX_STANDARD 14)

find_package(JNI REQUIRED)

add_library(myhtml4jnative SHARED myjni.h myjni.cpp myhtml4jnative.h myhtml4jnative.cpp)
add_dependencies(myhtml4jnative myhtmlbuild)

set(MyHTML_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/../vendor/include)
set(MyHTML_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/../vendor/lib/libmyhtml_static.a)

target_include_directories(myhtml4jnative
    PUBLIC ${JNI_INCLUDE_DIRS} ${MyHTML_INCLUDE_DIRS})
target_link_libraries(myhtml4jnative PUBLIC ${JNI_LIBRARIES} ${MyHTML_LIBRARY})
