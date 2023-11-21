#ifndef STRUCTURES_H
#define STRUCTURES_H

const int MAX_VARIABLES = 100;

enum PlacePref
{
    LEFT  = -1,
    AUTO  =  0,
    RIGHT =  1
};

enum NodeType
{
    UNK = 0,
    VAL = 1,
    VAR = 2,
    OP  = 3,
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
    Operator op;
    double  val;
    char    var;
} data_t;

struct Node
{
    data_t data;

    NodeType type;

    Node *left;
    Node *right;
};

struct Variable
{
    char name;
    double val;
};

struct Tree
{
    Node *root;
    size_t size;

    Variable variables[MAX_VARIABLES];
    size_t n_vars;
};


#endif //STRUCTURES_H