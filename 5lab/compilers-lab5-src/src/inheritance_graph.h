#pragma once

#include "cool-tree.h"

#include <unordered_map>
#include <vector>
#include <string>

extern Program ast_root;

template <typename Vertex>
class Graph
{
    std::unordered_map<Vertex, std::vector<Vertex>> adj_;

public:
    Graph() = default;

    void add_edge(const Vertex &v, const Vertex &w)
    {
        adj_.try_emplace(v, std::vector<Vertex>{});
        adj_.try_emplace(w, std::vector<Vertex>{});

        auto &children = adj_[v];
        if (std::find(children.begin(), children.end(), w) == children.end())
        {
            children.push_back(w);
        }
    }

    void print() const
    {
        for (auto const &[v, children] : adj_)
        {
            std::cout << v << ":";
            for (auto const &w : children)
            {
                std::cout << " -> " << w;
            }
            std::cout << "\n";
        }
    }
};

struct GraphNode
{
    std::string class_name;
    Class_ class_;

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

bool operator==(const GraphNode &lhs, const GraphNode &rhs);
std::ostream &operator<<(std::ostream &out, const GraphNode &node);
Graph<GraphNode> make_inheritance_graph();