#ifndef __TREE_H__
#define __TREE_H__

typedef struct node 
{
    unsigned char isLeaf;
    union
    {
        struct 
        {
            char operation;
            struct node* left;
            struct node* right;
        } innerNode;
        int value;
    } u;
} Node;

Node *newLeafNode(int value);
Node *newInnerNode(char operation, Node* left, Node* right);

#endif