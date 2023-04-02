#include "dict.h"
#include "listops.h"

void check_input(line_t line, FILE *input_file);
void read_line(line_t line, char *trading_name, record_t **record_data);
FILE *open_file(char *file_name, char *mode);
void malloc_key(char *key);
void read_field(line_t line, field_t field, int *curr_index);
int still_in_field(line_t line, int curr_index, int contains_commas);

int main(int argc, char *argv[]) {

    FILE *input_file = open_file(argv[1], "r");
    FILE *output_file = open_file(argv[2], "w");

    list_t *list;
    list = make_empty_list();

    char *key = (char*)malloc(sizeof(char) * MAX_FIELD_LEN);
    assert(key != NULL);

    record_t *record_data = malloc_record(MAX_FIELD_LEN);
    
    line_t header_line, line;

    check_input(header_line, input_file);
    record_t *header_data = malloc_record(MAX_FIELD_LEN);
    read_line(header_line, key, &header_data);

    while (fgets(line, MAX_LINE_LEN, input_file)) {
        read_line(line, key, &record_data);
        list = insert_at_foot(list, key, record_data);
    }   

    char *search_key = malloc(sizeof(char) * MAX_FIELD_LEN);
    strcpy(search_key, "Amott Quality Meats");

    while (fgets(search_key, MAX_LINE_LEN, stdin)) {
        search_key[strlen(search_key)-1] = '\0';
        search_list(list, output_file, search_key, *header_data);
    }

    free(key);
    free_record(record_data);
    free_record(header_data);
    free_list(list);
    fclose(input_file);
    fclose(output_file);
}

void check_input(line_t line, FILE *input_file) {
    if (fgets(line, MAX_LINE_LEN, input_file) == NULL) {
        printf("Error: failed to read output file.");
        exit(EXIT_FAILURE);
    }
}


void read_line(line_t line, char *key, record_t **record_data) {
    int field_num = 1,  curr_index = 0;
    field_t field; 

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

void read_field(line_t line, field_t field, int *curr_index) {

    int contains_commas = 0;
    if (line[*curr_index] == QUOTATION) {
        contains_commas = 1;
        *curr_index += 1;
    }

    int i = 0;
    while (still_in_field(line, *curr_index, contains_commas)) {
        field[i++] = line[*curr_index];
        *curr_index += 1;
    }
    field[i] = '\0';

    if (contains_commas) {
        *curr_index += 1;
    }
    while (line[*curr_index] == COMMA) {
        *curr_index += 1;
    }
}

int still_in_field(line_t line, int curr_index, int contains_commas) {
    if (line[curr_index] == NEWLINE) {
        return 0;
    } else if (line[curr_index] == QUOTATION && line[curr_index+1] == '\r') { 
        printf("here");
        return 0;
    } else if (line[curr_index] == COMMA && !contains_commas) {
        return 0;
    }
    return 1;
}

FILE *open_file(char *file_name, char *mode) {

    FILE *file;

    if ((file = fopen(file_name, mode)) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    return file;
}