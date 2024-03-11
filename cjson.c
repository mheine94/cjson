#include <stdio.h>
#include <stdlib.h>

enum ValueType {
    NULL_T = 1,
    OBJ,
    ARRAY,
    STRING,
    INT,
    FLOAT,
    BOOL
};
enum ParserState {
    STATE_FIND_OPENING_BRACKET = 1,
    STATE_PARSE_NAME,
    STATE_FIND_COLON,
    STATE_PARSE_VALUE,
    STATE_FIND_COMMA_OR_OBJ_END,
    STATE_FIND_CLOSING_BRACKET
};
union ValuePointers {
    void* voidValue;
    int* intValue;
    int* boolValue;
    char* stringValue;
    struct JsonObj* objValue;
    struct JsonArray* arrayValue;
};
struct JsonValue {
    enum ValueType valueType;
    union ValuePointers value;
};
struct JsonArray {
    int length;
    int capacity;
    struct JsonValue* values;
};
struct JsonKeyValue {
    char* key;
    struct JsonValue* value;
};
struct JsonObj {
    int dataCount;
    int dataCapacity;
    struct JsonKeyValue* keyValues;
};
struct JsonString {
    char* string;
    int length;
    int capacity;
};

int billigPow(int exp, int base){
    int result = 1;
    for(int i = 0; i < exp; i++){
        result *= base;
    }
    return result;
}

int findClosingBracket(int start, char* json){
    int openBrackets = 0;
    for(int i = start; json[i] != '\0'; i++){
        if(json[i] == '}'){
            if(openBrackets == 1){
                return i;
            }
            openBrackets--;
        }else if(json[i] == '{'){
            openBrackets++;
        }
    }
    printf("ERROR: did not find closing bracket of { at %d\n", start);
    return -1;
}

void cpChars(int start, int end, char* source, char* dest){
    int destIndex = 0;
    for(int i = start; i < end && source[i] != '\0'; i++){
        dest[destIndex]=source[i];
        destIndex++;
    }
}


void freeJsonValue(struct JsonValue* jsonValue);
void freeJsonObj(struct JsonObj* parsedObj){ 
    int dataCount = parsedObj->dataCount;
    for (int i = 0; i < dataCount; i++)
    {
        free(parsedObj->keyValues[i].key);
        freeJsonValue(parsedObj->keyValues[i].value);
    }
    free(parsedObj);
}

void freeJsonArray(struct JsonArray* array){
    for(int i= 0; i < array->length; i++){
        freeJsonValue(&array->values[i]);
    }
    free(array);
}

void freeJsonValue(struct JsonValue* jsonValue){
    switch (jsonValue->valueType)
    {
    case OBJ:
        freeJsonObj(jsonValue->value.objValue); 
        break;
    case ARRAY:
       freeJsonArray(jsonValue->value.arrayValue); 
    break;
    case NULL_T:
        free(jsonValue);
    break;
    default:
        free(jsonValue->value.voidValue);
        free(jsonValue);
        break;
    }
}

void writeChar(struct JsonString* jsonString, char c){
    if(jsonString->capacity == 0){
        int newCapacity = 50;
        char* newString = malloc(sizeof(char) * newCapacity);
        jsonString->capacity = newCapacity;
        jsonString->string = newString;
    }else if(jsonString->length + 1 > jsonString->capacity){
        int newCapacity = jsonString->capacity * 2;
        char* newString = malloc(sizeof(char) * newCapacity);
        for(int i = 0; i < jsonString->length; i++){
            newString[i] = jsonString->string[i];
        }
        free(jsonString->string);
        jsonString->string = newString;
        jsonString->capacity = newCapacity;
    }
    jsonString->string[jsonString->length] = c;
    jsonString->length++;
    //printf("%c", c);
}

void writeString(struct JsonString* jsonString, char* string){
    for(int i =0; string[i] != '\0'; i++){
        writeChar(jsonString, string[i]);
    }
}

void writeValue(struct JsonString* jsonString, struct JsonValue* jsonValue);
void writeKeyValue(struct JsonString* jsonString, struct JsonKeyValue* kv);
void writeObj(struct JsonString* jsonString, struct JsonObj* parsedObj){
    writeChar(jsonString, '{'); 
    for(int i = 0; i < parsedObj->dataCount; i++){
        writeKeyValue(jsonString, &parsedObj->keyValues[i]);
        if(i+1 <  parsedObj->dataCount){
            writeChar(jsonString, ',');
        }
    }
    writeChar(jsonString, '}'); 
}

