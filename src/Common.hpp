#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <variant>

// ── Value type used throughout the VM ────────────────────────────────────────
// A Value can hold an integer or a boolean.
using Value = std::variant<int64_t, bool>;

// Helper to print a Value
inline std::ostream &operator<<(std::ostream &os, const Value &v) {
  std::visit(
      [&os](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
          os << (arg ? "true" : "false");
        } else {
          os << arg;
        }
      },
      v);
  return os;
}

// Helper to extract int from Value (throws on type mismatch)
inline int64_t asInt(const Value &v) {
  if (auto *p = std::get_if<int64_t>(&v))
    return *p;
  throw std::runtime_error("Runtime error: expected integer value");
}

// Helper to extract bool from Value
inline bool asBool(const Value &v) {
  // integers: 0 is false, everything else true
  if (auto *p = std::get_if<int64_t>(&v))
    return *p != 0;
  if (auto *p = std::get_if<bool>(&v))
    return *p;
  throw std::runtime_error("Runtime error: expected boolean value");
}
