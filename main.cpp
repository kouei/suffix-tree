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

struct LastPosition {
    Node * last_node;
    int new_l;
    int new_r;
    bool should_end_extension;
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

LastPosition insert(Node * root, const string & s, int s_l, int s_r, int new_ch_pos, Node * & last_newly_created_internal_node) {

    Edge & e = find_edge_with_prefix(s, root->edges, s[s_l]);

    int e_len = e.r - e.l + 1;
    int s_len = s_r - s_l + 1;

    if(e_len < s_len) {
        return insert(e.next, s, s_l + e_len, s_r, new_ch_pos, last_newly_created_internal_node);
    } else if(e_len > s_len) {
        if(s[e.l + s_len] == s[new_ch_pos]) {
            return { root, e.l, e.l + s_len - 1, false };
        } else {
            Node * node = new_node();
            
            node->edges.push_back({ e.l + s_len, e.r, e.next });
            if(e.next) {
                e.next->parent = node;
            }
            node->edges.push_back({ new_ch_pos, new_ch_pos, nullptr });
            node->parent = root;

            e.r = e.l + s_len - 1;
            e.next = node;

            if(last_newly_created_internal_node) {
                last_newly_created_internal_node->suffix_link = node;
            }
            last_newly_created_internal_node = node;
            return { root, e.l, e.r, false };
        }
    } else {
        if(!e.next) {
            e.r += 1;
            return { root, e.l, e.r - 1, false };
        } else {
            for(Edge & ee : e.next->edges) {
                if(s[ee.l] == s[new_ch_pos]) {
                    return { root, e.l, e.r, false };
                }
            }
            e.next->edges.push_back({ new_ch_pos, new_ch_pos, nullptr });
            return { root, e.l, e.r, false };
        }
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

        LastPosition last_pos;
        last_pos.last_node = nullptr;

        for(int j = 1; j <= i; ++j) {
            printf("begin {extension %d}\n", j);

            if(!last_pos.last_node) {
                last_pos = insert(root, s, j, i, i + 1, last_newly_created_internal_node);
            } else {
                if(last_pos.last_node->suffix_link) {
                    last_pos = insert(last_pos.last_node->suffix_link, s, last_pos.new_l, last_pos.new_r, i + 1, last_newly_created_internal_node);
                } else {
                    last_pos = insert(root, s, j, i, i + 1, last_newly_created_internal_node);
                }
            }
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