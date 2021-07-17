#include<bits/stdc++.h>
using namespace std;

struct Node;

struct Edge {
    string label;
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

Edge & find_edge_with_prefix(vector<Edge> & edges, char prefix) {
    for(Edge & e : edges) {
        if(e.label[0] == prefix) {
            return e;
        }
    }
    return edges[0];
}

int get_first_mismatch_index(const string & a, const string & b) {
    int i = 0;
    while(i < a.size() && i < b.size()) {
        if(a[i] != b[i]) {
            break;
        }
        ++i;
    }
    return i;
}

void insert(Node * root, const string & s_suffix, char ch, Node * & last_newly_created_internal_node) {

    Edge & e = find_edge_with_prefix(root->edges, s_suffix[0]);

    int i = get_first_mismatch_index(e.label, s_suffix);

    if(i >= e.label.size() && i >= s_suffix.size()) {
        if(!e.next) {
            e.label += ch;
        } else {
            for(Edge & ee : e.next->edges) {
                if(ee.label[0] == ch) {
                    return;
                }
            }
            e.next->edges.push_back({ {ch}, nullptr });
        }
    } else if(i >= e.label.size()) {
        insert(e.next, s_suffix.substr(i), ch, last_newly_created_internal_node);
    } else if(i >= s_suffix.size()) {
        if(e.label[i] == ch) {
            return;
        } else {
            Node * node = new_node();
            
            node->edges.push_back({ e.label.substr(i), e.next });
            if(e.next) {
                e.next->parent = node;
            }
            node->edges.push_back({ {ch}, nullptr });
            node->parent = root;

            e.label = e.label.substr(0, i);
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

void insert(Node * root, char ch) {
    for(Edge & e : root->edges) {
        if(e.label[0] == ch) {
            return;
        }
    }
    root->edges.push_back({ {ch}, nullptr });
}

Node * build_tree(string s) {
    int m = s.size();
    s = "^" + s + "$";
    Node * root = new_node();
    root->edges.push_back({ {s[1]}, nullptr });

    for(int i = 1; i <= m - 1; ++i) {
        printf("begin {phase %d}\n", i + 1);
        Node * last_newly_created_internal_node = nullptr;
        for(int j = 1; j <= i; ++j) {
            printf("begin {extension %d}\n", j);
            insert(root, s.substr(j, i - j + 1), s[i + 1], last_newly_created_internal_node);
        }
        insert(root, s[i + 1]);
        if(last_newly_created_internal_node) {
            last_newly_created_internal_node->suffix_link = root;
        }
        printf("\n");
    }

    return root;
}

void print(Node * root) {
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
            printf("Label: %s,\tNext: %d\n", e.label.c_str(), e.next->id);
        } else {
            printf("Label: %s,\tNext: nullptr\n", e.label.c_str());
        }
    }
    printf("\n");

    for(const Edge & e : root->edges) {
        print(e.next);
    }
}

int main() {
    char buf[128];
    scanf("%s", buf);
    Node * root = build_tree(buf);
    print(root);
    return 0;
}