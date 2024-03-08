#include <stdio.h>
#include <stdlib.h>

const char OBJ = 0;
const char STRING = 1;
const char INT = 2;
const char FLOAT = 3;
const char ARRAY = 4;
const char BOOL = 5;
const char NULL_T = 6;

const char STATE_FIND_OPENING_BRACKET = 0;
const char STATE_PARSE_NAME = 1;
const char STATE_FIND_COLON = 2;
const char STATE_PARSE_VALUE = 3;
const char STATE_FIND_COMMA_OR_OBJ_END = 4;
const char STATE_FIND_CLOSING_BRACKET = 5;

struct JsonObj {
    struct JsonArray {
        int arrayType;
        int length;
        struct JsonObj* objValues;
        char** stringValues;
        int* intValues;
        int* boolValues;
    };
    struct JsonKeyValue {
        int type;
        char* key;
        struct JsonObj* objValue;
        char* stringValue;
        int intValue;
        struct JsonArray* arrayValue;
    };
    int dataCount;
    int dataCapacity;
    struct JsonKeyValue* data;
};

int billigPow(int exp, int base){
    int result = 1;
    for(int i = 0; i < exp; i++){
        result *= base;
    }
    return result;
}

int findClosingBracket(int currentlyOpenBrackets, int start, char json[]){
    int length = 0;
    for(; json[length] != '\0'; length++){
        
    }
    int openBrack = currentlyOpenBrackets;
    for(int i = start; i < length; i++){
        if(json[i] == '}'){
            if(openBrack == 1){
                return i;
            }
            openBrack--;
        }else if(json[i] == '{'){
            openBrack++;
        }
    }
    return -1;
}

void cpChars(int start, int end, char* source, char* dest){
    //printf("copying from %d to %d\n", start, end);
    int destIndex = 0;
    for(int i = start; i < end && source[i] != '\0'; i++){
        //printf("i=%d char=%c\n", i , source[i]);
        dest[destIndex]=source[i];
        destIndex++;
    }
    //printf("copied: %s", dest);
}
void printObj(struct JsonObj* parsedObj, int depth){ 
    depth++;
    if(depth == 1) {
        printf("{\n");
    }

    int dataCount = parsedObj->dataCount;
    char indentation[depth*2 + 1];
    for(int i = 0; i < (depth * 2) + 1; i++){
        indentation[i] = ' ';
    }
    indentation[depth*2] = '\0';
    for (int i = 0; i < dataCount; i++)
    {
        if (parsedObj->data[i].type == OBJ)
        {
            printf("%s\"%s\" : {\n",indentation, parsedObj->data[i].key);
            printObj(parsedObj->data[i].objValue, depth);
            printf("%s}\n", indentation);
            free(parsedObj->data[i].key);
            free(parsedObj->data[i].objValue);
        }else if(parsedObj->data[i].type == INT){
            printf("%s\"%s\" : %d\n",indentation, parsedObj->data[i].key, parsedObj->data[i].intValue);
        }else if(parsedObj->data[i].type == BOOL){
            if(parsedObj->data[i].intValue == 1){
                printf("%s\"%s\" : true\n",indentation, parsedObj->data[i].key);
            }
            else if (parsedObj->data[i].intValue == 0){
                printf("%s\"%s\" : false\n",indentation, parsedObj->data[i].key);
            }else
            {
                printf("ERROR: unknown bool value %d", parsedObj->data[i].intValue);
            }
        }else if(parsedObj->data[i].type == NULL_T){
            printf("%s\"%s\" : null\n", indentation, parsedObj->data[i].key);
        }else if (parsedObj->data[i].type == STRING) {
            printf("%s\"%s\" : \"%s\"", indentation, parsedObj->data[i].key, parsedObj->data[i].stringValue);
            if (i + 1 < dataCount){
                printf(",\n");
            }else{
                printf("\n");
            }

            free(parsedObj->data[i].key);
            free(parsedObj->data[i].stringValue);
        }else if(parsedObj->data[i].type == ARRAY){
            printf("%s\"%s\" : [ ", indentation, parsedObj->data->key);
            struct JsonArray* arr = parsedObj->data[i].arrayValue;
            for(int j = 0; j < arr->length; j++){
                if(arr->arrayType == OBJ){
                    printObj(&arr->objValues[j], depth);
                }else if(arr->arrayType == STRING) {
                    printf("\"%s\"", arr->stringValues[j]);
                    free(arr->stringValues[j]);
                }else if(arr->arrayType == BOOL) {
                    if(arr->boolValues[j] == 1){
                        printf("true");
                    }else if(arr->boolValues[j] == 0){
                        printf("false");
                    }else {
                        printf("ERROR: unknown bool value %d", arr->boolValues[j]);
                    }
                }else if(arr->arrayType == INT) {
                    printf("%d", arr->intValues[j]);
                }else if(arr->arrayType == NULL_T) {
                    printf("null");
                }else {
                    printf("ERROR: array type not yet implemented");
                }
                if (j + 1 < arr->length){
                    printf(",");
                }
            }
            printf("%s]", indentation);
            free(parsedObj->data[i].key);
            free(arr);
            if (i + 1 < dataCount){
                printf(",\n");
            }else{
                printf("\n");
            }
        }
    }
    if(depth == 1){
        printf("}");
    }
}
struct ParseStringResult {
    int length;
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
        int size = stringEnd - (stringStart+1);
        resultObj.length = size;
        char *string = malloc(sizeof(char) * size);
        char *jsonP = json;
        cpChars(stringStart + 1, stringEnd, jsonP, string);
        string[size] = '\0';
        //printf("Found string: %s\n", string);
        resultObj.string = string;
        return resultObj; 
    }else {
        printf("Error: did not find closing \" of \" at i=%d\n", start);
        resultObj.returnCode = -1;
        return resultObj;
    }
}

