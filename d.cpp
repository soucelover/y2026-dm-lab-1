#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class Node {
protected:
  size_t ref;

public:
  Node() : ref(0) {}
  Node(size_t ref) : ref(ref) {}

  virtual void construct_schema(std::vector<std::string> &dst, uint8_t n) = 0;
  size_t get_ref() { return ref; };
};

class Variable : public Node {
public:
  Variable(uint8_t no) : Node(no) {}

  void construct_schema(std::vector<std::string> &dst, uint8_t n) override {}
};

class Negation : public Node {
private:
  const std::shared_ptr<Node> arg;

public:
  Negation(const std::shared_ptr<Node> &arg) : arg(arg) {}

  void construct_schema(std::vector<std::string> &dst, uint8_t n) override {
    if (ref != 0) {
      return;
    }

    arg->construct_schema(dst, n);

    dst.push_back(std::string("1 ") + std::to_string(arg->get_ref()));
    ref = dst.size() + n;
  }
};

template <typename DerivedClass> class VarLengthFuncNode : public Node {
private:
  std::vector<std::shared_ptr<Node>> args;

  static constexpr const char *prefix = DerivedClass::prefix;

public:
  VarLengthFuncNode(const std::initializer_list<std::shared_ptr<Node>> &args)
      : args(args) {}
  VarLengthFuncNode(const std::vector<std::shared_ptr<Node>> args)
      : args(args) {}

  void construct_schema(std::vector<std::string> &dst, uint8_t n) override {
    if (ref != 0) {
      return;
    }

    if (args.size() == 1) {
      args[0]->construct_schema(dst, n);
      ref = args[0]->get_ref();
      return;
    }

    args[0]->construct_schema(dst, n);
    args[1]->construct_schema(dst, n);
    dst.push_back(prefix + std::to_string(args[0]->get_ref()) + " " +
                  std::to_string(args[1]->get_ref()));

    size_t curr_ref = dst.size() + n;

    for (size_t i = 2; i < args.size(); ++i) {
      args[i]->construct_schema(dst, n);
      dst.push_back(prefix + std::to_string(curr_ref) + " " +
                    std::to_string(args[i]->get_ref()));

      curr_ref = dst.size() + n;
    }

    ref = curr_ref;
  }
};

class Conjunction : public VarLengthFuncNode<Conjunction> {
private:
  static constexpr const char *prefix = "2 ";

public:
  Conjunction(const std::initializer_list<std::shared_ptr<Node>> &args)
      : VarLengthFuncNode(args) {}
  Conjunction(const std::vector<std::shared_ptr<Node>> args)
      : VarLengthFuncNode(args) {}

  friend VarLengthFuncNode;
};

class Disjunction : public VarLengthFuncNode<Disjunction> {
private:
  static constexpr const char *prefix = "3 ";

public:
  Disjunction(const std::initializer_list<std::shared_ptr<Node>> &args)
      : VarLengthFuncNode(args) {}
  Disjunction(const std::vector<std::shared_ptr<Node>> args)
      : VarLengthFuncNode(args) {}

  friend VarLengthFuncNode;
};

uint16_t int_from_binary(const std::string &binary);

// Wow, the base code is from e.cpp! Classes were not all for nothing!
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

  bool is_constant_0() const {
    for (int i = 0; i < n_vals; ++i) {
      if (values[i]) {
        return false;
      }
    }

    return true;
  }

  void construct_constant_0(std::vector<std::string> &dst) const {
    Conjunction schema = {
        std::make_shared<Variable>(1),
        std::make_shared<Negation>(std::make_shared<Variable>(1))};

    schema.construct_schema(dst, n);
  }

  void construct_schema(std::vector<std::string> &dst) const {
    if (is_constant_0()) {
      construct_constant_0(dst);
      return;
    }

    std::vector<std::shared_ptr<Variable>> variables(n);
    std::vector<std::shared_ptr<Negation>> negations(n);

    for (int i = 0; i < n; ++i) {
      variables[i] = std::make_shared<Variable>(i + 1);
      negations[i] = std::make_shared<Negation>(variables[i]);
    }

    std::vector<std::shared_ptr<Node>> conjuncts;
    uint8_t last_arg = n - 1;

    for (uint16_t i = 0; i < n_vals; ++i) {
      if (!values[i]) {
        continue;
      }

      std::vector<std::shared_ptr<Node>> args(n);

      for (int j = 0; j < n; ++j) {
        if ((i & (1u << (last_arg - j))) != 0) {
          args[j] = variables[j];
        } else {
          args[j] = negations[j];
        }
      }

      conjuncts.push_back(std::make_shared<Conjunction>(args));
    }

    Disjunction schema = conjuncts;
    schema.construct_schema(dst, n);
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

int main() {
  size_t n;
  std::cin >> n;

  Function function = Function::from_stdin(n);

  std::vector<std::string> schema;
  function.construct_schema(schema);

  std::cout << schema.size() + n << std::endl;

  for (const std::string &line : schema) {
    std::cout << line << std::endl;
  }
}