char* stringify(struct JsonValue* value){
    struct JsonString result;
    result.length = 0;
    result.capacity = 0;
    writeValue(&result, value);
    writeChar(&result, '\0');
    return result.string;  
}
void writeBool(struct JsonString* jsonString, int boolValue){
    if(boolValue == 0){
        char falseString[] = "false";
        writeString(jsonString, (char* ) &falseString);
    }else{
        char trueString[] = "true";
        writeString(jsonString, (char *) &trueString);
    }
}

void writeInt(struct JsonString* jsonString, int intValue){
     int temp = intValue;
     int maxIntDitgits = 10;
     int foundStart = 0;
     for(int i = maxIntDitgits-1; i >= 0; i--){
        int t = temp / billigPow(i, 10);
        char c = '0' + t;
        if(c > '0' || foundStart == 1){
            foundStart = 1;
            writeChar(jsonString, c);
            temp -= t * billigPow(i, 10);
        }
     }
}
void writeValue(struct JsonString* jsonString, struct JsonValue* jsonValue){
   int valueType = jsonValue->valueType;
   if(valueType == OBJ){
        //printf("write obj\n");
        writeObj(jsonString, jsonValue->value.objValue);
   } else if(valueType == STRING){
        //printf("write string\n");
        writeChar(jsonString, '"');
        writeString(jsonString, jsonValue->value.stringValue);
        writeChar(jsonString, '"');
   } else if(valueType == BOOL){
        //printf("write bool\n");
        writeBool(jsonString, *jsonValue->value.boolValue);
    } else if(valueType == INT){
        //printf("write int\n");
        writeInt(jsonString, *jsonValue->value.boolValue);
   } else if(valueType == NULL_T){
        //printf("write null\n");
        char nullString[] = "null";
        writeString(jsonString, (char *) &nullString);
   } else if(valueType == ARRAY){
        //printf("write array\n");
        writeChar(jsonString, '[');
        struct JsonArray* arr = jsonValue->value.arrayValue;
        for(int i =0; i < arr->length; i++){
            writeValue(jsonString, &arr->values[i]);
            if(i+1 < arr->length){
                writeChar(jsonString, ',');
            }
        }
        writeChar(jsonString, ']');
   }else {
        printf("unknown type %d\n", valueType);
   }
}

void writeKeyValue(struct JsonString* jsonString, struct JsonKeyValue* kv){
    writeChar(jsonString, '"');
    writeString(jsonString, kv->key);
    writeChar(jsonString, '"');
    writeChar(jsonString, ':');
    writeValue(jsonString, kv->value);
}

void printBool(int boolean){
    if(boolean == 1){
        printf("true");
    }
    else if (boolean == 0){
        printf("false");
    }else
    {
        printf("ERROR: unknown bool value %d", boolean);
    }
}


enum PrintMode {
    PRETTY,
    COMPACT
};

void printIndent(enum PrintMode mode, int depth){
    if(mode == PRETTY){
        for(int i= 0; i < depth; i++){
            printf(" ");
            printf(" ");
        }
    }
}
void printNewline(enum PrintMode mode){
    if(mode == PRETTY){
        printf("\n");
    }
}

void printSpace(enum PrintMode mode){
    if(mode == PRETTY){
        printf(" ");
    } 
}

void printValue(struct JsonValue* jsonValue, enum PrintMode mode, int depth);

void printObjValues(struct JsonObj* jsonObj, enum PrintMode mode, int depth){ 
    int dataCount = jsonObj->dataCount;
    for (int i = 0; i < dataCount; i++)
    {
        printIndent(mode, depth);
        printf("\"%s\"", jsonObj->keyValues[i].key);
        printSpace(mode);
        printf(":");
        printSpace(mode);
        printValue(jsonObj->keyValues[i].value, mode, depth);
        
        if (i + 1 < dataCount){
            printf(",");
            printNewline(mode);
        }
    }
}

