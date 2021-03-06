#include<cstdio>
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<cassert>
using namespace std;

//#define MY_DEBUG

#ifdef MY_DEBUG
auto comment = printf;
#else
inline void comment(...) {}
#endif

struct Node;

struct Edge {
    int l, r;
    Node* next;
};

struct Node {
    int id{-1};
    Node* suffix_link{nullptr};
    vector<Edge> edges;
};

struct ExtensionResult {
    Node* last_node;
    int characters_going_down_the_edge;
    bool is_extension_rule3_applied;
};

const int GLOBAL_LEAF_R_PLACE_HOLDER = -1;

Node* new_node() {
    static int id = 1;
	Node* node = new Node{ id++, nullptr };
    return node;
}

Edge* find_edge_with_prefix(const string& s, vector<Edge>& edges, char prefix) {
    for (Edge& e : edges) {
        if (s[e.l] == prefix) {
            return &e;
        }
    }
    return nullptr;
}

void set_suffix_link(Node*& last_newly_created_internal_node, Node * node) {
	if (last_newly_created_internal_node) {
		last_newly_created_internal_node->suffix_link = node;
		last_newly_created_internal_node = nullptr;
	}
}

ExtensionResult insert(Node* root, int s_l, int s_r, Node*& last_newly_created_internal_node, const string& s, int global_leaf_r) {

    Edge* e = find_edge_with_prefix(s, root->edges, s[s_l]);

    if (!e) {
        root->edges.push_back({ s_l, GLOBAL_LEAF_R_PLACE_HOLDER, nullptr });
		set_suffix_link(last_newly_created_internal_node, root);
        return { root, 1, false };
    }

    int e_r = e->r == GLOBAL_LEAF_R_PLACE_HOLDER ? global_leaf_r - 1 : e->r;

    int e_len = e_r - e->l + 1;
    int s_len_without_new_ch = s_r - s_l;

    if (e_len < s_len_without_new_ch) {
        return insert(e->next, s_l + e_len, s_r, last_newly_created_internal_node, s, global_leaf_r);
    }
    else if (e_len > s_len_without_new_ch) {
        if (s[e->l + s_len_without_new_ch] == s[s_r]) {
            // Rule 3 applied
			set_suffix_link(last_newly_created_internal_node, root);
            return { root, s_len_without_new_ch + 1, true };
        }
        else {
            Node* node = new_node();

            node->edges.push_back({ e->l + s_len_without_new_ch, e->r, e->next });
            node->edges.push_back({ s_r, GLOBAL_LEAF_R_PLACE_HOLDER, nullptr });
            
            e_r = e->l + s_len_without_new_ch - 1;
            e->next = node;
            e->r = e_r;

			set_suffix_link(last_newly_created_internal_node, node);
			last_newly_created_internal_node = node;
			return { node, 1, false };
        }
    }
    else {
        if (!e->next) {
			set_suffix_link(last_newly_created_internal_node, root);
			return { root, s_len_without_new_ch + 1, false };
        }
        else {
            return insert(e->next, s_l + s_len_without_new_ch, s_r, last_newly_created_internal_node, s, global_leaf_r);
        }
    }
}

void restore_leaf_r(Node* root, int global_leaf_r) {
    if (!root) {
        return;
    }

    for (Edge& e : root->edges) {
        if (!e.next) {
            e.r = global_leaf_r;
        }
        else {
            restore_leaf_r(e.next, global_leaf_r);
        }
    }
}

Node* build_tree(string& s) {
    int m = static_cast<int>(s.size());
    s = "^" + s;

    Node* root = new_node();
    root->edges.push_back({ 1, GLOBAL_LEAF_R_PLACE_HOLDER, nullptr });
    int global_leaf_r = 1;

    ExtensionResult last_extension_result{ root, 1, false };
	Node* last_newly_created_internal_node = nullptr;
	int last_j = 1;

    for (int i = 1; i <= m - 1; ++i) {
        comment("begin {phase %d}\n", i + 1);

        global_leaf_r += 1;

        for (int j = last_j; j <= i + 1; ++j) {

            comment("begin {extension %d}\n", j);

            if (j == last_j) {
                last_extension_result = insert(
                    last_extension_result.last_node,
                    i + 1 - last_extension_result.characters_going_down_the_edge,
                    i + 1,
                    last_newly_created_internal_node,
                    s,
                    global_leaf_r);
            }
            else {
                if (last_extension_result.last_node->suffix_link) {
                    last_extension_result = insert(
                        last_extension_result.last_node->suffix_link,
                        i + 2 - last_extension_result.characters_going_down_the_edge,
                        i + 1,
                        last_newly_created_internal_node,
                        s,
                        global_leaf_r);
                } else {
                    last_extension_result = insert(
                        root,
                        j,
                        i + 1,
                        last_newly_created_internal_node,
                        s,
                        global_leaf_r);
                }
            }

            last_j = j;
            if (last_extension_result.is_extension_rule3_applied) {
                comment("Extension Rule 3 is applied, skip to next phase\n");
                break;
            }
        }
        comment("\n");
    }

    restore_leaf_r(root, global_leaf_r);

    return root;
}

void print(Node* root, const string& s) {
    if (!root) {
        return;
    }

    comment("Node %d:\n", root->id);

    if (root->suffix_link) {
        comment("Suffix Link: %d\n", root->suffix_link->id);
    }
    else {
        comment("Suffix Link: nullptr\n");
    }

    for (const Edge& e : root->edges) {
        if (e.next) {
            comment("Left: %d, Right: %d, Label: %s,\tNext: %d\n", e.l, e.r, s.substr(e.l, e.r - e.l + 1).c_str(), e.next->id);
        }
        else {
            comment("Left: %d, Right: %d, Label: %s,\tNext: nullptr\n", e.l, e.r, s.substr(e.l, e.r - e.l + 1).c_str());
        }
    }
    comment("\n");

    for (const Edge& e : root->edges) {
        print(e.next, s);
    }
}

int main() {
    fstream fin("input.txt", fstream::in);
    string s;
    fin >> s;
    Node* root = build_tree(s);
    print(root, s);
    return 0;
}