#pragma once
#include <array>

#include "CompiledProgram.h"
#include "Common.h"

namespace sci {

template<std::size_t Size>
struct ScopeFrame
{
  std::array<int, Size> vars;
};

using FuncScopeFrame = ScopeFrame<8>;
using GlobalScopeFrame = ScopeFrame<20>;

template<std::size_t ScopeStackSize>
struct StackFrame
{
  Instruction const* next_ins_ptr;
  ConstexprStack<FuncScopeFrame, ScopeStackSize> scope_stack;
};

template<std::size_t FuncStackSize, std::size_t ScopeStackSize, std::size_t CompStackSize>
class Interpreter
{
public:
  constexpr auto interpret(CompiledProgram const& program) const noexcept -> int
  {
    ConstexprStack<StackFrame<ScopeStackSize>, FuncStackSize> func_stack;
    ConstexprStack<Literal, CompStackSize> comp_stack;
//    GlobalScopeFrame global_scope;
    func_stack.push({ &program.functions[0].instructions[0] });

    while (!func_stack.empty()) {
      Instruction const& current_instruction{ *(func_stack.top().next_ins_ptr++) };
      switch (current_instruction.type) {
      case Instruction::Type::RET:
        func_stack.pop();
        break;

      case Instruction::Type::VAL:
        comp_stack.push(current_instruction.par);
        break;

      case Instruction::Type::CALL:
        func_stack.push({ program.functions[std::get<int>(current_instruction.par.val)].instructions.data() });
        break;

      default:
        break;
      }
    }

    if (comp_stack.top().type == Literal::Type::INT_) {
      return std::get<int>(comp_stack.top().val);
    }
    return 0;
  }
};

}// namespace sci