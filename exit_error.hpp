#ifndef EXIT_ERROR_HPP
# define EXIT_ERROR_HPP

#if __cplusplus < 201703L
# error exit_error.hpp requires C++17 or above.
#else

#include <cerrno>
#include <cstdio>
#include <stdarg.h>
#include <cstdint>
#include <system_error>
#include <type_traits>

// On Linux glibc exposes program_invocation_* variables.
// Otherwise, fall-back to __progname. musl doesn't have
// program_invocation_* but it uses __progname.
#ifdef __GLIBC__
static const char *program_name = program_invocation_short_name;
#else
extern const char *__progname;
static const char *program_name = __progname;
#endif

namespace exit_error {
inline constexpr std::int32_t exit_success = 0;
inline constexpr std::int32_t exit_failure = 1;

/**********************************************************************************
 * @breif: Method exit() prints the user provided message followed by
 *         the error (provided by errno) and then exits. */
#if __cplusplus == 201703L
// We can just use enable_if but requires is much nicer to use.
// It also doesn't bloat with template heavy code.
// This part is only for compatibility.
template <typename ExitCode,
          typename = std::enable_if_t<std::is_same_v<ExitCode, std::int32_t>>,
          typename FormatPtr,
          typename = std::enable_if_t<
	      !std::is_reference_v<FormatPtr> &&
              std::is_same_v<std::remove_const_t<FormatPtr>, char>>>
#else
template <typename ExitCode, typename FormatPtr>
requires (std::is_same_v<ExitCode, std::int32_t> &&
	  !std::is_reference_v<FormatPtr> &&
	  std::is_same_v<std::remove_const_t<FormatPtr>, char>)
#endif
[[noreturn]]
void error(ExitCode exit_code, FormatPtr *fmt, ...) {
    va_list vl;
    
    va_start(vl, fmt);
    std::fprintf(stderr, "%s: ", program_name);
    vfprintf(stderr, fmt, vl);
    const auto errno_message =
	std::error_code(errno, std::generic_category()).message();
    std::fprintf(stderr, ": %s\n", errno_message.c_str());
    va_end(vl);
    std::exit(exit_code);
}

/**********************************************************************************
 * @breif: Same as error, but you can specify what type of error
 *         you want to raise, which you can pass either by using
 *         errno or error code as you desire. */
#if __cplusplus == 201703L
template <typename ErrorCode,
	  typename = std::enable_if_t<
	      std::is_same_v<std::remove_cv_t<ErrorCode>, std::int32_t>>,
	  typename ExitCode,
	  typename = std::enable_if_t<std::is_same_v<ExitCode, std::int32_t>>,
	  typename FormatPtr,
	  typename = std::enable_if_t<
	      !std::is_reference_v<FormatPtr> &&
	      std::is_same_v<std::remove_const_t<FormatPtr>, char>>>
#else
template <typename ErrorCode, typename ExitCode, typename FormatPtr>
requires (std::is_same_v<std::remove_cv_t<ErrorCode>, std::int32_t> &&
	  std::is_same_v<ExitCode, std::int32_t> &&
	  !std::is_reference_v<FormatPtr> &&
	  std::is_same_v<std::remove_const_t<FormatPtr>, char>)
#endif
[[noreturn]]
void with_error_code(ErrorCode error_code, ExitCode exit_code,
		     FormatPtr *fmt, ...) {
    va_list vl;

    va_start(vl, fmt);
    std::fprintf(stderr, "%s: ", program_name);
    vfprintf(stderr, fmt, vl);
    const auto errno_message =
	std::error_code(error_code, std::generic_category()).message();
    std::fprintf(stderr, ": %s\n", errno_message.c_str());
    va_end(vl);
    std::exit(exit_code);    
}

/**********************************************************************************/
// @breif: Method make_error() prints the user provided message then exits.
#if __cplusplus == 201703L
template <typename ExitCode,
          typename = std::enable_if_t<std::is_same_v<ExitCode, std::int32_t>>,
          typename FormatPtr,
          typename = std::enable_if_t<
	      !std::is_reference_v<FormatPtr> &&
              std::is_same_v<std::remove_const_t<FormatPtr>, char>>>
#else
template <typename ExitCode, typename FormatPtr>
requires (std::is_same_v<ExitCode, std::int32_t> &&
	  !std::is_reference_v<FormatPtr> &&
	  std::is_same_v<std::remove_const_t<FormatPtr>, char>)
#endif
[[noreturn]]
void make_error(ExitCode exit_code, FormatPtr *fmt, ...) {
    va_list vl;

    va_start(vl, fmt);
    std::fprintf(stderr, "%s: ", program_name);
    vfprintf(stderr, fmt, vl);
    std::fputc('\n', stderr);
    va_end(vl);
    std::exit(exit_code);
}

/**********************************************************************************
 * @breif: Method warn() prints the user provided message. It's similar to
 *         error, except it doesn't terminate the thread. */
#if __cplusplus == 201703L
template <typename FormatPtr,
          typename = std::enable_if_t<
	      !std::is_reference_v<FormatPtr> &&
              std::is_same_v<std::remove_const_t<FormatPtr>, char>>>
#else
template <typename ExitCode, typename FormatPtr>
requires (!std::is_reference_v<FormatPtr> &&
	  std::is_same_v<std::remove_const_t<FormatPtr>, char>)
#endif
void warn(FormatPtr *fmt, ...) {
    va_list vl;

    va_start(vl, fmt);
    std::fprintf(stderr, "%s: ", program_name);
    vfprintf(stderr, fmt, vl);
    const auto errno_message =
	std::error_code(errno, std::generic_category()).message();
    std::fprintf(stderr, ": %s\n", errno_message.c_str());
    va_end(vl);
}

/**********************************************************************************
 * @breif: Same as error, but you can specify what type of error
 *         you want to raise, which you can pass either by using
 *         errno or error code as you desire. */
#if __cplusplus == 201703L
template <typename ErrorCode,
	  typename = std::enable_if_t<std::is_same_v<ErrorCode, std::int32_t>>,
	  typename FormatPtr,
	  typename = std::enable_if_t<
	      !std::is_reference_v<FormatPtr> &&
	      std::is_same_v<std::remove_const_t<FormatPtr>, char>>>
#else
template <typename ErrorCode, typename FormatPtr>
requires (std::is_same_v<std::remove_cv_t<ErrorCode>, std::int32_t> &&
	  !std::is_reference_v<FormatPtr> &&
	  std::is_same_v<std::remove_const_t<FormatPtr>, char>)
#endif
void with_warn_code(ErrorCode error_code, FormatPtr *fmt, ...) {
    va_list vl;

    va_start(vl, fmt);
    std::fprintf(stderr, "%s: ", program_name);
    vfprintf(stderr, fmt, vl);
    const auto errno_message =
	std::error_code(errno, std::generic_category()).message();
    std::fprintf(stderr, ": %s\n", errno_message.c_str());
    va_end(vl);
}

/*********************************************************************************
 * @breif: Method make_warn() prints the user provided message.
 *         It doesn't terminate the thread. */
#if __cplusplus == 201703L
template <typename FormatPtr,
          typename = std::enable_if_t<
	      !std::is_reference_v<FormatPtr> &&
	      std::is_same_v<std::remove_const_t<FormatPtr>, char>>>
#else
template <typename FormatPtr>
requires (!std::is_reference_v<FormatPtr> &&
	  std::is_same_v<std::remove_const_t<FormatPtr>, char>)
#endif
void make_warn(FormatPtr fmt, ...) {
    va_list vl;

    va_start(vl, fmt);
    std::fprintf(stderr, "%s: ", program_name);
    vfprintf(stderr, fmt, vl);
    std::fputc('\n', stderr);
    va_end(vl);
}

};

#endif /* __cplusplus */
#endif /* EXIT_ERROR_HPP */
