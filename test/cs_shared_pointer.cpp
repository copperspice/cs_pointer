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

#include <cs_shared_pointer.h>
#include <cs_unique_pointer.h>

#include <cs_catch2.h>

TEST_CASE("CsSharedPointer traits", "[cs_sharedpointer]")
{
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsSharedPointer<int>> == true);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsSharedPointer<int>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsSharedPointer<int>> == true);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsSharedPointer<int>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsSharedPointer<int>> == false);
}

TEST_CASE("CsSharedPointer alias", "[cs_sharedpointer]")
{
   struct Data {
      Data(int id, std::string tag)
         : m_id(id), m_tag(tag)
      {
      }

      int m_id;
      std::string m_tag;
   };

   CsPointer::CsSharedPointer<Data> ptr1 = CsPointer::make_shared<Data>(42, "tag string");
   CsPointer::CsSharedPointer<int> ptr2(ptr1, &(ptr1->m_id));
   CsPointer::CsSharedPointer<std::string> ptr3(ptr1, &(ptr1->m_tag));

   REQUIRE(*ptr2 == 42);
   REQUIRE(*ptr3 == "tag string");
   REQUIRE(ptr2.use_count() == 3);
   REQUIRE(ptr3.use_count() == 3);

   ptr2 = CsPointer::CsSharedPointer<int>(std::move(ptr1), &(ptr1->m_id));

   REQUIRE(ptr1 == nullptr);
   REQUIRE(*ptr2 == 42);
   REQUIRE(*ptr3 == "tag string");
   REQUIRE(ptr2.use_count() == 2);
   REQUIRE(ptr3.use_count() == 2);

   ptr2.reset();
   REQUIRE(ptr2 == nullptr);
   REQUIRE(*ptr3 == "tag string");
   REQUIRE(ptr2.use_count() == 0);
   REQUIRE(ptr3.use_count() == 1);
}

TEST_CASE("CsSharedPointer cast", "[cs_sharedpointer]")
{
   class Fruit
   {
   };

   class Apple : public Fruit
   {
   };

   CsPointer::CsSharedPointer<Fruit> ptr1;
   CsPointer::CsSharedPointer<Apple> ptr2 = CsPointer::make_shared<Apple>();

   // part 1
   {
      CsPointer::CsSharedPointer<const Apple> ptr3 = ptr2;
      ptr1 = CsPointer::const_pointer_cast<Apple>(ptr3);

      REQUIRE(ptr1 == ptr2);
   }

   {
      ptr1 = CsPointer::dynamic_pointer_cast<Fruit>(ptr2);

      REQUIRE(ptr1 == ptr2);
   }

   {
      ptr1 = CsPointer::static_pointer_cast<Fruit>(ptr2);

      REQUIRE(ptr1 == ptr2);
   }


   // part 2
   {
      CsPointer::CsSharedPointer<const Apple> ptr3 = ptr2;
      ptr1 = CsPointer::const_pointer_cast<Apple>(std::move(ptr3));

      REQUIRE(ptr1 == ptr2);
   }

   {
      CsPointer::CsSharedPointer<Apple> ptr3 = ptr2;
      ptr1 = CsPointer::dynamic_pointer_cast<Fruit>(std::move(ptr3));

      REQUIRE(ptr1 == ptr2);
   }

   {
      CsPointer::CsSharedPointer<Apple> ptr3 = ptr2;
      ptr1 = CsPointer::static_pointer_cast<Fruit>(std::move(ptr3));

      REQUIRE(ptr1 == ptr2);
   }
}

TEST_CASE("CsSharedPointer convert_a", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1 = CsPointer::make_shared<int>(42);

   std::shared_ptr<int> ptr2 = std::move(ptr1);

   REQUIRE(ptr1 == nullptr);
   REQUIRE(ptr2 != nullptr);

   REQUIRE(*ptr2 == 42);
}

TEST_CASE("CsSharedPointer convert_b", "[cs_sharedpointer]")
{
   class Fruit
   {
   };

   class Apple : public Fruit
   {
   };

   class GreenApple : public Apple
   {
   };

   CsPointer::CsSharedPointer<Apple> ptr1 = CsPointer::make_shared<GreenApple>();

   std::shared_ptr<Fruit> ptr2 = ptr1;

   REQUIRE(ptr1 != nullptr);
   REQUIRE(ptr2 != nullptr);

   REQUIRE(ptr1.get() == ptr2.get());
}


