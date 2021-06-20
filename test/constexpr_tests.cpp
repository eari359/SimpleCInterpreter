#include <catch2/catch.hpp>

#include "../src/Parser.h"
#include "../src/SourceCode.h"
#include "../src/Tokenizer.h"
#include "../src/Interpreter.h"

TEST_CASE("Empty source code - constexpr", "[tokenizer]")
{
  constexpr sci::SourceCode src{ "" };
  constexpr sci::Tokenizer<100> tok{ src };

  constexpr auto tokens = tok.tokenize();
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::END_OF_SOURCECODE);
}

TEST_CASE("escaped char literal - constexpr", "[tokenizer]")
{
  constexpr sci::SourceCode src{ "'\n'" };
  constexpr sci::Tokenizer<100> tok{ src };

  constexpr auto tokens = tok.tokenize();
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::LITERAL);
  constexpr auto lit = std::get<sci::Literal>(tokens[0].val);
  STATIC_REQUIRE(lit.type == sci::Literal::Type::CHAR_);
  STATIC_REQUIRE(std::get<char>(lit.val) == '\n');
  STATIC_REQUIRE(tokens[1].type == sci::Token::Type::END_OF_SOURCECODE);
}

TEST_CASE("char literal - constexpr", "[tokenizer]")
{
  constexpr sci::SourceCode src{ "'k'" };
  constexpr sci::Tokenizer<100> tok{ src };

  constexpr auto tokens = tok.tokenize();
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::LITERAL);
  constexpr auto lit = std::get<sci::Literal>(tokens[0].val);
  STATIC_REQUIRE(lit.type == sci::Literal::Type::CHAR_);
  STATIC_REQUIRE(std::get<char>(lit.val) == 'k');
  STATIC_REQUIRE(tokens[1].type == sci::Token::Type::END_OF_SOURCECODE);
}

TEST_CASE("Source code with invalid character - constexpr", "[tokenizer]")
{
  constexpr sci::SourceCode src{ "void @" };
  constexpr sci::Tokenizer<100> tok{ src };

  constexpr auto tokens = tok.tokenize();
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::KWTYPE);
  STATIC_REQUIRE(std::get<sci::Token::TKW>(tokens[0].val) == sci::Token::TKW::VOID_);
  STATIC_REQUIRE(tokens[1].type == sci::Token::Type::ERROR);
}

TEST_CASE("Basic source code - constexpr", "[interpreter]")
{
  constexpr sci::SourceCode src{ R"(int main() { return 88; })" };
  constexpr sci::Tokenizer<100> tok{ src };

  constexpr auto tokens = tok.tokenize();

  constexpr sci::Parser<100, 100> par{ tokens };
  constexpr auto exe = par.parse();
  constexpr sci::Interpreter<10, 3, 10> interpreter;
  constexpr auto result = interpreter.interpret(exe);

  // TOKEN_CHECK
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::KWTYPE);
  STATIC_REQUIRE(std::get<sci::Token::TKW>(tokens[0].val) == sci::Token::TKW::INT_);
  STATIC_REQUIRE(tokens[1].type == sci::Token::Type::ID);
  STATIC_REQUIRE(std::get<std::string_view>(tokens[1].val) == "main");
  STATIC_REQUIRE(tokens[2].type == sci::Token::Type::OPEN_PAR);
  STATIC_REQUIRE(tokens[3].type == sci::Token::Type::CLOSE_PAR);
  STATIC_REQUIRE(tokens[4].type == sci::Token::Type::OPEN_CURLY);
  STATIC_REQUIRE(tokens[5].type == sci::Token::Type::KWRET);
  STATIC_REQUIRE(tokens[6].type == sci::Token::Type::LITERAL);
  constexpr auto lit = std::get<sci::Literal>(tokens[6].val);
  STATIC_REQUIRE(lit.type == sci::Literal::Type::INT_);
  STATIC_REQUIRE(std::get<int>(lit.val) == 88);
  STATIC_REQUIRE(tokens[7].type == sci::Token::Type::SEMICOLON);
  STATIC_REQUIRE(tokens[8].type == sci::Token::Type::CLOSE_CURLY);
  STATIC_REQUIRE(tokens[9].type == sci::Token::Type::END_OF_SOURCECODE);

  // PROGRAM_CHECK
  STATIC_REQUIRE(exe.functions[0].instructions[0].type == sci::Instruction::Type::VAL);
  STATIC_REQUIRE(exe.functions[0].instructions[1].type == sci::Instruction::Type::RET);
  STATIC_REQUIRE(exe.functions[0].instructions[2].type == sci::Instruction::Type::NONE);
  
  // INTERPRETING_CHECK
  STATIC_REQUIRE(result == 88);
}

