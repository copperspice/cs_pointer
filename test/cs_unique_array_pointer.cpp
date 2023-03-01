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

#include <cs_unique_array_pointer.h>

#include <cs_catch2.h>

TEST_CASE("CsUniqueArrayPointer traits", "[cs_uniquearraypointer]")
{
   //
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsUniqueArrayPointer<int>> == false);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsUniqueArrayPointer<int>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsUniqueArrayPointer<int>> == false);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsUniqueArrayPointer<int>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsUniqueArrayPointer<int>> == false);

   // with brackets
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsUniqueArrayPointer<int[]>> == false);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsUniqueArrayPointer<int[]>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsUniqueArrayPointer<int[]>> == false);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsUniqueArrayPointer<int[]>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsUniqueArrayPointer<int[]>> == false);
}

TEST_CASE("CsUniqueArrayPointer convert", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr1 = CsPointer::make_unique<int[]>(1);
   ptr1[0] = 42;

   std::unique_ptr<int[]> ptr2 = std::move(ptr1);

   REQUIRE(ptr1 == nullptr);
   REQUIRE(ptr2 != nullptr);

   REQUIRE(ptr2[0] == 42);

   // no brackets
   CsPointer::CsUniqueArrayPointer<double> ptr3 = CsPointer::make_unique<double[]>(1);
   ptr3[0] = 42.5;

   std::unique_ptr<double[]> ptr4 = std::move(ptr3);

   REQUIRE(ptr3 == nullptr);
   REQUIRE(ptr4 != nullptr);

   REQUIRE(ptr4[0] == 42.5);
}

TEST_CASE("CsUniqueArrayPointer empty", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr;

   REQUIRE(ptr == nullptr);
   REQUIRE(nullptr == ptr);
   REQUIRE(ptr == ptr);

   REQUIRE(! (ptr != nullptr));
   REQUIRE(! (nullptr != ptr));
   REQUIRE(! (ptr != ptr)) ;

   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsUniqueArrayPointer move_assign", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr1;
   int *rawPointer = nullptr;

   {
      CsPointer::CsUniqueArrayPointer<int[]> ptr2(new int[1]);
      rawPointer = ptr2.data();
      ptr1 = std::move(ptr2);

      REQUIRE(ptr2.is_null());
   }

   REQUIRE(rawPointer == ptr1.get());
}

TEST_CASE("CsUniqueArrayPointer move_construct", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr1 = CsPointer::make_unique<int[]>(1);
   CsPointer::CsUniqueArrayPointer<int>   ptr2(std::move(ptr1));

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr2.is_null() == false);

   CsPointer::CsUniqueArrayPointer<int[]> ptr3(std::move(ptr2));

   REQUIRE(ptr2.is_null() == true);
   REQUIRE(ptr3.is_null() == false);
}

TEST_CASE("CsUniqueArrayPointer release", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr = CsPointer::make_unique<int[]>(1);
   int *p1 = ptr.get();
   int *p2 = ptr.release();

   REQUIRE(p1 == p2);
   REQUIRE(ptr == nullptr);

   delete[] p2;

   REQUIRE(ptr.release() == nullptr);
}

TEST_CASE("CsUniqueArrayPointer reset", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr = CsPointer::make_unique<int[]>(1);
   ptr.reset();

   REQUIRE(ptr == nullptr);
   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsUniqueArrayPointer swap", "[cs_uniquearraypointer]")
{
   CsPointer::CsUniqueArrayPointer<int[]> ptr1 = CsPointer::make_unique<int[]>(1);
   CsPointer::CsUniqueArrayPointer<int[]> ptr2 = CsPointer::make_unique<int[]>(1);

   ptr1[0] = 8;
   ptr2[0] = 17;

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

