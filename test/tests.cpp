#include <catch2/catch.hpp>

#include "../src/SourceCode.h"
#include "../src/Tokenizer.h"

TEST_CASE("Empty source code", "[tokenizer]")
{
//  sci::SourceCode src{ "" };
//  sci::Tokenizer tok{ &src };
//
//  auto const t = tok.getNextToken();
//  REQUIRE(t.r == sci::Tokenizer::Result::END);
}

TEST_CASE("Basic source code", "[interpreter]")
{
//  sci::SourceCode src{ R"(int main() { return 0; })" };
//  sci::Tokenizer tok{ &src };
//
//  auto const tokens = tok.tokenize();
//
//  REQUIRE(tokens.size() == 9);
//  REQUIRE(tokens[0].type == sci::Token::Type::KWTYPE);
//  REQUIRE(std::get<sci::Token::TKW>(tokens[0].val) == sci::Token::TKW::INT_);
//  REQUIRE(tokens[1].type == sci::Token::Type::ID);
//  REQUIRE(std::get<std::string_view>(tokens[1].val) == "main");
//  REQUIRE(tokens[2].type == sci::Token::Type::OPEN_PAR);
//  REQUIRE(tokens[3].type == sci::Token::Type::CLOSE_PAR);
//  REQUIRE(tokens[4].type == sci::Token::Type::OPEN_CURLY);
//  REQUIRE(tokens[5].type == sci::Token::Type::KWRET);
//  REQUIRE(tokens[6].type == sci::Token::Type::LITERAL);
//  REQUIRE(std::get<int>(tokens[6].val) == 0);
//  REQUIRE(tokens[7].type == sci::Token::Type::SEMICOLON);
//  REQUIRE(tokens[8].type == sci::Token::Type::CLOSE_CURLY);
}
