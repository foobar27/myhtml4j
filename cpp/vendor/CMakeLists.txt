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

include(ExternalProject)

ExternalProject_Add(
    lexborbuild
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lexbor/
    PREFIX "lexbor/project"
    CMAKE_CACHE_ARGS -DPROJECT_OPTIMIZATION_LEVEL:STRING=-O3 -DLEXBOR_BUILD_SHARED:STRING=OFF -DPROJECT_INSTALL_WITHOUT_HEADERS:STRING=YES -DMyCORE_BUILD_WITHOUT_THREADS:STRING=YES -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}
    BUILD_IN_SOURCE 0
    GIT_SUBMODULES vendor/lexbor
# INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}
)
