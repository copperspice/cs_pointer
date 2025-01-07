/***********************************************************************
*
* Copyright (c) 2023-2025 Barbara Geller
* Copyright (c) 2023-2025 Ansel Sermersheim
*
* This file is part of CsPointer.
*
* CsPointer is free software which is released under the BSD 2-Clause license.
* For license details refer to the LICENSE provided with this project.
*
* CsPointer is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* https://opensource.org/licenses/BSD-2-Clause
*
***********************************************************************/

#include <cs_intrusive_pointer.h>

#include <cs_catch2.h>

class Fruit : public CsPointer::CsIntrusiveBase
{
 public:
   Fruit() = default;

   Fruit(std::string str)
      : m_tag(str)
   {
   }

   std::string getTag() {
      return m_tag;
   }

 private:
   std::string m_tag;
};

class Apple : public Fruit
{
 public:
   Apple() = default;

   Apple(std::string str)
      : Fruit(str)
   {
   }
};

TEST_CASE("CsIntrusivePointer traits", "[cs_intrusivepointer]")
{
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsIntrusivePointer<Fruit>> == true);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsIntrusivePointer<Fruit>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsIntrusivePointer<Fruit>> == true);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsIntrusivePointer<Fruit>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsIntrusivePointer<Fruit>> == false);
}

TEST_CASE("CsIntrusivePointer assign", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();
   Apple *rawPtr = ptr1.get();

   //
   ptr1 = ptr1;

   REQUIRE(ptr1.get() == rawPtr);
   REQUIRE(ptr1.use_count() == 1);

   //
   ptr1 = rawPtr;

   REQUIRE(ptr1.get() == rawPtr);
   REQUIRE(ptr1.use_count() == 1);

   ptr1 = std::move(ptr1);

   REQUIRE(ptr1.get() == rawPtr);
   REQUIRE(ptr1.use_count() == 1);
}

TEST_CASE("CsIntrusivePointer cast", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();

   //
   CsPointer::CsIntrusivePointer<const Apple> ptr2;
   ptr2 = CsPointer::const_pointer_cast<const Apple>(ptr1);

   REQUIRE(ptr1 == ptr2);

   //
   CsPointer::CsIntrusivePointer<Fruit> ptr3;
   ptr3 = CsPointer::dynamic_pointer_cast<Fruit>(ptr1);

   REQUIRE(ptr1 == ptr3);

   //
   CsPointer::CsIntrusivePointer<Fruit> ptr4;
   ptr4 = CsPointer::static_pointer_cast<Fruit>(ptr1);

   REQUIRE(ptr1 == ptr4);
}

TEST_CASE("CsIntrusivePointer conversion", "[cs_intrusivepointer]")
{
   REQUIRE(std::is_constructible_v<CsPointer::CsIntrusivePointer<const Fruit>,
         CsPointer::CsIntrusivePointer<Fruit>> == true);

   REQUIRE(std::is_assignable_v<CsPointer::CsIntrusivePointer<const Fruit>,
         CsPointer::CsIntrusivePointer<Fruit>> == true);

   //
   CsPointer::CsIntrusivePointer<Fruit> ptr1 = CsPointer::make_intrusive<Fruit>();
   CsPointer::CsIntrusivePointer<const Fruit> ptr2 = ptr1;

   REQUIRE(ptr1 == ptr2);
}

TEST_CASE("CsIntrusivePointer copy", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();
   CsPointer::CsIntrusivePointer<Apple> ptr2 = ptr1;

   REQUIRE(ptr1.use_count() == 2);
   REQUIRE(ptr2.use_count() == 2);
}

TEST_CASE("CsIntrusivePointer copy_use_count", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();
   CsPointer::CsIntrusivePointer<Apple> ptr2 = CsPointer::make_intrusive<Apple>();

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1.is_null() == false);
   REQUIRE(ptr2.is_null() == false);

   ptr1 = ptr2;

   REQUIRE(ptr1.use_count() == 2);
   REQUIRE(ptr2.use_count() == 2);

   REQUIRE(ptr1.is_null() == false);
   REQUIRE(ptr2.is_null() == false);
}

TEST_CASE("CsIntrusivePointer empty", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr;

   REQUIRE(ptr == nullptr);
   REQUIRE(nullptr == ptr);
   REQUIRE(ptr == ptr);

   REQUIRE(! (ptr != nullptr));
   REQUIRE(! (nullptr != ptr));
   REQUIRE(! (ptr != ptr)) ;

   REQUIRE(ptr.is_null() == true);
}

