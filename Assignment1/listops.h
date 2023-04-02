#ifndef LIST /* header guard */
#define LIST

#include <stdio.h>
#include <stdlib.h> 
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* buffer sizes for allocating memory */
#define START_SIZE 8
#define MAX_FIELD_LEN 128
#define MAX_LINE_LEN 512

#define NOTFOUND "NOTFOUND"

typedef char field_t[MAX_FIELD_LEN];
typedef char line_t[MAX_LINE_LEN];

/* define struct to store business data fields */
typedef struct {
    char *census_year, *block_id, *property_id, *base_property_id, *industry_code,
	*clue_small_area, *industry_description, *x_coord, *y_coord, *location;
} record_t;

/* node for linked list */
typedef struct node node_t;
struct node {
	record_t *record_data;
	char *key;
	node_t *next;
};

/* list type */
typedef struct {
	node_t *head;
	node_t *foot;
} dict_t;

dict_t *make_empty_list(void);
void is_empty_list(dict_t *list);
void malloc_string(char **string, int size);
void malloc_copy_string(char **dest, char *source);
dict_t *insert_node(dict_t *list, char *key, record_t *record_data);
void search_list(dict_t *list, FILE *output_file, char *search_key, record_t header_data);
void free_record(record_t **record_data);
void free_list(dict_t *list);



#endif