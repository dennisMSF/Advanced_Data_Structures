// Copyright 2021 Roger Peralta Aranibar
#ifndef SOURCE_PERSISTENCE_PARTIAL_DIRECTED_GRAPH_HPP_
#define SOURCE_PERSISTENCE_PARTIAL_DIRECTED_GRAPH_HPP_

#include <utility>
#include <map>
#include <vector>

namespace ADE {
namespace Persistence {
template <typename Type, typename Node>
class PartialDirectedGraph;
template <typename Type>
class PartialNode {
 public:
  typedef Type data_type;

  PartialNode():data_(nullptr), forward_(nullptr),backward_(nullptr), out_ptrs_size_(0), in_ptrs_size_(0) {}

  PartialNode(data_type const& data, std::size_t const& out_ptrs_size,std::size_t const& in_ptrs_size) 
  :data_(new data_type(data)), out_ptrs_size_(out_ptrs_size),in_ptrs_size_(in_ptrs_size){
    backward_ = new PartialNode<Type>*[in_ptrs_size]();
    forward_ = new PartialNode<Type>*[out_ptrs_size]();

  }

  data_type get_data(unsigned int version) { 
    if(modificaciones.empty()){
      return *data_;
    } 
    else{
      for(int i=0; i<versiones.size();i++){
        for (auto buscar : versiones[i]){
          if(buscar.first == version){
            return buscar.second.first;
          }
        }
      }
    }
  }

  bool set_data(data_type const& data) { 
    if(modificaciones.empty()){
      data(new data_type(data_));
      return true;
    }
    else if(modificaciones.size()< 2*in_ptrs_size_){
      std::pair<int,std::pair<int,PartialNode**>> principal;
      std::pair<int,PartialNode**> secundario;
      secundario = std::make_pair(data,nullptr);
      principal = std::make_pair(version_actual+1,secundario);
      modificaciones.insert(principal);
      version_actual++;
      return 1;
    }
    else{
      PartialNode* new_partial_node= new PartialNode(data,out_ptrs_size_,in_ptrs_size_);
      for(int i=0;i<out_ptrs_size_;i++){
        new_partial_node->forward_[i]=forward_[i];
      }
      for (size_t i = 0; i < in_ptrs_size_; i++){
        for (size_t j = 0; j < out_ptrs_size_; j++){
          if(backward_[i]->forwrad_[j].get_data(0)== *data_){
            backward_[i]->forwrad_[j] = new_partial_node;
          }
        }
        
      }
    }
    return 0;
  }
  bool set_pointer(PartialNode* ptr_mod_){
    if(modificaciones.size()<2*in_ptrs_size_){
      std::pair<int,std::pair<int,PartialNode**>> principal;
      std::pair<int,PartialNode**> secundario;
      secundario = std::make_pair(0,ptr_mod_);
      principal = std::make_pair(version_actual+1,secundario);
      modificaciones.insert(principal);
      version_actual++;
      return 1;
    }
    else{
      PartialNode* new_partial_node= new PartialNode();
      for(int i=0;i<out_ptrs_size_;i++){
        new_partial_node->forward_[i]=forward_[i];
      }
      for (size_t i = 0; i < in_ptrs_size_; i++){
        for (size_t j = 0; j < out_ptrs_size_; j++){
          if(backward_[i]->forwrad_[j].get_data(0)== *data_){
            backward_[i]->forwrad_[j] = new_partial_node;
          }
        }
        
      }
      
    }
    return 0;
  }

  PartialNode* insert_vertex(std::size_t const& position,data_type const& data) {
    PartialNode* next_node_ptr = forward_[position];
    PartialNode* back_node_ptr = backward_[position];
    PartialNode* new_node = new PartialNode(data, out_ptrs_size_,in_ptrs_size_);

    new_node->forward_[position] = next_node_ptr;

    forward_[position] = new_node;
    
    backward_[position]->forward_[position] = new_node;

    new_node->backward_[position] = back_node_ptr;


    
    return forward_[position];
  }

  bool update_edge(std::size_t const& position, PartialNode* v) { 
    v->backward_[position] = forward_[position]->backward_[position];
    forward_[position] = v;
    return true; }

  PartialNode& operator[](std::pair<std::size_t, unsigned int> const& version) const {
    for(int i=0; i<versiones.size();i++){
        for (auto buscar : versiones[i]){
          if(buscar.first == version.second){
            return buscar.second.first;
          }
        }
      }
  }


  data_type* data_;
  std::size_t out_ptrs_size_;
  PartialNode** forward_;

  std::size_t in_ptrs_size_;
  std::size_t current_modifications_size_;
  std::size_t current_back_pointer_size_;

  std::map<int,std::pair<int,PartialNode**>> modificaciones;
  std::vector<std::map<int,std::pair<int,PartialNode**>>> versiones;
  int version_actual=1;//2p
  int in_ptr_counter=0;
  PartialNode** backward_;

 
};

template <typename Type, typename Node>
class PartialDirectedGraph {
 public:
  typedef Type data_type;

  PartialDirectedGraph(data_type const data, std::size_t const& out_ptrs_size,std::size_t const& in_ptrs_size) 
  :root_ptr_(new Node(data,out_ptrs_size,in_ptrs_size)),in_ptrs_size_(in_ptrs_size), out_ptrs_size_(out_ptrs_size){}

  Node* get_root_ptr(unsigned int const& version) { 
    return root_ptr_; }

  Node get_root(unsigned int const& version) { return *root_ptr_; }

  Node* insert_vertex(data_type const data, Node* u, std::size_t position) {
    Node* new_partial_node= new Node(data,out_ptrs_size_,in_ptrs_size_);
    Node* next_node_ptr = u->forward_[position];
    Node* back_node_ptr = u->backward_[position];
    Node* new_node = new Node(data, out_ptrs_size_,in_ptrs_size_);

    new_node->forward_[position] = next_node_ptr;

    u->forward_[position] = new_node;
    
    u->backward_[position]->forward_[position] = new_node;

    new_node->backward_[position] = back_node_ptr;
    ++(*current_version_);
    return nullptr;
  }

  bool add_edge(Node* u, Node* v, std::size_t position) {
    if(u->in_ptr_counter == in_ptrs_size_){
      std::cout<<"no se puede recibir mas punteros"<<std::endl;
    }
    u->forward_[position] = v;
    v->backward[position] = u;
    
    ++(*current_version_);
    return true;
  }

 protected:
  unsigned int* current_version_;
  std::size_t in_ptrs_size_;

  Node* root_ptr_;
  std::size_t out_ptrs_size_;
};

}  // namespace Persistence
}  // namespace ADE

#endif  // SOURCE_PERSISTENCE_PARTIAL_DIRECTED_GRAPH_HPP_
