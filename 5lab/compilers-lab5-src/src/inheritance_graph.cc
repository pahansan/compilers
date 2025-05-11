#include "inheritance_graph.h"

bool operator==(const GraphNode &lhs, const GraphNode &rhs)
{
    return lhs.class_name == rhs.class_name;
}

std::ostream &operator<<(std::ostream &out, const GraphNode &node)
{
    out << node.class_name;
    return out;
}

Graph<GraphNode> make_inheritance_graph()
{
    Classes classes = ast_root->classes;
    std::unordered_map<GraphNode, size_t> classes_count;

    classes_count[GraphNode("Object")] = 1;
    classes_count[GraphNode("IO")] = 1;
    classes_count[GraphNode("Int")] = 1;
    classes_count[GraphNode("String")] = 1;
    classes_count[GraphNode("Bool")] = 1;

    Graph<GraphNode> graph;

    bool contains_main = false;

    graph.add_edge(GraphNode("Object"), GraphNode("IO"));
    graph.add_edge(GraphNode("Object"), GraphNode("Int"));
    graph.add_edge(GraphNode("Object"), GraphNode("String"));
    graph.add_edge(GraphNode("Object"), GraphNode("Bool"));

    for (int i = 0; i < classes->len(); ++i)
    {
        std::string child = classes->nth(i)->name->get_string();
        std::string parent = classes->nth(i)->parent->get_string();
        graph.add_edge(GraphNode(parent, classes->nth(i)), GraphNode(child, classes->nth(i)));
        if (child == "Main")
            contains_main = true;

        if (classes_count.find(child) == classes_count.end())
            classes_count[child] = 1;
        else
            classes_count[child]++;
    }

    if (!contains_main)
        std::cerr << "Error: File does not contain class Main\n";

    for (const auto &[name, count] : classes_count)
    {
        if (count > 1)
            std::cerr << "Error: file contains " << count << " classes with name \"" << name << "\"\n";
    }

    return graph;
}