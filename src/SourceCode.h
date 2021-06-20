#pragma once
#include <string_view>

#include <cassert>

#include "my_ctype.h"

namespace sci {

class SourceCode
{
  std::string_view src_;
  std::size_t size_{ 0 };

public:
  constexpr SourceCode(std::string_view const source)
    : src_{ source }, size_{ source.size() }
  {}

  [[nodiscard]] constexpr auto peekNextChar(int& i) const noexcept -> char const*
  {
    if (i < size_) {
      return &src_[i];
    }
    return nullptr;
  }
  constexpr auto removeChar(int& i) const noexcept -> void { ++i; }
  [[nodiscard]] constexpr auto getNextChar(int& i) const noexcept -> char const*
  {
    auto c = peekNextChar(i);
    removeChar(i);
    return c;
  }
  constexpr auto ignoreToNewLine(int& i) const noexcept -> void
  {
    for (auto c = getNextChar(i); c; c = getNextChar(i)) {
      if (*c == '\n') {
        return;
      }
    }
  }
  constexpr auto readWholeWord(char const* first_char, int& i) const noexcept -> std::string_view
  {
    int size{ 1 };
    for (auto c = peekNextChar(i); c; c = peekNextChar(i)) {
      if (*c == '_' || sci::isalnum(*c)) {
        ++size;
        removeChar(i);

      } else {
        break;
      }
    }
    return std::string_view(first_char, size);
  }
  constexpr auto readWholeInt(int const first_char, int& i) const noexcept -> int
  {
    assert(sci::isdigit(first_char));
    int result{ first_char - '0' };
    for (auto c = peekNextChar(i); c; c = peekNextChar(i)) {
      if (sci::isdigit(*c)) {
        result *= 10;
        result += *c - '0';
        removeChar(i);

      } else {
        break;
      }
    }
    return result;
  }
};

}// namespace sci
