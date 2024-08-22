
#include <stdio.h>
#include "chutil/string.h"
#include "chjson/json.h"

int main(void) {
    json_t *json = new_json_list_from_eles(
        new_json_object_from_kvps(
            "Name", new_json_string("Bobby"),
            "Age", new_json_number(32.4)
        )
    );

    string_t *out = new_string();
    json_to_string_spaced(json, out);

    printf("%s\n", s_get_cstr(out));
}
