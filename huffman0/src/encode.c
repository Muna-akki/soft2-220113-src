#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "./../include/encode.h"

#define NSYMBOLS 256

static int symbol_count[NSYMBOLS];

// 以下このソースで有効なstatic関数のプロトタイプ宣言

// ファイルを読み込み、static配列の値を更新する関数
static void count_symbols(const char *filename);

// symbol_count をリセットする関数
static void reset_count(void);

// 与えられた引数でNode構造体を作成し、そのアドレスを返す関数
static Node *create_node(int symbol, int count, Node *left, Node *right);

// Node構造体へのポインタが並んだ配列から、最小カウントを持つ構造体をポップしてくる関数
// n は 配列の実効的な長さを格納する変数を指している（popするたびに更新される）
static Node *pop_min(int *n, Node *nodep[]);

// ハフマン木を構成する関数
static Node *build_tree(void);

static void clone_char_array(int l, char* f, char* t);

static void clone_int_array(int l, int* f, int* t);

// 以下 static関数の実装
static void count_symbols(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
	    fprintf(stderr, "error: cannot open %s\n", filename);
	    exit(1);
    }

    // 1Byteずつ読み込み、カウントする
    /*
      write a code for counting
    */
    char c;
    while( fread(&c, sizeof(char), 1, fp) != 0 || c==EOF){
        symbol_count[(int)c]++;
    }
    fclose(fp);
}

static void reset_count(void)
{
    for (int i = 0 ; i < NSYMBOLS ; i++){
        symbol_count[i] = 0;
    }
}

static Node *create_node(int symbol, int count, Node *left, Node *right)
{
    Node *ret = (Node *)malloc(sizeof(Node));
    *ret = (Node){ .symbol = symbol, .count = count, .parent = NULL, .left = left, .right = right};
    char* cd = (char*) malloc(sizeof(char)*NSYMBOLS*2);
    int* t_m = (int*) malloc(sizeof(int)*NSYMBOLS*4);
    (*ret).tree_make = t_m;
    (*ret).code = cd;
    return ret;
}

static Node *pop_min(int *n, Node *nodep[])
{
    // Find the node with the smallest count
    // カウントが最小のノードを見つけてくる
    int argmin = 0;
    for (int i = 0; i < *n; i++) {
	    if (nodep[i]->count < nodep[argmin]->count) {
	        argmin = i;
	    }
    }
    
    Node *node_min = nodep[argmin];
    
    // Remove the node pointer from nodep[]
    // 見つかったノード以降の配列を前につめていく
    for (int i = argmin; i < (*n) - 1; i++) {
	    nodep[i] = nodep[i + 1];
    }
    // 合計ノード数を一つ減らす
    (*n)--;
    
    return node_min;
}

static Node *build_tree(void)
{
    int n = 0;
    Node *nodep[NSYMBOLS];
    
    for (int i = 0; i < NSYMBOLS; i++) {
	    // カウントの存在しなかったシンボルには何もしない
	    if (symbol_count[i] == 0) continue;
	
	    nodep[n++] = create_node(i, symbol_count[i], NULL, NULL);
    }

    const int dummy = -1; // ダミー用のsymbol を用意しておく
    while (n >= 2) {
	    Node *node1 = pop_min(&n, nodep);
	    Node *node2 = pop_min(&n, nodep);
	
	    // Create a new node
	    // 選ばれた2つのノードを元に統合ノードを新規作成
        Node* node3 = create_node(dummy, (node1->count)+(node2->count), node1, node2);
	    // 作成したノードはnodep にどうすればよいか?
        node1->parent = node3;
        node2->parent = node3;
        nodep[n] = node3;
        n++;
	
    }

    // なぜ以下のコードで木を返したことになるか少し考えてみよう
    return (n==0)?NULL:nodep[0];
}

static void clone_char_array(int l, char* f, char* t){
    for(int i=0 ; i<l ; i++){
        t[i] = f[i];
    }
}

static void clone_int_array(int l, int* f, int* t){
    for(int i=0 ; i<l ; i++){
        t[i] = f[i];
    }  
}


// Perform depth-first traversal of the tree
// 深さ優先で木を走査する
// 現状は何もしていない（再帰してたどっているだけ）
void traverse_tree(const int depth, const Node *np)
{			  
    if(np==NULL){
        return;
    }
    char c[4];
    c[0] = np->symbol;
    c[1] = '\0';
    if(c[0]=='\n'){
        c[0] = 'L';
        c[1] = 'F';
        c[2] = '\0';
    }

    //符号を確定
    if(depth!=0){
        clone_char_array(depth-1, np->parent->code, np->code);
        (np->code)[depth-1] = (np->parent->left == np)? '1':'0' ;
        (np->code)[depth] = '\0';
        
        clone_int_array(depth-1, np->parent->tree_make, np->tree_make);
        (np->tree_make)[depth-1] = (np->parent->left == np) ? 1:0 ;
    }else{
        (np->code)[0] = '0';
        (np->tree_make)[0] = 0;
    }

    for(int i=0 ; i<depth-1 ; i++){
        if( (np->tree_make)[i] == 1 ){
            printf("|  ");
        }else{
            printf("   ");
        }
    }
    if(np->left==NULL){
        //printf("");
        printf("+--%s  %s\n",c,np->code);
        //printf("+--%s\n",c);
        return;
    }else{
        printf("\n");
    }
    traverse_tree(depth + 1, np->left);
    traverse_tree(depth + 1, np->right);
}

// この関数は外部 (main) で使用される (staticがついていない)
Node *encode(const char *filename)
{
    reset_count();
    count_symbols(filename);
    Node *root = build_tree();
    if (root == NULL){
	    fprintf(stderr,"A tree has not been constructed.\n");
    }

    return root;
}