void printArrayValues(struct JsonArray* jsonArray, enum PrintMode mode, int depth){
    for(int i = 0; i < jsonArray->length; i++){
        if(jsonArray->values[i].valueType != OBJ && jsonArray->values[i].valueType != ARRAY){
            printIndent(mode, depth);
            printValue(&jsonArray->values[i], mode, depth + 1);
        }else if(jsonArray->values[i].valueType == OBJ) {
            printIndent(mode, depth);
            printf("{");
            printNewline(mode);
            printObjValues(jsonArray->values[i].value.objValue, mode, depth+1);
            printNewline(mode);
            printIndent(mode, depth);
            printf("}");
        } else {
            printIndent(mode, depth);
            printf("[");
            printNewline(mode);
            printArrayValues(jsonArray->values[i].value.arrayValue, mode, depth+1);
            printNewline(mode);
            printIndent(mode, depth);
            printf("]");
        }
        if(i+1 < jsonArray->length){
            printf(",");
            printNewline(mode);
        }
    }
}

void printValue(struct JsonValue* jsonValue, enum PrintMode mode, int depth){
    if(jsonValue->valueType == BOOL){
        printBool(*jsonValue->value.boolValue);
    } else if(jsonValue->valueType == INT){
        printf("%d", *jsonValue->value.intValue);
    } else if(jsonValue->valueType == STRING){
        printf("\"%s\"", jsonValue->value.stringValue);
    } else if(jsonValue->valueType == NULL_T){
        printf("null");
    } else if(jsonValue->valueType == ARRAY){
        printIndent(mode, depth);
        printf("[");
        printNewline(mode);
        printArrayValues(jsonValue->value.arrayValue, mode, depth+1);
        printNewline(mode);
        printIndent(mode, depth);
        printf("]");
    } else if(jsonValue->valueType == OBJ){
        if(depth > 0){
            printNewline(mode);
        }
        printIndent(mode, depth);
        printf("{");
        printNewline(mode);
        printObjValues(jsonValue->value.objValue, mode, depth+1);
        printNewline(mode);
        printIndent(mode, depth);
        printf("}");
    }
}

void print(struct JsonValue* jsonValue, enum PrintMode mode){
    printValue(jsonValue, mode, 0);
}

struct ParseStringResult {
    int end;
    char* string;
    int returnCode;
};

struct ParseStringResult parseString(char* json, int start, int end){
    struct ParseStringResult resultObj;
    resultObj.returnCode = 0;
    int stringStart = -1;
    int stringEnd = -1;
    for(int j = start; j < end && stringEnd == -1; j++){
        if(json[j] == ' '){
            // ignore
        }
        else if (json[j] == '"' && stringStart == -1)
        {
            // parse string
            stringStart = j;
        }
        else if (j > 0 && json[j - 1] == '\\')
        {
            //printf("Found escape character \\ \n");
        }
        else if (json[j] == '"')
        {
            stringEnd = j;
            //printf("Found string value from %d to %d\n", stringStart, stringEnd);
        }
    }
    if(stringStart != -1 && stringEnd != -1){
        int size = stringEnd - stringStart;
        resultObj.end = stringEnd;
        char *string = malloc(sizeof(char) * size);
        char *jsonP = json;
        cpChars(stringStart + 1, stringEnd, jsonP, string);
        string[size-1] = '\0';
       
        resultObj.string = string;
        return resultObj; 
    }else {
        printf("Error: did not find closing \" of \" at i=%d\n", start);
        resultObj.returnCode = 1;
        return resultObj;
    }
}

struct ParseIntResult {
    int parsedNumber;
    int returnCode;
    int end;
};

struct ParseIntResult parseInt(char* json, int start, int end){
    //printf("Parsing number at %d\n", start);
    int numEnd = -1;
    for (int j = start; j < end && numEnd == -1; j++)
    {
        if (json[j] < '0' || json[j] > '9')
        {
            numEnd = j;
        }
    }
    if (numEnd == -1)
    {
        //printf("number extends to obj end\n");
        numEnd = end;
    }
    //printf("Number ends at %d\n", numEnd);
    char string[numEnd - start + 1];
    char *stringP = string;
    char *jsonP = json;
    cpChars(start, numEnd, jsonP, stringP);
    string[numEnd - start] = '\0';
    //printf("Found number: %s\n", string);
    int parsedNumber = 0;
    int digit = 0;
    int numLen = numEnd - start;
    for (int j = 0; j < numLen; j++)
    {
        char curr = string[numLen - 1 - j];
        if (curr == '0')
        {
            digit = 0;
        }
        else if (curr == '1')
        {
            digit = 1;
        }
        else if (curr == '2')
        {
            digit = 2;
        }
        else if (curr == '3')
        {
            digit = 3;
        }
        else if (curr == '4')
        {
            digit = 4;
        }
        else if (curr == '5')
        {
            digit = 5;
        }
        else if (curr == '6')
        {
            digit = 6;
        }
        else if (curr == '7')
        {
            digit = 7;
        }
        else if (curr == '8')
        {
            digit = 8;
        }
        else if (curr == '9')
        {
            digit = 9;
        }

        parsedNumber += billigPow(j, 10) * digit;
    }
    //printf("Parsed number is: %d\n", parsedNumber);
    struct ParseIntResult result;
    result.parsedNumber = parsedNumber;
    result.end = numEnd-1;
    result.returnCode = 0;
    return result;
}

