#pragma once
#include <array>
#include <string_view>
#include <variant>

#include "Common.h"

namespace sci {
struct Instruction
{
  enum class Type {
     NONE,
     VAL,
     ADD,
     CALL,
     RET,
  };

  Type type{ Type::NONE };
  Literal par;
};

struct CompiledFunction
{
  std::array<Instruction, 30> instructions;
};

struct CompiledProgram
{
  constexpr static auto NUM_OF_FUNC{ 10 };
  std::array<CompiledFunction, NUM_OF_FUNC> functions;
};
}// namespace sci
