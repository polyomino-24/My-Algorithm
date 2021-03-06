#include "../header.hpp"

template<typename T> class segtree {
private:
    int n,sz;
    vector<pair<T, int> > node;
public:
    void resize(vector<T>& v){
        sz = (int)v.size();
        n = 1;
        while(n < sz){
            n *= 2;
        }
        node.resize(2*n);
        for(int i = 0; i < sz; i++){
            node[i+n] = make_pair(v[i], i);
        }
        for(int i=n-1; i>=1; i--){
            node[i] = min(node[2*i], node[2*i+1]);
        }
    }
    void update(int k, T a)
    {
    	node[k+=n] = make_pair(a, k);
    	while(k>>=1){
            node[k] = min(node[2*k], node[2*k+1]);
    	}
    }
    pair<T, int> query(int a,int b)
    {
        pair<T, int> res1 = make_pair(numeric_limits<T>::max(), -1);
        pair<T, int> res2 = make_pair(numeric_limits<T>::max(), -1);
        a += n, b += n;
        while(a != b){
            if(a % 2) res1 = min(res1, node[a++]);
            if(b % 2) res2 = min(res2, node[--b]);
            a >>= 1, b >>= 1;
        }
        return min(res1, res2);
    }
};

class LCA{
public:
    int V;
    vector<vector<int> > G;
    vector<int> ord,depth,id;
    segtree<int> st;
    LCA(int node_size) : V(node_size), G(V), depth(V), id(V, -1){}
    void add_edge(int from,int to){
        G[from].push_back(to),G[to].push_back(from);
    }
    void dfs(int u,int p,int k){
        id[u] = (int)ord.size();
        ord.push_back(u);
        depth[u] = k;
        for(int v : G[u]){
            if(v != p){
                dfs(v,u,k+1);
                ord.push_back(u);
            }
        }
    }
    void build(){
        ord.reserve(2*V-2);
        for(int i = 0; i < V; i++){
            if(id[i] < 0){
                dfs(i,-1,0);
            }
        }
        vector<int> stvec(2*V-2);
    	for(int i = 0; i < 2*V-2; i++){
    		stvec[i] = depth[ord[i]];
    	}
        st.resize(stvec);
    }
    int solve(int u,int v){
        return ord[st.query(min(id[u],id[v]),max(id[u],id[v])+1).second];
    }
    int dist(int u,int v){
        int lca = solve(u,v);
        return depth[u] + depth[v] - 2*depth[lca];
    }
    int construct_virtual_tree(vector<int>& ver_list, vector<int>& mapping, vector<vector<int> >& graph);
};

// ver_list 内の頂点から構成されるグラフ graph の virtual tree を構築し, その root を返す.
// mapping: 元の頂点番号 → graph の頂点番号(ver_list の逆変換) も同時に計算
// (元のグラフの頂点数以上の長さの vector を引数 mapping として与える)
int LCA::construct_virtual_tree(vector<int>& ver_list, vector<int>& mapping, vector<vector<int> >& graph){
    const int n = (int)ver_list.size();
    graph.resize(n);
    sort(ver_list.begin(), ver_list.end(), [&](const int a, const int b){ return id[a] < id[b]; });
    stack<int> st;
    st.push(ver_list[0]), mapping[ver_list[0]] = 0;
    int id = n;
    for(int i = 0; i < n-1; ++i){
        const int u = solve(ver_list[i], ver_list[i+1]);
        if(u != ver_list[i]){
            int mapped_ver = mapping[st.top()];
            while(true){
                st.pop();
                if(st.empty() || depth[u] >= depth[st.top()]) break;
                const int tmp = mapping[st.top()];
                graph[tmp].push_back(mapped_ver), mapped_ver = tmp;
            }
            if(st.empty() || st.top() != u){
                st.push(u), ver_list.push_back(u);
                graph.push_back({mapped_ver});
                mapping[u] = id++;
            }else{
                graph[mapping[u]].push_back(mapped_ver);
            }
        }
        st.push(ver_list[i+1]), mapping[ver_list[i+1]] = i+1;
    }
    int mapped_ver = ((st.size() > 1) ? mapping[st.top()] : -1);
    while(st.size() > 1){
        st.pop();
        const int tmp = mapping[st.top()];
        graph[tmp].push_back(mapped_ver), mapped_ver = tmp;
    }
    return st.top();
}
