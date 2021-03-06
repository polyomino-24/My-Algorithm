#include "../header.hpp"

// 木のパスについてのクエリについて答えるデータ構造
// 計算量:O(log^2(n))
// query は例えば segtree<int> seg を用意して
// 範囲更新なら hl.query(q, r, [&](int l, int r){ seg.range(l, r, val); });
// 区間総和なら hl.query(q, r, [&](int l, int r){ ans += seg.query(l, r); });
// のように呼び出す

class HLdecomposition{
public:
    int V;
    vector<vector<int> > G;
    vector<int> stsize, parent, pathtop, in;
    void BuildStsize(int u, int p){
        stsize[u] = 1, parent[u] = p;
        for(int& v : G[u]){
            if(v == p) continue;
            BuildStsize(v, u);
            stsize[u] += stsize[v];
            if(stsize[v] > stsize[G[u][0]]) swap(v, G[u][0]);
        }
    }
    void BuildPath(int u, int p, int& tm){
        in[u] = tm++;
        for(int v : G[u]){
            if(v == p) continue;
            pathtop[v] = (v == G[u][0] ? pathtop[u] : v);
            BuildPath(v, u, tm);
        }
    }

public:
    void add_edge(int u, int v){
        G[u].push_back(v), G[v].push_back(u);
    }
    void build(){
        int tm = 0;
        BuildStsize(0, -1);
        pathtop[0] = 0;
        BuildPath(0, -1, tm);
    }
    inline int get(int a){
        return in[a];
    }
    int lca(int a, int b){
        int pa = pathtop[a], pb = pathtop[b];
        while(pathtop[a] != pathtop[b]){
            if(in[pa] > in[pb]){
                a = parent[pa], pa = pathtop[a];
            }else{
                b = parent[pb], pb = pathtop[b];
            }
        }
        if(in[a] > in[b]) swap(a, b);
        return a;
    }
    void query(int a, int b, const function< void(int, int) > &func){
        int pa = pathtop[a], pb = pathtop[b];
        while(pathtop[a] != pathtop[b]){
            if(in[pa] > in[pb]){
                func(in[pa], in[a] + 1);
                a = parent[pa], pa = pathtop[a];
            }else{
                func(in[pb], in[b] + 1);
                b = parent[pb], pb = pathtop[b];
            }
        }
        if(in[a] > in[b]) swap(a, b);
        func(in[a], in[b] + 1);
    }
    HLdecomposition(int node_size) : V(node_size), G(V), stsize(V, 0),
        parent(V, -1), pathtop(V, -1), in(V, -1){}
};
