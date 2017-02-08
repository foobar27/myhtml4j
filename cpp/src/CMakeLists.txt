set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")

include(GenerateExportHeader)

find_package(JNI REQUIRED)

add_library(myhtml4jnative SHARED myjni.h myjni.cpp myhtml4jnative.h myhtml4jnative.cpp atoms.cpp atoms.h)
add_dependencies(myhtml4jnative myhtmlbuild)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic") # TODO only for this library?

set(GENERATED_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIRECTORY})
set(AttributeKeysCpp ${GENERATED_DIRECTORY}/attribute_keys.h)
set(AttributeKeysGperfFile ${CMAKE_CURRENT_SOURCE_DIR}/../scripts/attributes.gperf)

add_custom_command(
    OUTPUT ${AttributeKeysCpp}
    COMMAND gperf ${AttributeKeysGperfFile} > ${AttributeKeysCpp}
    DEPENDS ${AttributeKeysGperfFile}
    COMMENT "Transforming attributes.gperf"
)

add_custom_target(AttributeKeysGenerator DEPENDS ${AttributeKeysCpp}
                  COMMENT "Checking if re-generation of AttributeKeysCpp is required" )

add_dependencies(myhtml4jnative AttributeKeysGenerator)

set(MyHTML_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/../vendor/include)
set(MyHTML_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/../vendor/lib/libmyhtml_static.a)

target_include_directories(myhtml4jnative
    PUBLIC ${JNI_INCLUDE_DIRS} ${MyHTML_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(myhtml4jnative PUBLIC ${JNI_LIBRARIES} ${MyHTML_LIBRARY})

generate_export_header(myhtml4jnative
    EXPORT_FILE_NAME ${GENERATED_DIRECTORY}/myhtml4jnative_export.h)
