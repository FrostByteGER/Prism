#pragma once

#include <cstddef>
#include <utility>
#include <functional>
#include <memory>
#include <type_traits>

namespace Prism
{
    template <typename T>
    class ObjectPtr
    {
    public:
        /// Construct a null pointer
        constexpr ObjectPtr() noexcept : ptr(std::weak_ptr<T>())
        {
        }

        /// Construct a null pointer
        constexpr ObjectPtr(std::nullptr_t) noexcept : ptr(std::weak_ptr<T>())
        {
        }

        constexpr ObjectPtr(std::weak_ptr<T> ptr_) noexcept : ptr(ptr_)
        {
        }

        /// Get the raw pointer value
        constexpr T* get() const noexcept
        {
            return ptr.lock().get();
        }

        /// Dereference the pointer
        constexpr T& operator*() const noexcept
        {
            return *ptr.lock();
        }

        /// Dereference the pointer for ptr->m usage
        constexpr T* operator->() const noexcept
        {
            return ptr.lock().get();
        }

        /// Allow if(ptr) to test for null
        constexpr explicit operator bool() const noexcept
        {
            return !ptr.expired();
        }

        /// Convert to a raw pointer where necessary
        constexpr explicit operator T*() const noexcept
        {
            return ptr.lock().get();
        }

        /// !ptr is true if ptr is null
        constexpr bool operator!() const noexcept
        {
            return ptr.expired();
        }

        /// Change the value
        void reset(std::weak_ptr<T> ptr_ = std::weak_ptr<T>()) noexcept
        {
            ptr = ptr_;
        }

        /// Check for equality
        friend constexpr bool operator==(ObjectPtr const& lhs, ObjectPtr const& rhs) noexcept
        {
            return lhs.ptr.lock().get() == rhs.ptr.lock().get();
        }

        /// Check for inequality
        friend constexpr bool operator!=(ObjectPtr const& lhs, ObjectPtr const& rhs) noexcept
        {
            return !(lhs == rhs);
        }

    private:
        /// The stored pointer
        std::weak_ptr<T> ptr;
    };
}

namespace std
{
    /// Allow hashing object_ptrs so they can be used as keys in unordered_map
    template <typename T>
    struct hash<Prism::ObjectPtr<T>>
    {
        constexpr size_t operator()(Prism::ObjectPtr<T> const& p) const
            noexcept
        {
            return hash<T*>()(p.get());
        }
    };

    /// Do a static_cast with ObjectPtr
    template <typename To, typename From>
    typename std::enable_if<
        sizeof(decltype(static_cast<To*>(std::declval<From*>()))) != 0,
        Prism::ObjectPtr<To>>::type
    static_pointer_cast(Prism::ObjectPtr<From> p)
    {
        return static_cast<To*>(p.get());
    }

    /// Do a dynamic_cast with ObjectPtr
    template <typename To, typename From>
    typename std::enable_if<
        sizeof(decltype(dynamic_cast<To*>(std::declval<From*>()))) != 0,
        Prism::ObjectPtr<To>>::type
    dynamic_pointer_cast(Prism::ObjectPtr<From> p)
    {
        return dynamic_cast<To*>(p.get());
    }
}
