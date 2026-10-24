#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

class Node {
public:
  virtual int depth() = 0;
  virtual bool call(uint32_t params, uint8_t last_param) = 0;
  virtual uint8_t nparams() = 0;
};

class Variable : public Node {
private:
  const uint8_t no;

public:
  Variable(int no) : no(no) {}

  int depth() override { return 0; }
  bool call(uint32_t params, uint8_t last_param) override {
    return params & (1u << (last_param - no));
  }
  uint8_t nparams() override { return no + 1; }
};

// It would be so simple without STL pointers...
class Function : public Node {
private:
  typedef std::vector<std::shared_ptr<Node>> args_t;
  typedef std::vector<bool> truth_table_t;

  const args_t args;
  const truth_table_t truth_table;

public:
  Function(const args_t &args, const truth_table_t &truth_table)
      : args(args), truth_table(truth_table) {}

  static std::shared_ptr<Function>
  read_from_stdin(const std::vector<std::shared_ptr<Node>> &nodes, int nargs) {
    args_t args;

    for (int i = 0; i < nargs; ++i) {
      unsigned int index;
      std::cin >> index;
      --index;

      args.push_back(nodes[index]);
    }

    truth_table_t truth_table(1u << nargs);

    for (int i = 0; i < truth_table.size(); ++i) {
      bool value;
      std::cin >> value;
      truth_table[i] = value;
    }

    return std::make_shared<Function>(args, truth_table);
  }

  int depth() override {
    int depth = 0;

    for (const std::shared_ptr<Node> &arg : args) {
      int arg_depth = arg->depth();

      if (arg_depth > depth) {
        depth = arg_depth;
      }
    }

    return depth + 1;
  }

  bool call(uint32_t params, uint8_t last_param) override {
    uint8_t arg = 0;
    int last = args.size() - 1;

    for (int i = 0; i < args.size(); ++i) {
      arg |= static_cast<uint8_t>(args[i]->call(params, last_param))
             << (last - i);
    }

    return truth_table[arg];
  }

  uint8_t nparams() override {
    uint8_t max_nparams = 0;

    for (int i = 0; i < args.size(); ++i) {
      uint8_t nparams = args[i]->nparams();

      if (nparams > max_nparams) {
        max_nparams = nparams;
      }
    }

    return max_nparams;
  }
};

int main() {
  int n;
  std::cin >> n;

  std::vector<std::shared_ptr<Node>> nodes;

  int var_no = 0;

  for (int i = 0; i < n; ++i) {
    int m;
    std::cin >> m;

    if (m == 0) {
      nodes.push_back(std::make_shared<Variable>(var_no++));
    } else {
      nodes.push_back(Function::read_from_stdin(nodes, m));
    }
  }

  std::shared_ptr<Node> function = nodes[nodes.size() - 1];
  std::cout << function->depth() << std::endl;

  uint8_t nparams = function->nparams();

  for (uint32_t i = 0, to = 1u << nparams; i < to; ++i) {
    std::cout << function->call(i, nparams - 1);
  }
}
