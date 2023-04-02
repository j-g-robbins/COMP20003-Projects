#include "treeops.h"

/* safely creates an empty tree */
tree_t *make_empty_tree() {
	tree_t *tree; 
	tree = malloc(sizeof(*tree)); 
	assert(tree!=NULL); 
	tree->root = NULL;
	if (!is_empty_tree(tree)) {
		exit(EXIT_FAILURE);
	}
	return tree;
}

/* checks if it is an empty tree */
int is_empty_tree(tree_t *tree) {
	assert(tree!=NULL); 
	return tree->root==NULL;
}

/* safely allocate memory for a string */
void malloc_string(char **string, int size) {
	*string = malloc(sizeof(char) * size);
	assert(*string != NULL);
}

/* copy a string from one variable to another with malloc */
void copy_string(char **dest, char *source) {
	malloc_string(dest, strlen(source)+1);
	strcpy(*dest, source);
}

/* gets the k-index for a node from its parent's k-index */
char get_k_index(node_t *root) {
	if (root->k_index == X) {
        return Y;
    }
    return X;
}

/* chains duplicate coordinate nodes to a node in a linked list */
node_t *insert_duplicate(node_t *root, node_t *new) {
    if (root == NULL) {
        return new;
    } else if (root->next) {
        root->next = insert_duplicate(root->next, new);
    }
    return root;
}

/* recursively inserts a node into its correct position, initialising the k-index value */
node_t *recursive_insert(node_t *root, node_t *new) {

    /* base case, insert node, otherwise insert into linked list if match, or traverse required path */
    if (root == NULL) {
        return new; 
    } else if (new->data->x == root->data->x && new->data->y == root->data->y) {
		new->k_index = root->k_index;
        root->next = insert_duplicate(root->next, new);
    } else if (root->k_index == Y) {
        if (new->data->y < root->data->y) {
            root->left = recursive_insert(root->left, new);
            root->left->k_index = get_k_index(root);
        } else {
            root->right = recursive_insert(root->right, new); 
            root->right->k_index = get_k_index(root);
        }
	} else {
        if (new->data->x < root->data->x) {
            root->left = recursive_insert(root->left, new);
            root->left->k_index = get_k_index(root);
        } else {
            root->right = recursive_insert(root->right, new); 
            root->right->k_index = get_k_index(root);
        }
    }
    return root;
}

/* copies datainto a node */
node_t *copy_node(node_t *new, record_t *record_data, char *key) {
	copy_string(&(new->data->census_year), record_data->census_year);
	copy_string(&(new->data->block_id), record_data->block_id);
	copy_string(&(new->data->property_id), record_data->property_id);
	copy_string(&(new->data->base_property_id), record_data->base_property_id);
	copy_string(&(new->data->clue_small_area), record_data->clue_small_area);
	copy_string(&(new->key), key);
	copy_string(&(new->data->industry_code), record_data->industry_code);
	copy_string(&(new->data->industry_description), record_data->industry_description);
	new->data->x = record_data->x;
	new->data->y = record_data->y;
	copy_string(&(new->data->location), record_data->location);
    new->left = new->right = NULL;
    return new;
}

/* inserts a node into its correct position in tree */
tree_t *insert_node(tree_t *tree, record_t *record_data, char *key) {
    
    /* create node to store data, insert recursively */
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
	assert(new!=NULL);
    new->data = malloc(sizeof(record_t));
    assert(new->data != NULL);
	new = copy_node(new, record_data, key);
    tree->root = recursive_insert(tree->root, new);
    return tree;
}

/* prints a node with desired formatting */
void print_node(FILE *output_file, node_t *node, double x, double y) {
	fprintf(output_file, "%lf %lf --> Census year: %s || Block ID: %s || Property ID: %s || Base property ID: %s || CLUE small area: %s || Trading Name: %s || Industry (ANZSIC4) code: %s || Industry (ANZSIC4) description: %s || x coordinate: %lf || y coordinate: %lf || Location: %s || \n", x, y, node->data->census_year, node->data->block_id, node->data->property_id, node->data->base_property_id, node->data->clue_small_area, node->key, node->data->industry_description, node->data->industry_code, node->data->x, node->data->y, node->data->location);
	if (node->next) {
		print_node(output_file, node->next, x, y);
	}
}