struct ParseValueResult {
    struct JsonValue* value;
    int returnCode;
    int end;
};

void push(struct JsonArray* array, struct JsonValue* value){
    if(array->capacity == 0){
        int newCapacity = 50;
        struct JsonValue* valuesP = malloc(sizeof(struct JsonValue) * newCapacity);
        array->capacity = newCapacity;
        array->values = valuesP;
    }else if(array->length + 1 > array->capacity){
        int newCapacity = array->capacity * 2;
        struct JsonValue* newValues = malloc(sizeof(struct JsonValue) * newCapacity);
        for(int i = 0; i < array->length; i++){
            newValues[i] = array->values[i];
        }
        free(array->values);
        array->values = newValues;
        array->capacity = newCapacity;
    }
    array->values[array->length].valueType = value->valueType;
    array->values[array->length].value = value->value;
    array->length++;
}
struct ParseValueResult parseValue(char* json, int start, int end);
struct ParseValueResult parseArray(char* json, int start){
    struct ParseValueResult result;
    result.returnCode = 0;

    struct JsonValue* value = malloc(sizeof(struct JsonValue));
    value->valueType = ARRAY;

    struct JsonArray* array = malloc(sizeof(struct JsonArray));
    array->capacity = 0;
    array->length = 0;
    value->value.arrayValue = array;

    result.value = value;


    int arrayEnd = -1;
    int openArrayBrackets = 0;
    for(int j = start;  json[j] != '\0' && arrayEnd == -1; j++){
        if(json[j] == '['){
            openArrayBrackets++; 
        }else if(json[j] == ']'){
            openArrayBrackets--;
            if(openArrayBrackets == 0){
                arrayEnd = j;
            }
        }
    }
    if(arrayEnd == -1){
        printf("Error: did not find array end");
        result.returnCode = 1;
        return result;
    }
    result.end = arrayEnd;

    for(int i = start+1; i  < arrayEnd; i++){
        if(json[i] == ' ' || json[i] == ','){
            // ignore
        } else if(json[i] == '['){
            struct ParseValueResult parseArrayResult = parseArray(json, i);
            if(parseArrayResult.returnCode != 0){
                result.returnCode = parseArrayResult.returnCode;
                return result;
            }
            push(array, parseArrayResult.value);
            int valueType = array->values[array->length-1].valueType;
            //free(parseArrayResult.value);
            i = parseArrayResult.end;
        } else {
            struct ParseValueResult parseValueResult = parseValue(json, i, arrayEnd);
            if(parseValueResult.returnCode != 0){
                result.returnCode = parseValueResult.returnCode;
                return result;
            }
            push(array, parseValueResult.value);

            //free(parseValueResult.value);
            i = parseValueResult.end;
        }
    }
    return result;
}

int stringLength(char* string){
    int length = 0;
    for(int i = 0; string[i] != '\0'; i++){
        length++;
    }
    return length;
}

int stringEquals(char* a, char* b){
    int aLen = stringLength(a);
    int bLen = stringLength(b);

    if(aLen != bLen){
        return 0;
    }

    for(int i = 0; i < aLen; i++){
        if(a[i] != b[i]){
            return 0;
        }
    }
    return 1;
}

