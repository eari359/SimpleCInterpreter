#pragma once
#include "eternal.hpp"

#include "Common.h"
#include "CompiledProgram.h"
#include "Tokenizer.h"

namespace sci {

enum class Symbol {
#include "terminals.inl"

  TERMINALS_END,

  NT_PROGRAM,
  NT_FUNC_DEF,
  NT_FUNC_CALL,
  NT_FUNC_STATEMENT_BLOCK,
  NT_KWTYPE,
  NT_FUNC_STATEMENT_IDCONT,
  NT_FUNC_CALL_ARGS,
  NT_FUNC_CALL_ARGS_NEXT,
  NT_FUNC_DEF_PARAMS,
  NT_FUNC_DEF_PARAMS_NEXT,
  NT_EXPRESSION,

  NONTERMINALS_END,

  GEN_RET,
  GEN_NEW_FUNC,
};

struct TokenSymbol
{
  Token::Type t;
  Symbol s;
};

constexpr bool operator<(TokenSymbol const& lhs, TokenSymbol const& rhs)
{
  constexpr auto hash = [](auto const& ts) {
    return static_cast<std::size_t>(ts.t) + static_cast<std::size_t>(ts.s) * 100;
  };
  return hash(lhs) < hash(rhs);
}

struct SymbolSequence
{
  char count;
  std::array<Symbol, 10> seq;

  auto rbegin() noexcept { return seq.rbegin(); }
  auto rend() noexcept { return seq.rbegin() + count; }
};

class CompilingFunction
{
  friend class CompilingProgram;
  std::string_view name_;
  int next_index_{ 0 };
  CompiledFunction* func_;

public:
  constexpr auto set_name(std::string_view name) noexcept -> void { name_ = name; }
  constexpr auto add_instruction(Instruction const& ins) -> void
  {
    func_->instructions[next_index_] = ins;
    ++next_index_;
  }
};

class CompilingProgram
{
  CompiledProgram& prog_;
  std::array<CompilingFunction, CompiledProgram::NUM_OF_FUNC> functions_;
  int next_index_{ 1 };

public:
  constexpr explicit CompilingProgram(CompiledProgram& program) noexcept
    : prog_{ program }
  {
    for (int i{ 0 }; i < CompiledProgram::NUM_OF_FUNC; ++i) {
      functions_[i].func_ = &prog_.functions[i];
    }
  }

  constexpr auto new_function(std::string_view id) noexcept -> CompilingFunction* {
    auto const func_ptr = [this, &id] {
      if (id == "main") {
        return &functions_[0];

      } else {
        return &functions_[next_index_++];
      }
    }();

    func_ptr->set_name(id);
    return func_ptr;
  }

