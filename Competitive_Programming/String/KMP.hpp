#include "../header.hpp"

// res に文字列 text の中にある検索文字列 pattern のすべての一致箇所の開始位置が入る(0_indexed).
class KMP
{
public:
    string pattern;
    int plen;
    vector<int> table;
    KMP(const string& s) : pattern(s), plen((int)pattern.size()), table(plen+1){
        table[0] = -1;
        int j = -1;
        for(int i = 0; i < plen; i++){
            while(j >= 0 && pattern[i] != pattern[j]) j = table[j];
            if(pattern[i+1] == pattern[++j]) table[i+1] = table[j];
            else table[i+1] = j;
        }
    }
    void search(const string& text, vector<int>& res){
        int head = 0, j = 0, tlen = (int)text.size();
        while(head + j < tlen){
            if(pattern[j] == text[head + j]){
                if(++j != plen) continue;
                res.push_back(head);
            }
            head += j - table[j], j = max(table[j], 0);
        }
    }
};

class MP
{
public:
    string pattern;
    int plen;
    vector<int> table;
    MP(const string& s) : pattern(s), plen((int)pattern.size()), table(plen + 1){
        table[0] = -1;
        int j = -1;
        for(int i = 0; i < plen; ++i){
            while(j >= 0 && pattern[i] != pattern[j]){
                j = table[j];
            }
            table[i+1] = ++j;
        }
    }
    void search(const string& text, vector<int>& res){
        int head = 0, j = 0, tlen = (int)text.size();
        while(head + j < tlen){
            if(pattern[j] == text[head + j]) {
                if(++j == plen){
                    res.push_back(head);
                    head = head + j - table[j];
                    j = table[j];
                }
            }else{
                head = head + j - table[j];
                if(j) j = table[j];
            }
        }
    }
};
 
// 文字列 s[0, i] の最小周期の長さ
void minimum_cycle(const string& s, vector<int>& res){
	MP mp(s);
	res.resize((int)s.size());
	for(int i = 0; i < (int)s.size(); ++i){
		res[i] = i + 1 - mp.table[i+1];
	}
}
