// Copyright 2021 Roger Peralta Aranibar
#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include "persistence/partial_directed_graph.hpp"
#include "pointer_machine/directed_graph.hpp"

void test_directed_graph() {
  std::cout << "Partial Directed Graph"<< "\n";
  // Create a DirectedGraph, a maximum of 5 edge for each node and the root with
  // the starting value of 1.
  ADE::PointerMachine::DirectedGraph<int, ADE::PointerMachine::Node<int>>my_graph(1, 5);
  ADE::Persistence::PartialDirectedGraph<int,ADE::Persistence::PartialNode<int>>my_graph2(10,3,3);
  // We can obtain the root and print their value.
  ADE::PointerMachine::Node<int>* root_ptr = my_graph.get_root_ptr();
  std::cout << "Root Value: " << root_ptr->get_data() << "\n";

  ADE::Persistence::PartialNode<int>* root_ptr_2 = my_graph2.get_root_ptr(0);
  std::cout<<"aea manin: "<<root_ptr_2->get_data(0)<<"\n";
  // Also, we can insert a new vertex passing the new Node value, and the index
  // of the pointer that will be used to point the inserted node.
  my_graph.get_root().insert_vertex(0, 2);

  std::cout << "Inserted Value: " << my_graph.get_root()[0].get_data() << "\n";

  // Likewise, the method to insert a new vertex, returns a reference of the
  // new vertex inserted.
  ADE::PointerMachine::Node<int>* other_inserted_node_ptr =
      my_graph.get_root()[0].insert_vertex(1, 3);

  std::cout << "Inserted Value: " << my_graph.get_root()[0][1].get_data()
            << "\n";

  // To add an edge between two vertex, we call the function update edge from
  // the startin Node with the index id that will link both nodes.
  my_graph.get_root().update_edge(1, other_inserted_node_ptr);

  std::cout << "Following other Edge: " << my_graph.get_root()[1].get_data()
            << "\n";
}

int main() {
  test_directed_graph();

  return 0;
}
