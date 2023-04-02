#include "map2.h"
#include "treeops2.h"

record_t *malloc_record(int size);
FILE *open_file(char *file_name, char *mode);
void check_input(line_t line, FILE *input_file);
record_t *malloc_record(int size);
void read_line(line_t line, char *trading_name, record_t **record_data);
void read_field(line_t line, field_t field, int *curr_index);
int still_in_field(line_t line, int curr_index, int contains_commas);
void process_search(char *search_key, double *x_coord, double *y_coord);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Invalid command line arguments: Please pass in a data file and output file");
        exit(EXIT_FAILURE);
    }
	FILE *input_file = open_file(argv[1], "r");
	FILE *output_file = open_file(argv[2], "w");

	char *key;
    malloc_string(&key, MAX_FIELD_LEN);
    line_t header_line;
    check_input(header_line, input_file);
    record_t *header_data = malloc_record(MAX_FIELD_LEN);
    read_line(header_line, key, &header_data);
	record_t *record_data = malloc_record(MAX_FIELD_LEN);
    line_t line;
	tree_t *tree;
	tree = make_empty_tree();
	fgets(line, MAX_LINE_LEN, input_file);
	read_line(line, key, &record_data);
	tree = insert_node(tree, record_data, key);
	tree->root->k_index = X;
	while (fgets(line, MAX_LINE_LEN, input_file)) {
        read_line(line, key, &record_data);
        tree = insert_node(tree, record_data, key);
    }

    char *search_key;
    malloc_string(&search_key, MAX_FIELD_LEN);
    double x_coord, y_coord, radius;

    while (scanf("%lf %lf %lf", &y_coord, &x_coord, &radius) == 3) {
        int comparisons = 0;
 		traverse_tree(output_file, tree, x_coord, y_coord, radius, &comparisons);
        printf("%lf %lf %lf --> %d\n", x_coord, y_coord, radius, comparisons);
    }
    free(search_key);
    free(key);
    free_record(&record_data);
    free_record(&header_data);
    free_tree(tree);
	fclose(input_file);
    fclose(output_file);
	printf("end\n");
    return 0;
}

/* opens file and checks success */
FILE *open_file(char *file_name, char *mode) {

    FILE *file;

    if ((file = fopen(file_name, mode)) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    return file;
}

/* Allocate memory for record with each field of same size */
record_t *malloc_record(int size) {
	record_t *record_data;
	record_data = malloc(sizeof(record_t));
	assert(record_data != NULL);
	record_data->census_year = malloc(sizeof(char) * size+1);
	record_data->block_id = malloc(sizeof(char) * size+1);
	record_data->property_id = malloc(sizeof(char) * size+1);
	record_data->base_property_id = malloc(sizeof(char) * size+1);
    record_data->clue_small_area = malloc(sizeof(char) * size+1);
	record_data->industry_code = malloc(sizeof(char) * size+1);
    record_data->industry_description = malloc(sizeof(char) * size+1);
    record_data->location = malloc(sizeof(char) * size+1);
	return(record_data);
}


/* read one line to check the contents of input file */
void check_input(line_t line, FILE *input_file) {
    if (fgets(line, MAX_LINE_LEN, input_file) == NULL) {
        printf("Error: failed to read output file.");
        exit(EXIT_FAILURE);
    }
}

/* reads a record from an input line into the respective fields */
void read_line(line_t line, char *key, record_t **record_data) {
    int field_num = 1,  curr_index = 0;
    field_t field; 
    char *filler;

    /* read each line, assigning the value to the respective variable
    * depending on the field number */
    while (field_num <= 11) {
        read_field(line, field, &curr_index);

        switch (field_num) {
            case 1:
                strcpy((*record_data)->census_year, field);
                break;
            case 2:
                strcpy((*record_data)->block_id, field);
                break;
            case 3:
                strcpy((*record_data)->property_id, field);
                break;
            case 4:
                strcpy((*record_data)->base_property_id, field);
                break;
            case 5:
                strcpy((*record_data)->clue_small_area, field);
                break;
            case 6:
                strcpy(key, field);
                break;
            case 7:
                strcpy((*record_data)->industry_code, field);
                break;
            case 8:
                strcpy((*record_data)->industry_description, field);
                break;
            case 9:
                (*record_data)->x_coord = strtod(field, &filler);
                break;
            case 10:
                (*record_data)->y_coord = strtod(field, &filler);
                break;
            case 11:
				field[strlen(field)-2] = '\0';
                strcpy((*record_data)->location, field);
                break;
            default:
                break;
        }
        field_num++;
    }
    
}

/* reads one csv field */
void read_field(line_t line, field_t field, int *curr_index) {

    /* check if field contains commas */
    int contains_commas = 0;
    if (line[*curr_index] == QUOTATION) {
        contains_commas = 1;
        *curr_index += 1;
    }

    /* copy string into field variable */
    int i = 0;
    while (still_in_field(line, *curr_index, contains_commas)) {
        field[i++] = line[*curr_index];
        *curr_index += 1;
		if (line[*curr_index] == COMMA && line[*curr_index-1] == COMMA) {
			*curr_index += 1;
		}
		if (line[*curr_index] == QUOTATION && line[*curr_index+1] == QUOTATION) {
			field[i++] = line[*curr_index];
			*curr_index += 2;
		}
    }
    field[i] = '\0';

    /* increase index to beginning of next field */
    if (contains_commas) {
        *curr_index += 1;
    }
    while (line[*curr_index] == COMMA) {
        *curr_index += 1;
    }
}

/* check if index has reached end of a csv field */
int still_in_field(line_t line, int curr_index, int contains_commas) {
    if (line[curr_index] == NEWLINE || line[curr_index] == EOF) {
        return 0;
    } else if (line[curr_index] == QUOTATION && (line[curr_index+1] == COMMA) && contains_commas) {
        return 0;
    } else if (line[curr_index] == COMMA && !contains_commas) {
        return 0;
    }
    return 1;
}
