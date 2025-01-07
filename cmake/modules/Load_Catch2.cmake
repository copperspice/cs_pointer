# ***********************************************************************
#
# Copyright (c) 2023-2025 Barbara Geller
# Copyright (c) 2023-2025 Ansel Sermersheim
#
# This file is part of CsPointer.
#
# CsPointer is free software which is released under the BSD 2-Clause license.
# For license details refer to the LICENSE provided with this project.
#
# CsPointer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# https://opensource.org/licenses/BSD-2-Clause
#
# ***********************************************************************

find_package(Catch2 QUIET)

if (NOT TARGET Catch2::Catch2)
   message(STATUS "Catch2 was not found, CsPointer unit tests will not be built\n")
   return()
endif()
