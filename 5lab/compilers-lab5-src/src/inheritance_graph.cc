#include "inheritance_graph.h"
#include "semant.h"

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

    // if (parent.class_name == kid.class_name)
    // {
    //     first_level_.push_back(std::make_shared<GraphNode>(parent.class_name, kid.class_, 1));
    //     parent_vertex = find(parent);
    //     (*parent_vertex).kids.push_back(std::make_shared<GraphNode>(kid.class_name, kid.class_, (*parent_vertex).level_ + 1));
    //     return;
    // }

    if (kid_vertex == nullptr)
    {
        if (parent_vertex == nullptr)
        {
            first_level_.push_back(std::make_shared<GraphNode>(parent.class_name, parent.class_, 1));
            parent_vertex = find(parent);
        }
        (*parent_vertex).kids.push_back(std::make_shared<GraphNode>(kid.class_name, kid.class_, (*parent_vertex).level_ + 1));
    }
    else
    {
        auto old_kid = detach(kid);
        (*old_kid).class_ = kid.class_;
        if (parent_vertex == nullptr)
        {
            first_level_.push_back(std::make_shared<GraphNode>(parent.class_name, parent.class_, 1));
            parent_vertex = find(parent);
        }
        old_kid->level_ = (*parent_vertex).level_ + 1;
        (*parent_vertex).kids.push_back(old_kid);
        if (!find(kid))
        {
            old_kid->level_ = 1;
            first_level_.push_back(old_kid);
        }
    }
}

