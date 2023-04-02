#include "dict.h"
#include "listops.h"

FILE *open_file(char *file_name, char *mode);
void check_input(line_t line, FILE *input_file);
record_t *malloc_record(int size);
void read_line(line_t line, char *trading_name, record_t **record_data);
void read_field(line_t line, field_t field, int *curr_index);
int still_in_field(line_t line, int curr_index, int contains_commas);

int main(int argc, char *argv[]) {

    /* read names and open input and output files */
    FILE *input_file = open_file(argv[1], "r");
    FILE *output_file = open_file(argv[2], "w");
    
    /* create dictionary */
    dict_t *list;
    list = make_empty_list();

    /* check file input is valid, then read and store column names from header record */
    char *key;
    malloc_string(&key, MAX_FIELD_LEN);
    line_t header_line;
    check_input(header_line, input_file);
    record_t *header_data = malloc_record(MAX_FIELD_LEN);
    read_line(header_line, key, &header_data);

    /* allocate placeholder record to read each line into */
    record_t *record_data = malloc_record(MAX_FIELD_LEN);
    line_t line;
    
    /* read file data, store in nodes and add to foot of dictionary */
    while (fgets(line, MAX_LINE_LEN, input_file)) {
        read_line(line, key, &record_data);
        list = insert_node(list, key, record_data);
    }   

    /* take inputs from stdin and search for each key in dictionary, printing
    * formatted data in output file */
    char *search_key;
    malloc_string(&search_key, MAX_FIELD_LEN);
    strcpy(search_key, "Ithacan Philanthropic Society \"The Ulysses\"");
    fgets(search_key, MAX_LINE_LEN, stdin);
    while (fgets(search_key, MAX_LINE_LEN, stdin)) {
        search_key[strlen(search_key)-1] = '\0';
        search_list(list, output_file, search_key, *header_data);
    }

    /* free allocated memory and close files */
    free(search_key);
    free(key);
    free_record(&record_data);
    free_record(&header_data);
    free_list(list);
    fclose(input_file);
    fclose(output_file);
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
	record_data->x_coord = malloc(sizeof(char) * size+1);
	record_data->y_coord = malloc(sizeof(char) * size+1);
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
                strcpy((*record_data)->x_coord, field);
                break;
            case 10:
                strcpy((*record_data)->y_coord, field);
                break;
            case 11:
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

