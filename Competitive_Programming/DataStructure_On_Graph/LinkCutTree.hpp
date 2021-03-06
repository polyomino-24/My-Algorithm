#include "../header.hpp"

class LinkCutTree {
public:
    struct node {
        node *left, *right, *par;
        node() : left(nullptr), right(nullptr), par(nullptr){}
        bool isRoot(){
            return (!par) || (par->left != this && par->right != this);
        }
    };

private:
    // 回転
    void rotate(node* u, bool right){
        node *p = u->par, *g = p->par;
        if(right){
            if((p->left = u->right)) u->right->par = p;
            u->right = p, p->par = u;
        }else{
            if((p->right = u->left)) u->left->par = p;
            u->left = p, p->par = u;
        }
        u->par = g;
        if(!g) return;
        if(g->left == p) g->left = u;
        if(g->right == p) g->right = u;
    }
    // u を splay 木の根にする
    void splay(node* u){
        while(!(u->isRoot())){
            node *p = u->par, *gp = p->par;
            if(p->isRoot()){ // zig
                rotate(u, (u == p->left));
            }else{
                bool flag = (u == p->left);
                if((u == p->left) == (p == gp->left)){ // zig-zig
                    rotate(p, flag), rotate(u, flag);
                }else{ // zig-zag
                    rotate(u, flag), rotate(u, !flag);
                }
            }
        }
    }
    node* access(node* u){
        node* last = nullptr;
        for(node* v = u; v; v = v->par){
            splay(v);
            v->right = last;
            last = v;
        }
        splay(u);
        return last;
    }
    bool connected(node* u, node* v){
        access(u), access(v);
        return u == v || u->par;
    }
    void link(node* u, node* v){
        access(u), u->par = v;
    }
    void cut(node* u){
        access(u);
        u->left->par = nullptr, u->left = nullptr;
    }
    node* lca(node* u, node* v){
        access(u);
        return access(v);
    }

public:
    const int V;
    node** arr;
    LinkCutTree(const int node_size) : V(node_size){
        arr = new node*[V];
        for(int i = 0; i < V; i++){
            arr[i] = new node();
        }
    }
    // ~LinkCutTree(){
    //     for(int i = 0; i < V; i++){
    //         delete arr[i];
    //     }
    //     delete[] arr;
    // }
    // id1 と id2 が同じ木(連結成分)に属するか
    bool connected(int id1, int id2){ return connected(arr[id1], arr[id2]); }
    // id1 を id2 の non-preferred edge にする(id1 が木の根であることを仮定している)
    void link(int id1, int id2){ return link(arr[id1], arr[id2]); }
    // id とその親の間の辺を削除する
    void cut(int id){ return cut(arr[id]); }
    // id1 と id2 の LCA を求める
    int lca(int id1, int id2){ return static_cast<size_t>(lca(arr[id1], arr[id2])-arr[0]); }
};
