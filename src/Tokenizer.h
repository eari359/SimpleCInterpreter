#pragma once
#include <array>
#include <variant>

#include "eternal.hpp"

#include "my_ctype.h"

#include "Common.h"
#include "SourceCode.h"

namespace sci {

struct Token
{
  enum class Type {
#include "terminals.inl"

    EMPTY_TOKEN,
    ERROR,
  };

  enum class TKW {
    AUTO_,
    CHAR_,
    INT_,
    VOID_,
  };

  Type type{ Type::EMPTY_TOKEN };
  std::variant<int, std::string_view, TKW, Literal> val;
};

template<std::size_t MaxTokens>
class Tokenizer
{
public:
  using ResultingTokens = std::array<Token, MaxTokens>;
  struct TokenResult
  {
    Token t;
    Result r;
  };
  struct Error
  {
    int line;
    char c;
  };

private:
  SourceCode const& src_;
  int line_num_{ 1 };
  bool ended_{ false };
  Error current_error_{};

public:
  explicit constexpr Tokenizer(SourceCode const& src) noexcept
    : src_{ src }
  {}

  [[nodiscard]] constexpr auto ended() const noexcept { return ended_; }
  [[nodiscard]] constexpr auto getError() const noexcept { return current_error_; }
  [[nodiscard]] constexpr auto getNextToken(int& i) const noexcept -> TokenResult
  {
    constexpr auto const char_token_map = mapbox::eternal::map<char, Token::Type>({
      { '!', Token::Type::EXCLAMATION },
      { '"', Token::Type::QUOTATION },
      { '%', Token::Type::PERCENT },
      { '&', Token::Type::AMPERSAND },
      { '(', Token::Type::OPEN_PAR },
      { ')', Token::Type::CLOSE_PAR },
      { '*', Token::Type::STAR },
      { '+', Token::Type::PLUS },
      { ',', Token::Type::COMMA },
      { '-', Token::Type::MINUS },
      { '.', Token::Type::DOT },
      { '/', Token::Type::SLASH },
      { ':', Token::Type::COLON },
      { ';', Token::Type::SEMICOLON },
      { '<', Token::Type::LEFT },
      { '=', Token::Type::EQUAL },
      { '>', Token::Type::RIGHT },
      { '[', Token::Type::OPEN_BRACKET },
      { '\'', Token::Type::APOSTROPHE },
      { '\\', Token::Type::BACKSLASH },
      { ']', Token::Type::CLOSE_BRACKET },
      { '^', Token::Type::UP },
      { '{', Token::Type::OPEN_CURLY },
      { '|', Token::Type::PIPE },
      { '}', Token::Type::CLOSE_CURLY },
      { '~', Token::Type::TILDE },
    });

    constexpr auto const string_token_map = mapbox::eternal::map<std::string_view, Token>({
      { "auto", { Token::Type::KWTYPE, Token::TKW::AUTO_ } },
      { "char", { Token::Type::KWTYPE, Token::TKW::CHAR_ } },
      { "const", { Token::Type::KWCONST, 0 } },
      { "int", { Token::Type::KWTYPE, Token::TKW::INT_ } },
      { "return", { Token::Type::KWRET, 0 } },
      { "void", { Token::Type::KWTYPE, Token::TKW::VOID_ } },
    });

    for (auto c = src_.getNextChar(i); c != nullptr; c = src_.getNextChar(i)) {
      if (sci::isspace(*c)) {
//        if (*c == '\n') {
//          ++line_num_;
//        }
        continue;

      } else if (*c == '\'') {
        if (*src_.peekNextChar(i) == '\\') {
          src_.removeChar(i);
          char escaped = *src_.getNextChar(i);
          if (*src_.getNextChar(i) != '\'') {
            return { {}, Result::ERR };
            // TODO: octal numbers \nnn - n = number
            // TODO: hexa numbers \xhh - h = hexa number
            // TODO: unicode codepoint \uhhhh
            // TODO: unicode codepoint \Uhhhhhhhh

          } else {
            constexpr auto const escaped_literal_map = mapbox::eternal::map<char, char>({
              { 'a', '\a' },
              { 'b', '\b' },
              { 'f', '\f' },
              { 'n', '\n' },
              { 'r', '\r' },
              { 't', '\t' },
              { 'v', '\v' },
              { '\\', '\\' },
              { '\'', '\'' },
              { '"', '"' },
              { '?', '?' },
            });
            auto it = escaped_literal_map.find(escaped);
            if (it != escaped_literal_map.end()) {
              return { { Token::Type::LITERAL, Literal{ Literal::Type::CHAR_, it->second } }, Result::OK };
            } else {
              return { { Token::Type::ERROR, "Unknown escape char" }, Result::ERR };
            }
          }

        } else { // normal char
          char char_val = *src_.getNextChar(i);
          if (*src_.getNextChar(i) != '\'') {
            return { { Token::Type::ERROR, "Multibyte chars not allowed (char not closed)" } , Result::ERR };
          } else {
            return { { Token::Type::LITERAL, Literal{ Literal::Type::CHAR_, char_val } }, Result::OK };
          }
        }
        continue;

      } else if (*c == '#') {
        src_.ignoreToNewLine(i);
//        ++line_num_;
        continue;

      } else if (*c == '/' && *src_.peekNextChar(i) == '/') {
        src_.ignoreToNewLine(i);
//        ++line_num_;
        continue;

      } else if (sci::isalpha(*c) || *c == '_') {
        std::string_view word = src_.readWholeWord(src_.peekNextChar(i)-1, i);
        auto const it = string_token_map.find(word);
        if (it != string_token_map.end()) {
          return { it->second, Result::OK };

        } else {
          return { { Token::Type::ID, word }, Result::OK };
        }

      } else if (sci::isdigit(*c)) {
        return { { Token::Type::LITERAL, Literal{ Literal::Type::INT_, src_.readWholeInt(*c, i) } }, Result::OK };

      } else {
        auto const it = char_token_map.find(*c);
        if (it != char_token_map.end()) {
          return { { it->second, 0 }, Result::OK };

        } else {
          return { {}, Result::ERR };
        }
      }
    }

    return { {}, Result::END };
  }

  [[nodiscard]] constexpr auto tokenize() const -> ResultingTokens
  {
    ResultingTokens tokens;
    int i = 0;
    TokenResult tr;
    int tok_num = 0;
    for (; (tr = getNextToken(i)).r == Result::OK; ++tok_num) {
      tokens[tok_num] = tr.t;
    }
    if (tr.r == Result::END) {
      tokens[tok_num] = { Token::Type::END_OF_SOURCECODE };
    } else {
      tokens[tok_num] = { Token::Type::ERROR, {} };
    }
    return tokens;
  }
};

}// namespace sci