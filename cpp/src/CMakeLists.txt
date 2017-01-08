set(CMAKE_CXX_STANDARD 14)

find_package(JNI REQUIRED)

add_library(myhtml4jnative SHARED myjni.h myjni.cpp myhtml4jnative.h myhtml4jnative.cpp)

target_include_directories(myhtml4jnative PUBLIC ${JNI_INCLUDE_DIRS})
target_link_libraries(myhtml4jnative ${JNI_LIBRARIES}) # myhtml_static
