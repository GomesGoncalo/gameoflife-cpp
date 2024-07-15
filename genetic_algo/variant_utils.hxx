#pragma once

#include <variant>

/**
 * Check that variants provided have the same type
 */
struct VariantSameType {
  template <typename... Args, typename... Args2>
  [[nodiscard]] inline constexpr bool
  cmp(const std::variant<Args...> &l,
      const std::variant<Args2...> &r) const noexcept {
    return std::visit(
        [](const auto &l, const auto &r) noexcept {
          return std::is_same_v<decltype(l), decltype(r)>;
        },
        l, r);
  }

  template <typename... Args, typename... Args2, typename... MoreArgs>
  [[nodiscard]] inline constexpr bool cmp(const std::variant<Args...> &l,
                                          const std::variant<Args2...> &r,
                                          MoreArgs &&...more) const noexcept {
    return cmp(l, r) && cmp(r, std::forward<MoreArgs>(more)...);
  }
};

/**
 * Check that variants provided have different type
 */
struct VariantDifferentType {
  template <typename... Args, typename... Args2>
  [[nodiscard]] inline constexpr bool
  cmp(const std::variant<Args...> &l,
      const std::variant<Args2...> &r) const noexcept {
    constexpr VariantSameType v;
    return !v.cmp(l, r);
  }
};
