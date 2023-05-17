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

#ifndef LIB_CS_NODEMANAGER_H
#define LIB_CS_NODEMANAGER_H

#include <cs_intrusive_pointer.h>

#include <algorithm>
#include <vector>

namespace CsPointer {

enum class VisitChildren {
   Recursive,
   NonRecursive,
};

enum class VisitStatus {
   VisitMore,
   Finished,
};

template <typename T, typename Policy = CsIntrusiveDefaultPolicy>
class CsNodeManager
{
 public:
   using size_type = typename std::vector<CsIntrusivePointer<T, Policy>>::size_type;

   CsNodeManager() = default;

   virtual ~CsNodeManager()
   {
      // ensure the objects in m_children are destoryed before
      // m_children goes out of scope

      clear();
   }

   CsNodeManager(const CsNodeManager &other) = default;
   CsNodeManager &operator=(const CsNodeManager &other) = default;

   CsNodeManager(CsNodeManager &&other) = default;
   CsNodeManager &operator=(CsNodeManager && other) = default;

   void add_child(T *child) {
      m_children.push_back(CsIntrusivePointer<T, Policy>(child));
   }

   void add_child(CsIntrusivePointer<T, Policy> child) {
      m_children.push_back(std::move(child));
   }

   const std::vector<CsIntrusivePointer<T, Policy>> &children() const {
      return m_children;
   }

   void clear() {
      // swap used to prevent a race condition

      std::vector<CsIntrusivePointer<T, Policy>> tmp;
      swap(m_children, tmp);
   }

   template <typename U>
   CsIntrusivePointer<U, Policy> find_child() const;

   template <typename U, typename F>
   CsIntrusivePointer<U, Policy> find_child(const F &lambda) const;

   template <typename U>
   std::vector<CsIntrusivePointer<U, Policy>> find_children() const;

   template <typename U, typename F>
   std::vector<CsIntrusivePointer<U, Policy>> find_children(const F &lambda) const;

   void move_child(size_type source, size_type dest) {

      if (source == dest) {
         // do nothing

      } else if (source < dest) {
         std::rotate(m_children.begin() + source, m_children.begin() + source + 1,
               m_children.begin() + dest + 1);

      } else {
         std::rotate(m_children.rend() - source - 1, m_children.rend() - source,
               m_children.rend() - dest);
      }

   }

   bool remove_child(T *child) {
      auto iter = std::find(m_children.begin(), m_children.end(), child);

      if (iter != m_children.end()) {
         m_children.erase(iter);
         return true;
      }

      return false;
   }

   bool remove_child(const CsIntrusivePointer<T, Policy> &child) {
      T *ptr = child.get();
      return remove_child(ptr);
   }

   template <typename U = T, typename F>
   VisitStatus visit(const F &lambda, VisitChildren option = VisitChildren::Recursive) const;

 private:
   std::vector<CsIntrusivePointer<T, Policy>> m_children;
};

template <typename T, typename Policy>
template <typename U>
CsIntrusivePointer<U, Policy> CsNodeManager<T, Policy>::find_child() const
{
   CsIntrusivePointer<U, Policy> retval = nullptr;

   auto lambda_internal = [&retval] (auto item) {
      retval = item;
      return VisitStatus::Finished;
   };

   visit<U>(lambda_internal);

   return retval;
}

template <typename T, typename Policy>
template <typename U, typename F>
CsIntrusivePointer<U, Policy> CsNodeManager<T, Policy>::find_child(const F &lambda) const
{
   CsIntrusivePointer<U, Policy> retval = nullptr;

   auto lambda_internal = [&retval, &lambda] (auto item) {
      if (lambda(item) == true) {
         retval = item;
         return VisitStatus::Finished;
      }

      return VisitStatus::VisitMore;
   };

   visit<U>(lambda_internal);

   return retval;
}

template <typename T, typename Policy>
template <typename U>
std::vector<CsIntrusivePointer<U, Policy>> CsNodeManager<T, Policy>::find_children() const
{
   std::vector<CsIntrusivePointer<U, Policy>> retval;

   auto lambda_internal = [&retval] (auto item) {
      retval.push_back(item);
      return VisitStatus::VisitMore;
   };

   visit<U>(lambda_internal);

   return retval;
}

template <typename T, typename Policy>
template <typename U, typename F>
std::vector<CsIntrusivePointer<U, Policy>> CsNodeManager<T, Policy>::find_children(const F &lambda) const
{
   std::vector<CsIntrusivePointer<U, Policy>> retval;

   auto lambda_internal = [&retval, &lambda] (auto item) {
      if (lambda(item) == true) {
         retval.push_back(item);
      }

      return VisitStatus::VisitMore;
   };

   visit<U>(lambda_internal);

   return retval;
}

template <typename T, typename Policy>
template <typename U, typename F>
VisitStatus CsNodeManager<T, Policy>::visit(const F &lambda, VisitChildren option) const
{
   enum VisitStatus retval = VisitStatus::VisitMore;

   for (const auto &item : m_children) {

      if constexpr(std::is_same_v<T, U>) {
         retval = lambda(item);

         if (retval == VisitStatus::Finished) {
            return retval;
         }

      } else {
         const CsIntrusivePointer<U, Policy> &child = dynamic_pointer_cast<U>(item);

         if (child != nullptr) {
            retval = lambda(child);

            if (retval == VisitStatus::Finished) {
               return retval;
            }
         }
      }

      if (option == VisitChildren::Recursive) {
         retval = item->template visit<U>(lambda);

         if (retval == VisitStatus::Finished) {
            return retval;
         }
      }
   }

   return retval;
}

}   // end namespace

#endif
