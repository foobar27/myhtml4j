# This file is part of the myhtml4j distribution.
#   (https://github.com/foobar27/myhtml4j)
# Copyright (c) 2017 Sebastien Wagener.
#
# myhtml4j is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, version 3.
#
# myhtml4j is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

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

set_target_properties(myhtml4jnative PROPERTIES
    VERSION ${myhtml4jnative_VERSION_AS_STRING}
    SOVERSION ${myhtml4jnative_VERSION_AS_STRING})

set(MyHTML_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/../vendor/include)
set(MyHTML_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/../vendor/lib/libmyhtml_static.a)

target_include_directories(myhtml4jnative
    PUBLIC ${JNI_INCLUDE_DIRS} ${MyHTML_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(myhtml4jnative PUBLIC ${JNI_LIBRARIES} ${MyHTML_LIBRARY})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../java/src/main/resources/lib)
add_custom_command(TARGET myhtml4jnative
    POST_BUILD
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/libmyhtml4jnative.so ${CMAKE_CURRENT_SOURCE_DIR}/../../java/src/main/resources/lib/linux-amd64)

generate_export_header(myhtml4jnative
    EXPORT_FILE_NAME ${GENERATED_DIRECTORY}/myhtml4jnative_export.h)

install(TARGETS myhtml4jnative
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)

set(CPACK_PACKAGE_NAME "myhtml4jnative")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "myhtml4j native library")
set(CPACK_PACKAGE_VERSION ${myhtml4jnative_VERSION_AS_STRING})
set(CPACK_PACKAGE_VERSION_MAJOR "${myhtml4jnative_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${myhtml4jnative_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${myhtml4jnative_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "myhtml4j native library")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sebastien Wagener")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "java-runtime-headless (>= 1.8)")

# This must always be last!
include(CPack)