  constexpr auto get_func_ptr(std::string_view id) noexcept -> int const
  {
    int i{ -1 };
    for (auto const& f : functions_) {
      ++i;
      if (f.name_ == id) {
        return i;
      }
    }

    return -1;
  }
};

template<std::size_t MaxTokens, std::size_t MaxStackSize>
class Parser
{
public:
  using Tokens = std::array<Token, MaxTokens>;

private:
  static constexpr auto const symbol_map = mapbox::eternal::map<TokenSymbol, SymbolSequence>({
    { {
        Token::Type::KWTYPE,
        Symbol::NT_PROGRAM,
      },
      {
        2,
        {
          Symbol::NT_FUNC_DEF,
          Symbol::NT_PROGRAM,
        },
      } },

    { {
        Token::Type::END_OF_SOURCECODE,
        Symbol::NT_PROGRAM,
      },
      { 0,
        {} } },

    { {
        Token::Type::KWTYPE,
        Symbol::NT_FUNC_DEF,
      },
      { 9,
        {
          Symbol::KWTYPE,
          Symbol::ID,
          Symbol::GEN_NEW_FUNC,
          Symbol::OPEN_PAR,
          Symbol::NT_FUNC_DEF_PARAMS,
          Symbol::CLOSE_PAR,
          Symbol::OPEN_CURLY,
          Symbol::NT_FUNC_STATEMENT_BLOCK,
          Symbol::CLOSE_CURLY,
        } } },

    { {
        Token::Type::CLOSE_PAR,
        Symbol::NT_FUNC_DEF_PARAMS,
      },
      { 0,
        {} } },

    { {
        Token::Type::KWTYPE,
        Symbol::NT_FUNC_DEF_PARAMS,
      },
      { 3,
        {
          Symbol::KWTYPE,
          Symbol::ID,
          Symbol::NT_FUNC_DEF_PARAMS_NEXT,
        } } },

    { {
        Token::Type::COMMA,
        Symbol::NT_FUNC_DEF_PARAMS_NEXT,
      },
      { 3,
        {
          Symbol::KWTYPE,
          Symbol::ID,
          Symbol::NT_FUNC_DEF_PARAMS_NEXT,
        } } },

    { {
        Token::Type::CLOSE_PAR,
        Symbol::NT_FUNC_DEF_PARAMS_NEXT,
      },
      { 0,
        {} } },

    { {
        Token::Type::OPEN_CURLY,
        Symbol::NT_FUNC_STATEMENT_BLOCK,
      },
      { 3,
        {
          Symbol::OPEN_CURLY,
          Symbol::NT_FUNC_STATEMENT_BLOCK,
          Symbol::CLOSE_CURLY,
        } } },

    { {
        Token::Type::ID,
        Symbol::NT_FUNC_STATEMENT_BLOCK,
      },
      { 3,
        {
          Symbol::NT_EXPRESSION,
          Symbol::SEMICOLON,
          Symbol::NT_FUNC_STATEMENT_BLOCK,
        } } },

    { {
        Token::Type::KWRET,
        Symbol::NT_FUNC_STATEMENT_BLOCK,
      },
      { 5,
        {
          Symbol::KWRET,
          Symbol::NT_EXPRESSION,
          Symbol::GEN_RET,
          Symbol::SEMICOLON,
          Symbol::NT_FUNC_STATEMENT_BLOCK,
        } } },

    { {
        Token::Type::SEMICOLON,
        Symbol::NT_FUNC_STATEMENT_BLOCK,
      },
      { 2,
        {
          Symbol::SEMICOLON,
          Symbol::NT_FUNC_STATEMENT_BLOCK,
        } } },

    { {
        Token::Type::CLOSE_CURLY,
        Symbol::NT_FUNC_STATEMENT_BLOCK,
      },
      { 0, {} } },

    { {
        Token::Type::OPEN_PAR,
        Symbol::NT_FUNC_STATEMENT_IDCONT,
      },
      { 4,
        {
          Symbol::OPEN_PAR,
          Symbol::NT_FUNC_CALL_ARGS,
          Symbol::CLOSE_PAR,
          Symbol::SEMICOLON,
        } } },

    { {
        Token::Type::EQUAL,
        Symbol::NT_FUNC_STATEMENT_IDCONT,
      },
      { 3,
        {
          Symbol::EQUAL,
          Symbol::NT_EXPRESSION,
          Symbol::SEMICOLON,
        } } },

    { {
        Token::Type::ID,
        Symbol::NT_FUNC_CALL_ARGS,
      },
      { 2,
        {
          Symbol::NT_EXPRESSION,
          Symbol::NT_FUNC_CALL_ARGS_NEXT,
        } } },

    { {
        Token::Type::LITERAL,
        Symbol::NT_FUNC_CALL_ARGS,
      },
      { 2,
        {
          Symbol::NT_EXPRESSION,
          Symbol::NT_FUNC_CALL_ARGS_NEXT,
        } } },

    { {
        Token::Type::COMMA,
        Symbol::NT_FUNC_CALL_ARGS_NEXT,
      },
      { 3,
        {
          Symbol::COMMA,
          Symbol::NT_EXPRESSION,
          Symbol::NT_FUNC_CALL_ARGS_NEXT,
        } } },

    { {
        Token::Type::CLOSE_PAR,
        Symbol::NT_FUNC_CALL_ARGS_NEXT,
      },
      { 0, {} } },

    { {
        Token::Type::CLOSE_PAR,
        Symbol::NT_FUNC_CALL_ARGS,
      },
      { 0, {} } },
  });

  Tokens const& tokens_;

public:
  explicit constexpr Parser(Tokens const& tokens) noexcept
    : tokens_{ tokens }
  {}

