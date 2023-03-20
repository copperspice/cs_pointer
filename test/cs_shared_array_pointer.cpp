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

#include <cs_shared_array_pointer.h>

#include <cs_catch2.h>

TEST_CASE("CsSharedArrayPointer traits", "[cs_sharedarraypointer]")
{
   // with brackets
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsSharedArrayPointer<int[]>> == true);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsSharedArrayPointer<int[]>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsSharedArrayPointer<int[]>> == true);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsSharedArrayPointer<int[]>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsSharedArrayPointer<int[]>> == false);

   // no brackets
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsSharedArrayPointer<int>> == true);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsSharedArrayPointer<int>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsSharedArrayPointer<int>> == true);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsSharedArrayPointer<int>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsSharedArrayPointer<int>> == false);
}

TEST_CASE("CsSharedArrayPointer constructor", "[cs_sharedarraypointer]")
{
   CsPointer::CsSharedArrayPointer<int[]> ptr1 = CsPointer::make_shared<int[]>(1);
   CsPointer::CsSharedArrayPointer<int>   ptr2(ptr1);

   ptr1[0] = 8;
   ptr2[0] = 17;

   REQUIRE(ptr1.is_null() == false);
   REQUIRE(ptr2.is_null() == false);

   REQUIRE(*ptr1 == 17);
   REQUIRE(*ptr2 == 17);

   CsPointer::CsSharedArrayPointer<int[]> ptr3(ptr2);

   REQUIRE(ptr2.is_null() == false);
   REQUIRE(ptr3.is_null() == false);

   REQUIRE(*ptr1 == 17);
   REQUIRE(*ptr2 == 17);
   REQUIRE(*ptr3 == 17);
}

TEST_CASE("CsSharedArrayPointer convert", "[cs_sharedarraypointer]")
{
   // with bracets
   CsPointer::CsSharedArrayPointer<int[]> ptr1 = CsPointer::make_shared<int[]>(1);
   ptr1[0] = 42;

   std::shared_ptr<int[]> ptr2 = std::move(ptr1);

   REQUIRE(ptr1 == nullptr);
   REQUIRE(ptr2 != nullptr);

   REQUIRE(ptr2[0] == 42);

   // no brackets
   CsPointer::CsSharedArrayPointer<double> ptr3 = CsPointer::make_shared<double[]>(1);
   ptr3[0] = 42.5;

   std::shared_ptr<double[]> ptr4 = std::move(ptr3);

   REQUIRE(ptr3 == nullptr);
   REQUIRE(ptr4 != nullptr);

   REQUIRE(ptr4[0] == 42.5);
}

TEST_CASE("CsSharedArrayPointer empty", "[cs_sharedarraypointer]")
{
   CsPointer::CsSharedArrayPointer<int[]> ptr;

   REQUIRE(ptr == nullptr);
   REQUIRE(nullptr == ptr);
   REQUIRE(ptr == ptr);

   REQUIRE(! (ptr != nullptr));
   REQUIRE(! (nullptr != ptr));
   REQUIRE(! (ptr != ptr)) ;

   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsSharedArrayPointer move_assign", "[cs_sharedarraypointer]")
{
   CsPointer::CsSharedArrayPointer<int[]> ptr1;
   int *rawPointer = nullptr;

   {
      CsPointer::CsSharedArrayPointer<int[]> ptr2(new int[1]);
      rawPointer = ptr2.data();
      ptr1 = std::move(ptr2);

      REQUIRE(ptr2.is_null());
   }

   REQUIRE(rawPointer == ptr1.get());
}

TEST_CASE("CsSharedArrayPointer move_construct", "[cs_sharedarraypointer]")
{
   CsPointer::CsSharedArrayPointer<int[]> ptr1 = CsPointer::make_shared<int[]>(1);
   CsPointer::CsSharedArrayPointer<int>   ptr2(std::move(ptr1));

   ptr2[0] = 17;

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr2.is_null() == false);

   REQUIRE(*ptr2 == 17);

   CsPointer::CsSharedArrayPointer<int[]> ptr3(std::move(ptr2));

   REQUIRE(ptr2.is_null() == true);
   REQUIRE(ptr3.is_null() == false);

   REQUIRE(*ptr3 == 17);
}

TEST_CASE("CsSharedArrayPointer reset", "[cs_sharedarraypointer]")
{
   CsPointer::CsSharedArrayPointer<int[]> ptr = CsPointer::make_shared<int[]>(1);
   ptr.reset();

   REQUIRE(ptr == nullptr);
   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsSharedArrayPointer swap", "[cs_sharedarraypointer]")
{
   CsPointer::CsSharedArrayPointer<int[]> ptr1 = CsPointer::make_shared<int[]>(1);
   CsPointer::CsSharedArrayPointer<int[]> ptr2 = CsPointer::make_shared<int[]>(1);

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
