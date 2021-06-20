#pragma once

namespace sci {

[[nodiscard]] constexpr auto isdigit(int const c)
{
  return c >= '0' && c <= '9';
}

[[nodiscard]] constexpr auto isspace(int const c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

[[nodiscard]] constexpr auto isalpha(int const c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

[[nodiscard]] constexpr auto isalnum(int const c)
{
  return isalpha(c) || isdigit(c);
}

}// namespace sci