TEST_CASE("CsSharedPointer copy", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1;
   int *rawPtr = nullptr;

   {
      CsPointer::CsSharedPointer<int> ptr2 = CsPointer::make_shared<int>();
      rawPtr = ptr2.data();
      ptr1   = ptr2;
   }

   REQUIRE(rawPtr == ptr1.data());
}

TEST_CASE("CsSharedPointer custom_deleter", "[cs_sharedpointer]")
{
   bool deleterExecuted = false;

   {
      auto customDeleter = [&deleterExecuted] (int *obj) {
         deleterExecuted = true;
         delete obj;
      };

      CsPointer::CsSharedPointer<int> ptr(new int, customDeleter);
      REQUIRE(deleterExecuted == false);
   }

   REQUIRE(deleterExecuted == true);
}

TEST_CASE("CsSharedPointer empty", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1;

   REQUIRE(ptr1 == nullptr);
   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr1 == ptr1);

   REQUIRE(! (ptr1 != nullptr));
   REQUIRE(! (nullptr != ptr1));
   REQUIRE(! (ptr1 != ptr1)) ;

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr1.unique() == false);
   REQUIRE(ptr1.use_count() == 0);

   //
   CsPointer::CsSharedPointer<int> ptr2(nullptr, [](auto p) {});
   CsPointer::CsSharedPointer<int> ptr3(nullptr, [](auto p) {}, std::allocator<int>());
   CsPointer::CsSharedPointer<int> ptr4(static_cast<int*>(nullptr), [](auto p) {}, std::allocator<int>());

   REQUIRE(ptr2 == nullptr);
   REQUIRE(ptr3 == nullptr);
   REQUIRE(ptr4 == nullptr);
}

TEST_CASE("CsSharedPointer move_assign", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1;
   int *rawPtr = nullptr;

   {
      CsPointer::CsSharedPointer<int> ptr2(new int);
      rawPtr = ptr2.data();
      ptr1   = std::move(ptr2);

      REQUIRE(ptr2.is_null());
   }

   REQUIRE(rawPtr == ptr1.get());

   //
   CsPointer::CsUniquePointer<int> uniquePtr = CsPointer::make_unique<int>(42);

   rawPtr = uniquePtr.get();
   ptr1   = std::move(uniquePtr);

   REQUIRE(rawPtr == ptr1.get());
   REQUIRE(uniquePtr == nullptr);
   REQUIRE(*ptr1 == 42);

   //
   std::shared_ptr<int> stdSharedPtr = std::make_shared<int>(43);

   rawPtr = stdSharedPtr.get();
   ptr1   = std::move(stdSharedPtr);

   REQUIRE(rawPtr == ptr1.get());
   REQUIRE(stdSharedPtr == nullptr);
   REQUIRE(*ptr1 == 43);
}

TEST_CASE("CsSharedPointer move_construct", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1 = CsPointer::make_shared<int>();
   CsPointer::CsSharedPointer<int> ptr2(std::move(ptr1));

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr2.is_null() == false);

   //
   CsPointer::CsUniquePointer<int> uniquePtr = CsPointer::make_unique<int>(42);

   int *rawPtr = uniquePtr.get();
   CsPointer::CsSharedPointer<int> ptr3(std::move(uniquePtr));

   REQUIRE(rawPtr == ptr3.get());
   REQUIRE(uniquePtr == nullptr);
   REQUIRE(*ptr3 == 42);

   //
   std::shared_ptr<int> stdSharedPtr = std::make_shared<int>(43);

   rawPtr = stdSharedPtr.get();
   CsPointer::CsSharedPointer<int> ptr4(std::move(stdSharedPtr));

   REQUIRE(rawPtr == ptr4.get());
   REQUIRE(stdSharedPtr == nullptr);
   REQUIRE(*ptr4 == 43);
}

