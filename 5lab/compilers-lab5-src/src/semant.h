#pragma once

#include "inheritance_graph.h"

#include <unordered_map>
#include <set>

extern std::vector<Program> ast_roots;
extern size_t faults_attend;

size_t find_class_redefinitions();
Graph make_inheritance_graph();
std::set<std::string> make_types_table();
void semant();