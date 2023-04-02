#include "listops.h"


/* create and safely allocate memory for empty list */
dict_t *make_empty_list(void) {
	dict_t *list;
	list = (dict_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	is_empty_list(list);
	return list;
}

/* check if list is empty */
void is_empty_list(dict_t *list) {
	assert(list!=NULL);
	if (!(list->head==NULL)) {
		printf("Error building list");
        exit(EXIT_FAILURE);
	}
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

/* create, safely allocate memory and copy data into a node and add it to
* foot of a list */
dict_t *insert_node(dict_t *list, char *key, record_t *record_data) {

	/* create and malloc node */
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
    assert(list!=NULL && new!=NULL);
	
	/* malloc memory for fields */
	new->record_data = malloc(sizeof(record_t));
	copy_string(&(new->record_data->census_year), record_data->census_year);
	copy_string(&(new->record_data->block_id), record_data->block_id);
	copy_string(&(new->record_data->property_id), record_data->property_id);
	copy_string(&(new->record_data->base_property_id), record_data->base_property_id);
	copy_string(&(new->record_data->clue_small_area), record_data->clue_small_area);
	copy_string(&(new->key), key);
	copy_string(&(new->record_data->industry_code), record_data->industry_code);
	copy_string(&(new->record_data->industry_description), record_data->industry_description);
	copy_string(&(new->record_data->x_coord), record_data->x_coord);
	copy_string(&(new->record_data->y_coord), record_data->y_coord);
	copy_string(&(new->record_data->location), record_data->location);

	/* insert node into list */
	new->next = NULL;
	if (list->foot==NULL) {
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

/* print formatted record to output file */
void print_record(FILE *output_file, node_t *curr, record_t header_data) {
	fprintf(output_file, "%s --> %s: %s || %s: %s || %s: %s || %s: %s || %s: %s || %s: %s || %s: %s || %s: %s || %s: %s || %s: %s || \n", curr->key, header_data.census_year, curr->record_data->census_year, header_data.block_id, curr->record_data->block_id, header_data.property_id, curr->record_data->property_id, header_data.base_property_id, curr->record_data->base_property_id, header_data.clue_small_area, curr->record_data->clue_small_area, header_data.industry_code, curr->record_data->industry_code, header_data.industry_description, curr->record_data->industry_description, header_data.x_coord, curr->record_data->x_coord, header_data.y_coord, curr->record_data->y_coord, header_data.location, curr->record_data->location); 
}

/* search dictionary for specified key, printing corresponding output */
void search_list(dict_t *list, FILE *output_file, char *search_key, record_t header_data) {

    node_t *curr;
    curr = list->head;
	int found_record = 0;

	/* iterate through dictionary */
    while (curr) {
		if (!strcmp(search_key, curr->key)) {
			/* search key found, print formatted output */
			found_record = 1;
			print_record(output_file, curr, header_data);
		}
        curr = curr->next;
    }
	
	/* record not found, print corresponding statement */
	if (!found_record) {
		fprintf(output_file, "%s --> %s", search_key, NOTFOUND);
	}
}

/* free a record */
void free_record(record_t **record_data) {
    free((*record_data)->census_year);
    free((*record_data)->block_id);
    free((*record_data)->property_id);
    free((*record_data)->base_property_id);
    free((*record_data)->clue_small_area);
    free((*record_data)->industry_code);
    free((*record_data)->industry_description);
    free((*record_data)->x_coord);
    free((*record_data)->y_coord);
    free((*record_data)->location);    
    free((*record_data));
}

/* free a list */
void free_list(dict_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
        free(prev->key);
		free_record(&(prev->record_data));
		free(prev);
	}
	free(list);
}