// Brankovic et al. 2015 を参照
#include "header.hpp"
// #define rep(i,n) for(int i=0;i<(int)n;++i)
// #define show(x) std::cout<<#x<<" = "<<(x)<<std::endl
// #define spair(p) std::cout<<#p<<": "<<p.first<<" "<<p.second<<std::endl
// #define svec(v) std::cout<<#v<<":";for(int kbrni=0;kbrni<(int)v.size();kbrni++)std::cout<<" "<<v[kbrni];std::cout<<std::endl

template<typename T> class RMQ
{
private:
    struct RMQNode
    {
        int id;
        T val;    //インデックス, 値
        int par, left, right;   //親,　左の子, 右の子
    };
    std::vector<RMQNode> tree;
    std::vector<T> arr;
    std::vector<int> euler_tour, depth, diff, visit_id;
    int node_size, root, arr_len;
    bool max_query;
    std::vector<int> block_arr, diff_bit;   //各ブロックで深さが最小になるようなインデックス　各ブロックのdiffの情報をビットに詰めたもの
    std::vector<std::vector<int> > sparse_table;  //i番目から長さ2^kの区間に含まれるdepthのうち最小のインデックス
    std::vector<std::vector<std::vector<int> > > table_lookup;
    int block_size, block_rem, block_cnt, log_block_cnt;

    void make_cartesian_tree();
    void make_euler_tour(int cur_node, int& id, int cur_depth);
    void make_block_arr();
    void make_sparse_table();
    void make_diff_bit();
    void make_table_lookup();
    std::pair<int, T> PM_RMQ(int st, int ed);
public:
    void build(std::vector<T>& arg1, bool max_query_=false);    //trueのときrange_max_queryを表す
    std::pair<int, T> query(int st, int ed); // (最小(大)値を達成するインデックス, 値)
};

template<typename T>
void RMQ<T>::build(std::vector<T>& arg1, bool max_query_){
    node_size = (int)arg1.size();
    assert(node_size >= 2);
    arr.resize(node_size, 0);
    max_query = max_query_;
    if(max_query){
        for(int i = 0; i < node_size; i++){
            arr[i] = -arg1[i];
        }
    }else{
        for(int i = 0; i < node_size; i++){
            arr[i] = arg1[i];
        }
    }
    make_cartesian_tree();
    euler_tour.resize(2 * node_size - 1, -1), depth.resize(2 * node_size - 1, -1);
    diff.resize(2 * node_size - 2, -1), visit_id.resize(node_size, -1);
    int val = 0;
    make_euler_tour(root, val, 0);
    for(int i = 0;i < 2 * node_size - 2; i++){
        diff[i] = depth[i+1] - depth[i];
    }
    //±RMQの実装
    arr_len = 2 * node_size - 1;
    make_block_arr();
    make_sparse_table();
    make_diff_bit();
    make_table_lookup();
}

template<typename T>
void RMQ<T>::make_cartesian_tree()
{
    tree.resize(node_size);
    for(int i = 0; i < node_size; i++){
        tree[i] = (RMQNode){i, arr[i], -1, -1, -1};
    }
    std::stack<std::pair<int, T> > st;    //インデックス,値
    st.push(std::make_pair(0,arr[0]));
    root = 0;
    for(int i = 1; i < node_size ; i++){
        int buff;
        while(1){
            //iが根となる場合
            if(st.empty()){
                st.push(std::make_pair(i, arr[i]));
                tree[i].left = buff;
                tree[buff].par = i;
                root = i;
                break;
            }
            std::pair<int, T> top = st.top();
            if(top.second <= arr[i]){
                tree[i].par = top.first; //iの親を変更
                int nd = tree[top.first].right;  //iの親の右下のノード
                //iの子となるものが存在する場合
                if(nd != -1){
                    tree[i].left = nd;  //iの親の右下のノードをiの左の子とする
                    tree[nd].par = i; //iの親の右下のノードの親をiとする
                }
                tree[top.first].right = i;  //iの親の右下の子をiとする
                st.push(std::make_pair(i,arr[i]));
                break;
            }
            buff = top.first;   //最後にpopされたものを暗記
            st.pop();
        }
    }
}

