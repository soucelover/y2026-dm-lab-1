#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Maybe, I won't use OOP next time...
class Function {
private:
  std::vector<bool> values;
  uint16_t n_vals;
  uint8_t n;

public:
  Function(uint8_t n, const std::vector<bool> &values)
      : values(values), n(n), n_vals(1 << n) {
    if (values.size() != n_vals) {
      throw std::invalid_argument("Values vector of a wrong length...");
    }
  }

  static Function from_stdin(uint8_t n) {
    // Don't think I am optimizing anything )
    std::string raw_values;
    std::cin >> raw_values;

    const size_t n_vals = 1 << n;
    std::vector<bool> values(n_vals);

    for (int i = 0; i < n_vals; ++i) {
      values[i] = raw_values[i] - '0';
    }

    return Function(n, values);
  }

  bool preserves_0() const { return values[0] == false; }

  bool preserves_1() const { return values[n_vals - 1] == true; }

  bool is_self_dual() const {
    const int last = n_vals - 1;

    for (int i = 0, to = std::max(1, n_vals >> 1); i < to; ++i) {
      if (values[i] == values[last - i]) {
        return false;
      }
    }

    return true;
  }

  bool is_monotone() const {
    for (uint16_t arg = 0; arg < n_vals; ++arg) {
      for (uint8_t j = 0; j < n; ++j) {
        const uint16_t mask = 1u << j;

        if ((arg & mask) == 0 && values[arg] > values[arg | mask]) {
          return false;
        }
      }
    }

    return true;
  }

  bool is_linear() const {
    std::vector<bool> column = values;

    for (unsigned int i = 1; i < n_vals; ++i) {
      for (int j = 0, to = n_vals - i; j < to; ++j) {
        column[j] = column[j] != column[j + 1];
      }

      if (column[0] && std::popcount(i) > 1) {
        return false;
      }
    }

    return true;
  }
};

class Basis {
private:
  const std::vector<Function> functions;

public:
  Basis(const std::vector<Function> &functions) : functions(functions) {}

  bool test_preserves_0() const {
    for (const Function &function : functions) {
      if (!function.preserves_0()) {
        return false;
      }
    }
    return true;
  }

  bool test_preserves_1() const {
    for (const Function &function : functions) {
      if (!function.preserves_1()) {
        return false;
      }
    }
    return true;
  }
  bool test_self_duality() const {
    for (const Function &function : functions) {
      if (!function.is_self_dual()) {
        return false;
      }
    }
    return true;
  }
  bool test_linearity() const {
    for (const Function &function : functions) {
      if (!function.is_linear()) {
        return false;
      }
    }
    return true;
  }

  // Damn ye and yer English grammar...
  bool test_monotonicity() const {
    for (const Function &function : functions) {
      if (!function.is_monotone()) {
        return false;
      }
    }
    return true;
  }

  bool is_complete() const {
    return !(test_preserves_0() || test_preserves_1() || test_self_duality() ||
             test_linearity() || test_monotonicity());
  }
};

int main() {
  size_t n_funcs;
  std::cin >> n_funcs;

  std::vector<Function> functions;

  for (int i = 0; i < n_funcs; ++i) {
    int n_args;
    std::cin >> n_args;

    functions.emplace_back(Function::from_stdin(n_args));
  }

  Basis basis = Basis(functions);

  if (basis.is_complete()) {
    std::cout << "YES";
  } else {
    std::cout << "NO";
  }
}
