/***********************************************************************
*
* Copyright (c) 2023-2023 Barbara Geller
* Copyright (c) 2023-2023 Ansel Sermersheim
*
* This file is part of CsPointer.
*
* CsPointer is free software, released under the BSD 2-Clause license.
* For license details refer to LICENSE provided with this project.
*
* CsPointer is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#include <cs_weak_pointer.h>

#include <cs_catch2.h>

TEST_CASE("CsWeakPointer traits", "[cs_weakpointer]")
{
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsWeakPointer<int>> == true);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsWeakPointer<int>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsWeakPointer<int>> == true);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsWeakPointer<int>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsWeakPointer<int>> == false);
}

TEST_CASE("CsWeakPointer clear", "[cs_weakpointer]")
{
   CsPointer::CsSharedPointer<int> ptr = CsPointer::make_shared<int>();
   CsPointer::CsWeakPointer<int> weakPointer = ptr.toWeakRef();

   weakPointer.clear();

   REQUIRE(weakPointer == nullptr);
   REQUIRE(weakPointer.is_null() == true);
}

TEST_CASE("CsWeakPointer nullptr", "[cs_weakpointer]")
{
   CsPointer::CsSharedPointer<int> sharedPtr = CsPointer::make_shared<int>();
   CsPointer::CsWeakPointer<int> weakPtr = sharedPtr.toWeakRef();

   REQUIRE(sharedPtr == weakPtr);
   REQUIRE(weakPtr == sharedPtr);

   REQUIRE(sharedPtr == weakPtr.lock());
   REQUIRE(sharedPtr == weakPtr.toStrongRef());

   REQUIRE(sharedPtr != nullptr);
   REQUIRE(sharedPtr.is_null() == false);

   REQUIRE(static_cast<bool>(sharedPtr) == true);
   REQUIRE(static_cast<bool>(weakPtr) == true);

   {
      CsPointer::CsSharedPointer<int> tmp(weakPtr);

      REQUIRE(sharedPtr == tmp);

      tmp = weakPtr;

      REQUIRE(sharedPtr == tmp);
   }
   sharedPtr.reset();

   REQUIRE(static_cast<bool>(sharedPtr) == false);
   REQUIRE(static_cast<bool>(weakPtr) == false);

   REQUIRE(sharedPtr == nullptr);
   REQUIRE(sharedPtr.is_null() == true);

   REQUIRE(weakPtr == nullptr);
   REQUIRE(weakPtr.is_null() == true);

   REQUIRE(sharedPtr != weakPtr);   // unusual but accurate
   REQUIRE(weakPtr != sharedPtr);

   REQUIRE(sharedPtr == weakPtr.lock());
   REQUIRE(weakPtr.lock() == sharedPtr);

   REQUIRE(sharedPtr == weakPtr.toStrongRef());
   REQUIRE(weakPtr.toStrongRef() == sharedPtr);
}

