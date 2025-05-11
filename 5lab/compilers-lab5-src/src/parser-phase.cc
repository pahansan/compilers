#include "cool-tree.h"
#include "utilities.h"
#include "cool-parse.h"

#include <unistd.h>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <set>
#include <iostream>

std::FILE *token_file = stdin;
extern Classes parse_results;
extern Program ast_root;

extern int curr_lineno;
char *curr_filename;
extern int parse_errors;

// Debug flags
extern int yy_flex_debug;
extern int cool_yydebug;
int lex_verbose = 0;

extern int cool_yyparse();

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
    }

    graph.print();
    return graph;
}

int main(int argc, char **argv)
{
    yy_flex_debug = 0;
    cool_yydebug = 0;
    lex_verbose = 0;

    for (int i = 1; i < argc; i++)
    {
        curr_filename = argv[i];
        token_file = std::fopen(argv[i], "r");
        if (token_file == NULL)
        {
            std::cerr << "Error: can not open file " << argv[i] << std::endl;
            std::exit(1);
        }
        curr_lineno = 1;

        cool_yyparse();
        if (parse_errors != 0)
        {
            std::cerr << "Error: parse errors\n";
            std::exit(1);
        }

        /* TODO: dump AST tree (ast_root) to std::cerr */
        ast_root->dump_with_types(std::cerr, 0);

        stringtable.print();
        inttable.print();
        idtable.print();

        Classes classes = ast_root->classes;

        for (int i = 0; classes->more(i); i = classes->next(i))
        {
            Class_ class_ = classes->nth(i);
            std::cout << class_->name->get_string() << ':' << class_->parent->get_string() << '\n';
            if (std::string(classes->nth(i)->get_name()->get_string()) == "Main")
            {
                Features features = classes->nth(i)->features;
                for (int i = 0; features->more(i); i = features->next(i))
                {
                    std::cout << features->nth(i)->name->get_string() << ':';
                    std::cout << features->nth(i)->type_ << ':';
                    if (features->nth(i)->type_ == "method")
                        std::cout << features->nth(i)->return_type << '\n';
                    else
                        std::cout << features->nth(i)->type_decl << '\n';
                }
            }
        }
        std::cout << '\n';
        std::fclose(token_file);

        make_inheritance_graph();
    }

    return 0;
}