TEST_CASE("CsIntrusivePointer move_assign", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr1;
   Fruit *rawPtr = nullptr;

   {
      CsPointer::CsIntrusivePointer<Fruit> ptr2(new Fruit);

      REQUIRE(ptr2.use_count() == 1);

      rawPtr = ptr2.data();
      ptr1 = std::move(ptr2);

      REQUIRE(ptr2.is_null() == true);
   }

   REQUIRE(rawPtr == ptr1.get());

   {
      ptr1 = std::move(ptr1);

      REQUIRE(ptr1.is_null() == false);
      REQUIRE(ptr1.use_count() == 1);

      REQUIRE(rawPtr == ptr1.get());
   }
}

TEST_CASE("CsIntrusivePointer move_construct", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();

   CsPointer::CsIntrusivePointer<Apple> ptr2(std::move(ptr1));

   REQUIRE(ptr1.use_count() == 0);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr2.is_null() == false);
}

TEST_CASE("CsIntrusivePointer move_use_count", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();
   CsPointer::CsIntrusivePointer<Apple> ptr2 = CsPointer::make_intrusive<Apple>();

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1.is_null() == false);
   REQUIRE(ptr2.is_null() == false);

   ptr1 = std::move(ptr2);

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 0);

   REQUIRE(ptr1.is_null() == false);
   REQUIRE(ptr2.is_null() == true);
}

TEST_CASE("CsIntrusivePointer nullptr", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr1 = nullptr;
   CsPointer::CsIntrusivePointer<Fruit> ptr2 = ptr1;

   CsPointer::CsIntrusivePointer<Fruit> ptr3;
   ptr3 = ptr1;

   REQUIRE(ptr1.is_null() == true);
   REQUIRE(ptr2.is_null() == true);
   REQUIRE(ptr3.is_null() == true);
}

TEST_CASE("CsIntrusivePointer operator-compare", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr1 = CsPointer::make_intrusive<Fruit>("fruit");
   CsPointer::CsIntrusivePointer<Apple> ptr2 = CsPointer::make_intrusive<Apple>("apple");

   REQUIRE((ptr1 < ptr2) == (ptr1.get() < ptr2.get()));
   REQUIRE((ptr1 > ptr2) == (ptr1.get() > ptr2.get()));

   REQUIRE((ptr1 <= ptr2) == (ptr1.get() <= ptr2.get()));
   REQUIRE((ptr1 >= ptr2) == (ptr1.get() >= ptr2.get()));
}

TEST_CASE("CsIntrusivePointer operator-equality", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr1 = CsPointer::make_intrusive<Fruit>("fruit");
   CsPointer::CsIntrusivePointer<Apple> ptr2 = CsPointer::make_intrusive<Apple>("apple");

   ptr1 = ptr2;

   REQUIRE(ptr1->getTag() == "apple");
   REQUIRE(ptr2->getTag() == "apple");

   //
   CsPointer::CsIntrusivePointer<Apple> ptr3(nullptr);

   REQUIRE(ptr3 == nullptr);

   ptr3 = new Apple("pear");

   REQUIRE(ptr3->getTag() == "pear");

   ptr1 = std::move(ptr3);

   REQUIRE(ptr1->getTag() == "pear");
   REQUIRE(ptr2->getTag() == "apple");
   REQUIRE(ptr3 == nullptr);

   //
   Fruit *rawPtr = ptr1.get();

   REQUIRE(ptr1 == rawPtr);
   REQUIRE(rawPtr == ptr1);
   REQUIRE(ptr2 != rawPtr);
   REQUIRE(rawPtr != ptr2);

   REQUIRE((*ptr1).getTag() == "pear");
}

TEST_CASE("CsIntrusivePointer operator-logic", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr1 = CsPointer::make_intrusive<Fruit>("fruit");
   CsPointer::CsIntrusivePointer<Apple> ptr2 = CsPointer::make_intrusive<Apple>("apple");

   ptr1.reset();

   REQUIRE(bool(ptr1) == false);
   REQUIRE(bool(ptr2) == true);

   REQUIRE(! ptr1 == true);
   REQUIRE(! ptr2 == false);
}

TEST_CASE("CsIntrusivePointer release_a", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr = CsPointer::make_intrusive<Fruit>();

   Fruit *raw1 = ptr.get();
   Fruit *raw2 = ptr.release_if();

   REQUIRE(raw1 == raw2);
   REQUIRE(ptr == nullptr);

   delete raw2;

   REQUIRE(ptr.release_if() == nullptr);
}

TEST_CASE("CsIntrusivePointer release_b", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr = CsPointer::make_intrusive<Fruit>();

   Fruit *rawPtr = ptr.release_if();

   REQUIRE(ptr == nullptr);
   REQUIRE(ptr.release_if() == nullptr);

   ptr = rawPtr;

   REQUIRE(ptr.use_count() == 1);
   REQUIRE(ptr == rawPtr);
   REQUIRE(ptr != nullptr);

   ptr = rawPtr;                     // intended duplicate assignment

   REQUIRE(ptr.use_count() == 1);
   REQUIRE(ptr == rawPtr);
   REQUIRE(ptr != nullptr);
}

