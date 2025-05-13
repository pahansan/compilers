#include "inheritance_graph.h"

bool faults_attend = false;

bool operator==(const GraphNode &lhs, const GraphNode &rhs)
{
    return lhs.class_name == rhs.class_name;
}

std::ostream &operator<<(std::ostream &out, const GraphNode &node)
{
    out << node.class_name;
    return out;
}

Graph::Graph()
{
    // first_level_.push_back(std::make_shared<GraphNode>("Object"));
}

graph_node_ptr Graph::find(const GraphNode &v)
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

graph_node_ptr Graph::detach(const GraphNode &v)
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

void Graph::add_edge(const GraphNode &parent, const GraphNode &kid)
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

std::vector<std::vector<std::string>> Graph::find_cycles()
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

void Graph::print() const
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