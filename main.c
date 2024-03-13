#include <stdio.h>
#include "libcjson.h"

int main(){
    JSON* json = CJSON.parseStdIn();
    CJSON.print(json, CJSON_PRETTY);
    printf("\n");
    CJSON.free(json);
    return 0;
}