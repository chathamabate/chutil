
#ifndef CHJSON_JSON_PATHS_H
#define CHJSON_JSON_PATHS_H

#include "chutil/string.h"

typedef enum _json_path_part_type_t {
    JSON_PATH_KEY = 0,
    JSON_PATH_INDEX
} json_path_part_type_t;

typedef struct _json_path_part_t {
    json_path_part_type_t part_type;
    union {
        string_t *key;
        size_t index;
    };
} json_path_part_t;

typedef struct _json_path_t {

} json_path_t;

json_path_t *new_json_path(void);
void delete_json_path(json_path_t *jp);

void jp_add_key(json_path_t *jp, string_t *key);
void jp_add_index(json_path_t *jp, size_t i);


#endif
