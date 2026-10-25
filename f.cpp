#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

class HornDisjunct {
private:
  const std::vector<uint8_t> antecedent;
  const int8_t consequent;

  static int8_t find_consequent(const std::vector<int8_t> &disjunct) {
    for (int i = 0; i < disjunct.size(); ++i) {
      if (disjunct[i] == 1) {
        return i;
      }
    }

    return -1;
  }

  static std::vector<uint8_t>
  compose_antecedent(const std::vector<int8_t> &disjunct) {
    std::vector<uint8_t> out;

    for (int i = 0; i < disjunct.size(); ++i) {
      if (disjunct[i] == 0) {
        out.push_back(i);
      }
    }

    return out;
  }

public:
  HornDisjunct(const std::vector<int8_t> &disjunct)
      : consequent(find_consequent(disjunct)),
        antecedent(compose_antecedent(disjunct)) {}

  int8_t get_consequent() const { return consequent; }

  bool imply(const std::vector<bool> &vars) {
    for (uint8_t index : antecedent) {
      if (!vars[index]) {
        return false;
      }
    }

    return true;
  }
};

bool horn_constant_0(std::vector<std::shared_ptr<HornDisjunct>> disjuncts,
                     uint8_t n) {
  bool change_happened = true;
  std::vector<bool> vars(n);

  while (change_happened) {
    change_happened = false;

    for (int i = 0; i < disjuncts.size();) {
      if (!disjuncts[i]->imply(vars)) {
        ++i;
        continue;
      }

      if (disjuncts[i]->get_consequent() == -1) {
        return true;
      }

      vars[disjuncts[i]->get_consequent()] = true;
      disjuncts.erase(std::begin(disjuncts) + i);
      change_happened = true;
    }
  }

  return false;
}

int main() {
  size_t n, k;
  std::cin >> n >> k;

  std::vector<std::shared_ptr<HornDisjunct>> disjuncts;

  for (int i = 0; i < k; ++i) {
    std::vector<int8_t> disjunct(n);
    int16_t num;

    for (int j = 0; j < n; ++j) {
      std::cin >> num;
      disjunct[j] = num;
    }

    disjuncts.push_back(std::make_unique<HornDisjunct>(disjunct));
  }

  if (horn_constant_0(disjuncts, n)) {
    std::cout << "YES";
  } else {
    std::cout << "NO";
  }
}