TEST_CASE("Basic adding numbers - constexpr", "[interpreter]")
{
  constexpr sci::SourceCode src{ R"(
int main() {
   return 17+13;
}
)" };
  constexpr sci::Tokenizer<40> tok{ src };

  constexpr auto tokens = tok.tokenize();

//  constexpr sci::Parser<40, 50> par{ tokens };
//  constexpr auto exe = par.parse();
//  constexpr sci::Interpreter<10, 3, 10> interpreter;
//  constexpr auto result = interpreter.interpret(exe);

  // TOKEN CHECK
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::KWTYPE);
  STATIC_REQUIRE(std::get<sci::Token::TKW>(tokens[0].val) == sci::Token::TKW::INT_);
  STATIC_REQUIRE(tokens[1].type == sci::Token::Type::ID);
  STATIC_REQUIRE(std::get<std::string_view>(tokens[1].val) == "main");
  STATIC_REQUIRE(tokens[2].type == sci::Token::Type::OPEN_PAR);
  STATIC_REQUIRE(tokens[3].type == sci::Token::Type::CLOSE_PAR);
  STATIC_REQUIRE(tokens[4].type == sci::Token::Type::OPEN_CURLY);
  STATIC_REQUIRE(tokens[5].type == sci::Token::Type::KWRET);
  STATIC_REQUIRE(tokens[6].type == sci::Token::Type::LITERAL);
  constexpr auto lit = std::get<sci::Literal>(tokens[6].val);
  STATIC_REQUIRE(lit.type == sci::Literal::Type::INT_);
  STATIC_REQUIRE(std::get<int>(lit.val) == 17);
  STATIC_REQUIRE(tokens[7].type == sci::Token::Type::PLUS);
  STATIC_REQUIRE(tokens[8].type == sci::Token::Type::LITERAL);
  constexpr auto lit2 = std::get<sci::Literal>(tokens[8].val);
  STATIC_REQUIRE(lit2.type == sci::Literal::Type::INT_);
  STATIC_REQUIRE(std::get<int>(lit2.val) == 13);
  STATIC_REQUIRE(tokens[9].type == sci::Token::Type::SEMICOLON);
  STATIC_REQUIRE(tokens[10].type == sci::Token::Type::CLOSE_CURLY);
  STATIC_REQUIRE(tokens[11].type == sci::Token::Type::END_OF_SOURCECODE);

  // PROGRAM CHECK
//  STATIC_REQUIRE(exe.functions[0].instructions[0].type == sci::Instruction::Type::VAL);
//  STATIC_REQUIRE(exe.functions[0].instructions[1].type == sci::Instruction::Type::VAL);
//  STATIC_REQUIRE(exe.functions[0].instructions[2].type == sci::Instruction::Type::ADD);
//  STATIC_REQUIRE(exe.functions[0].instructions[3].type == sci::Instruction::Type::RET);
//  STATIC_REQUIRE(exe.functions[0].instructions[4].type == sci::Instruction::Type::NONE);

//  // INTERPRETER CHECK
//  STATIC_REQUIRE(result == 30);
}

