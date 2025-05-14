#pragma once

#include "cool-tree.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <utility>
#include <ranges>
#include <set>

extern Program ast_root;

struct GraphNode
{
    std::string class_name;
    Class_ class_;
    std::vector<std::shared_ptr<GraphNode>> kids;
    size_t level_;

    GraphNode(const std::string name = "", Class_ cl = nullptr, size_t level = 0) : class_name(name), class_(cl), level_(level) {}
};

using graph_node_ptr = std::shared_ptr<GraphNode>;

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
    std::vector<graph_node_ptr> first_level_;

public:
    Graph();
    graph_node_ptr find(const GraphNode &v);
    graph_node_ptr detach(const GraphNode &v);
    void add_edge(const GraphNode &parent, const GraphNode &kid);
    std::vector<std::vector<graph_node_ptr>> find_cycles();
    void print() const;
    std::vector<graph_node_ptr> check_first_level();
    size_t check_inheritance_from_basic();
    size_t make_all_checks(const std::set<std::string> &types_table);
};

bool operator==(const GraphNode &lhs, const GraphNode &rhs);
std::ostream &operator<<(std::ostream &out, const GraphNode &node);
