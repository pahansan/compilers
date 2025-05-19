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

graph_node_ptr Graph::find(const GraphNode &v) const
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

graph_node_ptr Graph::is_parent(const std::string &older, const std::string &younger) const
{
    std::stack<graph_node_ptr> stack_;
    graph_node_ptr start = find(GraphNode(older));
    for (auto &vertex : start->kids)
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

        if ((*vertex).class_name == younger)
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
                     { return node->class_name != "Object" && node->class_ == nullptr; });

    return extra_classes;
}

std::stack<Class_> Graph::get_scope_of_type(const std::string &type) const
{
    std::stack<Class_> scope;
    if (find(GraphNode(type)) == nullptr)
        return scope;
    std::stack<graph_node_ptr> stack_;
    for (auto &kid : first_level_)
        stack_.push(kid);
    std::vector<std::string> visited;

    size_t current_level = 0;

    while (!stack_.empty())
    {
        graph_node_ptr vertex = stack_.top();
        stack_.pop();

        if (std::ranges::find(visited, vertex->class_name) == visited.end())
            visited.push_back(vertex->class_name);
        else
            continue;

        for (size_t i = current_level; i >= (*vertex).level_; i--)
            scope.pop();

        std::string current_class_name = (*vertex).class_name;

        current_level = (*vertex).level_;
        auto current_class_features = (*vertex).class_->features;
        std::string current_class_filename = vertex->class_->filename->get_string();

        scope.push((*vertex).class_);

        if (current_class_name == type)
            return scope;

        for (auto &kid : vertex->kids)
            stack_.push(kid);
    }

    return std::stack<Class_>();
}

