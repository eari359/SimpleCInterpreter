#pragma once
#include <array>
#include <variant>

namespace sci {

enum class Result {
  OK,
  ERR,
  END
};

struct Literal
{
  enum class Type {
     NONE_,
     INT_,
     CHAR_,
     STRING_,
     DOUBLE_,
  };
  Type type{ Type::NONE_ };
  std::variant<int, double, char, std::string_view> val;
};

template<typename Type, std::size_t MaxSize>
class ConstexprStack
{
  using DataArray = std::array<Type, MaxSize>;
  DataArray data_{};
  std::size_t size_{ 0 };

public:
  constexpr ConstexprStack() noexcept = default;

  constexpr auto pop() noexcept -> void
  {
    if (size_ != 0) {
      --size_;
    }
  }

  constexpr auto top() noexcept -> Type&
  {
    if (!empty()) {
      return data_[size_ - 1];
    }

    return data_[MaxSize - 1];
  }

  constexpr auto push(Type const& val) noexcept -> void
  {
    if (!full()) {
      data_[size_] = val;
      ++size_;
    }
  }

  constexpr auto size() const noexcept -> std::size_t
  {
    return size_;
  }

  constexpr auto empty() const noexcept -> bool
  {
    return size_ == 0;
  }

  constexpr auto full() const noexcept -> bool
  {
    return size_ >= MaxSize;
  }

  constexpr auto data() const noexcept -> DataArray const&
  {
    return data_;
  }
};

}// namespace sci
