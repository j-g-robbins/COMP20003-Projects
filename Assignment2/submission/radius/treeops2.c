#include "treeops2.h"

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

char get_k_index(node_t *root) {
	if (root->k_index == X) {
        return Y;
    }
    return X;
}

node_t *insert_duplicate(node_t *root, node_t *new) {
    if (root == NULL) {
        return new;
    } else {
        root->next = insert_duplicate(root->next, new);
    }
    return root;
}

node_t *recursive_insert(node_t *root, node_t *new) {
    if (root == NULL) {
        return new; 
    } else if (fabs(new->data->x_coord - root->data->x_coord) < 0.00001 && fabs(new->data->y_coord - root->data->y_coord) < 0.00001) {
		new->k_index = root->k_index;
        root->next = insert_duplicate(root->next, new);
    } else if (root->k_index == Y) {
        if (new->data->y_coord < root->data->y_coord) {
            root->left = recursive_insert(root->left, new);
            root->left->k_index = get_k_index(root);
        } else {
            root->right = recursive_insert(root->right, new); 
            root->right->k_index = get_k_index(root);
        }
	} else {
        if (new->data->x_coord < root->data->x_coord) {
            root->left = recursive_insert(root->left, new);
            root->left->k_index = get_k_index(root);
        } else {
            root->right = recursive_insert(root->right, new); 
            root->right->k_index = get_k_index(root);
        }
    }
    return root;
}

node_t *copy_node(node_t *new, record_t *record_data, char *key) {
	copy_string(&(new->data->census_year), record_data->census_year);
	copy_string(&(new->data->block_id), record_data->block_id);
	copy_string(&(new->data->property_id), record_data->property_id);
	copy_string(&(new->data->base_property_id), record_data->base_property_id);
	copy_string(&(new->data->clue_small_area), record_data->clue_small_area);
	copy_string(&(new->key), key);
	copy_string(&(new->data->industry_code), record_data->industry_code);
	copy_string(&(new->data->industry_description), record_data->industry_description);
	new->data->x_coord = record_data->x_coord;
	new->data->y_coord = record_data->y_coord;
	copy_string(&(new->data->location), record_data->location);
    new->left = new->right = NULL;
    return new;
}

tree_t *insert_node(tree_t *tree, record_t *record_data, char *key) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
	assert(new!=NULL);
    new->data = malloc(sizeof(record_t));
    assert(new->data != NULL);
	new = copy_node(new, record_data, key);
    tree->root = recursive_insert(tree->root, new);
    return tree;
}

void print_node(FILE *output_file, node_t *node, double x_coord, double y_coord, double radius) {
	fprintf(output_file, "%lf %lf %lf --> %s | %s | %s | %s | %s | %s | %s | %s | %lf | %lf | %s \n", x_coord, y_coord, radius, node->data->census_year, node->data->block_id, node->data->property_id, node->data->base_property_id, node->data->clue_small_area, node->key, node->data->industry_description, node->data->industry_code, node->data->x_coord, node->data->y_coord, node->data->location);
	if (node->next) {
		print_node(output_file, node->next, x_coord, y_coord, radius);
	}
}

double dist_between_pts(double x1, double y1, double x2, double y2) {
    return(sqrt(pow(x2 - x1, 2)+pow(y2 - y1, 2)));
}

void *radius_search(FILE *output_file, node_t *root, double x_coord, double y_coord, double radius, int *comparisons) {
    if (root) {
        *comparisons += 1;
		recursive_traverse(output_file, root->left, x_coord, y_coord, comparisons);
		if (dist_between_pts(x_coord, y_coord, root->data->x_coord, root->data->y_coord) <= radius) {
            print_node(output_file, root, x_coord, y_coord, radius);
        } 
		recursive_traverse(output_file, root->right, x_coord, y_coord, comparisons);
	}
}
	
void traverse_tree(FILE *output_file, tree_t *tree, double x_coord, double y_coord, double radius, int *comparisons) {
    assert(tree!=NULL);
	radius_search(output_file, tree->root, x_coord, y_coord, radius, comparisons);
}

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

void free_duplicates(node_t *root) {
	if (root) {
		free_duplicates(root->next);
		free_record(&(root->data));
		free(root->key);
	}
}

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

void free_tree(tree_t *tree) {
	assert (tree!=NULL);
	recursive_free_tree(tree->root);
    free(tree);
}



