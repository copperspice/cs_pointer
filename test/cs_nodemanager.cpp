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

#include <cs_nodemanager.h>

#include <cs_catch2.h>

template <typename T>
using IntrusivePtr = CsPointer::CsIntrusivePointer<T>;

class Widget : public CsPointer::CsNodeManager<Widget>, public CsPointer::CsIntrusiveBase
{
 public:
   Widget() = default;

   ~Widget()
   {
      printf("Widget::Destructor invoked for tag = %s \n", m_tag.c_str());
   }

   Widget(std::string str)
      : m_tag(str)
   {
   }

   std::string getTag() {
      return m_tag;
   }

 private:
   std::string m_tag;
};

class Button : public Widget
{
 public:
   Button() = default;

   Button(std::string str)
      : Widget(str)
   {
   }
};

TEST_CASE("CsNodeManager traits", "[cs_nodemanager]")
{
   REQUIRE(std::is_copy_constructible_v<CsPointer::CsNodeManager<Widget>> == true);
   REQUIRE(std::is_move_constructible_v<CsPointer::CsNodeManager<Widget>> == true);

   REQUIRE(std::is_copy_assignable_v<CsPointer::CsNodeManager<Widget>> == true);
   REQUIRE(std::is_move_assignable_v<CsPointer::CsNodeManager<Widget>> == true);

   REQUIRE(std::has_virtual_destructor_v<CsPointer::CsNodeManager<Widget>> == true);
}

TEST_CASE("CsNodeManager clear", "[cs_nodemanager]")
{
   printf("\n* CsNodeManager: Test clear\n");

   // create a new node
   CsPointer::CsNodeManager<Widget> node;

   CsPointer::CsIntrusivePointer<Widget> ptrA = CsPointer::make_intrusive<Widget>("obj_A");
   Widget *ptrB = new Button("obj_B");

   node.add_child(ptrA);
   node.add_child(ptrB);

   REQUIRE(node.children().size() == 2);
   REQUIRE(node.children()[0] == ptrA);
   REQUIRE(node.children()[1] == ptrB);

   ptrA.reset();
   printf("CsNodeManager: Widget IP ptrA reset\n");

   REQUIRE(node.children().size() == 2);

   printf("CsNodeManager: Clear the node manager container\n");
   node.clear();

   REQUIRE(node.children().size() == 0);

   printf("End of scope, destroy objects\n");
}

TEST_CASE("CsNodeManager duplicate_child", "[cs_nodemanager]")
{
   printf("\n* CsNodeManager: Test duplicate child \n");

   // create a new node
   CsPointer::CsNodeManager<Widget> node;

   CsPointer::CsIntrusivePointer<Widget> ptrA  = CsPointer::make_intrusive<Widget>("obj_A");

   //
   node.add_child(ptrA);

   REQUIRE(node.children().size() == 1);
   REQUIRE(ptrA.use_count() == 2);

   //
   node.add_child(ptrA);

   REQUIRE(node.children().size() == 2);
   REQUIRE(ptrA.use_count() == 3);

   //
   node.remove_child(ptrA);

   REQUIRE(node.children().size() == 1);
   REQUIRE(node.children()[0] == ptrA);
   REQUIRE(ptrA.use_count() == 2);

   printf("End of scope, destroy objects\n");
}

TEST_CASE("CsNodeManager find", "[cs_nodemanager]")
{  
   printf("\n* CsNodeManager: Test find methods\n");

   IntrusivePtr<Widget> rootWindow = CsPointer::make_intrusive<Widget>("obj_window");

   Widget *pb1 = new Button("obj_pb1");
   Widget *pb2 = new Button("obj_pb2");

   IntrusivePtr<Widget> groupBox = CsPointer::make_intrusive<Button>("obj_groupBox");
   Widget *textEdit = new Button("obj_textEdit");

   IntrusivePtr<Button> cb = CsPointer::make_intrusive<Button>("obj_cb");
 
   // node 0
   rootWindow->add_child(pb1);
   rootWindow->add_child(pb2);
   rootWindow->add_child(groupBox);
   rootWindow->add_child(textEdit);

   REQUIRE(rootWindow->children().size() == 4);

   // node 1
   groupBox->add_child(cb);

   REQUIRE(groupBox->children().size() == 1);

   // find_children
   std::vector<IntrusivePtr<Widget>> result = rootWindow->find_children<Widget>();

   REQUIRE(result.size() == 5);

   REQUIRE(result[0] == pb1);
   REQUIRE(result[1] == pb2);
   REQUIRE(result[2] == groupBox);
   REQUIRE(result[3] == cb);
   REQUIRE(result[4] == textEdit);

   // find_children lambda
   result = rootWindow->find_children<Widget>( [] (auto item)
       { return (item->getTag().find("pb") != std::string::npos); });

   REQUIRE(result.size() == 2);

   // find_child
   IntrusivePtr<Button> ptr = rootWindow->find_child<Button>();
   REQUIRE(ptr.get() == pb1);

   // find_child lambda
   ptr = rootWindow->find_child<Button>( [] (auto item)
      { return (item->getTag() == "obj_cb"); } );

   REQUIRE(ptr == cb);

   printf("End of scope, destroy objects\n");
}

