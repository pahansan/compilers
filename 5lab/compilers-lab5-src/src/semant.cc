#include "inheritance_graph.h"
#include "semant.h"

#include <cstdlib>

std::vector<Program> ast_roots;
size_t faults_attend = 0;

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

size_t find_class_redefinitions()
{
    std::set<std::string> classes_set{"Object", "IO", "Int", "String", "Bool", "SELF_TYPE"};
    size_t redefinitions = 0;

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
                ++redefinitions;
            }
            else
                classes_set.insert(name);
        }
    }

    return redefinitions;
}

Graph make_inheritance_graph()
{
    Graph graph;

    auto object_node = GraphNode("Object", make_object_class());
    graph.add_edge(object_node, GraphNode("IO", make_io_class()));
    graph.add_edge(object_node, GraphNode("Int", make_int_class()));
    graph.add_edge(object_node, GraphNode("String", make_string_class()));
    graph.add_edge(object_node, GraphNode("Bool", make_bool_class()));

    for (const auto &root : ast_roots)
    {
        auto classes = root->classes;

        for (int i = 0; i < classes->len(); ++i)
        {
            auto current_class = classes->nth(i);
            std::string child = current_class->name->get_string();
            std::string parent = current_class->parent->get_string();
            graph.add_edge(GraphNode(parent), GraphNode(child, current_class));
        }
    }

    return graph;
}

std::set<std::string> make_types_table()
{
    std::set<std::string> types_table{"Object", "IO", "Int", "String", "Bool", "SELF_TYPE"};
    auto classes = ast_root->classes;

    for (int i = 0; i < classes->len(); i++)
        types_table.insert(classes->nth(i)->name->get_string());

    return types_table;
}

Class_ find_not_null(std::vector<graph_node_ptr> kids)
{
    for (const auto &kid : kids)
        if (kid->class_)
            return kid->class_;
    return nullptr;
}

void semant()
{
    faults_attend += find_class_redefinitions();
    if (faults_attend)
        return;
    auto graph = make_inheritance_graph();

    auto cycles = graph.find_cycles();
    faults_attend += cycles.size();

    for (const auto &cycle : cycles)
    {
        std::string cycle_string;

        for (const auto &node : cycle)
            cycle_string += (*node).class_name + "->";

        cycle_string += (*cycle[0]).class_name;

        auto current_class = find_not_null(cycle);
        auto filename = current_class->filename->get_string();
        auto line = current_class->line_number;
        print_error_message(filename, line, "cyclic inheritance: " + cycle_string);
    }

    if (faults_attend)
        return;

    auto extra_classes = graph.check_first_level();
    faults_attend += extra_classes.size();

    for (const auto &node : extra_classes)
    {
        for (const auto &kid : node->kids)
        {
            auto class_name = node->class_name;
            auto filename = kid->class_->filename->get_string();
            auto line = kid->class_->line_number;
            print_error_message(filename, line, "class \"" + kid->class_name + "\" inherits from class \"" + class_name + "\" that not defined");
        }
    }
    if (faults_attend)
        return;

    auto types_table = make_types_table();

    faults_attend += graph.make_all_checks(types_table);
}
