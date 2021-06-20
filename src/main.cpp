#include <algorithm>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <cassert>
#include <cctype>

#include <fmt/core.h>
#include <magic_enum.hpp>

#define SCI_NONCONSTEXPR
#include "Interpreter.h"
#include "Parser.h"
#include "SourceCode.h"
#include "Tokenizer.h"

auto main(/*int argc, char const **argv*/) -> int
{
  constexpr sci::SourceCode src{ R"(
int ahoj() {
   return 420;
}

int f() {
   return ahoj();
}

int main() {
   return f();
}
)"
  };
  sci::Tokenizer<100> tok{ src };

  auto tokens = tok.tokenize();

  sci::Parser<100, 100> par{ tokens };
  auto exe = par.parse();
  sci::Interpreter<10, 3, 10> interpreter;
  auto result = interpreter.interpret(exe);
  fmt::print("RESULT: {}\n", result);

  //sci::SourceCode src{ "" };
  //sci::Tokenizer tok{ &src };

  //auto t = tok.getNextToken();

  //fmt::print("{}", t.r == sci::Tokenizer::Result::END);
  //  if (argc < 2) {
  //    fmt::print("No parameter specified\n");
  //    return 0;
  //  }
  //
  //  sci::FileSourceCode src_stream{ argv[1] };
  //  if (!src_stream.is_open()) {
  //    fmt::print("Could not open specified source file: {}\n", argv[1]);
  //    exit(1);
  //  }
  //
  //  sci::SourceCode input_src{ &src_stream };
  //  sci::Tokenizer tokenizer{ &input_src };
  //
  //  std::vector<sci::Token> const tokens_forward = tokenizer.tokenize();
  //
  //  for (auto const &tok : tokens_forward) {
  //    fmt::print("{}\t\t", magic_enum::enum_name(tok.type));
  //    if (tok.type == sci::Token::Type::ID) {
  //      fmt::print("{}", std::get<std::string>(tok.val));
  //    } else if (tok.type == sci::Token::Type::LITERAL) {
  //      fmt::print("{}", std::get<int>(tok.val));
  //    }
  //    fmt::print("\n");
  //  }
  //
  //  std::vector<sci::Token> tokens;
  //  std::copy(std::crbegin(tokens_forward), std::crend(tokens_forward), std::back_inserter(tokens));
  //
  //
  //  return 0;
}
