#ifndef TREE /* header guard */
#define TREE

#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* buffer sizes for allocating memory */
#define MAX_FIELD_LEN 128
#define MAX_LINE_LEN 512
#define START_SIZE 8
#define X 'x'
#define Y 'y'

typedef char field_t[MAX_FIELD_LEN];
typedef char line_t[MAX_LINE_LEN];

/* define struct to store business data fields */
typedef struct {
    char *census_year, *block_id, *property_id, *base_property_id, *industry_code,
	*clue_small_area, *industry_description, *location;
    double x, y;
} record_t;

/* K-D tree node */
typedef struct node node_t;
struct node {
    record_t *data;
    char *key;
    char k_index;
    node_t *next;
    node_t *left;
    node_t *right;
};


/* K-D tree */
typedef struct {
    node_t *root;
} tree_t;


/* function prototypes */
void print_node(FILE *output_file, node_t *node, double x, double y);
void copy_string(char **dest, char *source);
tree_t *make_empty_tree();
int is_empty_tree(tree_t *tree); 
node_t *recursive_insert(node_t *root, node_t *new);
tree_t *insert_node(tree_t *tree, record_t *record_data, char *key);
void free_tree(tree_t *tree);
void recursive_free_tree(node_t *root);
void free_record(record_t **record_data);
record_t *malloc_record(int size);
void malloc_string(char **string, int size);
double dist_between_pts(double x1, double y1, double x2, double y2);
node_t *recursive_traverse(FILE *output_file, node_t *root, double x, double y, double *min, node_t *closest, int *comparisons);
void traverse_tree(FILE *output_file, tree_t *tree, double x, double y, int *comparisons);
node_t *copy_node(node_t *new, record_t *record_data, char *key);
void free_duplicates(node_t *root);



#endif