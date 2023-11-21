#ifndef STRUCTURES_H
#define STRUCTURES_H

const int MAX_LABELS  = 100;

enum PlacePref
{
    LEFT  = -1,
    AUTO  =  0,
    RIGHT =  1
};

enum NodeType
{
    VAL = 0,
    VAR = 1,
    OP  = 2
};

enum Operator
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3
};

typedef union
{
    double val;
    char   var;
    Operator op;
} data_t;

struct Node
{
    data_t data;

    NodeType type;

    Node *left;
    Node *right;
};

struct Label
{
    char name;
    double val;
};

struct Tree
{
    Node *root;
    size_t size;

    Label labels[MAX_LABELS];
    size_t n_labels;
};


#endif //STRUCTURES_H