template<typename T>
void RMQ<T>::make_euler_tour(int cur_node, int& id, int cur_depth)
{
    visit_id[cur_node] = id;
    euler_tour[id] = cur_node;
    depth[id++] = cur_depth;
    if(tree[cur_node].left >= 0){
        make_euler_tour(tree[cur_node].left, id, cur_depth+1);
        euler_tour[id] = cur_node;
        depth[id++] = cur_depth;
    }
    if(tree[cur_node].right >= 0){
        make_euler_tour(tree[cur_node].right, id, cur_depth+1);
        euler_tour[id] = cur_node;
        depth[id++] = cur_depth;
    }
}

template<typename T>
void RMQ<T>::make_block_arr()
{
    block_size = ceil(log2(arr_len)/2);
    block_cnt = (arr_len - 1) / block_size + 1;
    block_rem = arr_len % block_size;
    log_block_cnt = ceil(log2(block_cnt)) + 1;
    block_arr.resize(block_cnt);
    for(int i = 0; i < block_cnt; i++){
        int mn = numeric_limits<int>::max();
        int mn_id = -1;
        for(int j = 0; j < block_size; j++){
            int now_id = i * block_size + j;
            if(now_id >= arr_len){
                break;
            }
            if(depth[now_id] < mn){
                mn = depth[now_id];
                mn_id = now_id;
            }
        }
        block_arr[i] = mn_id;
    }
}

template<typename T>
void RMQ<T>::make_sparse_table()
{
    sparse_table.resize(block_cnt, std::vector<int>(log_block_cnt, -1));
    for(int i = 0; i < block_cnt; i++){
        sparse_table[i][0] = block_arr[i];
    }
    for(int j = 0; j < log_block_cnt - 1; j++){
        for(int i = 0; i < block_cnt; i++){
            if(i + (1 << j) >= block_cnt){
                sparse_table[i][j + 1] = sparse_table[i][j];
            }else{
                if(depth[sparse_table[i][j]] <= depth[sparse_table[i + (1 << j)][j]]){
                    sparse_table[i][j + 1] = sparse_table[i][j];
                }else{
                    sparse_table[i][j + 1] = sparse_table[i + (1 << j)][j];
                }
            }
        }
    }
}

template<typename T>
void RMQ<T>::make_diff_bit()
{
    diff_bit.resize(block_cnt, -1);
    for(int i = 0; i < block_cnt; i++){
        int num = 0;
        for(int j = 0; j < block_size - 1; j++){
            int now_id = i * block_size + j;
            if(now_id >= arr_len){
                break;
            }
            if(diff[now_id] > 0){
                num += (1 << (block_size - j - 2));
            }
        }
        diff_bit[i] = num;
    }
}

template<typename T>
void RMQ<T>::make_table_lookup()
{
    table_lookup.resize((1 << block_size), std::vector<std::vector<int> >(block_size + 1, std::vector<int>(block_size + 1, 0)));
    //0は減少,1は増加
    for(int i = 0; i < (1 << block_size); i++){
        std::vector<int> vec(block_size - 1, -1);
        for(int j = 0; j < block_size; j++){
            if(i & (1 << (block_size - j - 2))){
                vec[j] = 1;
            }
        }
        for(int j = 0; j < block_size; j++){
            int nw = 0, mn = 0, mn_id = j;
            table_lookup[i][j][j+1] = j;
            for(int k = j + 2; k <= block_size; k++){
                nw += vec[k-2];
                if(nw < mn){
                    mn = nw;
                    mn_id = k-1;
                }
                table_lookup[i][j][k] = mn_id;
            }
        }
    }
}

//インデックスと値のpair
template<typename T>
std::pair<int, T> RMQ<T>::PM_RMQ(int st, int ed)
{
    int st_block_id = (st == 0)?0:((st - 1) / block_size + 1);  //ブロック区間の開始
    int ed_block_id = ed / block_size - 1;    //ブロック区間の終了
    int st_id = st / block_size;
    int ed_id = ed / block_size;
    int st_rem = st % block_size;
    int ed_rem = ed % block_size;
    if(ed_block_id - st_block_id < 0){    //間にブロック区間がひとつもない場合
        if(st_id == ed_id){ //同じブロック区間に存在する場合
            int id_kari = block_size * st_id + table_lookup[diff_bit[st_id]][st_rem][ed_rem + 1];
            return std::make_pair(euler_tour[id_kari], arr[euler_tour[id_kari]]);
        }else{
            int cand1 = block_size * st_id + table_lookup[diff_bit[st_id]][st_rem][block_size];
            int cand2 = block_size * ed_id + table_lookup[diff_bit[ed_id]][0][ed_rem + 1];
            if(depth[cand1] <= depth[cand2]){
                return std::make_pair(euler_tour[cand1], arr[euler_tour[cand1]]);
            }else{
                return std::make_pair(euler_tour[cand2], arr[euler_tour[cand2]]);
            }
        }
    }else{  //間にブロックっ区間が存在する場合
        int num = floor(log2(ed_block_id - st_block_id + 1));
        int cand1 = sparse_table[st_block_id][num];
        int cand2 = sparse_table[ed_block_id - (1 << num) + 1][num];
        int c1,c2;
        if(depth[cand1] <= depth[cand2]){
            c1 = cand1;
        }else{
            c1 = cand2;
        }
        int cand3 = block_size * st_id + table_lookup[diff_bit[st_id]][st_rem][block_size];
        int cand4 = block_size * ed_id + table_lookup[diff_bit[ed_id]][0][ed_rem + 1];
        if(depth[cand3] <= depth[cand4]){
            c2 = cand3;
        }else{
            c2 = cand4;
        }
        if(depth[c1] <= depth[c2]){
            return std::make_pair(euler_tour[c1], arr[euler_tour[c1]]);
        }else{
            return std::make_pair(euler_tour[c2], arr[euler_tour[c2]]);
        }
    }
}

