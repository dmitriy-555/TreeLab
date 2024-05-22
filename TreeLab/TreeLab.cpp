#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

class Node {
public:
    enum NodeType { Operator, Constant } type;
    char op;
    double value;
    Node* left;
    Node* right;

    Node(char oper, Node* l, Node* r)
        : type(Operator), op(oper), value(0), left(l), right(r) {}

    Node(double val) : type(Constant), op(0), value(val), left(nullptr), right(nullptr) {}

    ~Node() {
        delete left;
        delete right;
    }
};

void skip_spaces(istringstream& input) {
    while (input && isspace(input.peek())) {
        input.get();
    }
}

double parse_number(istringstream& input) {
    double result;
    input >> result;
    if (!input) {
        throw invalid_argument("Invalid number");
    }
    return result;
}

Node* parse_primary(istringstream& input);

Node* parse_binary(istringstream& input, int min_precedence);

Node* parse_expression(istringstream& input);

double evaluate(const Node* node) {
    if (node->type == Node::Constant) {
        return node->value;
    }
    double left_value = evaluate(node->left);
    double right_value = evaluate(node->right);
    switch (node->op) {
    case '+': return left_value + right_value;
    case '-': return left_value - right_value;
    case '*': return left_value * right_value;
    case '/':
        if (right_value == 0) {
            throw invalid_argument("Division by zero");
        }
        return left_value / right_value;
    default: throw invalid_argument("Unknown operator");
    }
}

Node* parse_primary(istringstream& input) {
    skip_spaces(input);

    if (isdigit(input.peek())) {
        return new Node(parse_number(input));
    }
    else if (input.peek() == '(') {
        input.get();
        auto expr = parse_expression(input);
        skip_spaces(input);
        if (input.get() != ')') {
            throw invalid_argument("Mismatched parentheses");
        }
        return expr;
    }
    else {
        throw invalid_argument("Unexpected character");
    }
}

Node* parse_binary(istringstream& input, int min_precedence) {
    auto left = parse_primary(input);
    skip_spaces(input);

    while (true) {
        char op = input.peek();
        int precedence;
        switch (op) {
        case '+':
        case '-':
            precedence = 1;
            break;
        case '*':
        case '/':
            precedence = 2;
            break;
        default:
            return left;
        }

        if (precedence < min_precedence) {
            return left;
        }

        input.get();
        auto right = parse_binary(input, precedence + 1);
        left = new Node(op, left, right);
        skip_spaces(input);
    }
}

Node* parse_expression(istringstream& input) {
    return parse_binary(input, 0);
}

int main() {
    try {
        string expression;
        cout << "Enter an expression: ";
        getline(cin, expression);

        istringstream input(expression);
        auto tree = parse_expression(input);

        double result = evaluate(tree);
        cout << "Result: " << result << endl;

        delete tree;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