  constexpr auto parse() const noexcept -> CompiledProgram
  {
    ConstexprStack<Symbol, MaxStackSize> stack;
    CompiledProgram resulting_program;
    CompilingProgram program{ resulting_program };
    CompilingFunction* current_function{ nullptr };
    std::string_view last_identifier;

    stack.push(Symbol::NT_PROGRAM);

    std::size_t tok_index{ 0 };
    auto tokensPeek = [this, &tok_index]() -> Token {
      return tokens_[tok_index];
    };
    auto tokensNext = [this, &tok_index]() -> void {
      ++tok_index;
    };
    auto tokensEnd = [this, &tok_index]() -> bool {
      return tokens_[tok_index].type == Token::Type::EMPTY_TOKEN;
    };

    while (!stack.empty()) {
#ifdef SCI_NONCONSTEXPR
      auto printToken = [](Token const& a) { fmt::print("{} ", magic_enum::enum_name(a.type)); };
      auto printSymbol = [](Symbol const& a) { fmt::print("{} ", magic_enum::enum_name(a)); };
      for (std::size_t i = tok_index; tokens_[i].type != Token::Type::END_OF_SOURCECODE; ++i) {
        printToken(tokens_[i]);
      }
      puts("");
      for (int i = 0; i < stack.size(); ++i) {
        printSymbol(stack.data()[i]);
      }
      puts("");
      getchar();
#endif

      if (stack.top() < Symbol::TERMINALS_END) {
        if (static_cast<int>(stack.top()) != static_cast<int>(tokensPeek().type)) {
#ifdef SCI_NONCONSTEXPR
          fmt::print("Found wrong terminal: {}, expected {}", magic_enum::enum_name(tokensPeek().type), magic_enum::enum_name(stack.top()));
#endif
          //TODO: ERROR HANDLING
          return {};
        }

        switch (tokensPeek().type) {
        case Token::Type::ID:
          last_identifier = std::get<std::string_view>(tokensPeek().val);
          break;

        default:
          break;
        }

        stack.pop();
        tokensNext();

      } else if (stack.top() > Symbol::NONTERMINALS_END) {
        switch (stack.top()) {
        case Symbol::GEN_NEW_FUNC:
          current_function = program.new_function(last_identifier);
          break;

        case Symbol::GEN_RET:
          if (current_function) {
            current_function->add_instruction({ Instruction::Type::RET, {} });

          } else {
#ifdef SCI_NONCONSTEXPR
            fmt::print("Not inside a function");
#endif
          }
          break;

        default:
          break;
        }

        stack.pop();

      } else {
        if (stack.top() == Symbol::NT_EXPRESSION) {
          auto const is_tok_expr = [](Token::Type t) {
            Token::Type exprs[] = {
              Token::Type::LITERAL,
              Token::Type::PLUS,
              Token::Type::MINUS,
              Token::Type::STAR,
              Token::Type::ID,
              Token::Type::SLASH,
              Token::Type::OPEN_PAR,
              Token::Type::CLOSE_PAR,
            };
            return std::any_of(std::begin(exprs), std::end(exprs), [&t](Token::Type tt) {
              return t == tt;
            });
          };
          while (!tokensEnd()
                 && is_tok_expr(tokensPeek().type)) {
            switch (tokensPeek().type) {
            case Token::Type::LITERAL:
              current_function->add_instruction({ Instruction::Type::VAL, std::get<Literal>(tokensPeek().val) });
              break;

            case Token::Type::ID: {
              std::string_view const id = std::get<std::string_view>(tokensPeek().val);
              tokensNext();
              if (tokensPeek().type == Token::Type::OPEN_PAR) {
                // function call
                tokensNext();// close_par
                tokensNext();// next token
                current_function->add_instruction({ Instruction::Type::CALL, { Literal::Type::INT_, program.get_func_ptr(id) } });

              } else {
                // variable
              }

              continue;
            }

            default:
              break;
            }
            tokensNext();
          }
          stack.pop();
          continue;
        }
        TokenSymbol const ts = [&tokensEnd, &tokensPeek, &stack]() -> TokenSymbol {
          auto t = Token::Type::EMPTY_TOKEN;
          if (!tokensEnd()) {
            t = tokensPeek().type;
          }
          return { t, stack.top() };
        }();

        auto const it = symbol_map.find(ts);
        stack.pop();
        if (it != symbol_map.end()) {
          for (int i{ it->second.count - 1 }; i >= 0; --i) {
            stack.push(it->second.seq[i]);
          }

        } else {
#ifdef SCI_NONCONSTEXPR
          fmt::print("Syntax error:\nToken:{}\nSymbol:{}\n", magic_enum::enum_name(ts.t), magic_enum::enum_name(ts.s));
#endif
          return {};
        }
      }
    }
#ifdef SCI_NONCONSTEXPR
    fmt::print("finished syntax analysis\n");
#endif
    return resulting_program;
  }
};

}// namespace sci