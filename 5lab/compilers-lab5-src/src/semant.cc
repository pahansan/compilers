#include "inheritance_graph.h"
#include "semant.h"

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

    std::vector<graph_node_ptr> extra_classes = graph.check_first_level();

    std::cout << "Classes without parents: ";
    for (const auto &node : extra_classes)
    {
        std::cout << node->class_name << ", ";
    }
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

std::set<std::string> make_types_table()
{
    std::set<std::string> types_table{"Object", "IO", "Int", "String", "Bool"};
    auto classes = ast_root->classes;

    for (int i = 0; i < classes->len(); i++)
        types_table.insert(classes->nth(i)->name->get_string());

    return types_table;
}