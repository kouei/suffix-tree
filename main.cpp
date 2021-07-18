#include<bits/stdc++.h>
using namespace std;

struct Node;

struct Edge {
    int l, r;
    Node * next;
};

struct Node {
    int id;
    Node * parent;
    Node * suffix_link;
    vector<Edge> edges;
};

Node * new_node() {
    static int id = 1;
    Node * node = new Node();
    node->id = id++;
    node->parent = nullptr;
    node->suffix_link = nullptr;
    return node;
}

Edge & find_edge_with_prefix(const string & s, vector<Edge> & edges, char prefix) {
    for(Edge & e : edges) {
        if(s[e.l] == prefix) {
            return e;
        }
    }
    return edges[0];
}

int get_first_mismatch_index(const string & s, int a_l, int a_r, int b_l, int b_r) {
    int i = 0;
    while(a_l + i <= a_r && b_l + i <= b_r) {
        if(s[a_l + i] != s[b_l + i]) {
            break;
        }
        ++i;
    }
    return i;
}

void insert(Node * root, const string & s, int s_l, int s_r, int new_ch_pos, Node * & last_newly_created_internal_node) {

    Edge & e = find_edge_with_prefix(s, root->edges, s[s_l]);

    int i = get_first_mismatch_index(s, e.l, e.r, s_l, s_r);

    if(e.l + i > e.r && s_l + i > s_r) {
        if(!e.next) {
            e.r += 1;
        } else {
            for(Edge & ee : e.next->edges) {
                if(s[ee.l] == s[new_ch_pos]) {
                    return;
                }
            }
            e.next->edges.push_back({ new_ch_pos, new_ch_pos, nullptr });
        }
    } else if(e.l + i > e.r) {
        insert(e.next, s, s_l + i, s_r, new_ch_pos, last_newly_created_internal_node);
    } else if(s_l + i > s_r) {
        if(s[e.l + i] == s[new_ch_pos]) {
            return;
        } else {
            Node * node = new_node();
            
            node->edges.push_back({ e.l + i, e.r, e.next });
            if(e.next) {
                e.next->parent = node;
            }
            node->edges.push_back({ new_ch_pos, new_ch_pos, nullptr });
            node->parent = root;

            e.r = e.l + i - 1;
            e.next = node;

            if(last_newly_created_internal_node) {
                last_newly_created_internal_node->suffix_link = node;
            }
            last_newly_created_internal_node = node;
        }
    } else {
        assert("Reach impossible code path!" && false);
    }
}

void insert(Node * root, int new_ch_pos, const string & s) {
    for(Edge & e : root->edges) {
        if(s[e.l] == s[new_ch_pos]) {
            return;
        }
    }
    root->edges.push_back({ new_ch_pos, new_ch_pos, nullptr });
}

Node * build_tree(string & s) {
    int m = s.size();
    s = "^" + s + "$";
    Node * root = new_node();
    root->edges.push_back({ 1, 1, nullptr });

    for(int i = 1; i <= m - 1; ++i) {
        printf("begin {phase %d}\n", i + 1);
        Node * last_newly_created_internal_node = nullptr;
        for(int j = 1; j <= i; ++j) {
            printf("begin {extension %d}\n", j);
            insert(root, s, j, i, i + 1, last_newly_created_internal_node);
        }
        insert(root, i + 1, s);
        if(last_newly_created_internal_node) {
            last_newly_created_internal_node->suffix_link = root;
        }
        printf("\n");
    }

    return root;
}

void print(Node * root, const string & s) {
    if(!root) {
        return;
    }

    printf("Node %d:\n", root->id);
    if(root->parent) {
        printf("Parent: %d\n", root->parent->id);
    } else {
        printf("Parent: nullptr\n");
    }

    if(root->suffix_link) {
        printf("Suffix Link: %d\n", root->suffix_link->id);
    } else {
        printf("Suffix Link: nullptr\n");
    }

    for(const Edge & e : root->edges) {
        if(e.next) {
            printf("Label: %s,\tNext: %d\n", s.substr(e.l, e.r - e.l + 1).c_str(), e.next->id);
        } else {
            printf("Label: %s,\tNext: nullptr\n", s.substr(e.l, e.r - e.l + 1).c_str());
        }
    }
    printf("\n");

    for(const Edge & e : root->edges) {
        print(e.next, s);
    }
}

int main() {
    char buf[128];
    scanf("%s", buf);
    string s(buf);
    Node * root = build_tree(s);
    print(root, s);
    return 0;
}