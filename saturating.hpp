#ifndef SATURATING_HPP
# define SATURATING_HPP

#include <limits>
#include <type_traits>

// This isn't much useful as the compiler will try compile
// even after reaching the error.
#if __cplusplus < 201703L
# error this header requires C++17 or above.
#else

// Both __likely() and __predict_* are pre-defined
// in various Unix-like systems. So I always have
// to prepend key to make it unique.
#undef __saturating_predict_false
#define __saturating_predict_false(x)   __builtin_expect(!!(x), 0)

// swapinfo for linux
namespace sat_arith {
template <typename __First>
using __Is_Integral =
    std::enable_if_t<std::is_signed_v<__First> || std::is_unsigned_v<__First>>;

template <typename __First, typename __Second>
using __Is_Integral_Both = std::enable_if_t<
    (std::is_signed_v<__First> || std::is_unsigned_v<__First>)&&(
        std::is_signed_v<__Second> || std::is_unsigned_v<__Second>)>;

template <typename __Type, typename = __Is_Integral<__Type>>
constexpr inline __Type sat_add(__Type __a, __Type __b) noexcept {
    __Type __res{};
    if (!__builtin_add_overflow(__a, __b, &__res))
	return __res;

    if constexpr (std::is_signed_v<__Type>) {
	if (__a < 0 || __b < 0)
	    return std::numeric_limits<__Type>::min();
	else
	    return std::numeric_limits<__Type>::max();
    } else {
	return std::numeric_limits<__Type>::max();
    }
}

template <typename  __Type, typename = __Is_Integral<__Type>>
constexpr inline __Type sat_sub(__Type __a, __Type __b) noexcept {
    __Type __res{};
    if (!__builtin_sub_overflow(__a, __b, &__res))
	return __res;

    if constexpr (std::is_signed_v<__Type>) {
	if (__a < 0 || __b < 0)
	    return std::numeric_limits<__Type>::min();
	else
	    return std::numeric_limits<__Type>::max();
    } else {
	return std::numeric_limits<__Type>::max();
    }
}

template <typename __Type, typename = __Is_Integral<__Type>>
constexpr inline __Type sat_mul(__Type __a, __Type __b) noexcept {
    __Type __res{};
    if (!__builtin_mul_overflow(__a, __b, &__res))
	return __res;

    if constexpr (std::is_signed_v<__Type>) {
	if (__a < 0 && __b < 0)
	    return std::numeric_limits<__Type>::max();
	else
	    return std::numeric_limits<__Type>::min();
    } else {
	return std::numeric_limits<__Type>::max();
    }
}

template <typename __Type, typename = __Is_Integral<__Type>>
constexpr inline __Type sat_div(__Type __a, __Type __b) noexcept {
    if (__saturating_predict_false(__b == 0)) {
	__builtin_printf("abort: cannot divide by 0, __b == 0\n");
        __builtin_abort();
    }

    if (__saturating_predict_false(
	    __a == std::numeric_limits<__Type>::min() &&
	    __b == -1))
	return std::numeric_limits<__Type>::max();

    __Type __res = __a / __b;
    return __res;
}

template <typename __ToType, typename __Value,
	  typename = __Is_Integral_Both<__ToType, __Value>>
constexpr inline __ToType saturate_cast(__Value __a) noexcept {
    constexpr __ToType __max = std::numeric_limits<__ToType>::max();
    constexpr __ToType __min = std::numeric_limits<__ToType>::min();

    if constexpr (std::is_signed_v<__ToType> && std::is_signed_v<__Value>) {
	if (__a > __max)
	    return __max;
	if (__a < __min)
	    return __min;
    }

    if constexpr (std::is_unsigned_v<__ToType> && std::is_signed_v<__Value>) {
	if (__a < __min)
	    return 0;
	if (__a > __max)
	    return __max;
    }

    if constexpr (std::is_signed_v<__ToType> && std::is_unsigned_v<__Value>) {
	// unsigned long long __a value > int max
	if (__a > __max)
	    return __max;
    }

    return static_cast<__ToType>(__a);
}
} // namespace sat_arith

#endif /* __cplusplus */

#endif /* SATURATING_HPP */
