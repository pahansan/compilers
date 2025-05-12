#pragma once

#include "cool-tree.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <utility>
#include <ranges>

extern Program ast_root;

extern bool faults_attend;

struct GraphNode
{
    std::string class_name;
    Class_ class_;
    std::vector<std::shared_ptr<GraphNode>> kids;

    GraphNode(const std::string name = "", Class_ cl = nullptr) : class_name(name), class_(cl) {}
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
    Graph()
    {
        first_level_.push_back(std::make_shared<GraphNode>("Object"));
    }

    graph_node_ptr find(const GraphNode &v)
    {
        std::stack<graph_node_ptr> stack_;
        for (auto &vertex : first_level_)
            stack_.push(vertex);
        std::vector<std::string> visited;
        while (!stack_.empty())
        {
            graph_node_ptr vertex = stack_.top();
            stack_.pop();
            if (std::ranges::find(visited, vertex->class_name) == visited.end())
                visited.push_back(vertex->class_name);
            else
                continue;

            if ((*vertex).class_name == v.class_name)
                return vertex;
            else
                for (auto &kid : vertex->kids)
                    stack_.push(kid);
        }
        return nullptr;
    }

    graph_node_ptr detach(const GraphNode &v)
    {
        std::stack<graph_node_ptr> stack_;

        for (auto &kid : first_level_)
        {
            if (kid->class_name == v.class_name)
            {
                graph_node_ptr detached = kid;
                first_level_.erase(std::ranges::find(first_level_, kid));
                return detached;
            }
            stack_.push(kid);
        }

        std::vector<std::string> visited;
        while (!stack_.empty())
        {
            graph_node_ptr vertex = stack_.top();
            stack_.pop();
            if (std::ranges::find(visited, vertex->class_name) == visited.end())
                visited.push_back(vertex->class_name);
            else
                continue;

            for (auto &kid : (*vertex).kids)
            {
                if (kid->class_name == v.class_name)
                {
                    graph_node_ptr detached = kid;
                    (*vertex).kids.erase(std::ranges::find((*vertex).kids, kid));
                    return detached;
                }
                stack_.push(kid);
            }
        }
        return nullptr;
    }

    void add_edge(const GraphNode &parent, const GraphNode &kid)
    {
        auto parent_vertex = find(parent);
        auto kid_vertex = find(kid);

        if (kid_vertex == nullptr)
        {
            if (parent_vertex == nullptr)
            {
                first_level_.push_back(std::make_shared<GraphNode>(parent.class_name, parent.class_));
                parent_vertex = find(parent);
            }
            (*parent_vertex).kids.push_back(std::make_shared<GraphNode>(kid.class_name, kid.class_));
        }
        else
        {
            auto old_kid = detach(kid);
            (*old_kid).class_ = kid.class_;
            if (parent_vertex == nullptr)
            {
                first_level_.push_back(std::make_shared<GraphNode>(parent.class_name, parent.class_));
                parent_vertex = find(parent);
            }
            (*parent_vertex).kids.push_back(old_kid);
            if (!find(kid))
                first_level_.push_back(old_kid);
        }
    }

    std::vector<std::vector<std::string>> find_cycles()
    {
        std::vector<std::vector<std::string>> cycles;
        std::vector<std::string> cycle;
        std::stack<graph_node_ptr> stack_;
        for (auto &kid : first_level_)
            stack_.push(kid);
        std::vector<std::string> visited;
        while (!stack_.empty())
        {
            graph_node_ptr vertex = stack_.top();
            stack_.pop();
            const auto &it = std::ranges::find(visited, vertex->class_name);
            if (it == visited.end())
            {
                visited.push_back(vertex->class_name);
            }
            else
            {
                std::copy(it, visited.end(), std::back_inserter(cycle));
                cycles.push_back(std::move(cycle));
                continue;
            }
            for (auto &kid : vertex->kids)
            {
                stack_.push(kid);
            }
        }
        return cycles;
    }

    void print() const
    {
        std::stack<graph_node_ptr> stack_;
        for (auto &kid : first_level_)
            stack_.push(kid);
        std::vector<std::string> visited;
        while (!stack_.empty())
        {
            graph_node_ptr vertex = stack_.top();
            stack_.pop();
            if (std::ranges::find(visited, vertex->class_name) == visited.end())
            {
                visited.push_back(vertex->class_name);
            }
            else
            {
                std::cout << "Cycle\n";
                continue;
            }
            std::cout << (*vertex).class_name << ": ";
            for (auto &kid : vertex->kids)
            {
                std::cout << (*kid).class_name << ", ";
                stack_.push(kid);
            }
            std::cout << '\n';
        }
    }
};

bool operator==(const GraphNode &lhs, const GraphNode &rhs);
std::ostream &operator<<(std::ostream &out, const GraphNode &node);
void count_classes();
Graph make_inheritance_graph();