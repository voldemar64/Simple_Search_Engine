#include "SyntaxParser.h"

SyntaxParser::SyntaxParser() : root_(nullptr) {}

SyntaxParser::~SyntaxParser() {
    delete root_;
    root_ = nullptr;
}

void SyntaxParser::MakeQuery(const std::string& query)  {
    auto tokens = ParseToTokens(query);
    CheckTokens(tokens);
    BuildQueryTree(tokens);
}

auto SyntaxParser::GetRoot()  {
    return root_;
}

std::vector<std::string> SyntaxParser::MakeParseOrder()  {
    std::vector<std::string> order;
    PostOrder(order, root_);
    return order;
}

void SyntaxParser::CheckTokens(const std::vector<std::string>& tokens)  {
    if (tokens.size() == 1) {
        return;
    }
    for (size_t i = 0; i < tokens.size(); ++i) {
        if ((tokens[i] == ")" || tokens[i] == "OR" || tokens[i] == "AND") &&
            (!i || i && (tokens[i - 1] == "OR" || tokens[i - 1] == "AND"))) {
            throw std::runtime_error("Invalid query");
        }
    }
}

std::vector<std::string> SyntaxParser::ClearFromBrk(std::string& token) {
    std::vector<std::string> result;
    bool front = token[0] == '(';
    bool back = token.back() == ')';
    if (front) {
        result.push_back("(");
        token = token.substr(1, token.size());
    }
    if (back) {
        token = token.substr(0, token.size() - 1);
    }
    if (token[0] == '(' || token.back() == ')') {
        auto tmp = ClearFromBrk(token);
        for (const auto& tk: tmp) {
            result.push_back(tk);
        }
    } else {
        result.push_back(token);
    }
    if (back) {
        result.push_back(")");
    }
    return result;
}

std::vector<std::string> SyntaxParser::ParseToTokens(const std::string& query) {
    std::vector<std::string> result;
    std::istringstream iss(query);
    std::string token;
    while (iss >> token) {
        auto tmp = ClearFromBrk(token);
        for (const auto& item: tmp) {
            result.push_back(item);
        }
    }
    return result;
}

void SyntaxParser::StackEmptyError(const std::stack<Node*>& st) {
    if (st.empty()) {
        throw std::runtime_error("Invalid query");
    }
}

void SyntaxParser::BuildQueryTree(const std::vector<std::string>& tokens) {
    std::stack<Node*> st;
    for (const auto& item: tokens) {
        if (item == "AND") {
            auto node = new AndNode();
            st.push(node);
        } else if (item == "OR") {
            auto node = new OrNode();
            st.push(node);
        } else if (item == "(") {
            auto node = new OpenBkt();
            st.push(node);
        } else if (item == ")") {
            CloseBrk(st);
        } else {
            if (!st.empty() && dynamic_cast<WordNode*>(st.top())) {
                throw std::runtime_error("Invalid query");
            }
            if (st.empty() || !st.empty() && dynamic_cast<OpenBkt*>(st.top())) {
                auto node = new WordNode(item);
                st.push(node);
                continue;
            }
            auto rhs = new WordNode(item);
            HangNode(st, rhs);
        }
    }
    if (st.empty() || st.size() > 1) {
        throw std::runtime_error("Invalid query");
    }
    root_ = st.top();
    st.pop();
}

void SyntaxParser::PostOrder(std::vector<std::string>& priority, SyntaxParser::Node* node) {
    if (!node) {
        return;
    }
    PostOrder(priority, node->left_);
    PostOrder(priority, node->right_);
    priority.push_back(NodeToText(node));
}

std::string SyntaxParser::NodeToText(SyntaxParser::Node* node) {
    if (dynamic_cast<WordNode*>(node)) {
        return dynamic_cast<WordNode*>(node)->data_;
    }
    if (dynamic_cast<AndNode*>(node)) {
        return "AND";
    }
    return "OR";
}

void SyntaxParser::CloseBrk(std::stack<Node*>& st) {
    std::stack<Node*> tmp;
    while (!st.empty() && !dynamic_cast<OpenBkt*>(st.top())) {
        tmp.push(st.top());
        st.pop();
    }
    StackEmptyError(st);
    st.pop();
    if (tmp.size() > 1) {
        throw std::runtime_error("Invalid query");
    }
    auto node = tmp.top();
    tmp.pop();
    if (dynamic_cast<LogicNode*>(node)) {
        node->MakePrior();
    }
    if (!st.empty() && dynamic_cast<LogicNode*>(st.top())) {
        HangNode(st, node);
        return;
    }
    st.push(node);
}

void SyntaxParser::HangNode(std::stack<Node*>& st, SyntaxParser::Node* rhs) {
    auto connector = st.top();
    st.pop();
    StackEmptyError(st);
    auto lhs = st.top();
    st.pop();

    if (dynamic_cast<OrNode*>(lhs) && dynamic_cast<AndNode*>(connector) && !lhs->IsPrior()) {
        connector->right_ = rhs;
        connector->left_ = lhs->right_;
        lhs->right_ = connector;
        st.push(lhs);
        return;
    }
    connector->right_ = rhs;
    connector->left_ = lhs;
    st.push(connector);
}