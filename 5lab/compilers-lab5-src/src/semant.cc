#include "inheritance_graph.h"

std::unordered_map<GraphNode, size_t> count_classes()
{
    Classes classes = ast_root->classes;
    std::unordered_map<GraphNode, size_t> classes_count;

    for (int i = 0; i < classes->len(); ++i)
    {
        std::string name = classes->nth(i)->name->get_string();

        if (classes_count.find(name) == classes_count.end())
            classes_count[GraphNode(name, classes->nth(i))] = 1;
        else
            classes_count[GraphNode(name, classes->nth(i))]++;
    }

    classes_count[GraphNode("Object")]++;
    classes_count[GraphNode("IO")]++;
    classes_count[GraphNode("Int")]++;
    classes_count[GraphNode("String")]++;
    classes_count[GraphNode("Bool")]++;

    // for (const auto &[name, count] : classes_count)
    // {
    //     if (count > 1)
    //     {
    //         std::cerr << "Error: file \"" << name.class_->filename->get_string() << "\" contains " << count << " classes with name \"" << name << "\"\n";
    //         bool faults_attend = true;
    //     }
    // }

    return classes_count;
}

Graph make_inheritance_graph()
{
    Classes classes = ast_root->classes;

    Graph graph;

    bool contains_main = false;

    auto object_node = GraphNode("Object", make_object_class());

    graph.add_edge(object_node, GraphNode("IO", make_io_class()));
    graph.add_edge(object_node, GraphNode("Int", make_int_class()));
    graph.add_edge(object_node, GraphNode("String", make_string_class()));
    graph.add_edge(object_node, GraphNode("Bool", make_bool_class()));

    for (int i = 0; i < classes->len(); ++i)
    {
        std::string child = classes->nth(i)->name->get_string();
        std::string parent = classes->nth(i)->parent->get_string();
        graph.add_edge(GraphNode(parent), GraphNode(child, classes->nth(i)));
        if (child == "Main")
            contains_main = true;
    }

    if (!contains_main)
    {
        std::cerr << "Error: File does not contain class Main\n";
        bool faults_attend = true;
    }

    graph.print();
    std::cout << '\n';

    std::vector<std::vector<std::string>> cycles = graph.find_cycles();
    for (const auto &cycle : cycles)
    {
        for (const auto &node : cycle)
            std::cout << node << ':';
        std::cout << '\n';
    }
    return graph;
}