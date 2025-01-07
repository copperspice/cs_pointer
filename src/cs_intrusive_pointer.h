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

#ifndef LIB_CS_INTRUSIVE_POINTER_H
#define LIB_CS_INTRUSIVE_POINTER_H

#include <atomic>
#include <memory>

namespace CsPointer {

enum class CsIntrusiveAction {
   Normal,
   NoDelete,
};

class CsIntrusiveBase
{
 public:
   virtual ~CsIntrusiveBase() = default;

 private:
   mutable std::atomic<std::size_t> m_count = 0;

   void cs_inc_ref_count() const noexcept {
      m_count.fetch_add(1);
   }

   void cs_dec_ref_count(CsIntrusiveAction action) const {
      std::size_t old_count = m_count.fetch_sub(1);

      if (action != CsIntrusiveAction::NoDelete) {
         if (old_count == 1) {
            delete this;
         }
      }
   }

   std::size_t cs_get_ref_count() const {
      return m_count.load();
   }

   friend class CsIntrusiveDefaultPolicy;
};

class CsIntrusiveBase_CM
{
 public:
   CsIntrusiveBase_CM() = default;

   CsIntrusiveBase_CM(const CsIntrusiveBase_CM &other) noexcept {
      m_count = 0;
   }

   CsIntrusiveBase_CM &operator=(const CsIntrusiveBase_CM &other) noexcept {
      // copy assignment does not alter the reference count
      return *this;
   }

   CsIntrusiveBase_CM(CsIntrusiveBase_CM &&other) noexcept {
      m_count = 0;
   }

   CsIntrusiveBase_CM &operator=(CsIntrusiveBase_CM &&other) noexcept {
      // move assignment does not alter the reference count
      return *this;
   }

   virtual ~CsIntrusiveBase_CM() = default;

 private:
   mutable std::atomic<std::size_t> m_count = 0;

   void cs_inc_ref_count() const noexcept {
      m_count.fetch_add(1);
   }

   void cs_dec_ref_count(CsIntrusiveAction action) const {
      std::size_t old_count = m_count.fetch_sub(1);

      if (action != CsIntrusiveAction::NoDelete) {
         if (old_count == 1) {
            delete this;
         }
      }
   }

   std::size_t cs_get_ref_count() const {
      return m_count.load();
   }

   friend class CsIntrusiveDefaultPolicy;
};

class CsIntrusiveDefaultPolicy
{
 public:
   template <typename T>
   static void inc_ref_count(const T *ptr) noexcept {
      ptr->cs_inc_ref_count();
   }

   template <typename T>
   static void dec_ref_count(const T *ptr, CsIntrusiveAction action = CsIntrusiveAction::Normal) {
      ptr->cs_dec_ref_count(action);
   }

   template <typename T>
   static std::size_t get_ref_count(const T *ptr) noexcept {
      return ptr->cs_get_ref_count();
   }
};

template <typename T, typename Policy = CsIntrusiveDefaultPolicy>
class CsIntrusivePointer
{
 public:
   using pointer      = T *;
   using element_type = T;

   using Pointer      = pointer;
   using ElementType  = element_type;

   constexpr CsIntrusivePointer() noexcept
      : m_ptr(nullptr)
   {
   }

   constexpr CsIntrusivePointer(std::nullptr_t) noexcept
      : m_ptr(nullptr)
   {
   }

   template <typename U>
   explicit CsIntrusivePointer(U *p)
      : m_ptr(p)
   {
      // static_assert(std::is_base_of_v<CsIntrusiveBase, T>, "Class T must inherit from CsIntrusiveBase");

      if (m_ptr != nullptr) {
         Policy::inc_ref_count(m_ptr);
      }
   }

   ~CsIntrusivePointer()
   {
      if (m_ptr != nullptr) {
         Policy::dec_ref_count(m_ptr);
      }
   }

   // copy constructor
   CsIntrusivePointer(const CsIntrusivePointer &other)
      : m_ptr(other.m_ptr)
   {
      if (m_ptr != nullptr) {
         Policy::inc_ref_count(m_ptr);
      }
   }

   CsIntrusivePointer &operator=(const CsIntrusivePointer &other) {
      return *this = other.m_ptr;
   }

   // move constructor
   CsIntrusivePointer(CsIntrusivePointer &&other) noexcept
      : m_ptr(other.m_ptr)
   {
      other.m_ptr = nullptr;
   }

   CsIntrusivePointer &operator=(CsIntrusivePointer &&other) noexcept {
      if (m_ptr == other.m_ptr) {
         return *this;
      }

      if (m_ptr != nullptr) {
         Policy::dec_ref_count(m_ptr);
         m_ptr = nullptr;
      }

      std::swap(m_ptr, other.m_ptr);
      return *this;
   }

   CsIntrusivePointer &operator=(T *p) {
      if (m_ptr != p) {
         if (p != nullptr) {
            Policy::inc_ref_count(p);
         }

         if (m_ptr != nullptr) {
            Policy::dec_ref_count(m_ptr);
         }

         m_ptr = p;
      }

      return *this;
   }

   template <typename U>
   CsIntrusivePointer(const CsIntrusivePointer<U> &p) noexcept
      : m_ptr(p.m_ptr)
   {
      if (m_ptr != nullptr) {
         Policy::inc_ref_count(m_ptr);
      }
   }

   template <typename U>
   CsIntrusivePointer & operator=(const CsIntrusivePointer<U> &p) {
      CsIntrusivePointer<T>(p).swap(*this);
      return *this;
   }

