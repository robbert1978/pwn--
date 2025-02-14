#pragma once

#include "Error.hpp"

#define __STR(x) #x
#define STR(x) __STR(x)
#define __WIDE(x) L#x
#define WIDECHAR(x) __WIDE(x)
#define __WIDE2(x) L##x
#define WIDECHAR2(x) __WIDE2(x)
#define CONCAT(x, y) (x##y)


#if defined(PWN_BUILD_FOR_WINDOWS)
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define UMDF_USING_NTSTATUS

// Windows Header Files
#pragma warning(push)
#pragma warning(disable : 4005) // Disable macro re-definition warnings

// clang-format off
#include <phnt_windows.h>
#include <phnt.h>
// clang-format on

#pragma warning(pop)


#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) ((void)x)
#endif // UNREFERENCED_PARAMETER

#ifndef UnusedParameter
#define UnusedParameter UNREFERENCED_PARAMETER
#endif // UnusedParameter

#elif defined(PWN_BUILD_FOR_LINUX)

//
// Windows SAL compat stuff
//
#define _In_
#define _In_opt_
#define _Out_
#define _Out_opt_
#define _Inout_
#define _Inout_opt_

#define _Success_(c)

#include <dlfcn.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif // MAX_PATH

#ifndef UnusedParameter
#define UnusedParameter(x) (void)(x)
#endif // UnusedParameter

#endif

#ifndef PWN_DEPRECATED
#define PWN_DEPRECATED __declspec(deprecated)
#endif

#ifndef PWNAPI
#define PWNAPI __declspec(dllexport)
#endif

#ifndef __countof
#define __countof(x) (sizeof(x) / sizeof(x[0]))
#endif


#ifndef MIN
#define MIN(x, y) ((((size_t)x) < ((size_t)y)) ? (x) : (y))
#endif


using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

#if defined(PWN_BUILD_FOR_WINDOWS)
using uptr  = ULONG_PTR;
using usize = SIZE_T;
using ssize = SSIZE_T;
#elif defined(PWN_BUILD_FOR_LINUX)
using uptr  = uintptr_t;
using usize = size_t;
using ssize = ssize_t;
#endif

#ifndef UnreferencedParameter
#define UnreferencedParameter(x) ((void)(x))
#endif // UnreferencedParameter

using namespace std::literals::string_view_literals;
using namespace std::literals::chrono_literals;

///
/// @brief A constexpr map
/// @ref https://xuhuisun.com/post/c++-weekly-2-constexpr-map/
///
/// @tparam Key
/// @tparam Value
/// @tparam Size
///
template<typename Key, typename Value, usize Size>
struct CMap
{
    std::array<std::pair<Key, Value>, Size> data;

    [[nodiscard]] constexpr Value
    at(const Key& key) const
    {
        const auto itr = std::find_if(
            cbegin(data),
            cend(data),
            [&key](const auto& v)
            {
                return v.first == key;
            });
        if ( itr != end(data) )
        {
            return itr->second;
        }
        else
        {
            throw std::range_error("Not Found");
        }
    }

    [[nodiscard]] constexpr Value
    operator[](const Key& key) const
    {
        return at(key);
    }
};


///
/// @brief A basic constexpr generic buffer
/// @ref https://www.cppstories.com/2021/constexpr-new-cpp20/
///
/// @tparam T
///
template<typename T>
class CBuffer
{
public:
    constexpr CBuffer(size_t n) noexcept : size_(n), mem_(new T[n])
    {
    }
    constexpr ~CBuffer() noexcept
    {
        delete[] mem_;
    }

    constexpr CBuffer(const CBuffer& other) noexcept : size_(other.size_)
    {
        if ( &other != this )
        {
            mem_ = new T[size_];
            std::copy(other.mem_, other.mem_ + size_, mem_);
        }
    }

    constexpr CBuffer(CBuffer&& other) noexcept
    {
        if ( &other != this )
        {
            mem_        = other.mem_;
            size_       = other.size_;
            other.mem_  = nullptr;
            other.size_ = 0;
        }
    }

    constexpr CBuffer&
    operator=(const CBuffer& other) noexcept
    {
        if ( &other != this )
        {
            mem_ = new T[size_];
            std::copy(other.mem_, other.mem_ + size_, mem_);
        }
        return *this;
    }

    constexpr CBuffer&
    operator=(CBuffer&& other) noexcept
    {
        if ( &other != this )
        {
            mem_        = other.mem_;
            size_       = other.size_;
            other.mem_  = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    constexpr T&
    operator[](size_t id) noexcept
    {
        return mem_[id];
    }
    constexpr const T&
    operator[](size_t id) const noexcept
    {
        return mem_[id];
    }

    constexpr T*
    data() const noexcept
    {
        return mem_;
    }
    constexpr size_t
    size() const noexcept
    {
        return size_;
    }

private:
    T* mem_ {nullptr};
    size_t size_ {0};
};


///
/// @brief A basic constexpr bitmask class
///
/// @tparam T
/// @tparam std::enable_if<std::is_enum<T>::value>::type
///
template<typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
class CBitMask
{
    using N = typename std::underlying_type<T>::type;

    static constexpr N
    get(T a)
    {
        return static_cast<N>(a);
    }

    explicit constexpr CBitMask(N a) : m_val(a)
    {
    }

public:
    constexpr CBitMask() : m_val(0)
    {
    }

    constexpr CBitMask(T a) : m_val(get(a))
    {
    }

    constexpr CBitMask
    operator|(T t)
    {
        return CBitMask(m_val | get(t));
    }

    constexpr bool
    operator&(T t)
    {
        return m_val & get(t);
    }

    constexpr N const
    get() const
    {
        return m_val;
    }

private:
    N m_val = 0;
};


///
///@brief Flattenable types
///
///@tparam T
///
template<typename T>
concept Flattenable = std::same_as<T, std::vector<u8>> || std::same_as<T, std::string> || std::same_as<T, std::wstring>;


///
///@brief Calculate the size of a buffer that could contain *all* the given
/// flattenable typed arguments.
///
///@tparam T
///@tparam Args
///@param arg
///@param args
///@return constexpr usize
///
template<Flattenable T, Flattenable... Args>
constexpr usize
SumSizeOfFlattenable(T arg, Args... args)
{
    usize sz = 0;

    if constexpr ( std::is_same_v<T, std::string> )
    {
        sz += arg.size();
    }
    else if constexpr ( std::is_same_v<T, std::wstring> )
    {
        sz += arg.size() * sizeof(wchar_t);
    }
    else if constexpr ( std::is_same_v<T, std::vector<u8>> )
    {
        sz += arg.size();
    }
    else
    {
        sz += sizeof(arg);
    }

    if constexpr ( sizeof...(args) > 0 )
    {
        return sz + SumSizeOfFlattenable(args...);
    }

    return sz;
}
