#include <stdbool.h>

#ifndef GMETRICL_CSV_H
#define GMETRICL_CSV_H

#define MAX_DATA_LEN 128 ///max length any single data entry
#define DATA_FIELD_CNT 12 ///number of data fields
#define DATA_FIELD_COUNT DATA_FIELD_CNT ///alias
#define MAX_LINE_LEN (MAX_DATA_LEN * (DATA_FIELD_COUNT + 1)) ///max length of any single data line (added one just in case)
  
/**
 * @brief Parse Input file
 */
bool parse_input_file(char* file_name);

#endif
