#pragma once

namespace VSTZ {
struct Channel {
  double Left{0.0};
  double Right{0.0};
  Channel operator*=(const Channel &other) {
    Left *= other.Left;
    Right *= other.Right;
    return *this;
  }
  Channel operator-(const Channel &other) const {
    return {Left - other.Left, Right - other.Right};
  }
  Channel operator*(const Channel &other) const {
    return {Left * other.Left, Right * other.Right};
  }
};
}; // namespace VSTZ