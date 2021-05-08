// Author: Jakub Kuska

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAGIC_CONSTANT 3

typedef struct bdd_node {
    char* m_data;
    struct bdd_node* m_left;
    struct bdd_node* m_right;

} bdd_node;

typedef struct bdd_tree {
    int m_height;
    struct bdd_node* m_root;
} bdd_tree;

// inspired from https://stackoverflow.com/questions/19305980/splitting-string-in-half-using-c
char* split(char* root, int low, int high)
{
    int length = high - low;
    char* tmp = (char*)malloc(sizeof(char) * (length + 1));
    for (int i = low; i < high && (*(root + 1) != '\0'); i++)
    {
        *tmp = *(root + i);
        tmp++;
    }

    *tmp = '\0';

    return tmp - length;
}

struct bdd_node* create(char* data, int* height)
{

    struct bdd_node* tmp = (struct bdd_node*)malloc(sizeof(struct bdd_node));
    tmp->m_data = data;
    tmp->m_left = tmp->m_right = NULL;
    *height += 1;

    return tmp;
}

struct bdd_node* bdd_create(bdd_node* root, char* data, int* height)
{

    char* left = split(data, 0, strlen(data) / 2);
    char* right = split(data, strlen(data) / 2, strlen(data));
    root->m_left = create(left, height);
    root->m_right = create(right, height);

    if(strlen(data) >= MAGIC_CONSTANT)
    {
        root->m_left = bdd_create(root->m_left, left, height);
        root->m_right = bdd_create(root->m_right, right, height);
    }

    return root;
}


char bdd_use(bdd_node* root, char* key)
{
    for (int i = 0; i < strlen(key); i++)
    {
        if (root == NULL || root->m_left == NULL || root->m_right == NULL)
            break;
        if (key[i] == '0')
            root = root->m_left;
        else if (key[i] == '1')
            root = root->m_right;
    }

    return *root->m_data;
}

// generate character vector of 2^exponent length
char* gen_vector(int exponent)
{
    int high = (int)pow(2, exponent);
    char* tmp = (char*)malloc(high);

    srand(time(NULL));

    for(int i = 0; i < high; i++)
    {
        int bin_val = (int)(rand() % 2);
        char char_bin_val = bin_val + '0';
        tmp[i] = char_bin_val;
    }
    
    
    return tmp;
}

char* gen_key(int length)
{
    char* tmp = (char*)malloc(length);

    srand(time(NULL));

    for(int i = 0; i < length; i++)
    {
        int bin_val = (int)(rand() % 2);
        char char_bin_val = bin_val + '0';
        tmp[i] = char_bin_val;
    }

    return tmp;
}



int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Wrong argument count! Exiting...\n");
        return -1;
    }

    // timing vars
    double time_delta = 0.0;
    clock_t begin = 0;
    clock_t end = 0;
    // end timing vars


    struct bdd_tree* tree = (struct bdd_tree*)malloc(sizeof(struct bdd_tree));
    tree->m_height = 0;
    struct bdd_node* root = NULL;


    int exponent = atoi(argv[1]);

    begin = clock();
    char* vector = gen_vector(exponent);
    end = clock();
    time_delta = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Generated vector: %s\n", vector);

    printf("Vector creation of size 2^%d took %f seconds!\n", exponent, time_delta);

    root = create(vector, &tree->m_height);
    tree->m_root = root;
    time_delta = (double)(end - begin) / CLOCKS_PER_SEC;


    begin = clock();
    root = bdd_create(root, vector, &tree->m_height);
    end = clock();
    time_delta = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("BDD creation took %f seconds!\nNodes in tree: %d\n", time_delta, tree->m_height);

    begin = clock();
    printf("Result: %c\n", bdd_use(tree->m_root, gen_key(exponent)));
    end = clock();
    time_delta = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Calculation of result took %f seconds!\n", time_delta);


    return 0;
}
