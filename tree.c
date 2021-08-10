#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

Node *newLeafNode(int value)
{
    Node* result;
    result = malloc(sizeof(Node));
    if(result == NULL) { printf("error: no memory available\n"); exit(99);}

    result->isLeaf= 0;
    result->u.value = value;
    return result;
}
Node *newInnerNode(char operation,Node* left,Node* right)
{
    Node* result;
    result = malloc(sizeof(Node));
    if(result == NULL) { printf("error: no memory available\n"); exit(99);}

    result->isLeaf=0;
    result->u.innerNode.operation = operation;
    result->u.innerNode.left = left;
    result->u.innerNode.right = right;

    return result;
}