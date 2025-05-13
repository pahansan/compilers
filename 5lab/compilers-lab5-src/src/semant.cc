#include "inheritance_graph.h"
#include "semant.h"

#include <cstdlib>

std::vector<Program> ast_roots;

std::string get_full_path(const std::string &filename)
{
    char resolved_path[PATH_MAX];
    if (realpath(filename.c_str(), resolved_path))
        return resolved_path;
    else
        return "";
}

void print_error_message(const std::string &filename, const int &line, const std::string &message)
{
    std::string full_path = get_full_path(filename);
    std::cerr << "\033[1m" << full_path << ':';
    if (line != -1)
        std::cerr << line << ':';
    std::cerr << " \033[31m" << "error:" << "\033[0m " << message << '\n';
}

bool find_class_redefinitions()
{
    std::set<std::string> classes_set{"Object", "IO", "Int", "String", "Bool"};
    bool redefinitions = false;

    for (const auto root : ast_roots)
    {
        auto classes = root->classes;

        for (int i = 0; i < classes->len(); ++i)
        {
            auto current_class = classes->nth(i);
            std::string name = current_class->name->get_string();
            if (classes_set.find(name) != classes_set.end())
            {
                print_error_message(current_class->filename->get_string(), current_class->line_number, "redifinition of class " + name);
                redefinitions = true;
            }
            else
                classes_set.insert(name);
        }
    }

    return redefinitions;
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

bool semant()
{
    return find_class_redefinitions();
}