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

install(TARGETS myhtml4jnative
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION bin)

set(CPACK_PACKAGE_NAME "myhtml4jnative")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "myhtml4j native library")
set(CPACK_PACKAGE_VERSION "0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "myhtml4j native library")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastien Wagener")

# This must always be last!
include(CPack)
