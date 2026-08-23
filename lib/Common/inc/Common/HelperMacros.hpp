/**
 * \file HelperMacros.hpp
 * \brief A file that contains a collection of helper macros
 */
#pragma once
#ifndef HELPERMACROS__
#define HELPERMACROS__

/// INCLUDES
// #include <csignal>
// #include <cstdio>
//
/// USER INCLUDES

/// NAMESPACE

/// DEFINES
#define PARENS ()

#define STRINGIZE(x) #x
#define ENUM_ITEM(tuple) ENUM_ITEM_IMPL tuple
#define ENUM_ITEM_IMPL(name, ...) name __VA_OPT__(= __VA_ARGS__)

#define STRING_ITEM(tuple) STRING_ITEM_IMPL tuple
#define STRING_ITEM_IMPL(name, ...) #name

// The following macros support the expansion of up to 342 variadic arguments
// I dgenuinely took this from stack overflow about a year ago
// I don't think this is valid anymore
#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

// This NEEDs C++20 or greater!
#define FOR_EACH(macro, ...)                                                   \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define FOR_EACH_HELPER(macro, a1, ...)                                        \
  macro(a1) __VA_OPT__(, FOR_EACH_AGAIN PARENS(macro, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define ENUM_AND_STRING(enumClassName, ...)                                    \
  enum class enumClassName { FOR_EACH(ENUM_ITEM, __VA_ARGS__), MAX };          \
  constexpr std::string_view enumClassName##Strings[] = {                      \
      FOR_EACH(STRING_ITEM, __VA_ARGS__), "MAX"};                              \
  constexpr std::string_view get##enumClassName##str(enumClassName rhs) {      \
    return enumClassName##Strings[static_cast<std::size_t>(rhs)];              \
  }

#define ENUM_EXACT_BASE_AND_STRING(enumClassName, deriveFromType, ...)         \
  enum class enumClassName : deriveFromType {                                  \
    FOR_EACH(ENUM_ITEM, __VA_ARGS__)                                           \
  };                                                                           \
  constexpr std::string_view enumClassName##Strings[] = {                      \
      FOR_EACH(STRING_ITEM, __VA_ARGS__)};                                     \
  constexpr std::string_view get##enumClassName##Str(enumClassName rhs);

#define ENUM_BASE_AND_STRING(enumClassName, deriveFromType, ...)               \
  enum class enumClassName : deriveFromType {                                  \
    FOR_EACH(ENUM_ITEM, __VA_ARGS__),                                          \
    MAX                                                                        \
  };                                                                           \
  constexpr std::string_view enumClassName##Strings[] = {                      \
      FOR_EACH(STRING_ITEM, __VA_ARGS__), "MAX"};                              \
  constexpr std::string_view get##enumClassName##Str(enumClassName rhs);

#define ENUM(enumClassName, ...) enum class enumClassName { __VA_ARGS__, MAX };

#define GET_ENUM_IMPL(enumClassName)                                           \
  constexpr std::string_view get##enumClassName##str(enumClassName rhs) {      \
    return enumClassName##Strings[static_cast<std::size_t>(rhs)];              \
  }

#define ENUM_EXACT_AND_STRING(enumClassName, ...)                              \
  enum class enumClassName { __VA_ARGS__ };                                    \
  constexpr std::string_view enumClassName##Strings[] = {                      \
      FOR_EACH(STRINGIZE, __VA_ARGS__)};                                       \
  constexpr std::string_view get##enumClassName##str(enumClassName rhs);

#define ENUM_EXACT(enumClassName, ...) enum class enumClassName { __VA_ARGS__ };

// I am lazy
#define aRetVal auto aReturn

#endif // HELPERMACROS__