std::vector<std::vector<graph_node_ptr>> Graph::find_cycles()
{
    std::vector<std::vector<graph_node_ptr>> cycles;
    std::vector<graph_node_ptr> cycle;
    std::stack<graph_node_ptr> stack_;
    for (auto &kid : first_level_)
        stack_.push(kid);
    std::vector<graph_node_ptr> visited;
    while (!stack_.empty())
    {
        graph_node_ptr vertex = stack_.top();
        stack_.pop();
        const auto &it = std::find_if(visited.begin(), visited.end(), [vertex](const graph_node_ptr node)
                                      { return node->class_name == vertex->class_name; });
        if (it == visited.end())
        {
            visited.push_back(vertex);
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

std::vector<graph_node_ptr> Graph::check_first_level()
{
    std::vector<graph_node_ptr> extra_classes;
    if (first_level_.size() > 1)
        std::copy_if(first_level_.begin(), first_level_.end(), std::back_inserter(extra_classes), [](graph_node_ptr node)
                     { return node->class_name != "Object"; });

    return extra_classes;
}

std::vector<Feature> unroll_stack(std::stack<Features> stack)
{
    std::vector<Feature> features_list;
    while (!stack.empty())
    {
        auto frame = stack.top();
        stack.pop();
        for (int i = 0; i < frame->len(); i++)
            features_list.push_back(frame->nth(i));
    }
    return features_list;
}

bool check_signatures(const Feature &first, const Feature &second)
{
    auto first_formals = first->formals;
    auto second_formals = second->formals;
    auto first_return_type = first->return_type;
    auto second_return_type = second->return_type;
    if (std::string(first_return_type->get_string()) != std::string(second_return_type->get_string()))
        return false;
    if (first_formals->len() != second_formals->len())
        return false;
    for (int i = 0; i < first_formals->len(); i++)
        if (std::string(first_formals->nth(i)->type_decl->get_string()) != std::string(second_formals->nth(i)->type_decl->get_string()))
            return false;
    return true;
}

size_t check_feature_redefinitions(const Features &features, const std::string &filename, const std::stack<Features> &stack)
{
    size_t redefinitions = 0;
    auto scope = unroll_stack(stack);
    std::set<std::string> local_scope;

    for (int i = 0; i < features->len(); ++i)
    {
        auto current_feature = features->nth(i);
        std::string type = current_feature->type_;
        std::string name = current_feature->name->get_string();
        auto it = std::ranges::find_if(scope, [current_feature](Feature feature)
                                       { return std::string(current_feature->name->get_string()) == std::string(feature->name->get_string()); });

        if (it != scope.end())
        {
            if (type == "method")
            {
                if ((*it)->type_ == "method")
                {
                    if (!check_signatures((*it), current_feature))
                    {
                        print_error_message(filename, current_feature->line_number, "redifinition of method \"" + name + "\" with wrong signature");
                    }
                }
                else
                {
                    print_error_message(filename, current_feature->line_number, "redifinition of attribute \"" + std::string((*it)->name->get_string()) + "\" with method \"" + name + "\"");
                }
            }
            else
            {
                if ((*it)->type_ == "method")
                {
                    print_error_message(filename, current_feature->line_number, "redifinition of method \"" + std::string((*it)->name->get_string()) + "\" with attribute \"" + name + "\"");
                }
                else
                {
                    print_error_message(filename, current_feature->line_number, "redifinition of attribute \"" + std::string((*it)->name->get_string()) + "\" with attribute \"" + name + "\"");
                }
            }
            ++redefinitions;
        }
        if (local_scope.contains(name))
        {
            print_error_message(filename, current_feature->line_number, "redifinition of feature \"" + name + "\"");
            ++redefinitions;
        }
        else
            local_scope.insert(name);
    }

    return redefinitions;
}

size_t check_feature_types(const Features &features, const std::string &filename, const std::set<std::string> types_table)
{
    size_t faults = 0;

    for (int i = 0; i < features->len(); ++i)
    {
        auto current_feature = features->nth(i);
        std::string name = current_feature->name->get_string();
        std::string type = current_feature->type_;
        if (type == "method")
        {
            std::string return_type = current_feature->return_type->get_string();
            if (!types_table.contains(return_type))
            {
                print_error_message(filename, current_feature->line_number, "return type \"" + return_type + "\" of method \"" + name + "\" undefined");
                ++faults;
            }
        }
        else if (type == "attr")
        {
            std::string type_decl = current_feature->type_decl->get_string();
            if (!types_table.contains(type_decl))
            {
                print_error_message(filename, current_feature->line_number, "type \"" + type_decl + "\" of attribute \"" + name + "\" undefined");
                ++faults;
            }
        }
    }

    return faults;
}

size_t Graph::check_inheritance_from_basic()
{
    size_t faults_count = 0;
    auto object_class = std::ranges::find_if(first_level_, [](const graph_node_ptr &node)
                                             { return node->class_name == "Object"; });
    for (const auto &kid : (*object_class)->kids)
    {
        if (kid->class_name == "Int" || kid->class_name == "String" || kid->class_name == "Bool")
        {
            if (kid->kids.size() > 0)
            {
                for (const auto &inherited : kid->kids)
                {
                    std::string filename = inherited->class_->filename->get_string();
                    auto line = inherited->class_->line_number;
                    std::string name = inherited->class_name;
                    std::string parent_name = kid->class_name;
                    print_error_message(filename, line, "class \"" + name + "\" inherits from basic class \"" + parent_name + "\"");
                    faults_count++;
                }
            }
        }
    }
    return faults_count;
}

size_t check_methods_formal_parameters(const Features &features, const std::string &filename, std::stack<Features> features_table)
{
    size_t redefinitions = 0;
    auto scope = unroll_stack(features_table);
    std::set<std::string> formals_set;

    for (int i = 0; i < features->len(); i++)
    {
        auto current_feature = features->nth(i);
        if (current_feature->type_ == "method")
        {
            auto formals = current_feature->formals;
            std::string method_name = current_feature->name->get_string();
            for (int i = 0; i < formals->len(); i++)
            {
                auto formal = formals->nth(i);
                std::string formal_name = formal->name->get_string();
                auto line = formal->line_number;
                auto it = std::ranges::find_if(scope, [formal_name](Feature feature)
                                               { return formal_name == std::string(feature->name->get_string()); });

                if (it != scope.end())
                {
                    if ((*it)->type_ == "method")
                    {
                        print_error_message(filename, line, "formal parameter \"" + formal_name + "\" of method \"" + method_name + "\" has same name as method \"" + std::string((*it)->name->get_string()) + "\"");
                        redefinitions++;
                    }
                }
                if (formals_set.contains(formal_name))
                {
                    print_error_message(filename, line, "redifinition of formal parameter \"" + formal_name + "\" in method \"" + method_name + "\"");
                    redefinitions++;
                }
                else
                    (formals_set.insert(formal_name));
            }
            formals_set.clear();
        }
    }
    return redefinitions;
}

size_t Graph::make_all_checks(const std::set<std::string> &types_table)
{
    size_t faults_count = 0;
    std::stack<Features> features_table;
    std::stack<graph_node_ptr> stack_;
    for (auto &kid : first_level_)
        stack_.push(kid);
    std::vector<std::string> visited;

    size_t current_level = 0;
    faults_count += check_inheritance_from_basic();

    while (!stack_.empty())
    {
        graph_node_ptr vertex = stack_.top();
        stack_.pop();

        if (std::ranges::find(visited, vertex->class_name) == visited.end())
            visited.push_back(vertex->class_name);
        else
            continue;

        for (size_t i = current_level; i >= (*vertex).level_; i--)
            features_table.pop();

        current_level = (*vertex).level_;
        auto current_class_features = (*vertex).class_->features;
        std::string current_class_filename = vertex->class_->filename->get_string();

        faults_count += check_feature_redefinitions(current_class_features, current_class_filename, features_table);
        faults_count += check_feature_types(current_class_features, current_class_filename, types_table);
        faults_count += check_methods_formal_parameters(current_class_features, current_class_filename, features_table);

        features_table.push(current_class_features);

        for (auto &kid : vertex->kids)
            stack_.push(kid);
    }
    return faults_count;
}