template<typename T>
std::pair<int, T> RMQ<T>::query(int st, int ed)
{
    assert(st < ed);
    std::pair<int, T> res = PM_RMQ(std::min(visit_id[st], visit_id[ed-1]), std::max(visit_id[st], visit_id[ed-1]));
    if(max_query) res.second = -res.second;
    return res;
}

void graph_input(int edge_size, std::vector<std::vector<int> >& graph, std::vector<std::vector<int> >& rgraph)
{
    for(int i = 0; i < edge_size; i++){
        int arg1, arg2;
        scanf("%d%d", &arg1, &arg2);
        graph[arg1].push_back(arg2);
        rgraph[arg2].push_back(arg1);
    }
}

void recursive_topological_sort(int u, int& index, std::vector<int>& visit, std::vector<int>& ord,
     std::vector<int>& ordD, std::vector<std::vector<int> >& graph)
{
    if(visit[u] == 1){
        return;
    }
    visit[u] = 0;
	for(int nx : graph[u]){
        recursive_topological_sort(nx, index, visit, ord, ordD, graph);
	}
    visit[u] = 1;
    ord[index] = u;
	ordD[u] = index--;
}

void topological_sort(int node_size, std::vector<int>& ord, std::vector<int>& ordD, std::vector<std::vector<int> >& graph)
{
    //与えられたグラフがconnected かつ DAGであることを仮定
    std::vector<int> visit(node_size, -1);
    int index = node_size - 1;
    for(int i = 0; i < node_size; i++){
        if(visit[i] == -1){
            recursive_topological_sort(i, index, visit, ord, ordD, graph);
        }
    }
}

void prepare_superbubble(int node_size, std::vector<bool>& ent, std::vector<bool>& exi,std::vector<int>& ordD, std::vector<int>& outparent,
     std::vector<int>& outchild, std::vector<std::vector<int> >& graph, std::vector<std::vector<int> >& rgraph)
{
    for(int i = 0; i < node_size; i++){
        bool ok = false;
        for(int j = 0; j < (int)rgraph[i].size(); j++){
            if((int)graph[rgraph[i][j]].size() == 1){
                ok = true;
                break;
            }
        }
        exi[i] = ok;
        ok = false;
        for(int j = 0; j < (int)graph[i].size(); j++){
            if((int)rgraph[graph[i][j]].size() == 1){
                ok = true;
                break;
            }
        }
        ent[i] = ok;
    }
    for(int i = 0; i < node_size; i++){
        int mn_ordD = node_size;
        for(int j = 0; j < (int)rgraph[i].size(); j++){
            mn_ordD = std::min(mn_ordD, ordD[rgraph[i][j]]);
        }
        outparent[ordD[i]] = mn_ordD;
        int mx_ordD = -1;
        for(int j = 0; j < (int)graph[i].size(); j++){
            mx_ordD = std::max(mx_ordD, ordD[graph[i][j]]);
        }
        outchild[ordD[i]] = mx_ordD;
    }
}

