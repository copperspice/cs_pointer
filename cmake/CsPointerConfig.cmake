# ***********************************************************************
#
# Copyright (c) 2023-2023 Barbara Geller
# Copyright (c) 2023-2023 Ansel Sermersheim
#
# This file is part of CsPointer.
#
# CsPointer is free software, released under the BSD 2-Clause license.
# For license details refer to LICENSE provided with this project.
#
# CsPointer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# https://opensource.org/licenses/BSD-2-Clause
#
# ***********************************************************************

if(CsPointer_FOUND)
   return()
endif()

set(CsPointer_FOUND TRUE)

# figure out install path
get_filename_component(CsPointer_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(CsPointer_PREFIX ${CsPointer_CMAKE_DIR}/ ABSOLUTE)

# library dependencies (contains definitions for imported targets)
include("${CsPointer_CMAKE_DIR}/CsPointerLibraryTargets.cmake")

# imported targets  INCLUDE_DIRECTORIES
get_target_property(CsPointer_INCLUDES  CsPointer INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(CsPointer_LIBRARIES CsPointer LOCATION)

# export include base dir, imported in other projects
set(CsPointer_INCLUDE_DIR  "@CMAKE_INSTALL_PREFIX@/@CMAKE_INSTALL_INCLUDEDIR@")
