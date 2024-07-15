#pragma once

#include <variant>

struct VariantSameType {
  template <typename... Args>
  bool cmp(const std::variant<Args...> &l,
           const std::variant<Args...> &r) const noexcept {
    return std::visit(
        [](const auto &l, const auto &r) noexcept {
          return std::is_same_v<decltype(l), decltype(r)>;
        },
        l, r);
  }
};

struct VariantDifferentType {
  template <typename... Args>
  bool cmp(const std::variant<Args...> &l,
           const std::variant<Args...> &r) const noexcept {
    VariantSameType v;
    return !v.cmp(l, r);
  }
};
