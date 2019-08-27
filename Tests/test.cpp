#include <bits/stdc++.h>
#define ll long long
#define INF 1000000005
#define MOD 1000000007
#define EPS 1e-10
#define rep(i,n) for(int i=0;i<(int)(n);++i)
#define rrep(i,n) for(int i=(int)(n)-1;i>=0;--i)
#define srep(i,s,t) for(int i=(int)(s);i<(int)(t);++i)
#define each(a,b) for(auto& (a): (b))
#define all(v) (v).begin(),(v).end()
#define len(v) (int)(v).size()
#define zip(v) sort(all(v)),v.erase(unique(all(v)),v.end())
#define cmx(x,y) x=max(x,y)
#define cmn(x,y) x=min(x,y)
#define fi first
#define se second
#define pb push_back
#define show(x) cout<<#x<<" = "<<(x)<<endl
#define sar(a,n) {cout<<#a<<":";rep(pachico,n)cout<<" "<<a[pachico];cout<<endl;}

using namespace std;

template<typename S,typename T>auto&operator<<(ostream&o,pair<S,T>p){return o<<"{"<<p.fi<<","<<p.se<<"}";}
template<typename T>auto&operator<<(ostream&o,set<T>s){for(auto&e:s)o<<e<<" ";return o;}
template<typename S,typename T,typename U>
auto&operator<<(ostream&o,priority_queue<S,T,U>q){while(!q.empty())o<<q.top()<<" ",q.pop();return o;}
template<typename K,typename T>auto&operator<<(ostream&o,map<K,T>&m){for(auto&e:m)o<<e<<" ";return o;}
template<typename T>auto&operator<<(ostream&o,vector<T>v){for(auto&e:v)o<<e<<" ";return o;}
void ashow(){cout<<endl;}template<typename T,typename...A>void ashow(T t,A...a){cout<<t<<" ";ashow(a...);}
template<typename S,typename T,typename U>
struct TRI{S fi;T se;U th;TRI(){}TRI(S f,T s,U t):fi(f),se(s),th(t){}
bool operator<(const TRI&_)const{return(fi==_.fi)?((se==_.se)?(th<_.th):(se<_.se)):(fi<_.fi);}};
template<typename S,typename T,typename U>
auto&operator<<(ostream&o,TRI<S,T,U>&t){return o<<"{"<<t.fi<<","<<t.se<<","<<t.th<<"}";}

typedef pair<int, int> P;
typedef pair<ll, ll> pll;
typedef TRI<int, int, int> tri;
typedef vector<int> vi;
typedef vector<ll> vl;
typedef vector<vi> vvi;
typedef vector<vl> vvl;
typedef vector<P> vp;
typedef vector<double> vd;
typedef vector<string> vs;

const int MAX_N = 100005;

