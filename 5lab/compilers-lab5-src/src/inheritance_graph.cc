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

auto find_in_unrolled_by_name(const std::vector<Feature> &unrolled, const std::string &str)
{
    return std::ranges::find_if(unrolled, [str](const Feature &feature)
                                { return str == std::string(feature->name->get_string()); });
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

std::vector<Feature> unroll_stack(std::stack<Class_> stack)
{
    std::vector<Feature> features_list;
    while (!stack.empty())
    {
        auto frame = stack.top()->features;
        stack.pop();
        for (int i = 0; i < frame->len(); i++)
            features_list.push_back(frame->nth(i));
    }
    // features_list.push_back(attr(make_symbol("self"), make_symbol("SELF_TYPE"), no_expr()));
    return features_list;
}

bool check_signatures(const method_class &first, const method_class &second)
{
    auto first_formals = first.formals;
    auto second_formals = second.formals;
    auto first_return_type = first.return_type;
    auto second_return_type = second.return_type;
    if (std::string(first_return_type->get_string()) != std::string(second_return_type->get_string()))
        return false;
    if (first_formals->len() != second_formals->len())
        return false;
    for (int i = 0; i < first_formals->len(); i++)
        if (std::string(first_formals->nth(i)->type_decl->get_string()) != std::string(second_formals->nth(i)->type_decl->get_string()))
            return false;
    return true;
}

size_t check_feature_redefinitions(const Features &features, const std::string &filename, const std::stack<Class_> &stack)
{
    size_t redefinitions = 0;
    auto scope = unroll_stack(stack);
    std::set<std::string> local_scope;

    for (int i = 0; i < features->len(); ++i)
    {
        auto current_feature = features->nth(i);
        std::string type = current_feature->type_;
        std::string name = current_feature->name->get_string();
        auto line = current_feature->line_number;
        auto it = std::ranges::find_if(scope, [current_feature](Feature feature)
                                       { return std::string(current_feature->name->get_string()) == std::string(feature->name->get_string()); });

        if (it != scope.end())
        {
            if (type == "method")
            {
                auto first = static_cast<method_class *>(current_feature);
                if ((*it)->type_ == "method")
                {
                    auto second = static_cast<method_class *>(*it);
                    if (!check_signatures(*first, *second))
                    {
                        print_error_message(filename, line, "redifinition of method \"" + name + "\" with wrong signature");
                    }
                }
                else
                {
                    print_error_message(filename, line, "redifinition of attribute \"" + std::string((*it)->name->get_string()) + "\" with method \"" + name + "\"");
                }
            }
            else
            {
                if ((*it)->type_ == "method")
                {
                    auto second = static_cast<method_class *>(*it);
                    print_error_message(filename, line, "redifinition of method \"" + std::string(second->name->get_string()) + "\" with attribute \"" + name + "\"");
                }
                else
                {
                    auto second = static_cast<attr_class *>(*it);
                    print_error_message(filename, line, "redifinition of attribute \"" + std::string(second->name->get_string()) + "\" with attribute \"" + name + "\"");
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
        auto line = current_feature->line_number;
        if (type == "method")
        {
            auto method = static_cast<method_class *>(current_feature);
            std::string return_type = method->return_type->get_string();
            if (!types_table.contains(return_type))
            {
                print_error_message(filename, line, "return type \"" + return_type + "\" of method \"" + name + "\" undefined");
                ++faults;
            }
        }
        else if (type == "attr")
        {
            auto attr = static_cast<attr_class *>(current_feature);
            std::string type_decl = attr->type_decl->get_string();
            if (!types_table.contains(type_decl))
            {
                print_error_message(filename, line, "type \"" + type_decl + "\" of attribute \"" + name + "\" undefined");
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

size_t check_methods_formal_parameters(const Features &features, const std::string &filename, std::stack<Class_> features_table)
{
    size_t redefinitions = 0;
    auto scope = unroll_stack(features_table);
    std::set<std::string> formals_set;

    for (int i = 0; i < features->len(); i++)
    {
        auto current_feature = features->nth(i);
        if (current_feature->type_ == "method")
        {
            std::string method_name = current_feature->name->get_string();
            auto method = static_cast<method_class *>(current_feature);
            auto formals = method->formals;
            for (int i = 0; i < formals->len(); i++)
            {
                auto formal = formals->nth(i);
                std::string formal_name = formal->name->get_string();
                auto line = formal->line_number;
                auto it = find_in_unrolled_by_name(scope, formal_name);
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

size_t Graph::check_main_class()
{
    size_t faults_count = 0;
    auto main_class = find(GraphNode("Main"));
    if (main_class == nullptr)
    {
        print_error_message("", -1, "none of the files contain the Main class");
        ++faults_count;
        return faults_count;
    }
    auto main_features = main_class->class_->features;
    std::string filename = main_class->class_->filename->get_string();
    bool contains_main_method = false;
    for (int i = 0; i < main_features->len(); i++)
    {
        auto current_feature = main_features->nth(i);
        std::string name = current_feature->name->get_string();
        std::string type = current_feature->type_;
        if (type == "method" && name == "main")
        {
            auto method = static_cast<method_class *>(current_feature);
            contains_main_method = true;
            if (method->formals->len() > 0)
            {
                auto line = current_feature->line_number;
                print_error_message(filename, line, "formal parameters in method \"main\"");
                ++faults_count;
            }
        }
    }
    if (!contains_main_method)
    {
        auto line = main_class->class_->line_number;
        print_error_message(filename, line, "class \"Main\" does not contain method \"main\"");
        ++faults_count;
    }
    return faults_count;
}

std::string find_parent_of_feature(std::stack<Class_> features_table, const std::string &id)
{
    while (!features_table.empty())
    {
        auto current_class = features_table.top();
        features_table.pop();
        auto features = current_class->features;
        for (int i = 0; i < features->len(); i++)
        {
            auto feature = features->nth(i);
            std::string name = feature->name->get_string();
            if (name == id)
                return current_class->name->get_string();
        }
    }
    return "";
}

Feature find_feature_in_its_class(std::stack<Class_> features_table, const std::string &classname, const std::string &id)
{
    while (!features_table.empty())
    {
        auto current_class = features_table.top();
        auto current_class_name = current_class->name->get_string();
        features_table.pop();
        if (current_class_name == classname)
        {
            auto features = current_class->features;
            for (int i = 0; i < features->len(); i++)
            {
                auto feature = features->nth(i);
                std::string name = feature->name->get_string();
                if (name == id)
                    return feature;
            }
        }
    }
    return nullptr;
}

size_t parse_expression(const std::string &filename,
                        std::vector<Feature> unrolled_stack,
                        const Expression &expression,
                        const std::string &feature_name,
                        const std::string &target_type,
                        const std::stack<Class_> &features_table)
{
    const std::string &type = expression->type_;
    size_t faults_count = 0;
    if (type == "int_const" || type == "string_const" || type == "bool_const")
    {
        std::string expression_type = expression->type->get_string();
        if (target_type != "")
            if (target_type != expression_type)
            {
                auto line = expression->line_number;
                print_error_message(filename, line, "expression has type \"" + expression_type + "\" and feature \"" + feature_name + "\" has type \"" + target_type + "\"");
                ++faults_count;
                return faults_count;
            }
    }
    else if (type == "object")
    {
        std::string object_name = expression->name->get_string();
        if (object_name == "self")
        {
            auto parent = find_parent_of_feature(features_table, feature_name);
            if (parent == target_type || target_type != "")
                return faults_count;
        }
        auto expr_object = find_in_unrolled_by_name(unrolled_stack, object_name);
        if (expr_object == unrolled_stack.end())
        {
            auto line = expression->line_number;
            print_error_message(filename, line, "using undefined object \"" + object_name + "\"");
            ++faults_count;
            return faults_count;
        }
        std::string expr_object_type = (*expr_object)->type_;
        if (expr_object_type == "method")
        {
            auto line = expression->line_number;
            print_error_message(filename, line, "using method \"" + object_name + "\" like it's object");
            ++faults_count;
        }

        auto line = expression->line_number;
        if (expr_object_type == "method")
        {
            auto method = static_cast<method_class *>(*expr_object);
            std::string return_type = method->return_type->get_string();
            if (target_type != "")
                if (return_type != target_type)
                {
                    print_error_message(filename, line, "method has return type \"" + return_type + "\" and feature \"" + feature_name + "\" has type \"" + target_type + "\"");
                    ++faults_count;
                    return faults_count;
                }
        }
        else
        {
            auto attr = static_cast<attr_class *>(*expr_object);
            std::string type_decl = attr->type_decl->get_string();
            if (target_type != "")
                if (type_decl != target_type)
                {
                    print_error_message(filename, line, "attribute has type \"" + type_decl + "\" and feature \"" + feature_name + "\" has type \"" + target_type + "\"");
                    ++faults_count;
                    return faults_count;
                }
        }
    }
    else if (type == "assign")
    {
        auto assign = static_cast<assign_class *>(expression);
        std::string lvalue = assign->name->get_string();
        auto rvalue = assign->expr;
        auto expr_object = find_in_unrolled_by_name(unrolled_stack, lvalue);
        if (expr_object == unrolled_stack.end())
        {
            auto line = assign->line_number;
            print_error_message(filename, line, "using undefined object \"" + lvalue + "\"");
            ++faults_count;
            return faults_count;
        }
        std::string expr_object_type = (*expr_object)->type_;
        auto line = assign->line_number;
        if (expr_object_type == "method")
        {
            auto method = static_cast<method_class *>(*expr_object);
            print_error_message(filename, line, "using method \"" + lvalue + "\" like it's object");
            ++faults_count;
            std::string return_type = method->return_type->get_string();
            faults_count += parse_expression(filename, unrolled_stack, rvalue, lvalue, return_type, features_table);
        }
        else
        {
            auto attr = static_cast<attr_class *>(*expr_object);
            std::string type_decl = attr->type_decl->get_string();
            faults_count += parse_expression(filename, unrolled_stack, rvalue, lvalue, type_decl, features_table);
        }
    }
    else if (type == "dispatch")
    //  <expr>.<id>(<expr>,...,<expr>)
    //  <id>(<expr>,...,<expr>)
    {
        auto dispatch = static_cast<dispatch_class *>(expression);
        auto left_expr = dispatch->expr;
        std::string id = dispatch->name->get_string();
        auto args = dispatch->actual;
        auto it = find_in_unrolled_by_name(unrolled_stack, id);
        auto line = dispatch->line_number;

        if (it == unrolled_stack.end())
        {
            print_error_message(filename, line, "method \"" + id + "\" undefined");
            faults_count++;
            return faults_count;
        }

        if ((*it)->type_ == "method")
        {
            auto method = static_cast<method_class *>(*it);
            auto return_type = method->return_type->get_string();
            faults_count += parse_expression(filename, unrolled_stack, left_expr, feature_name, find_parent_of_feature(features_table, id), features_table);
            if (method->formals->len() != args->len())
                print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

            for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
                faults_count += parse_expression(filename, unrolled_stack, args->nth(i), feature_name, method->formals->nth(i)->type_decl->get_string(), features_table);
        }
        else
        {
            auto attr = static_cast<attr_class *>(*it);
            auto type_decl = attr->type_decl;
            print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
        }
    }
    else if (type == "static_dispatch")
    // <expr>@<type>.id(<expr>,...,<expr>)
    {
        auto dispatch = static_cast<static_dispatch_class *>(expression);
        auto left_expr = dispatch->expr;
        std::string type_name = dispatch->type_name->get_string();
        std::string id = dispatch->name->get_string();
        auto args = dispatch->actual;
        auto line = dispatch->line_number;
        auto feature = find_feature_in_its_class(features_table, type_name, id);

        if (feature == nullptr)
        {
            print_error_message(filename, line, "class \"" + type_name + "\" does not contain method \"" + id + "\"");
            faults_count++;
            return faults_count;
        }

        if (feature->type_ == "method")
        {
            auto method = static_cast<method_class *>(feature);
            auto return_type = method->return_type->get_string();
            faults_count += parse_expression(filename, unrolled_stack, left_expr, feature_name, find_parent_of_feature(features_table, id), features_table);
            if (method->formals->len() != args->len())
                print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

            for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
                faults_count += parse_expression(filename, unrolled_stack, args->nth(i), feature_name, method->formals->nth(i)->type_decl->get_string(), features_table);
        }
        else
        {
            auto attr = static_cast<attr_class *>(feature);
            auto type_decl = attr->type_decl;
            print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
        }
    }
    else if (type == "cond")
    {
        auto cond = static_cast<cond_class *>(expression);
        auto pred = cond->pred;
        auto then_exp = cond->then_exp;
        auto else_exp = cond->else_exp;

        faults_count += parse_expression(filename, unrolled_stack, pred, feature_name, "Bool", features_table);
        faults_count += parse_expression(filename, unrolled_stack, then_exp, feature_name, target_type, features_table);
        faults_count += parse_expression(filename, unrolled_stack, pred, feature_name, target_type, features_table);
    }
    else if (type == "loop")
    {
        auto loop = static_cast<loop_class *>(expression);
        auto pred = loop->pred;
        auto body = loop->body;

        faults_count += parse_expression(filename, unrolled_stack, pred, feature_name, "Bool", features_table);
        faults_count += parse_expression(filename, unrolled_stack, body, feature_name, target_type, features_table);
    }
    else if (type == "block")
    {
        auto block = static_cast<block_class *>(expression);
        auto expressions = block->body;
        for (int i = 0; i < expressions->len(); i++)
            if (i == expressions->len() - 1)
                faults_count += parse_expression(filename, unrolled_stack, expressions->nth(i), feature_name, target_type, features_table);
            else
                faults_count += parse_expression(filename, unrolled_stack, expressions->nth(i), feature_name, "", features_table);
    }
    return faults_count;
}

Features formals_to_features(const Formals formals)
{
    if (formals->len() == 0)
        return nullptr;
    auto features = single_Features(attr(formals->nth(0)->name, formals->nth(0)->type_decl, no_expr()));
    for (int i = 0; i < formals->len(); i++)
        features = append_Features(features, single_Features(attr(formals->nth(i)->name, formals->nth(i)->type_decl, no_expr())));

    return features;
}

size_t check_expressions(const std::string &filename, const Features &features, const std::stack<Class_> &features_table)
{
    size_t faults_count = 0;
    for (int i = 0; i < features->len(); i++)
    {
        auto current_feature = features->nth(i);
        std::string type = current_feature->type_;
        std::string name = current_feature->name->get_string();
        if (type == "method")
        {
            auto method = static_cast<method_class *>(current_feature);
            auto unrolled = unroll_stack(features_table);
            auto new_frame = formals_to_features(method->formals);
            if (new_frame)
                for (int i = 0; i < new_frame->len(); i++)
                    unrolled.insert(unrolled.begin(), new_frame->nth(i));

            std::string return_type = method->return_type->get_string();
            if (return_type == "SELF_TYPE")
                return_type = find_parent_of_feature(features_table, method->name->get_string());
            faults_count += parse_expression(filename, unrolled, method->expr, name, return_type, features_table);
        }
        else
        {
            auto attr = static_cast<attr_class *>(current_feature);
            auto unrolled = unroll_stack(features_table);
            std::string type_decl = attr->type_decl->get_string();
            faults_count += parse_expression(filename, unrolled, attr->init, name, type_decl, features_table);
        }
    }
    return faults_count;
}

size_t Graph::make_all_checks(const std::set<std::string> &types_table)
{
    size_t faults_count = 0;
    std::stack<Class_> features_table;
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

        features_table.push((*vertex).class_);

        faults_count += check_expressions(current_class_filename, current_class_features, features_table);

        for (auto &kid : vertex->kids)
            stack_.push(kid);
    }
    faults_count += check_main_class();
    return faults_count;
}