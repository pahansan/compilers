#pragma once

#include "inheritance_graph.h"

#include <unordered_map>
#include <set>

std::unordered_map<GraphNode, size_t> count_classes();
Graph make_inheritance_graph();
std::set<std::string> make_types_table();