// predecessor の verify もする
// msb define でも良いか？
// vanEmdeBoasTree も move 対抗などする.
// max_size を入れる
class vanEmdeBoasTree
{
private:
    static const size_t LENGTH = 1073741824;
    size_t _size;
    #define msb(u) (63 - __builtin_clzll(u))
    #define lsb(u) (__builtin_ctzll(u))
    struct first_layer;
    struct middle_layer;
    struct last_layer;
    struct first_layer {
        unsigned long long *data;
        middle_layer *summary;
        int _max, _min;
        first_layer() : _max(-1), _min(1073741824){
            data = new unsigned long long[16777216](), summary = new middle_layer();
        }
        inline bool empty() const noexcept { return (_max == -1); }
        inline bool isOne() const noexcept { return (_max == _min); }
        inline int max() const noexcept { return _max; }
        inline int min() const noexcept { return _min; }
        bool find(const int value) const noexcept {
            return (value == _min) || ((data[value >> 6] >> (value & 63)) & 1ULL);
        }
        bool insert(int value) noexcept {
            if(value == _min) return false;
            else if(_max == -1) return _max = _min = value, true;
            else if(value < _min) swap(value, _min);
            else if(value > _max) _max = value;
            const int id = (value >> 6);
            if((data[id] >> (value & 63)) & 1ULL) return false;
            else{
                if(data[id] == 0) summary->insert(id);
                data[id] ^= (1ULL << (value & 63));
                return true;
            }
        }
        void erase(int value) noexcept {
            if(_max == _min){
                _max = -1, _min = 1073741824;
                return;
            }else if(value == _min){
                const int id = summary->min();
                 _min = value = (id << 6) + lsb(data[id]);
            }
            const int id = (value >> 6);
            data[id] ^= (1ULL << (value & 63));
            if(data[id] == 0) summary->erase(id);
            if(value == _max){
                if(summary->empty()) _max = _min;
                else{
                    const int id = summary->max();
                    _max = (id << 6) + msb(data[id]);
                }
            }
        }
        int predecessor(const int value) const noexcept {
            if(_min >= value) return -1;
            else if(value > _max) return _max;
            const int id = (value >> 6), sm = (id << 6);
            if(value > sm + lsb(data[id]))
                return sm + msb(data[id] & ((1ULL << (value & 63)) - 1ULL));
            else{
                const int id2 = summary->predecessor(id);
                return (id2 << 6) + msb(data[id2]);
            }
        }
        int successor(const int value) const noexcept {
            if(value < _min) return _min;
            else if(value >= _max) return 1073741824;
            const int id = (value >> 6), sm = (id << 6);
            if(value < sm + msb(data[id]))
                return sm + lsb(data[id] & ~((1ULL << ((value & 63) + 1)) - 1ULL));
            else{
                const int id2 = summary->successor(id);
                return (id2 << 6) + lsb(data[id2]);
            }
        }
    };
    struct middle_layer{
        last_layer *sublayers, *summary;
        int _max, _min;
        middle_layer() : _max(-1), _min(16777216){
            sublayers = new last_layer[4096](), summary = new last_layer();
        }
        inline bool empty() const noexcept { return (_max == -1); }
        inline bool isOne() const noexcept { return (_max == _min); }
        inline int max() const noexcept { return _max; }
        inline int min() const noexcept { return _min; }
        bool find(const int value) const noexcept {
            return (value == _min) || sublayers[value >> 12].find(value & 4095);
        }
        bool insert(int value) noexcept {
            if(value == _min) return false;
            else if(_max == -1) return _max = _min = value, true;
            else if(value < _min) swap(value, _min);
            else if(value > _max) _max = value;
            const int id = (value >> 12);
            if(sublayers[id].insert(value & 4095)){
                if(sublayers[id].isOne()) summary->insert(id);
                return true;
            }else return false;
        }
        void erase(int value){
            if(_max == _min){
                _max = -1, _min = 4096;
                return;
            }else if(value == _min){
                const int id = summary->min();
                 _min = value = (id << 12) + sublayers[id].min();
            }
            const int id = (value >> 12);
            sublayers[id].erase(value & 4095);
            if(sublayers[id].empty()) summary->erase(id);
            if(value == _max){
                if(summary->empty()) _max = _min;
                else{
                    const int id = summary->max();
                    _max = (id << 12) + sublayers[id].max();
                }
            }
        }
        int predecessor(const int value) const noexcept {
            if(_min >= value) return -1;
            else if(value > _max) return _max;
            const int id = (value >> 12), sm = (id << 12);
            if(value > sm + sublayers[id].min()){
                return sm + sublayers[id].predecessor(value & 4095);
            }else{
                const int id2 = summary->predecessor(id);
                return (id2 << 12) + sublayers[id2].max();
            }
        }
        int successor(const int value) const noexcept {
            if(value < _min) return _min;
            else if(value >= _max) return 4096;
            const int id = (value >> 12), sm = (id << 12);
            if(value < sm + sublayers[id].max()){
                return sm + sublayers[id].successor(value & 4095);
            }else{
                const int id2 = summary->successor(id);
                return (id2 << 12) + sublayers[id2].min();
            }
        }
    };
    struct last_layer{
        unsigned long long data[64], summary;
        int _max, _min;
        last_layer() : summary(0ULL), _max(-1), _min(64){
            memset(data, 0, sizeof(data));
        }
        inline bool empty() const noexcept { return (_max == -1); }
        inline bool isOne() const noexcept { return (_max == _min); }
        inline int max() const noexcept { return _max; }
        inline int min() const noexcept { return _min; }
        bool find(const int value) const noexcept {
            return (value == _min) || ((data[value >> 6] >> (value & 63)) & 1ULL);
        }
        bool insert(int value) noexcept {
            if(value == _min) return false;
            else if(_max == -1) return _max = _min = value, true;
            else if(value < _min) swap(value, _min);
            else if(value > _max) _max = value;
            const int id = (value >> 6);
            if((data[id] >> (value & 63)) & 1ULL) return false;
            else{
                data[id] ^= (1ULL << (value & 63)), summary |= (1ULL << id);
                return true;
            }
        }
        void erase(int value) noexcept {
            if(_max == _min){
                _max = -1, _min = 64;
                return;
            }else if(value == _min){
                const int id = lsb(summary);
                _min = value = (id << 6) + lsb(data[id]);
            }
            const int id = (value >> 6);
            data[id] ^= (1ULL << (value & 63));
            if(data[id] == 0) summary ^= (1ULL << id);
            if(value == _max){
                if(summary == 0) _max = _min;
                else{
                    const int id = msb(summary);
                    _max = (id << 6) + msb(data[id]);
                }
            }
        }
        int predecessor(const int value) const noexcept {
            if(_min >= value) return -1;
            else if(value > _max) return _max;
            const int id = (value >> 6), sm = (id << 6);
            if(value > sm + lsb(data[id]))
                return sm + msb(data[id] & ((1ULL << (value & 63)) - 1ULL));
            else{
                const int id2 = msb(summary & ((1ULL << id) - 1ULL));
                return (id2 << 6) + msb(data[id2]);
            }
        }
        int successor(const int value) const noexcept {
            if(value < _min) return _min;
            else if(value >= _max) return 64;
            const int id = (value >> 6), sm = (id << 6);
            if(value < sm + msb(data[id]))
                return sm + lsb(data[id] & ~((1ULL << ((value & 63) + 1)) - 1ULL));
            else{
                const int id2 = lsb(summary & ~((1ULL << (id + 1)) - 1ULL));
                return (id2 << 6) + lsb(data[id2]);
            }
        }
    };
    first_layer base_layer;
public:
    vanEmdeBoasTree() : _size(0), base_layer(){}
    friend ostream& operator<< (ostream& os, vanEmdeBoasTree& veb) noexcept {
        for(unsigned int st = veb.successor(-1); st != veb.LENGTH; st = veb.successor(st))
            os << st << " ";
        return os;
    }
    bool empty() const noexcept { return (_size == 0); }
    size_t size() const noexcept { return _size; }
    size_t max_size() const noexcept { return LENGTH; }
    bool find(const unsigned int value) const noexcept {
        if(value >= LENGTH) return false;
        return base_layer.find(value);
    }
    int max() const noexcept { return base_layer.max(); }
    int min() const noexcept { return base_layer.min(); }
    void insert(const unsigned int value){
        assert(value < LENGTH);
        _size += base_layer.insert(value);
    }
    void erase(const unsigned int value){
        assert(value < LENGTH);
        base_layer.erase(value), --_size;
    }
    int predecessor(const int value) const noexcept {
        return base_layer.predecessor(value);
    }
    int successor(const int value) const noexcept {
        return base_layer.successor(value);
    }
};

#define getchar getchar_unlocked
#define putchar putchar_unlocked

inline int in() {
    int n = 0; short c;
    while ((c = getchar()) >= '0') n = n * 10 + c - '0';
    return n;
}

inline void out(int n) {
    short res[10], i = 0;
    do { res[i++] = n % 10, n /= 10; } while (n);
    while (i) putchar(res[--i] + '0');
    putchar('\n');
}

int main()
{
    cin.tie(0);
    ios::sync_with_stdio(false);
    vanEmdeBoasTree veb;
    int n = in(), a, b;
    rep(i,n){
        a = in();
        if(a == 0){
            veb.insert(in());
            cout << (int)veb.size() << endl;
            // out(veb.size());
        }else if(a == 1){
            cout << veb.find(in()) << endl;
            // out(veb.find(in()));
        }else if(a == 2){
            a = in();
            if(veb.find(a)) veb.erase(a);
        }else{
            a = in(), b = in();
            for(int st = veb.successor(a-1); st <= b; st = veb.successor(st))
                cout << st << endl;
                // cout << st << "\n";
        }
    }
    return 0;
}
