
#include <stdio.h>
#include "chutil/string.h"
#include "chjson/json.h"

int main(void) {
    json_t *json = new_json_list_from_eles(
        new_json_object_from_kvps(
            new_string_from_literal("Name"), 
            new_json_string(new_string_from_literal("Bobby")),

            new_string_from_literal("Age"), new_json_number(32),

            new_string_from_literal("Tags"), new_json_list_from_eles(
                new_json_string(new_string_from_literal("blue")),
                new_json_string(new_string_from_literal("red")),
                new_json_object_from_kvps(
                    new_string_from_literal("Thingy"), new_json_boolean(true)
                )
            )
        )
    );

    string_t *out = new_string();
    json_to_string_spaced(json, out);

    printf("%s\n", s_get_cstr(out));
}