   template <typename U>
   CsIntrusivePointer(CsIntrusivePointer<U> &&p) noexcept
      : m_ptr(p.m_ptr)
   {
      p.m_ptr = nullptr;
   }

   template <typename U>
   CsIntrusivePointer & operator=(CsIntrusivePointer<U> &&p) {
      if (m_ptr == p.m_ptr) {
         return *this;
      }

      if (m_ptr != nullptr) {
         Policy::dec_ref_count(m_ptr);
      }

      m_ptr   = p.m_ptr;
      p.m_ptr = nullptr;

      return *this;
   }

   T &operator*() const noexcept {
      return *m_ptr;
   }

   T *operator->() const noexcept {
      return m_ptr;
   }

   bool operator !() const noexcept {
      return m_ptr == nullptr;
   }

   operator bool() const {
      return m_ptr != nullptr;
   }

   //
   void clear() noexcept {
      reset();
   }

   Pointer data() const noexcept {
      return m_ptr;
   }

   Pointer get() const noexcept
   {
      return m_ptr;
   }

   bool is_null() const noexcept {
      return m_ptr == nullptr;
   }

   Pointer release_if() noexcept {
      if (use_count() == 1) {
         Pointer tmpPtr = m_ptr;
         Policy::dec_ref_count(m_ptr, CsIntrusiveAction::NoDelete);

         m_ptr = nullptr;

         return tmpPtr;

      } else {
         return nullptr;

      }
   }

   void reset() {
      if (m_ptr != nullptr) {
         Policy::dec_ref_count(m_ptr);
      }

      m_ptr = nullptr;
   }

   template <typename U>
   void reset(U *p) {
      CsIntrusivePointer<T>(p).swap(*this);
   }

   void swap(CsIntrusivePointer &other) noexcept {
      std::swap(m_ptr, other.m_ptr);
   }

   std::size_t use_count() const noexcept {
      if (m_ptr == nullptr) {
         return 0;

      } else {
         return Policy::get_ref_count(m_ptr);
      }
   }

 private:
   T *m_ptr;

   template <typename U, typename OtherPolicy>
   friend class CsIntrusivePointer;
};

template <typename T, typename... Args>
CsIntrusivePointer<T> make_intrusive(Args &&... args)
{
   return CsIntrusivePointer<T>(new T(std::forward<Args>(args)...));
}

// equal
template <typename T1, typename T2>
bool operator==(const CsIntrusivePointer<T1> &ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
   return ptr1.get() == ptr2.get();
}

template <typename T1, typename T2>
bool operator==(const CsIntrusivePointer<T1> &ptr1, const T2 *ptr2) noexcept
{
    return ptr1.get() == ptr2;
}

template <typename T1, typename T2>
bool operator==(const T1 *ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
    return ptr1 == ptr2.get();
}

template <typename T>
bool operator==(const CsIntrusivePointer<T> &ptr1, std::nullptr_t) noexcept
{
   return ptr1.get() == nullptr;
}

template <typename T>
bool operator==(std::nullptr_t, const CsIntrusivePointer<T> &ptr2) noexcept
{
   return nullptr == ptr2.get();
}

// not equal
template <typename T1, typename T2>
bool operator!=(const CsIntrusivePointer<T1> &ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
   return ptr1.get() != ptr2.get();
}

template <typename T1, typename T2>
bool operator!=(const CsIntrusivePointer<T1> &ptr1, const T2 *ptr2) noexcept
{
    return ptr1.get() != ptr2;
}

template <typename T1, typename T2>
bool operator!=(const T1 *ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
    return ptr1 != ptr2.get();
}

template <typename T>
bool operator!=(const CsIntrusivePointer<T> &ptr1, std::nullptr_t) noexcept
{
   return ptr1.get() != nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const CsIntrusivePointer<T> &ptr2) noexcept
{
   return nullptr != ptr2.get();
}

// compare
template <typename T1, typename T2>
bool operator<(const CsIntrusivePointer<T1> &ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
   return ptr1.get() < ptr2.get();
}

template <typename T1, typename T2>
bool operator<=(const CsIntrusivePointer<T1> &ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
   return ptr1.get() <= ptr2.get();
}

template <typename T1, typename T2>
bool operator>(const CsIntrusivePointer<T1> &ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
   return ptr1.get() > ptr2.get();
}

template <typename T1, typename T2>
bool operator>=(const CsIntrusivePointer<T1> &ptr1, const CsIntrusivePointer<T2> &ptr2) noexcept
{
   return ptr1.get() >= ptr2.get();
}

template <typename T>
void swap(CsIntrusivePointer<T> &ptr1, CsIntrusivePointer<T> &ptr2) noexcept
{
   ptr1.swap(ptr2);
}

// cast functions
template <typename T, typename U>
CsIntrusivePointer<T> const_pointer_cast(const CsIntrusivePointer<U> &ptr)
{
   return CsIntrusivePointer<T>(const_cast<T *> (ptr.get()));
}

template <typename T, typename U>
CsIntrusivePointer<T> dynamic_pointer_cast(const CsIntrusivePointer<U> &ptr)
{
   return CsIntrusivePointer<T>(dynamic_cast<T *> (ptr.get()));
}

template <typename T, typename U>
CsIntrusivePointer<T> static_pointer_cast(const CsIntrusivePointer<U> &ptr)
{
   return CsIntrusivePointer<T>(static_cast<T *> (ptr.get()));
}

}   // end namespace

#endif