TEST_CASE("CsNodeManager move_children", "[cs_nodemanager]")
{
   printf("\n* CsNodeManager: Test move children\n");

   IntrusivePtr<Widget> objRoot = CsPointer::make_intrusive<Widget>();

   IntrusivePtr<Widget> objA = CsPointer::make_intrusive<Widget>("A");
   IntrusivePtr<Widget> objB = CsPointer::make_intrusive<Widget>("B");
   IntrusivePtr<Widget> objC = CsPointer::make_intrusive<Widget>("C");;

   objRoot->add_child(objA);
   objRoot->add_child(objB);
   objRoot->add_child(objC);

   REQUIRE(objRoot->children()[0] == objA);
   REQUIRE(objRoot->children()[1] == objB);
   REQUIRE(objRoot->children()[2] == objC);

   //
   objRoot->move_child(1, 1);

   REQUIRE(objRoot->children()[0] == objA);
   REQUIRE(objRoot->children()[1] == objB);
   REQUIRE(objRoot->children()[2] == objC);

   //
   objRoot->move_child(1, 2);

   REQUIRE(objRoot->children()[0] == objA);
   REQUIRE(objRoot->children()[1] == objC);
   REQUIRE(objRoot->children()[2] == objB);

   REQUIRE(objRoot->children()[0]->getTag() == "A");
   REQUIRE(objRoot->children()[1]->getTag() == "C");
   REQUIRE(objRoot->children()[2]->getTag() == "B");

   //
   objRoot->move_child(2, 0);

   REQUIRE(objRoot->children()[0] == objB);
   REQUIRE(objRoot->children()[1] == objA);
   REQUIRE(objRoot->children()[2] == objC);

   REQUIRE(objRoot->children()[0]->getTag() == "B");
   REQUIRE(objRoot->children()[1]->getTag() == "A");
   REQUIRE(objRoot->children()[2]->getTag() == "C");
 
   printf("End of scope, destroy objects\n");
}

TEST_CASE("CsNodeManager remove", "[cs_nodemanager]")
{
   printf("\n* CsNodeManager: Test remove\n");

   // create a new node
   CsPointer::CsNodeManager<Widget> node;

   IntrusivePtr<Widget> ptrA = CsPointer::make_intrusive<Widget>("obj_A");
   Widget *ptrB              = new Button("obj_B");
   IntrusivePtr<Button> ptrC = CsPointer::make_intrusive<Button>("obj_C");

   node.add_child(ptrA);
   node.add_child(ptrB);
   node.add_child(ptrC);

   REQUIRE(node.children().size() == 3);

   //
   bool ok;
   ok = node.remove_child(ptrB);

   REQUIRE(ok == true);
   REQUIRE(node.children().size() == 2);

   REQUIRE(node.children()[0] == ptrA);
   REQUIRE(node.children()[1] == ptrC);

   //
   ok = node.remove_child(ptrB);             // intended duplicate

   REQUIRE(ok == false);
   REQUIRE(node.children().size() == 2);

   REQUIRE(node.children()[0] == ptrA);
   REQUIRE(node.children()[1] == ptrC);

   //
   ok = node.remove_child(ptrA);

   REQUIRE(ok == true);
   REQUIRE(node.children().size() == 1);

   REQUIRE(node.children()[0] == ptrC);

   REQUIRE(ptrA.use_count() == 1);
   REQUIRE(ptrC.use_count() == 2);

   printf("End of scope, destroy objects\n");  
}

class Simple : public CsPointer::CsIntrusiveBase
{
 public:
   Simple() = default;

   ~Simple()
   {
      printf("Simple::Destructor invoked for tag = %s \n", m_tag.c_str());
   }

   Simple(std::string str)
      : m_tag(str)
   {
   }

   std::string getTag() {
      return m_tag;
   }

 private:
   std::string m_tag;
};

TEST_CASE("CsNodeManager node-noninheriting", "[cs_nodemanager]")
{
   printf("\n* CsNodeManager: Test not inherting from node manager\n");

   // create a new node
   CsPointer::CsNodeManager<Simple> node;

   IntrusivePtr<Simple> ptrA = CsPointer::make_intrusive<Simple>("obj_A1");
   IntrusivePtr<Simple> ptrB = CsPointer::make_intrusive<Simple>("obj_B1");
   IntrusivePtr<Simple> ptrC = CsPointer::make_intrusive<Simple>("obj_C1");

   node.add_child(ptrA);
   node.add_child(ptrB);
   node.add_child(ptrC);

   REQUIRE(node.children().size() == 3);

   IntrusivePtr<Simple> ptrD = CsPointer::make_intrusive<Simple>("obj_D2");

   // following will not compile since class Simple does not inherit from CsNodeManager
   // ptrB->add_child(ptrD);

   printf("End of scope, destroy objects\n");
}
