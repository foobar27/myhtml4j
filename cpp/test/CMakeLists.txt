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

enable_testing()

find_package(Boost REQUIRED COMPONENTS system filesystem unit_test_framework)

set(CMAKE_CXX_STANDARD 14)
#set (CMAKE_CXX_FLAGS "--std=c++14 ${CMAKE_CXX_FLAGS}")

add_executable(jniTest jniTest.cpp)

# TODO should not be necessary?
INCLUDE_DIRECTORIES(../src)

target_compile_definitions(jniTest PUBLIC BOOST_TEST_DYN_LINK) 
target_link_libraries(jniTest myhtml4jnative ${Boost_LIBRARIES})
add_test(jniTest jniTest)