std::vector<Feature> unroll_stack(std::stack<Class_> stack)
{
    std::vector<Feature> features_list;
    while (!stack.empty())
    {
        std::string current_class_name = stack.top()->name->get_string();
        auto frame = stack.top()->features;
        stack.pop();
        for (int i = 0; i < frame->len(); i++)
        {
            auto feature = frame->nth(i);
            features_list.push_back(feature);
        }
    }
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

Feature find_feature_in_feature_list(const Features &features, const std::string &name)
{
    for (int i = 0; i < features->len(); i++)
        if (std::string(features->nth(i)->name->get_string()) == name)
            return features->nth(i);

    return nullptr;
}

std::string parse_expression(const std::string &filename,
                             const std::string &feature_name,
                             std::vector<Feature> unrolled_stack,
                             const Expression &expression,
                             const std::string &target_type,
                             const Graph &graph,
                             const std::stack<Class_> &features_table)
{
    const std::string &type = expression->type_;
    if (type == "int_const" || type == "string_const" || type == "bool_const")
    {
        std::string expression_type = expression->type->get_string();
        return expression_type;
    }
    else if (type == "object")
    {
        auto object = static_cast<object_class *>(expression);
        std::string object_name = expression->name->get_string();
        if (object_name == "self")
            return target_type;

        auto expr_object = find_in_unrolled_by_name(unrolled_stack, object_name);
        if (expr_object == unrolled_stack.end())
        {
            auto line = expression->line_number;
            print_error_message(filename, line, "using undefined object \"" + object_name + "\"");
            return "unknown";
        }
        std::string expr_object_type = (*expr_object)->type_;
        auto line = expression->line_number;
        if (expr_object_type == "method")
        {
            auto method = static_cast<method_class *>(*expr_object);
            print_error_message(filename, line, "using method \"" + object_name + "\" like it's object");
            std::string return_type = method->return_type->get_string();
            return return_type;
        }
        else
        {
            auto attr = static_cast<attr_class *>(*expr_object);
            std::string type_decl = attr->type_decl->get_string();
            return type_decl;
        }
    }
    else if (type == "assign")
    // <id> <- <expr>
    {
        auto assign = static_cast<assign_class *>(expression);
        std::string lvalue = assign->name->get_string();
        auto rvalue = assign->expr;
        auto expr_object = find_in_unrolled_by_name(unrolled_stack, lvalue);
        if (expr_object == unrolled_stack.end())
        {
            auto line = assign->line_number;
            print_error_message(filename, line, "using undefined object \"" + lvalue + "\"");
            return "unknown";
        }
        std::string expr_object_type = (*expr_object)->type_;
        auto line = assign->line_number;
        auto expression_type = parse_expression(filename, feature_name, unrolled_stack, rvalue, lvalue, graph, features_table);
        if (expr_object_type == "method")
        {
            auto method = static_cast<method_class *>(*expr_object);
            print_error_message(filename, line, "using method \"" + lvalue + "\" like it's object");
            std::string return_type = method->return_type->get_string();
            if (return_type != expression_type)
            {
                print_error_message(filename, line, "identifier \"" + lvalue + "\" has type \"" + return_type + "\" and expression has type \"" + expression_type + "\"");
            }
            return return_type;
        }
        else
        {
            auto attr = static_cast<attr_class *>(*expr_object);
            std::string type_decl = attr->type_decl->get_string();
            if (type_decl != expression_type)
            {
                print_error_message(filename, line, "identifier \"" + lvalue + "\" has type \"" + type_decl + "\" and expression has type \"" + expression_type + "\"");
            }
            return type_decl;
        }
    }
    else if (type == "dispatch")
    //  <expr>.<id>(<expr>,...,<expr>)
    //  <id>(<expr>,...,<expr>)
    // Если expr имеет тип object и равен self,
    //   тогда вызывается метод, который объявлен в текущем классе,
    //   либо был унаследован от кого-то
    // В таком случае нужно искать его в области видимости, специфичной для данного класса
    //
    // Если же expr имеет тип object и не равен self,
    //   Тогда он вызывается для конкретного объекта, который должен быть объявлен ранее
    // В таком случае нужно проверить идентификатор этого объекта в текущей области видимости,
    // а также его тип
    // Обработка object уже это делает
    //
    // Так как любой разбор выражения возвращает тип, то после получения типа нужно просто убедиться,
    // что данный тип обладает указанным методом
    // Для этого можно просто найти этот тип в графе и проверить его список методов
    {
        auto dispatch = static_cast<dispatch_class *>(expression);
        auto line = dispatch->line_number;
        auto expr = dispatch->expr;
        std::string id = dispatch->name->get_string();
        auto args = dispatch->actual;

        if (expr->type_ == "object")
        {
            std::string object_name = expr->name->get_string();
            if (object_name == "self")
            {
                auto it = find_in_unrolled_by_name(unrolled_stack, id);
                if (it == unrolled_stack.end())
                {
                    print_error_message(filename, line, "using undefined method \"" + id + "\"");
                    return "unknown";
                }
                auto feature = (*it);
                auto feature_parent = find_parent_of_feature(features_table, id);
                if (feature->type_ == "method")
                {
                    auto method = static_cast<method_class *>(feature);
                    auto return_type = std::string(method->return_type->get_string());

                    if (return_type == "SELF_TYPE")
                        return_type = feature_parent;

                    if (method->formals->len() != args->len())
                        print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

                    for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
                    {
                        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, args->nth(i), target_type, graph, features_table);
                        auto type_decl = std::string(method->formals->nth(i)->type_decl->get_string());
                        if (expr_type != type_decl)
                        {
                            print_error_message(filename, line, std::to_string(i + 1) + " parameter of method \"" + id + "\" has type \"" + type_decl + "\" and argument has type \"" + expr_type + "\"");
                        }
                    }
                    return return_type;
                }
                else
                {
                    auto attr = static_cast<attr_class *>(feature);
                    auto type_decl = std::string(attr->type_decl->get_string());
                    print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
                    return type_decl;
                }
            }
            else
            {
                auto it = find_in_unrolled_by_name(unrolled_stack, object_name);
                if (it == unrolled_stack.end())
                {
                    print_error_message(filename, line, "using indefined object \"" + object_name + "\"");
                    return "unknown";
                }
                auto object_feature = (*it);
                auto object_feature_type = object_feature->type_;
                if (object_feature_type == "method")
                {
                    auto object_method = static_cast<method_class *>(object_feature);
                    auto object_type = std::string(object_method->return_type->get_string());
                    auto object_scope = unroll_stack(graph.get_scope_of_type(object_type));

                    auto it = find_in_unrolled_by_name(object_scope, id);
                    if (it == object_scope.end())
                    {
                        print_error_message(filename, line, "using undefined method \"" + id + "\"");
                        return "unknown";
                    }
                    auto feature = (*it);
                    auto feature_parent = find_parent_of_feature(features_table, id);
                    if (feature->type_ == "method")
                    {
                        auto method = static_cast<method_class *>(feature);
                        auto return_type = std::string(method->return_type->get_string());

                        if (return_type == "SELF_TYPE")
                            return_type = feature_parent;

                        if (method->formals->len() != args->len())
                            print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

                        for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
                        {
                            auto expr_type = parse_expression(filename, feature_name, unrolled_stack, args->nth(i), target_type, graph, features_table);
                            auto type_decl = std::string(method->formals->nth(i)->type_decl->get_string());
                            if (expr_type != type_decl)
                            {
                                print_error_message(filename, line, std::to_string(i + 1) + " parameter of method \"" + id + "\" has type \"" + type_decl + "\" and argument has type \"" + expr_type + "\"");
                            }
                        }
                        return return_type;
                    }
                    else
                    {
                        auto attr = static_cast<attr_class *>(feature);
                        auto type_decl = std::string(attr->type_decl->get_string());
                        print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
                        return type_decl;
                    }
                }
                else
                {
                    auto object_method = static_cast<attr_class *>(object_feature);
                    auto object_type = std::string(object_method->type_decl->get_string());
                    auto object_scope = unroll_stack(graph.get_scope_of_type(object_type));

                    auto it = find_in_unrolled_by_name(object_scope, id);
                    if (it == object_scope.end())
                    {
                        print_error_message(filename, line, "using undefined method \"" + id + "\"");
                        return "unknown";
                    }
                    auto feature = (*it);
                    auto feature_parent = find_parent_of_feature(features_table, id);
                    if (feature->type_ == "method")
                    {
                        auto method = static_cast<method_class *>(feature);
                        auto return_type = std::string(method->return_type->get_string());

                        if (return_type == "SELF_TYPE")
                            return_type = feature_parent;

                        if (method->formals->len() != args->len())
                            print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

                        for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
                        {
                            auto expr_type = parse_expression(filename, feature_name, unrolled_stack, args->nth(i), target_type, graph, features_table);
                            auto type_decl = std::string(method->formals->nth(i)->type_decl->get_string());
                            if (expr_type != type_decl)
                            {
                                print_error_message(filename, line, std::to_string(i + 1) + " parameter of method \"" + id + "\" has type \"" + type_decl + "\" and argument has type \"" + expr_type + "\"");
                            }
                        }
                        return return_type;
                    }
                    else
                    {
                        auto attr = static_cast<attr_class *>(feature);
                        auto type_decl = std::string(attr->type_decl->get_string());
                        print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
                        return type_decl;
                    }
                }
            }
        }

        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, expr, "", graph, features_table);
        auto expr_scope = unroll_stack(graph.get_scope_of_type(expr_type));

        auto it = find_in_unrolled_by_name(expr_scope, id);
        if (it == expr_scope.end())
        {
            print_error_message(filename, line, "using undefined method \"" + id + "\"");
            return "unknown";
        }
        auto feature = (*it);
        auto feature_parent = find_parent_of_feature(features_table, id);
        if (feature->type_ == "method")
        {
            auto method = static_cast<method_class *>(feature);
            auto return_type = std::string(method->return_type->get_string());

            if (return_type == "SELF_TYPE")
                return_type = expr_type;

            if (method->formals->len() != args->len())
                print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

            for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
            {
                auto expr_type = parse_expression(filename, feature_name, unrolled_stack, args->nth(i), target_type, graph, features_table);
                auto type_decl = std::string(method->formals->nth(i)->type_decl->get_string());
                if (expr_type != type_decl)
                {
                    print_error_message(filename, line, std::to_string(i + 1) + " parameter of method \"" + id + "\" has type \"" + type_decl + "\" and argument has type \"" + expr_type + "\"");
                }
            }
            return return_type;
        }
        else
        {
            auto attr = static_cast<attr_class *>(feature);
            auto type_decl = std::string(attr->type_decl->get_string());
            print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
            return type_decl;
        }
    }
    else if (type == "static_dispatch")
    // <expr>@<type>.id(<expr>,...,<expr>)
    {
        auto static_dispatch = static_cast<static_dispatch_class *>(expression);
        auto line = static_dispatch->line_number;
        auto expr = static_dispatch->expr;
        auto type_decl = std::string(static_dispatch->type_name->get_string());
        std::string id = static_dispatch->name->get_string();
        auto args = static_dispatch->actual;
        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, expr, "", graph, features_table);
        auto type_class = graph.is_parent(type_decl, expr_type);

        if (type_class == nullptr)
        {
            print_error_message(filename, line, "class \"" + type_decl + "\" is not an ancestor of class \"" + expr_type + "\"");
            return "unknown";
        }

        auto type_features = type_class->class_->features;

        for (int i = 0; i < type_features->len(); i++)
        {
            auto current_feature = type_features->nth(i);
            auto current_feature_name = current_feature->name->get_string();
            if (current_feature_name == id)
            {
                if (current_feature->type_ == "method")
                {
                    auto method = static_cast<method_class *>(current_feature);
                    auto return_type = std::string(method->return_type->get_string());

                    if (return_type == "SELF_TYPE")
                        return_type = type_decl;

                    if (method->formals->len() != args->len())
                        print_error_message(filename, line, "wrong number of arguments in method \"" + id + "\"");

                    for (int i = 0; i < std::min(method->formals->len(), args->len()); i++)
                    {
                        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, args->nth(i), target_type, graph, features_table);
                        auto type_decl = std::string(method->formals->nth(i)->type_decl->get_string());
                        if (expr_type != type_decl)
                        {
                            print_error_message(filename, line, std::to_string(i + 1) + " parameter of method \"" + id + "\" has type \"" + type_decl + "\" and argument has type \"" + expr_type + "\"");
                        }
                    }
                    return return_type;
                }
                else
                {
                    auto attr = static_cast<attr_class *>(current_feature);
                    auto type_decl = std::string(attr->type_decl->get_string());
                    print_error_message(filename, line, "using attr \"" + id + "\" like it's method");
                    return type_decl;
                }
            }
        }
        print_error_message(filename, line, "class \"" + type_decl + "\" does not contain feature \"" + id + "\"");
    }
    else if (type == "cond")
    {
        auto cond = static_cast<cond_class *>(expression);
        auto pred = cond->pred;
        auto then_exp = cond->then_exp;
        auto else_exp = cond->else_exp;

        auto pred_type = parse_expression(filename, feature_name, unrolled_stack, pred, target_type, graph, features_table);
        if (pred_type != "Bool")
        {
            auto line = pred->line_number;
            print_error_message(filename, line, "Conditions must have type \"Bool\" but it's \"" + pred_type + "\"");
        }

        auto then_type = parse_expression(filename, feature_name, unrolled_stack, then_exp, target_type, graph, features_table);
        auto else_type = parse_expression(filename, feature_name, unrolled_stack, else_exp, target_type, graph, features_table);

        if (then_type != else_type)
        {
            auto line = pred->line_number;
            print_error_message(filename, line, "Then branch has type \"" + then_type + "\" and else branch has type \"" + else_type + "\"");
        }

        return then_type;
    }
    else if (type == "loop")
    {
        auto loop = static_cast<loop_class *>(expression);
        auto pred = loop->pred;
        auto body = loop->body;

        auto pred_type = parse_expression(filename, feature_name, unrolled_stack, pred, target_type, graph, features_table);
        if (pred_type != "Bool")
        {
            auto line = pred->line_number;
            print_error_message(filename, line, "Conditions must have type \"Bool\" but it's \"" + pred_type + "\"");
        }

        auto body_type = parse_expression(filename, feature_name, unrolled_stack, body, target_type, graph, features_table);
        return "Object";
    }
    else if (type == "block")
    {
        auto block = static_cast<block_class *>(expression);
        auto expressions = block->body;
        for (int i = 0; i < expressions->len(); i++)
            if (i == expressions->len() - 1)
                return parse_expression(filename, feature_name, unrolled_stack, expressions->nth(i), target_type, graph, features_table);
            else
                parse_expression(filename, feature_name, unrolled_stack, expressions->nth(i), target_type, graph, features_table);
    }
    else if (type == "let")
    {
        auto let = static_cast<let_class *>(expression);
        std::string identifier = let->identifier->get_string();
        std::string type_decl = let->type_decl->get_string();
        auto init = let->init;
        auto body = let->body;
        auto unrolled_copy = unrolled_stack;
        auto line = let->line_number;
        auto it = find_in_unrolled_by_name(unrolled_stack, identifier);
        if (identifier == "self")
            print_error_message(filename, line, "using name \"self\" in let");
        if (it != unrolled_stack.end())
        {
            auto feature = (*it);
            auto feature_name = std::string(feature->name->get_string());
            print_error_message(filename, line, "redefinition of feature \"" + feature_name + "\"");
        }
        auto init_type = parse_expression(filename, feature_name, unrolled_stack, init, type_decl, graph, features_table);
        if (init_type != type_decl)
        {
            print_error_message(filename, line, "identifier has type \"" + type_decl + "\" and expression has type \"" + init_type + "\"");
        }
        unrolled_copy.insert(unrolled_copy.begin(), attr(make_symbol(identifier), make_symbol(type_decl), no_expr()));
        return parse_expression(filename, feature_name, unrolled_copy, body, target_type, graph, features_table);
    }
    else if (type == "typcase")
    {
        auto typcase = static_cast<typcase_class *>(expression);
        auto expr = typcase->expr;
        auto cases = typcase->cases;
        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, expr, "", graph, features_table);
        for (int i = 0; i < cases->len(); i++)
        {
            auto this_case = cases->nth(i);
            auto line = this_case->line_number;
            auto name = std::string(this_case->name->get_string());
            auto type_decl = std::string(this_case->type_decl->get_string());
            auto type_node = graph.find(GraphNode(type_decl));

            if (type_node == nullptr)
                print_error_message(filename, line, "using undefined type \"" + type_decl + "\"");

            auto expr = this_case->expr;
            auto unrolled_copy = unrolled_stack;
            unrolled_copy.insert(unrolled_copy.begin(), attr(make_symbol(name), make_symbol(type_decl), no_expr()));
            auto case_expr_type = parse_expression(filename, feature_name, unrolled_copy, expr, "", graph, features_table);
            if (expr_type == type_decl)
            {
                return case_expr_type;
            }
        }
        return "unknown";
    }
    else if (type == "new_")
    {
        auto new_ = static_cast<new__class *>(expression);
        std::string type_name = new_->type_name->get_string();
        auto type_node = graph.find(GraphNode(type_name));
        auto line = new_->line_number;
        if (type_node == nullptr)
        {
            print_error_message(filename, line, "an attempt to allocate memory for an undeclared class \"" + type_name + "\"");
        }
        return type_name;
    }
    else if (type == "isvoid")
    {
        auto isvoid = static_cast<isvoid_class *>(expression);
        auto expr = isvoid->e1;
        parse_expression(filename, feature_name, unrolled_stack, expr, "", graph, features_table);
        return "Bool";
    }
    else if (type == "plus")
    {
        auto plus = static_cast<plus_class *>(expression);
        auto left = plus->e1;
        auto right = plus->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = plus->line_number;
        if (left_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + left_type + "\"");
        if (right_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + right_type + "\"");

        return left_type;
    }
    else if (type == "sub")
    {
        auto sub = static_cast<sub_class *>(expression);
        auto left = sub->e1;
        auto right = sub->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = sub->line_number;
        if (left_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + left_type + "\"");
        if (right_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + right_type + "\"");

        return left_type;
    }
    else if (type == "mul")
    {
        auto mul = static_cast<mul_class *>(expression);
        auto left = mul->e1;
        auto right = mul->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = mul->line_number;
        if (left_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + left_type + "\"");
        if (right_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + right_type + "\"");

        return left_type;
    }
    else if (type == "divide")
    {
        auto divide = static_cast<divide_class *>(expression);
        auto left = divide->e1;
        auto right = divide->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = divide->line_number;
        if (left_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + left_type + "\"");
        if (right_type != "Int")
            print_error_message(filename, line, "arithmetic operations only support \"Int\" values, but the operand has type \"" + right_type + "\"");

        return left_type;
    }
    else if (type == "neg")
    {
        auto neg = static_cast<neg_class *>(expression);
        auto expr = neg->e1;
        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, expr, "Int", graph, features_table);
        auto line = neg->line_number;
        if (expr_type != "Int")
            print_error_message(filename, line, "negation applies only to \"Int\", but expression type is \"" + expr_type + "\"");

        return "Int";
    }
    else if (type == "comp")
    {
        auto comp = static_cast<comp_class *>(expression);
        auto expr = comp->e1;
        auto expr_type = parse_expression(filename, feature_name, unrolled_stack, expr, "Int", graph, features_table);
        auto line = comp->line_number;
        if (expr_type != "Bool")
            print_error_message(filename, line, "complement applies only to \"Bool\", but expression type is \"" + expr_type + "\"");

        return "Bool";
    }
    else if (type == "eq")
    {
        auto eq = static_cast<eq_class *>(expression);
        auto left = eq->e1;
        auto right = eq->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = eq->line_number;
        if (left_type != right_type)
            if (left_type == "Int" || left_type == "Bool" || left_type == "String")
                print_error_message(filename, line, "when comparing equality, the type must be the same for the base types");

        return "Bool";
    }
    else if (type == "lt")
    {
        auto lt = static_cast<lt_class *>(expression);
        auto left = lt->e1;
        auto right = lt->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = lt->line_number;
        if (left_type != "Int")
            print_error_message(filename, line, "less than operator only supports \"Int\" values, but the operand has type \"" + left_type + "\"");
        if (right_type != "Int")
            print_error_message(filename, line, "less than operator only supports \"Int\" values, but the operand has type \"" + right_type + "\"");

        return "Bool";
    }
    else if (type == "leq")
    {
        auto leq = static_cast<leq_class *>(expression);
        auto left = leq->e1;
        auto right = leq->e2;
        auto left_type = parse_expression(filename, feature_name, unrolled_stack, left, "Int", graph, features_table);
        auto right_type = parse_expression(filename, feature_name, unrolled_stack, right, "Int", graph, features_table);
        auto line = leq->line_number;
        if (left_type != "Int")
            print_error_message(filename, line, "less or equal operator only supports \"Int\" values, but the operand has type \"" + left_type + "\"");
        if (right_type != "Int")
            print_error_message(filename, line, "less or equal operator only supports \"Int\" values, but the operand has type \"" + right_type + "\"");

        return "Bool";
    }
    else if (type == "no_expr")
    {
        return target_type;
    }
    return "unknown";
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

