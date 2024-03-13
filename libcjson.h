enum ValueType {
    CJSON_NULL = 1,
    CJSON_OBJ,
    CJSON_ARRAY,
    CJSON_STRING,
    CJSON_INT,
    CJSON_FLOAT,
    CJSON_BOOL
};

enum PrintMode {
    CJSON_PRETTY,
    CJSON_COMPACT
};
typedef struct JSON JSON;

typedef struct {
    char* (*stringify)(JSON*);
    JSON* (*parse)(char*);
    void (*print)(JSON*, enum PrintMode);
    JSON* (*newObj)();
    JSON* (*newIntValue)(int value);
    JSON* (*newStringValue)(char* value);
    JSON* (*newBooleanValue)(int boolean);
    JSON* (*newArrayValue)();
    JSON* (*newNullValue)();
    void (*arrayPush)(JSON* value);
    int (*arrayLen)(JSON* array);
    JSON* (*arrayGet)(JSON* array, int i);
    void (*arraySet)(JSON* array, JSON* value, int i);
    void (*objGet)(char* key);
    void (*objSet)(JSON* obj, char* key, JSON* value);
    enum ValueType (*getType)(JSON* json);
    JSON* (*parseStdIn)();
    void (*free)(JSON* json);
} ModuleFunctions;

extern const ModuleFunctions CJSON;