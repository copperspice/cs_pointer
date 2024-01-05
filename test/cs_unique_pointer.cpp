/***********************************************************************
*
* Copyright (c) 2023-2024 Barbara Geller
* Copyright (c) 2023-2024 Ansel Sermersheim
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

#include <cs_unique_pointer.h>

#include <cs_catch2.h>

TEST_CASE("CsUniquePointer traits", "[cs_uniquepointer]")
{
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsUniquePointer<int>> == false);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsUniquePointer<int>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsUniquePointer<int>> == false);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsUniquePointer<int>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsUniquePointer<int>> == false);
}

TEST_CASE("CsUniquePointer convert", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr1 = CsPointer::make_unique<int>(42);

   std::unique_ptr<int> ptr2 = std::move(ptr1);

   REQUIRE(ptr1 == nullptr);
   REQUIRE(ptr2 != nullptr);

   REQUIRE(*ptr2 == 42);
}

TEST_CASE("CsUniquePointer custom_deleter", "[cs_uniquepointer]")
{
   bool deleterExecuted = false;

   {
      auto customDeleter = [&deleterExecuted] (int *obj) {
         deleterExecuted = true;
         delete obj;
      };

      CsPointer::CsUniquePointer<int, decltype(customDeleter)> ptr(new int, customDeleter);
      REQUIRE(deleterExecuted == false);
   }

   REQUIRE(deleterExecuted == true);
}

TEST_CASE("CsUniquePointer empty", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr;

   REQUIRE(ptr == nullptr);
   REQUIRE(nullptr == ptr);
   REQUIRE(ptr == ptr);

   REQUIRE(! (ptr != nullptr));
   REQUIRE(! (nullptr != ptr));
   REQUIRE(! (ptr != ptr)) ;

   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsUniquePointer equality", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr1 = CsPointer::make_unique<int>();
   CsPointer::CsUniquePointer<int> ptr2(nullptr);

   REQUIRE((ptr1 == ptr2) == false);
   REQUIRE((ptr1 != ptr2) == true);

   REQUIRE((ptr1 == ptr2.get()) == false);
   REQUIRE((ptr1.get() == ptr2) == false);

   REQUIRE((ptr1 != ptr2.get()) == true);
   REQUIRE((ptr1.get() != ptr2) == true);
}

TEST_CASE("CsUniquePointer move_assign", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr1;
   int *rawPointer = nullptr;

   {
      CsPointer::CsUniquePointer<int> ptr2(new int);
      rawPointer = ptr2.data();
      ptr1 = std::move(ptr2);

      REQUIRE(ptr2.is_null());
   }

   REQUIRE(rawPointer == ptr1.get());
}

TEST_CASE("CsUniquePointer move_construct", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr1 = CsPointer::make_unique<int>();
   CsPointer::CsUniquePointer<int> ptr2(std::move(ptr1));

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr2.is_null() == false);
}

TEST_CASE("CsUniquePointer release", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr = CsPointer::make_unique<int>();
   int *p1 = ptr.get();
   int *p2 = ptr.release();

   REQUIRE(p1 == p2);
   REQUIRE(ptr == nullptr);

   delete p2;

   REQUIRE(ptr.release() == nullptr);
}

TEST_CASE("CsUniquePointer reset", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr = CsPointer::make_unique<int>();
   int *rawPtr = ptr.get();

   ptr.reset(rawPtr);
   REQUIRE(ptr == rawPtr);

   ptr.reset();

   REQUIRE(ptr == nullptr);
   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsUniquePointer swap", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr1 = CsPointer::make_unique<int>(8);
   CsPointer::CsUniquePointer<int> ptr2 = CsPointer::make_unique<int>(17);

   REQUIRE(*ptr1 == 8);
   REQUIRE(*ptr2 == 17);

   ptr1.swap(ptr2);

   REQUIRE(*ptr1 == 17);
   REQUIRE(*ptr2 == 8);

   ptr1.reset();
   ptr1.swap(ptr2);

   REQUIRE(*ptr1 == 8);
   REQUIRE(ptr2 == nullptr);

   ptr1.swap(ptr1);

   REQUIRE(*ptr1 == 8);
   REQUIRE(ptr2 == nullptr);

   ptr2.swap(ptr2);

   REQUIRE(*ptr1 == 8);
   REQUIRE(ptr2 == nullptr);
}

TEST_CASE("CsUniquePointer take", "[cs_uniquepointer]")
{
   CsPointer::CsUniquePointer<int> ptr1 = CsPointer::make_unique<int>(42);
   CsPointer::CsUniquePointer<int> ptr2(ptr1.take());

   REQUIRE(ptr1 == nullptr);
   REQUIRE(*ptr2 == 42);
}