size_t check_expressions(const std::string &filename, const Features &features, const std::stack<Class_> &features_table, const Graph &graph)
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
            auto line = method->line_number;
            auto unrolled = unroll_stack(features_table);
            auto new_frame = formals_to_features(method->formals);
            if (new_frame)
            {
                for (int i = 0; i < new_frame->len(); i++)
                {
                    auto formal = new_frame->nth(i);
                    auto attr = static_cast<attr_class *>(formal);
                    auto type_decl = std::string(attr->type_decl->get_string());
                    auto attr_name = std::string(attr->name->get_string());
                    if (attr_name == "self")
                        print_error_message(filename, line, "using \"self\" as name of formal parameter");
                    if (graph.find(GraphNode(type_decl)) == nullptr)
                        print_error_message(filename, line, "using undefined type \"" + type_decl + "\"");
                    unrolled.insert(unrolled.begin(), formal);
                }
            }
            std::string return_type = method->return_type->get_string();

            auto method_name = std::string(method->name->get_string());
            auto expression_type = parse_expression(filename, method_name, unrolled, method->expr, return_type, graph, features_table);
            if (return_type != expression_type && return_type != "SELF_TYPE")
                print_error_message(filename, line, "expression has type \"" + expression_type + "\" and method \"" + method_name + "\" has type \"" + return_type + "\"");
        }
        else
        {
            auto attr = static_cast<attr_class *>(current_feature);
            auto unrolled = unroll_stack(features_table);
            std::string type_decl = attr->type_decl->get_string();
            auto attr_name = std::string(attr->name->get_string());
            auto expression_type = parse_expression(filename, attr_name, unrolled, attr->init, type_decl, graph, features_table);
            auto line = attr->line_number;
            if (attr_name == "self")
                print_error_message(filename, line, "using \"self\" as name of attribute");
            if (type_decl != expression_type)
                print_error_message(filename, line, "expression has type \"" + expression_type + "\" and attr \"" + attr_name + "\" has type \"" + type_decl + "\"");
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

        faults_count += check_expressions(current_class_filename, current_class_features, features_table, *this);

        for (auto &kid : vertex->kids)
            stack_.push(kid);
    }
    faults_count += check_main_class();
    return faults_count;
}