TEST_CASE("CsIntrusivePointer reset", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Apple> ptr1 = CsPointer::make_intrusive<Apple>();
   CsPointer::CsIntrusivePointer<Apple> ptr2 = CsPointer::make_intrusive<Apple>();

   SECTION ("test-one") {
      ptr1.reset();

      REQUIRE(ptr1.use_count() == 0);
      REQUIRE(ptr2.use_count() == 1);

      REQUIRE(ptr1.is_null() == true);
      REQUIRE(ptr2.is_null() == false);

      ptr1.reset();                     // intended duplicate reset()

      REQUIRE(ptr1.use_count() == 0);
      REQUIRE(ptr2.use_count() == 1);

      REQUIRE(ptr1.is_null() == true);
      REQUIRE(ptr2.is_null() == false);

      ptr2.clear();

      REQUIRE(ptr2.is_null() == true);
      REQUIRE(ptr2.use_count() == 0);
   }

   SECTION ("test-two") {
      CsPointer::CsIntrusivePointer<Apple> ptr3;

      ptr3 = ptr1;

      REQUIRE(ptr1.use_count() == 2);
      REQUIRE(ptr3.use_count() == 2);

      ptr1.reset(ptr2.get());

      REQUIRE(ptr1.use_count() == 2);
      REQUIRE(ptr2.use_count() == 2);
      REQUIRE(ptr3.use_count() == 1);
   }
}

TEST_CASE("CsIntrusivePointer swap", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Fruit> ptr1 = CsPointer::make_intrusive<Fruit>("fruit");
   CsPointer::CsIntrusivePointer<Fruit> ptr2 = CsPointer::make_intrusive<Apple>("apple");

   REQUIRE(ptr1->getTag() == "fruit");
   REQUIRE(ptr2->getTag() == "apple");

   ptr1.swap(ptr2);

   REQUIRE(ptr1->getTag() == "apple");
   REQUIRE(ptr2->getTag() == "fruit");

   ptr1.reset();
   ptr1.swap(ptr2);

   REQUIRE(ptr1->getTag() == "fruit");
   REQUIRE(ptr2 == nullptr);

   ptr1.swap(ptr1);

   REQUIRE(ptr1->getTag() == "fruit");
   REQUIRE(ptr2 == nullptr);

   ptr2.swap(ptr2);

   REQUIRE(ptr1->getTag() == "fruit");
   REQUIRE(ptr2 == nullptr);

   //
   CsPointer::swap(ptr1, ptr2);
   REQUIRE(ptr1 == nullptr);
   REQUIRE(ptr2->getTag() == "fruit");
}


// part 2

class Bread : public CsPointer::CsIntrusiveBase_CM
{
 public:
   Bread() = default;

   Bread(std::string str)
      : m_tag(str)
   {
   }

   std::string getTag() {
      return m_tag;
   }

 private:
   std::string m_tag;
};

TEST_CASE("CsIntrusivePointer deep_copy_assign", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Bread> ptr1 = CsPointer::make_intrusive<Bread>("one");
   CsPointer::CsIntrusivePointer<Bread> ptr2 = CsPointer::make_intrusive<Bread>("two");

   *ptr1 = *ptr2;

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1->getTag() == "two");
   REQUIRE(ptr2->getTag() == "two");
}

TEST_CASE("CsIntrusivePointer deep_copy", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Bread> ptr1;
   CsPointer::CsIntrusivePointer<Bread> ptr2 = CsPointer::make_intrusive<Bread>("two");

   ptr1 = CsPointer::make_intrusive<Bread>(*ptr2);

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1->getTag() == "two");
   REQUIRE(ptr2->getTag() == "two");
}

TEST_CASE("CsIntrusivePointer deep_move_assign", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Bread> ptr1 = CsPointer::make_intrusive<Bread>("one");
   CsPointer::CsIntrusivePointer<Bread> ptr2 = CsPointer::make_intrusive<Bread>("two");

   *ptr1 = std::move(*ptr2);

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1->getTag() == "two");
}

TEST_CASE("CsIntrusivePointer deep_move", "[cs_intrusivepointer]")
{
   CsPointer::CsIntrusivePointer<Bread> ptr1;
   CsPointer::CsIntrusivePointer<Bread> ptr2 = CsPointer::make_intrusive<Bread>("two");

   ptr1 = CsPointer::make_intrusive<Bread>(std::move(*ptr2));

   REQUIRE(ptr1.use_count() == 1);
   REQUIRE(ptr2.use_count() == 1);

   REQUIRE(ptr1->getTag() == "two");
}