/* returns the distance between two points */
double dist_between_pts(double x1, double y1, double x2, double y2) {
    return(sqrt(pow(x2 - x1, 2)+pow(y2 - y1, 2)));
}

/* recursively searchs tree for x-y coordinate pair, counting number of comparisons and returning closest node */
node_t *recursive_traverse(FILE *output_file, node_t *root, double x, double y, double *min, node_t *closest, int *comparisons) {

    /* if a node is found, increment comparison number */
	if (root) {
        *comparisons += 1;

        /* update nearest node and smallest distance if node is closer */
        if (dist_between_pts(x, y, root->data->x, root->data->y) < *min) {
			*min = dist_between_pts(x, y, root->data->x, root->data->y);
	        closest = copy_node(closest, root->data, root->key);
			closest->next = root->next;
        }

        /* depending on k-index and distance from node to query, search desired path */
        if (root->k_index == X) {
			if (pow(root->data->x - x, 2) > pow(*min, 2))  {
				closest = recursive_traverse(output_file, root->left, x, y, min, closest, comparisons);
				closest = recursive_traverse(output_file, root->right, x, y, min, closest, comparisons);
			} else if (x < root->data->x) {
				closest = recursive_traverse(output_file, root->left, x, y, min, closest, comparisons);
			} else {
                closest = recursive_traverse(output_file, root->right, x, y, min, closest, comparisons);
            }
        } else {
            if (pow(root->data->y - y, 2) > pow(*min, 2))  {
				closest = recursive_traverse(output_file, root->left, x, y, min, closest, comparisons);
				closest = recursive_traverse(output_file, root->right, x, y, min, closest, comparisons);
			} else if (y < root->data->y) {
				closest = recursive_traverse(output_file, root->left, x, y, min, closest, comparisons);
			} else {
                closest = recursive_traverse(output_file, root->right, x, y, min, closest, comparisons);
            }
        }
	}
    return closest;
}

/* searches a tree for a x-y coordinate pair, printing the closest node(s) */
void traverse_tree(FILE *output_file, tree_t *tree, double x, double y, int *comparisons) {
    assert(tree!=NULL); 

    /* create variables to track closest node and its distance */
    double min = dist_between_pts(x, y, tree->root->data->x, tree->root->data->y);
    node_t *closest;
    closest = (node_t*)malloc(sizeof(*closest));
	assert(closest!=NULL);
    closest->data = malloc(sizeof(record_t));
    assert(closest->data != NULL);
	closest = copy_node(closest, tree->root->data, tree->root->key);
	closest->next = tree->root->next;

    /* search tree recursively, returning and printing closest node */
    closest = recursive_traverse(output_file, tree->root, x, y, &min, closest, comparisons);
    print_node(output_file, closest, x, y);

    /* free memory */
	free_record(&(closest->data));
	free(closest->key);
	free(closest);
}

/* frees a record */
void free_record(record_t **record_data) {
    free((*record_data)->census_year);
    free((*record_data)->block_id);
    free((*record_data)->property_id);
    free((*record_data)->base_property_id);
    free((*record_data)->clue_small_area);
    free((*record_data)->industry_code);
    free((*record_data)->industry_description);
    free((*record_data)->location);
    free(*record_data);
}

/* frees duplicate coordinate nodes in a linked list */
void free_duplicates(node_t *root) {
	if (root) {
		free_duplicates(root->next);
		free_record(&(root->data));
		free(root->key);
	}
}

/* recursively frees each node in a tree */
void recursive_free_tree(node_t *root) {
    if (root) {
        recursive_free_tree(root->left);
        recursive_free_tree(root->right);
		recursive_free_tree(root->next);
        free_record(&(root->data));
		free(root->key);
    }
	free(root);
}

/* frees a tree */
void free_tree(tree_t *tree) {
	assert (tree!=NULL);
	recursive_free_tree(tree->root);
    free(tree);
}



