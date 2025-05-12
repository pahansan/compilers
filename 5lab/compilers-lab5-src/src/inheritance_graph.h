#pragma once

#include "cool-tree.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <utility>

extern Program ast_root;

extern bool faults_attend;

struct GraphNode
{
    std::string class_name;
    Class_ class_;
    std::vector<std::unique_ptr<GraphNode>> kids;

    GraphNode(const std::string name = "", Class_ cl = nullptr) : class_name(name), class_(cl) {}
};

namespace std
{
    template <>
    struct hash<GraphNode>
    {
        size_t operator()(const GraphNode &node) const
        {
            return hash<std::string>{}(node.class_name);
        }
    };
}

class Graph
{
    std::vector<std::unique_ptr<GraphNode>> first_level_;

public:
    Graph()
    {
        first_level_.push_back(std::make_unique<GraphNode>("Object"));
    }

    GraphNode *find(const GraphNode &v)
    {
        std::stack<GraphNode *> stack_;
        for (auto &vertex : first_level_)
            stack_.push(vertex.get());
        std::vector<std::string> visited;
        while (!stack_.empty())
        {
            GraphNode *vertex = stack_.top();
            stack_.pop();
            if (std::find(visited.begin(), visited.end(), vertex->class_name) == visited.end())
                visited.push_back(vertex->class_name);
            else
                continue;

            if (vertex->class_name == v.class_name)
                return vertex;
            else
                for (auto &kid : vertex->kids)
                    stack_.push(kid.get());
        }
        return nullptr;
    }

    bool add_edge(const GraphNode &parent, const GraphNode &kid)
    {
        auto parent_vertex = find(parent);
        auto kid_vertex = find(kid);

        if (kid_vertex == nullptr)
        {
            if (parent_vertex == nullptr)
                first_level_.push_back(std::make_unique<GraphNode>(kid.class_name, kid.class_));
            else
                parent_vertex->kids.push_back(std::make_unique<GraphNode>(kid.class_name, kid.class_));
        }
        else
        {
            // Удалить kid_vertex там, где он уже есть
            // Поставить kid_vertex на новое место
        }
        std::cout << parent.class_name << ':' << kid.class_name << '\n';
        return true;
    }

    // void print() const
    // {
    //     std::stack<std::unique_ptr<GraphNode> &> stack_;
    //     stack_.push(root_);
    //     while (!stack_.empty())
    //     {
    //         std::unique_ptr<GraphNode> &vertex = stack_.top();
    //         stack_.pop();
    //         for (auto &kid : vertex->kids)
    //             stack_.push(kid);
    //     }
    // }
};

bool operator==(const GraphNode &lhs, const GraphNode &rhs);
std::ostream &operator<<(std::ostream &out, const GraphNode &node);
void count_classes();
Graph make_inheritance_graph();