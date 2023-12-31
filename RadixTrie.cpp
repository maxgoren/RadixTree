#include <iostream>
#include <queue>
#include <unordered_map>
#include <map>
using namespace std;

struct CTNode {
    string key;
    map<char, CTNode*> next;
    CTNode(string k = "") {
        key = k;
    }
};

class RecTrace {
    private:
        int depth;
    public:
        RecTrace() {
            depth = 0;
        }
        void onEnter(string str) {
            depth++;
            say(str);
        }
        void onExit(string str) {
            say(str);
            depth--;
        }
        void say(string str) {
            for (int i = 0; i < depth; i++) cout<<"  ";
            cout<<str<<endl;
        }
};

class CompressedTrie {
    private:
        RecTrace dbg;
        using nodeptr = CTNode*;
        CTNode* head;
        int words;
        CTNode* split(CTNode* x, string word, string newWord, int i, int j) {
            string newkey = x->key.substr(0, j);
            string rest = x->key.substr(i);
            dbg.say("split "+x->key+" into "+newkey+" and "+rest);
            dbg.say("to add "+newWord+" to "+newkey+" to make "+word);
            CTNode* t = new CTNode(newkey);
            x->key = rest;
            t->next[rest[0]] = x;
            if (newWord.size() > 0)
                t->next[newWord[0]] = insert(t->next[newWord[0]], newWord);
            return t;
        }
        CTNode* insert(CTNode* root, string word) {
            dbg.onEnter("insert " + word);
            if (root == nullptr) {
                dbg.onExit("hooking in " + word);
                return new CTNode(word);
            }
            dbg.say("Comparing Key: " + root->key);
            int i = 0, j = 0;
            nodeptr x = root;
            while (i < word.length() && j < x->key.length()) {
                if (word[i] == x->key[j]) { 
                    i++; j++;
                } else break;
            }
            string newWord = word.substr(i);
            if (j == x->key.size()) {
                x->next[newWord[0]] = insert(x->next[newWord[0]], newWord);
            } else {
                x = split(x, word, newWord, i, j);
            }
            dbg.onExit("returning " + x->key);
            return x;
        }
        void preorder(nodeptr h, string& sofar) {
            if (h == nullptr) return;
            string curr = sofar + h->key;
            if (h->next.empty()) {
                cout<<curr<<endl;
            } else {
                for (auto child : h->next) {
                    preorder(child.second, curr);
                }
            }
        }
        nodeptr erase(nodeptr node, string key, string subkey) {
            if (node == nullptr)
                return node;
            dbg.onEnter("Enter node " + node->key + " with key + " + key);
            if (node->key == key) {
                return nullptr;
            }
            int i = node->key.length();
            string remainder = key.substr(i);
            node->next[key[i]] = erase(node->next[key[i]], remainder, node->key.substr(0, i));
            dbg.onExit("Exiting: " + node->key);
            return node;
        }
    public:
        CompressedTrie() {
            head = new CTNode("#");
            words = 0;
        }      
        void insert(string word) {
            head->next[word[0]] = insert(head->next[word[0]], word);
            words++;
        }
        void erase(string str) {
            head->next[str[0]] = erase(head->next[str[0]], str, "");
        }
        bool contains(string str) {
            cout<<"Searching For: "<<str<<endl;
            CTNode* x = head->next[str[0]];
            int j = 0;
            for (;;) {
                int i = 0;  
                while (i < x->key.size() && j < str.size()) {
                    cout<<str[j]<<" == "<<x->key[i]<<"? "<<endl;
                    if (str[j] != x->key[i])
                        return false;
                    i++;
                    j++;
                }
                if (j == str.size())
                    return true;
                if (x->next.find(str[j]) == x->next.end())
                    return false;
                else  {
                    cout<<"Next Node: "<<str[j]<<endl;
                    x = x->next[str[j]];
                }
            }
        }
        void traverse() {
            for (auto p : head->next) {
                string str = "";
                preorder(p.second, str);
            }
        }
        CTNode* letter(char c) {
            return head->next[c];
        }
        CTNode* get() {
            return head;
        }
};

void levelorder(CTNode* h) {
    queue<CTNode*> fq;
    fq.push(h);
    CTNode* prev = nullptr;
    while (!fq.empty()) {
        int nc = fq.size();
        cout<<"[ ";
        while (nc > 0) {
            CTNode* curr = fq.front();
            
            fq.pop();
            nc--;
            if (curr != nullptr) {
                cout<<"("<<curr->key<<") ";
                for (auto cn : curr->next) {
                    if (cn.second != nullptr)
                        fq.push(cn.second);
                }
            }
            prev = curr;
        }
        cout<<"]"<<endl;
    }
}

int main() {
    CompressedTrie trie;
    vector<string> words = /*{"test","slow","water",
     "mushroom", "pasta", "privacy", "marinara", "primaveras", "zombie", "marijuana", "zoology",
   */
  {"romane", "romanus", "romulus","rubens", "rubicon", "ruber", "rubicundus"}; 
    for (string s : words) {
        cout<<"insert: "<<s<<endl;
        trie.insert(s);
        levelorder(trie.get());
        cout<<"--------------"<<endl;
    }
    for (string s : words) {
        trie.erase(s);
        levelorder(trie.get());
    }
    return 0;
}