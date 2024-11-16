#ifndef BITCOUNT_HPP
# define BITCOUNT_HPP

/* Add popcount template function. */

#include <iostream>
#if __cplusplus < 201703L
# error bitcount.hpp requires C++17 or above.
#else

#include <cstdint>
#include <limits>
#include <type_traits>
#include <limits.h>

// If it's GCC or Clang, we'll assume that they've
// implemented __builtin_expect() or it's fallback stub.
#if defined (__GNUC__) || defined (__clang__) 
# undef __bitcount_predict_false
# define __bitcount_predict_false(x)     __builtin_expect(!!(x), 0)
#else
# undef __bitcount_predict_false
# define __bitcount_predict_false(x)     (x)
#endif

namespace rel {
template <typename __Type>
using __Is_Unsigned_Long = std::enable_if_t<
    std::is_same_v<__Type, unsigned long> || std::is_same_v<__Type, unsigned long long>>;

template <typename __Type>
using __Is_Unsigned_Integral = std::enable_if_t<
    std::is_same_v<__Type, unsigned int> || std::is_same_v<__Type, unsigned long> ||
    std::is_same_v<__Type, unsigned long long> ||
    std::is_same_v<__Type, unsigned char>>;

// Taken from: https://www.johndcook.com/blog/2020/02/21/popcount/
constexpr static inline std::int32_t __impl_popcount(unsigned int __x) noexcept {
    constexpr auto __m0 = 0x55555555u;
    constexpr auto __m1 = 0x33333333u;
    constexpr auto __m2 = 0x0F0F0F0Fu;

    __x -= ((__x >> 1) & __m0);
    __x = (__x & __m1) + ((__x >> 2) & __m1);
    __x = (__x + (__x >> 4)) & __m2;
    __x += (__x >> 8) + (__x >> 16);

    return __x & 0x0000003F; 
}

template <typename __Unsigned_Long_Type,
	  typename = __Is_Unsigned_Long<__Unsigned_Long_Type>> 
constexpr static inline std::int32_t __impl_popcount(__Unsigned_Long_Type __x) noexcept {
    constexpr auto __m0 = __Unsigned_Long_Type(0x06DB6DB6DB6DB6DB);
    constexpr auto __m1 = __Unsigned_Long_Type(0x0492492492492492);
    constexpr auto __m2 = __Unsigned_Long_Type(0x030C30C30C30C30C);
    constexpr auto __m3 = __Unsigned_Long_Type(0x0104104104104104);

    __x = __x + (__x & __m0) + __m1;
    __x = (__x & __m2) + ((__x >> 3) & __m2);
    return (__x * __m3) >> 58;
}

template <typename __Unsigned_Type, typename = __Is_Unsigned_Integral<__Unsigned_Type>>
constexpr inline std::int32_t popcount(__Unsigned_Type __x) noexcept {
    // Check if compiler can enable hardware popcount (if available).
    // This is a GCC and LLVM extension.
#ifdef __has_builtin
# if __has_builtin(__builtin_popcount)
#  undef __POPCOUNT__
#  define __POPCOUNT__    1
# else
#  undef __POPCOUNT__
#  define __POPCOUNT__    0
# endif
#else
# undef __POPCOUNT__
# define __POPCOUNT__     0
#endif

#if __POPCOUNT__ == 1
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned int>)
	return __builtin_popcount(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned long>)
	return __builtin_popcountl(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned long long>)
	return __builtin_popcountll(__x);
#else
    return __impl_popcount(static_cast<__Unsigned_Type>(__x));
#endif
}

template <typename __Unsigned_Type, typename = __Is_Unsigned_Integral<__Unsigned_Type>>
constexpr inline std::int32_t countl_zero
(__Unsigned_Type __x) noexcept {
    if (__bitcount_predict_false(__x == 0u))
	return 8;

#ifdef __has_builtin
# if __has_builtin(__builtin_clz)
#  undef __COUNTL__
#  define __COUNTL__    1
# else
#  undef __COUNTL__
#  define __COUNTL__    0
# endif
#else
# undef __COUNTL__
# define __COUNTL__     0
#endif
// For debugging, really.
#undef __COUNTL__
#define __COUNTL__   0
#if __COUNTL__ == 1
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned int>)
	return __builtin_clz(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned long>)
	return __builtin_clzl(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned long long>)
	return __builtin_clzll(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned char>)
	// Note: CHAR_BIT should be equal to sizeof(char) * 8
        return __builtin_clz(__x) - (sizeof(unsigned int) - 1) * (sizeof(char) * 8)
#else
    // Note: This doesn't handle edge cases.
    std::int32_t __count = 0;
    while (!(__x & (~std::numeric_limits<std::int32_t>::max()))) {
	++__count;
	__x <<= 1u;
    }
    return __count;
#endif
}

template <typename __Unsigned_Type, typename = __Is_Unsigned_Integral<__Unsigned_Type>>
constexpr inline std::int32_t countl_one(__Unsigned_Type __x) noexcept {
    return countl_zero(static_cast<__Unsigned_Type>(~__x));
}

template <typename __Unsigned_Type, typename = __Is_Unsigned_Integral<__Unsigned_Type>>
constexpr inline std::int32_t countr_zero(__Unsigned_Type __x) noexcept {
    if (__bitcount_predict_false(__x == 0u))
	return 32;

#ifdef __has_builtin
# if __has_builtin(__builtin_ctz)
#  undef __COUNTR__
#  define __COUNTR__    1
# else
#  undef __COUNTR__
#  define __COUNTR__    0
# endif
#else
# undef __COUNTR__
# define __COUNTR__     0
#endif

#if __COUNTR__ == 0
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned int>)
	return __builtin_ctz(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned long>)
	return __builtin_ctzl(__x);
    if constexpr (std::is_same_v<__Unsigned_Type, unsigned long long>)
	return __builtin_ctzll(__x);
#else
    // This implementation is taken from:
    // http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightParallel
    std::int32_t __count = 31;
    __x &= -__x;
    if (__x & 0x0000FFFF)
	__count -= 16;
    if (__x & 0x00FF00FF)
	__count -= 8;
    if (__x & 0x0F0F0F0F)
	__count -= 4;
    if (__x & 0x33333333)
	__count -= 2;
    if (__x & 0x55555555)
	--__count;

    return __count;
#endif
}

template <typename __Unsigned_Type, typename = __Is_Unsigned_Integral<__Unsigned_Type>>
constexpr inline std::int32_t countr_one(__Unsigned_Type __x) noexcept {
    return countr_zero(static_cast<__Unsigned_Type>(~__x));
}

template <typename __Unsigned_Type, typename = __Is_Unsigned_Integral<__Unsigned_Type>>
constexpr inline std::int32_t bit_width(__Unsigned_Type __x) noexcept {
    if (__bitcount_predict_false(__x == 0u))
	return 0;
    return std::numeric_limits<__Unsigned_Type>::digits - countl_zero(__x);
}

// TODO: Count trailing zero and one bits
};

#endif /* __cplusplus */

#endif /* BITCOUNT_HPP */