struct ParseIntResult {
    int parsedNumber;
    int returnCode;
    int length;
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
    result.length = numLen;
    result.returnCode = 0;
    return result;
}

int parseObj(char* json, struct JsonObj* result, int start, int end){
    result->dataCapacity = 0;
    result->dataCount = 0;
    
    int openBracketCount = 0;
    int parseState = 0;

    int maxObjDepth = 100;
    int objEnds[maxObjDepth];
    struct JsonObj* objs[maxObjDepth];
    int depth = 0;
    objs[0] = result;

    
    for(int i = 0; i < maxObjDepth; i++){
        objEnds[i] = -1;
    }

    for(int i = start; (end == -1 || i < end)  && json[i] != '\0'; i++){
        char currentChar = json[i];
        if(parseState == STATE_FIND_OPENING_BRACKET){
            if(currentChar == ' ' || currentChar == '}'){
                //ignore
            } else if(currentChar == '{'){
                openBracketCount++;
                objEnds[depth] = findClosingBracket(openBracketCount, i+1, json);
                if(objEnds[depth] != -1){
                    //printf("Found closing bracket at %d\n",objEnds[depth]);
                    parseState = STATE_PARSE_NAME;
                }else{
                    printf("Error: did not find the closing bracket of i=%d\n", i);
                }
            } else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        } else if(parseState == STATE_PARSE_NAME){
            int nameStart = -1;
            int nameEnd = -1;

            for (int j = i; j < objEnds[depth] && nameEnd == -1; j++){
                if (json[j] == ' '){
                    // ignore
                }
                else if (json[j] == '"' && nameStart == -1){
                    // parse name
                    nameStart = j;
                }
                else if (json[j] == '"'){
                    nameEnd = j;
                    //printf("Found name from %d to %d \n", nameStart, nameEnd);
                }
            }
            if (nameStart != -1 && nameEnd != -1){
                char *jsonP = json;
                int size = nameEnd - nameStart;
                char *mem = malloc(sizeof(char) * (size));

                cpChars(nameStart + 1, nameEnd, jsonP, mem);
                mem[size - 1] = '\0';
                //printf("Found name: %s\n", mem);
                struct JsonKeyValue keyValue;
                keyValue.key = mem;
                struct JsonObj *parsedObj = objs[depth];
                if (parsedObj->dataCount + 1 > parsedObj->dataCapacity)
                {
                    if (parsedObj->dataCount > 0)
                    {
                        struct JsonKeyValue *oldDataP = parsedObj->data;
                        int newCapacity = parsedObj->dataCapacity * 2;
                        struct JsonKeyValue *dataP = malloc(newCapacity * sizeof(struct JsonKeyValue));
                        parsedObj->data = dataP;
                        //printf("Copying %d elements\n", parsedObj->dataCount);
                        for (int j = 0; j < parsedObj->dataCount; j++)
                        {
                            dataP[j].type =  oldDataP[j].type;
                            dataP[j].key =  oldDataP[j].key;
                            dataP[j].stringValue  = oldDataP[j].stringValue; 
                            dataP[j].objValue  = oldDataP[j].objValue; 
                            dataP[j].arrayValue  = oldDataP[j].arrayValue;
                            dataP[j].intValue  = oldDataP[j].intValue;
                        }
                        free(oldDataP);
                        parsedObj->dataCapacity = newCapacity;
                    }
                    else
                    {
                        //printf("Creating initial data array of 4\n");
                        struct JsonKeyValue *dataP = malloc(4 * sizeof(struct JsonKeyValue));
                        parsedObj->data = dataP;
                        parsedObj->dataCapacity = 4;
                        parsedObj->dataCount = 0;
                    }
                }
                parsedObj->data[parsedObj->dataCount++] = keyValue;
                parseState = STATE_FIND_COLON;
                i = nameEnd;
            }
            else
            {
                printf("Error: did not find closing \" of \" at i=%d\n", i);
            }
        }
        else if (parseState == STATE_FIND_COLON)
        {
            if(currentChar == ' '){
                //ignore
            } else if(currentChar == ':'){
                parseState = STATE_PARSE_VALUE;
                //printf("Found \":\" parsing value next\n");
            }else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        }
        else if (parseState == STATE_PARSE_VALUE)
        {
            if(currentChar == ' '){
                //ignore
            } else if(currentChar == '{'){
                objs[depth]->data[objs[depth]->dataCount-1].type = OBJ;

                struct JsonObj* objP = malloc(sizeof(struct JsonObj));
                objP->dataCapacity = 0;
                objP->dataCount = 0;

                objs[depth]->data[objs[depth]->dataCount-1].objValue = objP;
                objs[depth]->data[objs[depth]->dataCount-1].stringValue = 0;

                depth++;
                objs[depth] = objP;
                
                parseState=STATE_FIND_OPENING_BRACKET;
                i--;
            } else if(currentChar == '['){
                objs[depth]->data[objs[depth]->dataCount-1].type = ARRAY;
                
                struct JsonArray* jsonArray = malloc(sizeof(struct JsonArray));
                jsonArray->length = 0;
                objs[depth]->data[objs[depth]->dataCount-1].arrayValue = jsonArray;  

                //printf("Found array start at %d\n", i);
                int arrayEnd = -1;
                for(int j = i; j < objEnds[depth] && arrayEnd == -1; j++){
                    if(json[j] == ']'){
                        arrayEnd = j;
                    }
                }
                if(arrayEnd == -1){
                    printf("Error: did not find array end");
                    return 1;
                }

                char string[arrayEnd-i];
                char *stringP = string;
                char *jsonP = json;
                cpChars(i, arrayEnd+1, jsonP, stringP);
                //printf("Found array: %s\n", string);
                //printf("Array ends at %d\n", arrayEnd);
                
                int arrayType = -1;
                
                for(int j = i; j < arrayEnd && arrayType == -1; j++){
                    if(json[j] == '{'){
                        //printf("array is object type\n");
                        arrayType = OBJ;
                    } else if(json[j] >= '0' && json[j] <= '9'){
                        //printf("array is int type\n");
                        arrayType = INT;
                    } else if(json[j] == 't' || json[j] == 'f'){
                        //printf("array is bool type\n");
                        arrayType = BOOL;
                    } else if(json[j] == '"'){
                        //printf("array is string type\n");
                        arrayType = STRING;
                    } else if(json[j] == 'n'){
                        //printf("array is null type\n");
                        arrayType = NULL_T;
                    }
                }
            
                if(arrayType == OBJ){
                    int lenght = 0;
                    for (int j = i; j < arrayEnd; j++)
                    {
                        if (json[j] == '{')
                        {
                            int index = findClosingBracket(openBracketCount , j+1, json);
                            j = index;                        
                            lenght++;
                        }
                    } 
                    //printf("array has length %d\n", lenght);
                    if(lenght > 0){
                        struct JsonObj* objectsMem = malloc(sizeof(struct JsonObj) * lenght);
                        jsonArray->length = lenght;
                        jsonArray->objValues = objectsMem;
                        jsonArray->arrayType = OBJ;                     
                        int count = 0; 
                        for (int j = i; j < arrayEnd; j++)
                        {
                            if (json[j] == '{')
                            {
                                //printf("parsing obj in array\n");
                                int index = findClosingBracket(openBracketCount , j+1, json);
                                //printf("obj ends at index %d\n", index);
                                //printf("obj is:\n");
                                char objstring[index - j];
                                char *stringP = objstring;
                                char *jsonP = json;
                                cpChars(j, index+1, jsonP, stringP);
                                printf("%s\n", stringP);
                                int returnCode = parseObj(json, &objectsMem[count], j, index+1);
                                if(returnCode == -1){
                                    return returnCode;
                                }
                                //printf("parsed array obj success\n");
                                count++;
                                j = index;                        
                            }
                        }
                    }
                } else if(arrayType == STRING){
                    jsonArray->length = 0;
                    jsonArray->arrayType = STRING;

                    int lenght = 0;
                    int inString = 0;
                    for (int j = i; j < arrayEnd; j++)
                    {
                        if(json[j] == '"'){
                            if(j-1 >= i && json[j-1] != '\\'){
                               if(inString == 1){
                                inString = 0;
                                lenght++;
                               }else {
                                 inString = 1;
                               }
                            }else {
                                inString = 1;
                            }
                        }
                       
                    } 
                    jsonArray->length = lenght;
                    //printf("array has length %d\n", lenght);
                    if(lenght > 0){
                        char** stringArray = malloc(sizeof(char*) * lenght);
                        jsonArray->stringValues = stringArray;
                        int searchComma = 0;
                        int count = 0;
                        for(int j = i; j < arrayEnd; j++){
                            if(searchComma == 1 ){
                                if( json[j] == ','){
                                    searchComma = 0;
                                }
                            } else {
                                struct ParseStringResult res = parseString(json, j, arrayEnd);
                                if(res.returnCode != 0){
                                    printf("Error: could not parse string array value");
                                    return 1;
                                }
                                
                                //printf("String is length %d\n", res.length);
                                //printf("String is: %s\n", res.string);
                                stringArray[count] = res.string;
                                count++;
                                j += res.length;
                                searchComma = 1;
                            }
                        }
                    }
                } else if(arrayType == BOOL){
                    jsonArray->arrayType = BOOL;
                    int lenght = 0;
                    int inValue = 0;
                    for(int j = i; j < arrayEnd; j++){
                       if(inValue == 0 && json[j] == 't' || json[j] == 'f'){
                           lenght++;
                           inValue = 1;
                       } else {
                           inValue = 0;
                       }
                    }
                    if(lenght > 0){
                        //printf("Arrray has length %d\n", lenght);
                        jsonArray->length = lenght;
                        jsonArray->boolValues = malloc(sizeof(int) * lenght); 
                        int count = 0;
                        for(int j = i; j < arrayEnd; j++){
                            if(json[j] == 't'){
                                int hasR = json[j+1] != '\0' && json[j+1] == 'r';
                                int hasU = json[j+2] != '\0' && json[j+2] == 'u';
                                int hasE = json[j+3] != '\0' && json[j+3] == 'e';
                                if( hasR + hasU + hasE < 3){
                                    printf("Error: unexpected bool fragmet\n");
                                    return 1;
                                }
                                //printf("Found boolean true\n");
                                jsonArray->boolValues[count] = 1;
                                count++;
                                j +=3;
                            }else if(json[j] == 'f'){
                                int hasA = json[j+1] != '\0' && json[j+1] == 'a';
                                int hasL = json[j+2] != '\0' && json[j+2] == 'l';
                                int hasS = json[j+3] != '\0' && json[j+3] == 's';
                                int hasE = json[j+4] != '\0' && json[j+4] == 'e';
                                if( hasA + hasL + hasS + hasE < 4){
                                    printf("Error: unexpected bool fragmet\n");
                                    return 1;
                                }
                                //printf("Found boolean false\n");
                                jsonArray->boolValues[count] = 0;
                                count++;
                                j += 4;
                            }
                        }
                    }
                } else if(arrayType == INT){
                    jsonArray->arrayType = INT;
                    jsonArray->length = 0;
                    int inValue = 0;
                    for(int j = i; j < arrayEnd; j++){
                       if(inValue == 0 && json[j] >= '0' && json[j] <= '9'){
                           jsonArray->length++;
                           inValue = 1;
                       } else {
                           inValue = 0;
                       }
                    }
                    if(jsonArray->length > 0){
                        //printf("Arrray has length %d\n", jsonArray->length);
                        jsonArray->intValues = malloc(sizeof(int) * jsonArray->length);
                        int count = 0;
                        for(int j = i; j < arrayEnd; j++){
                            if(json[j] >= '0' && json[j] <= '9'){
                                struct ParseIntResult result = parseInt(json, j, arrayEnd);
                                if(result.returnCode != 0){
                                   printf("ERROR: could not parse number");
                                   return result.returnCode; 
                                }
                                jsonArray->intValues[count] = result.parsedNumber;
                                count++;
                                j += result.length;
                            }
                        }
                    }
                } else if(arrayType == NULL_T){
                   jsonArray->arrayType = NULL_T;
                    jsonArray->length = 0;
                    int inValue = 0;
                    for(int j = i; j < arrayEnd; j++){
                       if(inValue == 0 && json[j] >= 'a' && json[j] <= 'z'){
                           jsonArray->length++;
                           inValue = 1;
                       } else {
                           inValue = 0;
                       }
                    }
                    if(jsonArray->length > 0){
                        //printf("Arrray has length %d\n", jsonArray->length);
                        int count = 0;
                        for(int j = i; j < arrayEnd; j++){
                            if(json[j] == 'n'){
                                int hasU = json[j + 1] != '\0' && json[j + 1] == 'u';
                                int hasL = json[j + 2] != '\0' && json[j + 2] == 'l';
                                int hasL2 = json[j + 3] != '\0' && json[j + 3] == 'l';
                                if (hasU + hasL + hasL2 < 3)
                                {
                                    printf("Error: unexpected null fragmet\n");
                                    return 1;
                                }
                                //printf("Found null\n");

                                count++;
                                j += 3;
                            }
                        }
                        jsonArray->length = count;
                    }  
                }
                
                i = arrayEnd;
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
            } else if(currentChar == '"'){
                int stringStart = -1;
                int stringEnd = -1;
                for(int j = i; j < objEnds[depth] && stringEnd == -1; j++){
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
                    int size = stringEnd - (stringStart+1);
                    char *string = malloc(sizeof(char) * size);
                    char *jsonP = json;
                    cpChars(stringStart + 1, stringEnd, jsonP, string);
                    string[size] = '\0';
                    //printf("Found string: %s\n", string);
                    struct JsonObj* parsedObj = objs[depth];
                    parsedObj->data[parsedObj->dataCount - 1].type = STRING;
                    parsedObj->data[parsedObj->dataCount - 1].stringValue = string;
                    parsedObj->data[parsedObj->dataCount - 1].objValue = 0;
                    parseState = STATE_FIND_COMMA_OR_OBJ_END;
                    i = stringEnd;
                }else {
                    printf("Error: did not find closing \" of \" at i=%d\n", i);
                }
            } else if(currentChar >= '0' && currentChar <= '9'){
                objs[depth]->data[objs[depth]->dataCount-1].type = INT;
                //printf("Parsing number at %d\n", i);
                int numEnd = -1;
                for(int j = i; j < objEnds[depth] && numEnd == -1; j++){
                    if(json[j] < '0' || json[j] > '9'){
                        numEnd = j;
                    }
                }
                if(numEnd == -1){
                    //printf("number extends to obj end\n");
                    numEnd = objEnds[depth];
                }
                //printf("Number ends at %d\n", numEnd);
                char string[numEnd-i];
                char *stringP = string;
                char *jsonP = json;
                cpChars(i, numEnd, jsonP, stringP);
                //printf("Found number: %s\n", string);
                int parsedNumber = 0;
                int digit = 0;
                int numLen = numEnd - i;
                for(int j = 0; j < numLen; j++){
                    char curr = string[numLen - 1 - j];
                    if(curr == '0'){
                        digit = 0;
                    } else if(curr == '1'){
                        digit = 1;
                    } else if(curr == '2'){
                        digit = 2;
                    } else if(curr == '3'){
                        digit = 3;
                    } else if(curr == '4'){
                        digit = 4;
                    } else if(curr == '5'){
                        digit = 5;
                    } else if(curr == '6'){
                        digit = 6;
                    } else if(curr == '7'){
                        digit = 7;
                    } else if(curr == '8'){
                        digit = 8;
                    } else if(curr == '9'){
                        digit = 9;
                    }

                    parsedNumber += billigPow(j, 10) * digit;
                }
                //printf("Parsed number is: %d\n", parsedNumber);
                struct JsonObj* currentObj = objs[depth];
                currentObj->data[currentObj->dataCount-1].intValue = parsedNumber;
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = numEnd-1;
            } else if(currentChar == 't'){
                int hasR = json[i+1] != '\0' && json[i+1] == 'r';
                int hasU = json[i+2] != '\0' && json[i+2] == 'u';
                int hasE = json[i+3] != '\0' && json[i+3] == 'e';
                if( hasR + hasU + hasE < 3){
                    printf("Error: unexpected bool fragmet\n");
                    return 1;
                }
                struct JsonObj* currentObj = objs[depth];
                currentObj->data[currentObj->dataCount-1].intValue = 1;
                currentObj->data[currentObj->dataCount-1].type = BOOL;
                //printf("Found boolean true\n");
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = i+3;
                
            }else if(currentChar == 'f'){
                int hasA = json[i+1] != '\0' && json[i+1] == 'a';
                int hasL = json[i+2] != '\0' && json[i+2] == 'l';
                int hasS = json[i+3] != '\0' && json[i+3] == 's';
                int hasE = json[i+4] != '\0' && json[i+4] == 'e';
                if( hasA + hasL + hasS + hasE < 4){
                    printf("Error: unexpected bool fragmet\n");
                    return 1;
                }
                struct JsonObj* currentObj = objs[depth];
                currentObj->data[currentObj->dataCount-1].intValue = 0;
                currentObj->data[currentObj->dataCount-1].type = BOOL;
                //printf("Found boolean false\n");
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = i+4;
            }else if(currentChar == 'n'){
                objs[depth]->data[objs[depth]->dataCount-1].type = NULL_T;
                int hasU = json[i+1] != '\0' && json[i+1] == 'u';
                int hasL = json[i+2] != '\0' && json[i+2] == 'l';
                int hasL2 = json[i+3] != '\0' && json[i+3] == 'l';
                if( hasU + hasL + hasL2 < 3){
                    printf("Error: unexpected null fragmet\n");
                    return 1;
                }
                //printf("Found null\n");
                parseState = STATE_FIND_COMMA_OR_OBJ_END;
                i = i+3;
            } else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        }
        else if (parseState == STATE_FIND_COMMA_OR_OBJ_END)
        {
            if(currentChar == ' '){
                //ignore
            } 
            else if(currentChar == '}'){
                //printf("Found object end at i=%d\n", i);
                if(depth > 0){
                    objEnds[depth] =-1;
                    depth--;
                }
            }else if(currentChar == ','){
                //printf("Found \",\" parsing next key value\n");
                parseState = STATE_PARSE_NAME;
            } else{
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        }
    }
}

int main(){
    char json1[] = "{ \"stringInString\": \"out\\\"in\\\"\" , \"key2\": null, \"key3\": 1234, \"key4\": true,\"key5\": false, \"key6\": { \"subkey1\": \"sval 2\", \"subkey2\": {\"subsubkey1\": 12}}}";
    char json2[] = "{ \"arrayKey\": [\"stringval1\", \"stringval2\"] , \"key2\": 2}";
    char json3[] = "{ \"numkey\": 123,  \"key6\": { \"subkey1\": \"sval 2\", \"subkey2\": { \"subsubkey1\": \"sval 3\" } } }";
    char json4[] = "{ \"key\": [ 1, 2, 3] }";
    char json5[] = "{ \"key\": [ null, null, null] }";  
    
    char* jsonP[] = { json1, json2, json3, json4, json5};
     
    for(int i = 0; i < 5; i++){
        struct JsonObj parsedObj;
        int returnCode = parseObj(jsonP[i], &parsedObj, 0, -1);
        if(returnCode != 0){
            //todo free parsed obj
            return returnCode;
        }    
     
        if(parsedObj.data != 0){
            printf("Printing parsed json %d\n", i);
            printObj(&parsedObj, 0);
            printf("\n");
        }
    } 
    return 0;
}