int validate_superbubble(int st_node, int ed_node, std::vector<bool>& ent, std::vector<int>& ord, std::vector<int>& ordD,
     std::vector<int>& prev_ent, RMQ<int>& parent_rmq, RMQ<int>& child_rmq)
{
    int st_id = ordD[st_node];
    int ed_id = ordD[ed_node];
    int mx_child = child_rmq.query(st_id, ed_id).second;
    int mn_parent = parent_rmq.query(st_id + 1, ed_id + 1).second;
    if(mx_child != ed_id){
        return -1;
    }
    if(mn_parent == st_id){
        return st_node;
    }else if(ent[ord[mn_parent]]){
        return ord[mn_parent];
    }else{
        return prev_ent[ord[mn_parent]];
    }
}

void report_superbubble(std::list<std::pair<int, int> >& cand, int st_node, int ed_node, std::vector<bool>& ent, std::vector<bool>& exi,
     std::vector<int>& ord, std::vector<int>& ordD, std::vector<int>& prev_ent, std::vector<int>& next_ent, std::vector<int>& alt_ent,
      RMQ<int>& parent_rmq, RMQ<int>& child_rmq, std::vector<std::pair<int, int> >& superbubble_set)
     //ex_nodeで終わるsuperbubbleを存在すれば報告する
{
    if(st_node < 0 || ed_node < 0 || ordD[st_node] >= ordD[ed_node]){
        cand.pop_back();
        return;
    }
    int prevent_node = prev_ent[ed_node];
    int valid = -1;
    while(ordD[prevent_node] >= ordD[st_node]){
        valid = validate_superbubble(prevent_node, ed_node, ent, ord, ordD, prev_ent, parent_rmq, child_rmq);
        if(valid == prevent_node || valid == alt_ent[prevent_node] || valid == -1){
            break;
        }
        alt_ent[prevent_node] = valid;
        prevent_node = valid;
    }
    cand.pop_back();
    if(valid == prevent_node){
        superbubble_set.push_back(std::make_pair(prevent_node, ed_node));
        while((cand.back()).first != prevent_node){
            if((cand.back()).second == 1){
                //nestされたsuperbubbleについて調べる
                report_superbubble(cand, next_ent[prevent_node], (cand.back()).first, ent, exi, ord, ordD, prev_ent, next_ent, alt_ent,
                 parent_rmq, child_rmq, superbubble_set);
            }else{
                cand.pop_back();
            }
        }
    }
}

void superbubble(int node_size, std::vector<std::vector<int> >& graph, std::vector<std::vector<int> >& rgraph,
 std::vector<std::pair<int, int> >& superbubble_set)
{
    std::vector<int> ord(node_size, -1);
    std::vector<int> ordD(node_size, -1);
    topological_sort(node_size, ord, ordD, graph);
    std::vector<bool> ent(node_size, false), exi(node_size, false);
    std::vector<int> outparent(node_size, -1), outchild(node_size, -1);
    std::vector<int> alt_ent(node_size, -1), prev_ent(node_size, -1), next_node(node_size, -1);
    prepare_superbubble(node_size, ent, exi, ordD, outparent, outchild, graph, rgraph);
    std::list<std::pair<int, int> > cand;   //(頂点の値,entかexiのフラグ)
    std::vector<int> next_ent(node_size, -1);
    RMQ<int> parent_rmq, child_rmq;
    parent_rmq.build(outparent, 0), child_rmq.build(outchild, 1);
    int prev_entrance = -1;
    int next_entrance = -1;
    for(int ver : ord){
        alt_ent[ver] = -1;
        prev_ent[ver] = prev_entrance;
        if(exi[ver]){
            cand.push_back(std::make_pair(ver, 1));
        }
        if(ent[ver]){
            cand.push_back(std::make_pair(ver, 0));
            prev_entrance = ver;
            if(next_entrance >= 0){
                next_ent[next_entrance] = ver;
            }
            next_entrance = ver;
        }
    }
    while(!cand.empty()){
        if((cand.back()).second == 0){
            cand.pop_back();
        }else{
            report_superbubble(cand, (cand.front()).first, (cand.back()).first, ent, exi, ord, ordD, prev_ent, next_ent, alt_ent,
             parent_rmq, child_rmq, superbubble_set);
        }
    }
}

int main()
{
    int node_size, edge_size;
	scanf("%d%d", &node_size, &edge_size);
    std::vector<std::vector<int> > graph(node_size);
    std::vector<std::vector<int> > rgraph(node_size);
    graph_input(edge_size, graph, rgraph);
    std::vector<std::pair<int, int> > superbubble_set;
    superbubble(node_size, graph, rgraph, superbubble_set);
    for(int i = 0; i < (int)superbubble_set.size(); i++){
        printf("%d %d\n",superbubble_set[i].first, superbubble_set[i].second);
    }
}