void put(struct JsonObj* jsonObj, char* key, struct JsonValue* value){

    for(int i =0; i< jsonObj->dataCount; i++){
        if(stringEquals(jsonObj->keyValues[i].key, key) == 1){
            // todo free value;
            free(jsonObj->keyValues[i].value);
            jsonObj->keyValues[i].value = value;
            return;
        }
    }

    if (jsonObj->dataCount + 1 > jsonObj->dataCapacity)
    {
        if (jsonObj->dataCount > 0)
        {
            struct JsonKeyValue *oldDataP = jsonObj->keyValues;
            int newCapacity = jsonObj->dataCapacity * 2;
            struct JsonKeyValue *dataP = malloc(newCapacity * sizeof(struct JsonKeyValue));
            jsonObj->keyValues = dataP;
            //printf("Copying %d elements\n", parsedObj->dataCount);
            for (int j = 0; j < jsonObj->dataCount; j++)
            {
                dataP[j].key =  oldDataP[j].key; 
                dataP[j].value  = oldDataP[j].value; 
            }
            free(oldDataP);
            jsonObj->dataCapacity = newCapacity;
        }
        else
        {
            //printf("Creating initial data array of 4\n");
            struct JsonKeyValue *dataP = malloc(4 * sizeof(struct JsonKeyValue));
            jsonObj->keyValues = dataP;
            jsonObj->dataCapacity = 4;
            jsonObj->dataCount = 0;
        }
    }
    jsonObj->keyValues[jsonObj->dataCount].key = key;
    jsonObj->keyValues[jsonObj->dataCount].value = value;
    jsonObj->dataCount+=1;
}

struct ParseValueResult parseValue(char* json, int start, int end){
    struct ParseValueResult result;
    result.returnCode = 0;
    struct JsonValue* value = malloc(sizeof(struct JsonValue));
    value->valueType = NULL_T;
    result.value = value;
    
    int parseState = STATE_PARSE_VALUE;

