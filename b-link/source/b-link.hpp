// Copyright
#ifndef SOURCE_B_LINK_HPP_
#define SOURCE_B_LINK_HPP_

#include <utility>

namespace EDA {
  namespace Concurrent {
    template <std::size_t B, typename Type>
    class BLinkTree {
    public:
      typedef Type data_type;
      class Bplusnode{
      public:  
        bool leaf;
        Type* key;
        Type size;
        Bplusnode** ptr;
        Bplusnode(): key(new Type[B]), ptr(new Bplusnode*[B+1]){};
      };
      Bplusnode* root;
      void insert_nodo_interno(Type value,Bplusnode* ayuda,Bplusnode* child){
        if(ayuda->size < B){
          int i=0;
          while(value > ayuda->key[i] && i < ayuda->size) i++;
          for(int j = ayuda->size; j > i; j--) ayuda->key[j]=ayuda->key[j-1];
          for(int j = ayuda->size+1; j > i+1; j--) ayuda->ptr[j]=ayuda->ptr[j-1];

          ayuda->key[i]=value;
          ayuda->size++;
          ayuda->ptr[i+1]=child;
        }
        else{
          Bplusnode* ninternal = new Bplusnode;
          int virtualkey[B+1];
          Bplusnode* virtualptr[B+2];
          for(int i=0;i < B;i++) virtualkey[i]= ayuda->key[i];
          for(int i=0;i<B+1;i++) virtualptr[i]=ayuda->ptr[i];

          int i=0,j;
          while (value > virtualkey[i] && i < B) i++;

          for(int j = B; j > i; j--) virtualkey[j]= virtualkey[j-1];

          virtualkey[i]=value;
          for(int j = B+1; j > i+1; j--) virtualptr[j]= virtualptr[j-1];

          virtualptr[i+1]=child;
          ninternal->leaf=false;
          ayuda->size=(B+1)/2;
          ninternal->size=B - (B+1)/2;
          for(i=0,j=ayuda->size+1;i<ninternal->size;i++,j++) ninternal->key[i]=virtualkey[j];

          for(i=0,j=ayuda->size+1;i<ninternal->size+1;i++,j++) ninternal->ptr[i]=virtualptr[j];
          if(ayuda == root){
            Bplusnode* nroot = new Bplusnode;
            nroot->key[0]= ayuda->key[ayuda->size];
            nroot->ptr[0]=ayuda;
            nroot->ptr[1]=ninternal;
            nroot->leaf=false;
            nroot->size=1;
            root=nroot;
          }
          else{
            insert_nodo_interno(ayuda->key[ayuda->size],Buscar_padre(root,ayuda),ninternal);
          }
        }
      }
      Bplusnode* Buscar_padre(Bplusnode* ayuda, Bplusnode* child){
        Bplusnode* padre;
        if(ayuda->leaf || (ayuda->ptr[0])->leaf){
          return nullptr;
        }
        for(int i=0; i<ayuda->size+1;i++){
          if(ayuda->ptr[i]==child){
            padre=ayuda;
            return padre;
          }
          else{
            padre=Buscar_padre(ayuda->ptr[i],child);
            if(padre != nullptr){
              return padre;
            }
          }
        }
        return padre;
      }
      BLinkTree() {
        root = nullptr;
      }

      ~BLinkTree() {}

      std::size_t size() const {}

      bool empty() const {}

      bool search(const data_type& value) const {
        if (root == nullptr){
          return 0;
        }
        else{
          Bplusnode* ayuda = root;
          while(ayuda->leaf == false){
            for(int i=0;i<ayuda->size;i++){
              if(value < ayuda->key[i]){
                ayuda = ayuda->ptr[i];
                break;
              }
              if(i == ayuda->size-1){
                ayuda = ayuda->ptr[i+1];
                break;
              }
            }
          }
          for (int i=0;i<ayuda->size;i++){
            if(ayuda->key[i]==value){
              return 1;
            }
          }
        }
        return 0;
      }

      void insert(const data_type& value) {
        if(search(value))return;
        if(root == nullptr){
          root = new Bplusnode;
          root->key[0]=value;
          root->leaf=true;
          root->size=1;
          std::cout<<"elemento insertado exitosamente"<<std::endl;
        }
        else{
          Bplusnode* ayuda = root;
          Bplusnode* parent;
          while(ayuda->leaf == false){
            parent = ayuda;
            for(int i=0; i<ayuda->size;i++){
              if(value < ayuda->key[i]){
                ayuda = ayuda->ptr[i];
                break;
              }
              if(i== ayuda->size-1){
                ayuda = ayuda->ptr[i+1];
                break;
              }
            }
          }
          if(ayuda->size < B){
            int i=0;
            while (value > ayuda->key[i] && i < ayuda->size) i++;
            for(int j = ayuda->size; j > i; j--) ayuda->key[j]= ayuda->key[j-1];
            ayuda->key[i] = value;
            ayuda->size++;
            ayuda->ptr[ayuda->size]= ayuda->ptr[ayuda->size-1];
            ayuda->ptr[ayuda->size-1] = nullptr;
            std::cout<<"elemento insertado exitosamente"<<std::endl;
          }
          else{
            std::cout<<"elemento insertado exitosamente"<<std::endl;
            std::cout<<"overflow.... spliting child"<<std::endl;
            Bplusnode* nleaf = new Bplusnode;
            int virtual_node[B + 1];
            for(int i=0;i<B;i++) virtual_node[i]=ayuda->key[i];
            int j,i=0;
            while(value > virtual_node[i] && i < B) i++;
            for(int j = B; j > i; j--) virtual_node[j]=virtual_node[j-1];
            virtual_node[i]= value;
            nleaf->leaf = true;
            ayuda->size = (B+1)/2;
            nleaf->size = B + 1 - (B+1)/2;
            ayuda->ptr[ayuda->size]=nleaf;
            nleaf->ptr[nleaf->size]=ayuda->ptr[B];
            ayuda->ptr[B]=nullptr;
            for(int i=0; i<ayuda->size;i++) ayuda->key[i]=virtual_node[i];
            for(int i=0, j = ayuda->size; i<nleaf->size;i++,j++) nleaf->key[i]=virtual_node[j];
            if(ayuda == root){
              Bplusnode* nroot= new Bplusnode;
              nroot->key[0]=nleaf->key[0];
              nroot->ptr[0]= ayuda;
              nroot->ptr[1]=nleaf;
              nroot->leaf=false;
              nroot->size=1;
              root = nroot;
              std::cout<<"Nueva raiz creada\n";
            }
            else{
              insert_nodo_interno(nleaf->key[0],parent,nleaf);
            }
          }
        }
      }

      void remove(const data_type& value) {}

    private:
      data_type* data_;
    };

  }  // namespace Concurrent
}  // namespace EDA

#endif  // SOURCE_B_LINK_HPP_
