
#include <stdio.h>
#include "chutil/string.h"
#include "chjson/json.h"

int main(void) {
    json_t *json = new_json_list_from_eles(3, 
        new_json_object_from_kvps(2,
            json_kvp("first_name", new_json_string("Bobby")),
            json_kvp("last_name", new_json_string("Reese"))
        ),
        new_json_object_from_kvps(2,
            json_kvp("first_name", new_json_string("Mark")),
            json_kvp("last_name", new_json_string("Smith"))
        ),
        new_json_object_from_kvps(3,
            json_kvp("first_name", new_json_string("John")),
            json_kvp("last_name", new_json_string("Farl")),
            json_kvp("age", new_json_number(35.0))
        )
    );

    string_t *out = new_string();
    json_to_string_spaced(json, out);

    printf("%s\n", s_get_cstr(out));
}