TEST_CASE("CsSharedPointer nullptr", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr = nullptr;

   REQUIRE(ptr == nullptr);
   REQUIRE(nullptr == ptr);

   REQUIRE(!ptr == true);
   REQUIRE(ptr.is_null() == true);

   ptr = nullptr;

   REQUIRE(ptr == nullptr);
   REQUIRE(nullptr == ptr);

   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsSharedPointer operators", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1;
   CsPointer::CsSharedPointer<int> ptr2 = CsPointer::make_shared<int>();
   CsPointer::CsSharedPointer<int> ptr3 = ptr2;

   REQUIRE( (ptr1 == ptr2) == false);
   REQUIRE( (ptr2 == ptr3) == true);
   REQUIRE( (ptr1 != ptr2) == true);
   REQUIRE( (ptr2 != ptr3) == false);

   REQUIRE( (ptr1.get() == ptr2) == false);
   REQUIRE( (ptr2.get() == ptr3) == true);
   REQUIRE( (ptr1.get() != ptr2) == true);
   REQUIRE( (ptr2.get() != ptr3) == false);

   REQUIRE( (ptr1 == ptr2.get()) == false);
   REQUIRE( (ptr2 == ptr3.get()) == true);
   REQUIRE( (ptr1 != ptr2.get()) == true);
   REQUIRE( (ptr2 != ptr3.get()) == false);

   REQUIRE( (ptr1 < ptr2) == true);
   REQUIRE( (ptr2 < ptr1) == false);
   REQUIRE( (ptr2 < ptr2) == false);
   REQUIRE( (ptr1 < ptr1) == false);

   REQUIRE( (ptr1 > ptr2) == false);
   REQUIRE( (ptr2 > ptr1) == true);
   REQUIRE( (ptr2 > ptr2) == false);
   REQUIRE( (ptr1 > ptr1) == false);

   REQUIRE( (ptr1 <= ptr2) == true);
   REQUIRE( (ptr2 <= ptr1) == false);
   REQUIRE( (ptr2 <= ptr2) == true);
   REQUIRE( (ptr1 <= ptr1) == true);

   REQUIRE( (ptr1 >= ptr2) == false);
   REQUIRE( (ptr2 >= ptr1) == true);
   REQUIRE( (ptr2 >= ptr2) == true);
   REQUIRE( (ptr1 >= ptr1) == true);

   REQUIRE( (ptr2 <  ptr3) == false);
   REQUIRE( (ptr2 >  ptr3) == false);
   REQUIRE( (ptr2 <= ptr3) == true);
   REQUIRE( (ptr2 >= ptr3) == true);
}
TEST_CASE("CsSharedPointer reset", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr = CsPointer::make_shared<int>();
   ptr.reset();

   REQUIRE(ptr == nullptr);
   REQUIRE(ptr.is_null() == true);

   ptr.reset(new int(42));

   REQUIRE(ptr != nullptr);
   REQUIRE(*ptr == 42);

   ptr.reset(ptr.get());

   REQUIRE(ptr != nullptr);
   REQUIRE(*ptr == 42);

   ptr.reset(new int(43), std::default_delete<int>());

   REQUIRE(ptr != nullptr);
   REQUIRE(*ptr == 43);

   ptr.reset(ptr.get(), std::default_delete<int>());
   REQUIRE(ptr != nullptr);
   REQUIRE(*ptr == 43);

   ptr.reset(new int(44), std::default_delete<int>(), std::allocator<int>());
   REQUIRE(ptr != nullptr);
   REQUIRE(*ptr == 44);

   ptr.reset(ptr.get(), std::default_delete<int>(), std::allocator<int>());
   REQUIRE(ptr != nullptr);
   REQUIRE(*ptr == 44);
}

TEST_CASE("CsSharedPointer swap", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1 = CsPointer::make_shared<int>(8);
   CsPointer::CsSharedPointer<int> ptr2 = CsPointer::make_shared<int>(17);

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

TEST_CASE("CsSharedPointer use_count", "[cs_sharedpointer]")
{
   CsPointer::CsSharedPointer<int> ptr1 = CsPointer::make_shared<int>(8);
   CsPointer::CsSharedPointer<int> ptr2 = CsPointer::make_shared<int>(17);

   REQUIRE(ptr1.unique() == true);
   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.unique() == true);
   REQUIRE(ptr2.use_count() == 1);

   ptr1 = ptr2;

   REQUIRE(ptr1.unique() == false);
   REQUIRE(ptr1.use_count() == 2);
   REQUIRE(ptr2.unique() == false);
   REQUIRE(ptr2.use_count() == 2);

   ptr2.clear();

   REQUIRE(ptr1.unique() == true);
   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.unique() == false);
   REQUIRE(ptr2.use_count() == 0);
}

