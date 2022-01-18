#pragma once

typedef struct node Node;
   
struct node{
    int symbol;
    int count;
    int* tree_make;
    char* code;
    Node* parent;
    Node *left;
    Node *right;
};

// ファイルをエンコードし木のrootへのポインタを返す
Node *encode(const char *filename);

// Treeを走査して表示する
void traverse_tree(const int depth, const Node *root);