TEST_CASE("Basic function call - constexpr", "[interpreter]")
{
  constexpr sci::SourceCode src{ R"(
int f() {
   return 10;
}

int main() {
   return f();
}
)"
  };
  constexpr sci::Tokenizer<40> tok{ src };

  constexpr auto tokens = tok.tokenize();

  constexpr sci::Parser<40, 50> par{ tokens };
  constexpr auto exe = par.parse();
  constexpr sci::Interpreter<10, 3, 10> interpreter;
  constexpr auto result = interpreter.interpret(exe);

  // TOKEN_CHECK
  STATIC_REQUIRE(tokens[0].type == sci::Token::Type::KWTYPE);
  STATIC_REQUIRE(std::get<sci::Token::TKW>(tokens[0].val) == sci::Token::TKW::INT_);
  STATIC_REQUIRE(tokens[1].type == sci::Token::Type::ID);
  STATIC_REQUIRE(std::get<std::string_view>(tokens[1].val) == "f");
  STATIC_REQUIRE(tokens[2].type == sci::Token::Type::OPEN_PAR);
  STATIC_REQUIRE(tokens[3].type == sci::Token::Type::CLOSE_PAR);
  STATIC_REQUIRE(tokens[4].type == sci::Token::Type::OPEN_CURLY);
  STATIC_REQUIRE(tokens[5].type == sci::Token::Type::KWRET);
  STATIC_REQUIRE(tokens[6].type == sci::Token::Type::LITERAL);
  constexpr auto lit = std::get<sci::Literal>(tokens[6].val);
  STATIC_REQUIRE(lit.type == sci::Literal::Type::INT_);
  STATIC_REQUIRE(std::get<int>(lit.val) == 10);
  STATIC_REQUIRE(tokens[7].type == sci::Token::Type::SEMICOLON);
  STATIC_REQUIRE(tokens[8].type == sci::Token::Type::CLOSE_CURLY);

  STATIC_REQUIRE(tokens[9].type == sci::Token::Type::KWTYPE);
  STATIC_REQUIRE(std::get<sci::Token::TKW>(tokens[9].val) == sci::Token::TKW::INT_);
  STATIC_REQUIRE(tokens[10].type == sci::Token::Type::ID);
  STATIC_REQUIRE(std::get<std::string_view>(tokens[10].val) == "main");
  STATIC_REQUIRE(tokens[11].type == sci::Token::Type::OPEN_PAR);
  STATIC_REQUIRE(tokens[12].type == sci::Token::Type::CLOSE_PAR);
  STATIC_REQUIRE(tokens[13].type == sci::Token::Type::OPEN_CURLY);
  STATIC_REQUIRE(tokens[14].type == sci::Token::Type::KWRET);
  STATIC_REQUIRE(tokens[15].type == sci::Token::Type::ID);
  STATIC_REQUIRE(tokens[16].type == sci::Token::Type::OPEN_PAR);
  STATIC_REQUIRE(tokens[17].type == sci::Token::Type::CLOSE_PAR);
  STATIC_REQUIRE(tokens[18].type == sci::Token::Type::SEMICOLON);
  STATIC_REQUIRE(tokens[19].type == sci::Token::Type::CLOSE_CURLY);
  STATIC_REQUIRE(tokens[20].type == sci::Token::Type::END_OF_SOURCECODE);

  // PROGRAM_CHECK
  STATIC_REQUIRE(exe.functions[0].instructions[0].type == sci::Instruction::Type::CALL);
  STATIC_REQUIRE(exe.functions[0].instructions[1].type == sci::Instruction::Type::RET);
  STATIC_REQUIRE(exe.functions[0].instructions[2].type == sci::Instruction::Type::NONE);

  STATIC_REQUIRE(exe.functions[1].instructions[0].type == sci::Instruction::Type::VAL);
  STATIC_REQUIRE(exe.functions[1].instructions[1].type == sci::Instruction::Type::RET);
  STATIC_REQUIRE(exe.functions[1].instructions[2].type == sci::Instruction::Type::NONE);
  
//  // INTERPRETING_CHECK
  STATIC_REQUIRE(result == 10);
}

TEST_CASE("Parsing empty tokens - constexpr", "[parser]")
{

}
