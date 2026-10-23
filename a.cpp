#include <iostream>
#include <stdexcept>

class Relation {
private:
  const bool *const relation;
  const int n;

public:
  Relation(int n, bool *relation) : relation(relation), n(n) {
    if (n <= 0) {
      throw std::invalid_argument(
          "Please, don't define relation over an empty set :)");
    }
  }

  Relation(const Relation &) = delete;
  Relation(Relation &&) = delete;

  static Relation from_stdin(int n) {
    bool *relation = new bool[n * n];

    for (int i = 0, to = n * n; i < to; ++i) {
      std::cin >> relation[i];
    }

    return Relation(n, relation);
  }

  inline bool at(int i, int j) const { return relation[i * n + j]; }

  bool reflexive() const {
    for (int i = 0; i < n; ++i) {
      if (!at(i, i)) {
        return false;
      }
    }

    return true;
  }

  bool antireflexive() const {
    for (int i = 0; i < n; ++i) {
      if (at(i, i)) {
        return false;
      }
    }

    return true;
  }

  bool symmetric() const {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < i; ++j) {
        if (at(i, j) != at(j, i)) {
          return false;
        }
      }
    }

    return true;
  }

  bool antisymmetric() const {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < i; ++j) {
        if (at(i, j) && at(j, i)) {
          return false;
        }
      }
    }

    return true;
  }

  bool transitive() const {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (!at(i, j)) {
          continue;
        }

        for (int k = 0; k < n; ++k) {
          if (!at(j, k)) {
            continue;
          }

          if (!at(i, k)) {
            return false;
          }
        }
      }
    }

    return true;
  }

  void describe() const {
    std::cout << reflexive() << " " << antireflexive() << " " << symmetric()
              << " " << antisymmetric() << " " << transitive() << std::endl;
  }

  void output() const {
    for (int i = 0; i < n; ++i) {
      for (int j = 0, to = n - 1; j < to; ++j) {
        std::cout << at(i, j) << " ";
      }

      std::cout << at(i, n - 1) << std::endl;
    }
  }

  Relation compose_with(const Relation &other) {
    if (this->n != other.n) {
      throw std::invalid_argument("Relations composition over different sets.");
    }

    bool *result = new bool[n * n]{};

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (!this->at(i, j)) {
          continue;
        }

        for (int k = 0; k < n; ++k) {
          if (other.at(j, k)) {
            result[i * n + k] = true;
          }
        }
      }
    }

    return Relation(n, result);
  }
};

int main() {
  int n;

  std::cin >> n;

  Relation rel1 = Relation::from_stdin(n);
  Relation rel2 = Relation::from_stdin(n);

  Relation composition = rel1.compose_with(rel2);

  rel1.describe();
  rel2.describe();
  composition.output();
}
