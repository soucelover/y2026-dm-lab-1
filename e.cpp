#include <cstdint>
#include <iostream>
#include <vector>

uint16_t int_from_binary(const std::string &binary);

// Wow, the base code is from b.cpp! Classes were not all for nothing!
class Function {
private:
  std::vector<bool> values;
  uint16_t n_vals;
  uint8_t n;

public:
  Function(uint8_t n, const std::vector<bool> &values)
      : values(values), n(n), n_vals(1u << n) {
    if (values.size() != n_vals) {
      throw std::invalid_argument("Values vector of a wrong length...");
    }
  }

  static Function from_stdin(uint8_t n) {
    const uint16_t n_vals = 1u << n;
    std::vector<bool> values(n_vals);

    for (uint16_t i = 0; i < n_vals; ++i) {
      std::string raw_idx;
      std::cin >> raw_idx;
      uint16_t index = int_from_binary(raw_idx);

      bool value;
      std::cin >> value;

      values[index] = value;
    }

    return Function(n, values);
  }

  std::vector<bool> linear_coefficients() const {
    std::vector<bool> out(values.size());
    std::vector<bool> column = values;

    out[0] = column[0];

    for (uint16_t i = 1; i < n_vals; ++i) {
      for (int j = 0, to = n_vals - i; j < to; ++j) {
        column[j] = column[j] != column[j + 1];
      }

      out[i] = column[0];
    }

    return out;
  }
};

uint16_t int_from_binary(const std::string &binary) {
  uint16_t out = 0;

  for (int i = 0; i < binary.length(); ++i) {
    out <<= 1;
    out |= binary[i] - '0';
  }

  return out;
}

std::string int_to_binary(uint16_t number, size_t nbits) {
  std::string out(nbits, '0');

  for (int i = nbits - 1; i >= 0; --i) {
    out[i] = '0' + (number & 1);
    number >>= 1;
  }

  return out;
}

int main() {
  uint16_t n;
  std::cin >> n;

  Function function = Function::from_stdin(n);
  std::vector<bool> liear_coefficients = function.linear_coefficients();

  for (uint16_t i = 0, to = 1u << n; i < to; ++i) {
    std::cout << int_to_binary(i, n) << " " << liear_coefficients[i]
              << std::endl;
  }
}