    for(int i = start;  i < end && json[i] != '\0'; i++){
        char currentChar = json[i];
        if(parseState == STATE_PARSE_NAME){

            struct ParseStringResult parseStringResult = parseString(json, i, end);

            if(parseStringResult.returnCode != 0){
                result.returnCode = parseStringResult.returnCode;
                return result;
            }

            put(value->value.objValue, parseStringResult.string, NULL);

            parseState = STATE_FIND_COLON;
            i = parseStringResult.end;
       } else if (parseState == STATE_FIND_COLON){
            if(currentChar == ' '){
                //ignore
            } else if(currentChar == ':'){
                parseState = STATE_PARSE_VALUE;
                //printf("Found \":\" parsing value next\n");
            }else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                result.returnCode = 1;
                return result;
            }
        }
        else if (parseState == STATE_PARSE_VALUE)
        {
            if(currentChar == ' '){
                //ignore
            } else if(currentChar == '{'){
               

                 if(value->valueType == OBJ){
                    struct ParseValueResult parseObjResult = parseValue(json, i, end);
                    if(parseObjResult.returnCode != 0){
                        result.returnCode = parseObjResult.returnCode;
                        return result;
                    }
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = parseObjResult.value;
                    i = parseObjResult.end;
                    parseState = STATE_FIND_COMMA_OR_OBJ_END;
                }else{
                    value->valueType = OBJ;
                    struct JsonObj* objP = malloc(sizeof(struct JsonObj));
                    objP->dataCapacity = 0;
                    objP->dataCount = 0;
                    value->value.objValue = objP;
                    parseState = STATE_PARSE_NAME;
                }
             
            } else if(currentChar == '['){
         
                struct ParseValueResult parseArrayResult = parseArray(json, i);
 
                if(parseArrayResult.returnCode != 0){
                    result.returnCode = parseArrayResult.returnCode;
                    return result;
                }
                if(value->valueType == OBJ){
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = parseArrayResult.value;
                }else {
                    free(value);
                    result.value = parseArrayResult.value;
                    result.end = parseArrayResult.end;
                    return result;
                }
            
                i = parseArrayResult.end;
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
            } else if(currentChar == '"'){
                struct ParseStringResult parseStringResult = parseString(json, i, end); 
                if(parseStringResult.returnCode != 0){
                    result.returnCode = parseStringResult.returnCode;
                    return result;
                }

                if(value->valueType == OBJ){
                    struct JsonValue* valueP = malloc(sizeof(struct JsonValue));
                    valueP->value.stringValue = parseStringResult.string;
                    valueP->valueType = STRING;
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = valueP;
                }else {
                    value->valueType = STRING;
                    value->value.stringValue = parseStringResult.string;
                    result.end = parseStringResult.end;
                    return result;
                }
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = parseStringResult.end;
            } else if(currentChar >= '0' && currentChar <= '9'){
                struct ParseIntResult parseIntResult = parseInt(json, i, end);
                if(parseIntResult.returnCode != 0){
                    result.returnCode = parseIntResult.returnCode;
                    return result;
                }
                
                int* intP = malloc(sizeof(int));
                intP[0] = parseIntResult.parsedNumber;
                
                if(value->valueType == OBJ){
                    struct JsonValue* valueP = malloc(sizeof(struct JsonValue));
                    valueP->value.intValue = intP;
                    valueP->valueType = INT;
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = valueP;
                }else {
                    value->valueType = INT;
                    value->value.intValue = intP;
                    result.end = parseIntResult.end;
                    return result;
                }

                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = parseIntResult.end;
            } else if(currentChar == 't'){
                int hasR = json[i+1] != '\0' && json[i+1] == 'r';
                int hasU = json[i+2] != '\0' && json[i+2] == 'u';
                int hasE = json[i+3] != '\0' && json[i+3] == 'e';
                if( hasR + hasU + hasE < 3){
                    printf("Error: unexpected bool fragmet\n");
                    result.returnCode = 1;
                    return result;
                }
                
                
                int* intP = malloc(sizeof(int));
                intP[0] = 1;
                if(value->valueType == OBJ){
                    struct JsonValue* valueP = malloc(sizeof(struct JsonValue));
                    valueP->value.intValue = intP;
                    valueP->valueType = BOOL;
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = valueP;
                }else {
                    value->valueType = BOOL;
                    value->value.intValue = intP;
                    result.end = i+3;
                    return result;
                }

                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = i+3;
            }else if(currentChar == 'f'){
                int hasA = json[i+1] != '\0' && json[i+1] == 'a';
                int hasL = json[i+2] != '\0' && json[i+2] == 'l';
                int hasS = json[i+3] != '\0' && json[i+3] == 's';
                int hasE = json[i+4] != '\0' && json[i+4] == 'e';
                if( hasA + hasL + hasS + hasE < 4){
                    printf("Error: unexpected bool fragmet\n");
                    return result;
                }

                int* intP = malloc(sizeof(int));
                intP[0] = 0;
                if(value->valueType == OBJ){
                    struct JsonValue* valueP = malloc(sizeof(struct JsonValue));
                    valueP->value.intValue = intP;
                    valueP->valueType = BOOL;
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = valueP;
                }else {
                    value->valueType = BOOL;
                    value->value.intValue = intP;
                    result.end = i+4;
                    return result;
                }

               parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = i+4;
            }else if(currentChar == 'n'){
                int hasU = json[i+1] != '\0' && json[i+1] == 'u';
                int hasL = json[i+2] != '\0' && json[i+2] == 'l';
                int hasL2 = json[i+3] != '\0' && json[i+3] == 'l';
                if( hasU + hasL + hasL2 < 3){
                    printf("Error: unexpected null fragmet\n");
                    result.returnCode = 1;
                    return result;
                }
                
                if(value->valueType == OBJ){
                    struct JsonValue* valueP = malloc(sizeof(struct JsonValue));
                    valueP->valueType = NULL_T;
                    valueP->value.voidValue = NULL;
                    struct JsonObj* currentObj = value->value.objValue;
                    currentObj->keyValues[currentObj->dataCount-1].value = valueP;
                }else {
                    value->valueType = NULL_T;
                    value->value.voidValue = NULL;
                    result.end = i+3;
                    return result;
                }

                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = i+3;
            } else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                result.returnCode = 1;
                return result;
            }
        }
        else if (parseState == STATE_FIND_COMMA_OR_OBJ_END)
        {
            if(currentChar == ' '){
                //ignore
            } 
            else if(currentChar == ','){
                //printf("found next name\n");
                parseState = STATE_PARSE_NAME;
            } else if(currentChar == '}'){
                // ignore
                result.end = i;
                return result;
            } else{
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                result.returnCode = 1;
                return result;
            }
        } else{
            printf("Error: unknown parser state %d\n", parseState);
                result.returnCode = 1;
                return result;
        }
    }
    return result;
}

int main(){
    struct JsonString string;
    string.capacity = 0;
    string.length = 0;

    char nextChar;
    while(fread(&nextChar, sizeof(char), 1,  stdin) > 0){ 
        writeChar(&string, nextChar);
    }
    writeChar(&string, '\0');
    
    struct ParseValueResult result = parseValue(string.string, 0, string.length);
    if(result.returnCode != 0){
        freeJsonValue(result.value);
        return result.returnCode;
    }    
    //print(result.value, PRETTY);    
    char* jsonString = stringify(result.value);
    printf("%s", jsonString);
    free(jsonString);
    printf("\n");

    freeJsonValue(result.value);
    return 0;
}
