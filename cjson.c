#include <stdio.h>
#include <stdlib.h>

const char OBJ = 0;
const char STRING = 1;
const char INT = 2;
const char FLOAT = 3;
const char ARRAY = 4;
const char BOOL = 5;
const char NULL_T = 6;


struct jsonObj {
    struct jsonKeyValue {
        int type;
        char* key;
        struct jsonObj* objValue;
        char* stringValue;
        int intValue;
    };
    int dataCount;
    int dataCapacity;
    struct jsonKeyValue* data;
};

int billigPow(int exp, int base){
    int result = 1;
    for(int i = 0; i < exp; i++){
        result *= base;
    }
    return result;
}

int findClosingBracket(int currentlyOpenBrackets, char json[]){
    int length = 0;
    for(; json[length] != '\0'; length++){
        
    }
    int openBrack = currentlyOpenBrackets;
    for(int i = length-1; i > 0; i--){
        if(json[i] == '}'){
            if(openBrack == 1){
                return i;
            }
            openBrack--;
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
void printObj(struct jsonObj* parsedObj, int depth){ 
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
        }
        else if (parsedObj->data[i].type == STRING)
        {
            printf("%s\"%s\" : \"%s\"", indentation, parsedObj->data[i].key, parsedObj->data[i].stringValue);
            if (i + 1 < dataCount){
                printf(",\n");
            }else{
                printf("\n");
            }

            free(parsedObj->data[i].key);
            free(parsedObj->data[i].stringValue);
        }
       
    }
    // if(parsedObj.data->key != 0 && parsedObj.data->type == 2 && parsedObj.data->stringValue != 0){
    //    printf("%s : %s\n", parsedObj.data->key, parsedObj.data->stringValue);
    //}
    if(depth == 1){
        printf("}");
    }
}

struct jsonObj* parseObj(char* json, int start, int end){

}

int main(){
    //char json[] = "{ \"arrayKey\": [1, 2, 3 , 4], \"stringInString\": \"bo\\\"test\\\"\" ,\"nullkey\": null, \"key3\": 1234, \"key4\": true,\"key5\":false, \"key6\": { \"subkey1\": \"sval 2\", \"subkey2\": {\"subsubkey1\": 12}}}";

    //char json[] = "{ \"arrayKey\": [1, 2, 3 , 4] }";
    char json[] = "{ \"numkey\": 123,  \"key6\": { \"subkey1\": \"sval 2\", \"subkey2\": { \"subsubkey1\": \"sval 3\" } } }";
    
    
    int openBracketCount = 0;
    for(int i = 0; json[i] != '\0'; i++){
        char currentChar = json[i];
        if(currentChar == '{'){
            openBracketCount++;
        }else if(currentChar == '}'){
            openBracketCount--;
        }
    }

    if(openBracketCount != 0){
        printf("json invalid: bracket count mismatch\n");
        return 1;
    }else{
        printf("json valid\n");
    }

    struct jsonObj parsedObj;
    parsedObj.dataCapacity = 0;
    parsedObj.dataCount = 0;
    
    openBracketCount = 0;
    int parseState = 0;

    int maxObjDepth = 100;
    int objEnds[maxObjDepth];
    struct jsonObj* objs[maxObjDepth];
    int depth = 0;
    int inArray = 1;
    objs[0] = &parsedObj;

    
    for(int i = 0; i < maxObjDepth; i++){
        objEnds[i] = -1;
    } 
     
    for(int i = 0; json[i] != '\0'; i++){
        char currentChar = json[i];
        if(parseState == 0){
            if(currentChar == ' ' || currentChar == '}'){
                //ignore
            } else if(currentChar == '{'){
                openBracketCount++;
                objEnds[depth] = findClosingBracket(openBracketCount, json);
                if(objEnds[depth] != -1){
                    printf("Found closing bracket at %d\n",objEnds[depth]);
                    parseState = 1;
                }else{
                    printf("Error: did not find the closing bracket of i=%d\n", i);
                }
            } else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        } else if(parseState == 1){
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
                    printf("Found name from %d to %d \n", nameStart, nameEnd);
                }
            }
            if (nameStart != -1 && nameEnd != -1){
                char *jsonP = json;
                int size = nameEnd - nameStart;
                char *mem = malloc(sizeof(char) * (size));

                cpChars(nameStart + 1, nameEnd, jsonP, mem);
                mem[size - 1] = '\0';
                printf("Found name: %s\n", mem);
                struct jsonKeyValue keyValue;
                keyValue.key = mem;
                struct jsonObj *parsedObj = objs[depth];
                if (parsedObj->dataCount + 1 > parsedObj->dataCapacity)
                {
                    if (parsedObj->dataCount > 0)
                    {
                        struct jsonKeyValue *oldDataP = parsedObj->data;
                        int newCapacity = parsedObj->dataCapacity * 2;
                        struct jsonKeyValue *dataP = malloc(newCapacity * sizeof(struct jsonKeyValue));
                        parsedObj->data = dataP;
                        printf("Copying %d elements\n", parsedObj->dataCount);
                        for (int j = 0; j < parsedObj->dataCount; j++)
                        {
                            struct jsonKeyValue kvValue;
                            keyValue.type = oldDataP[j].type;
                            kvValue.key = oldDataP[j].key;
                            kvValue.stringValue = oldDataP[j].stringValue;
                            kvValue.objValue = oldDataP[j].objValue;
                            dataP[j] = kvValue;
                        }
                        free(oldDataP);
                        parsedObj->dataCapacity = newCapacity;
                    }
                    else
                    {
                        printf("Creating initial data array of 4\n");
                        struct jsonKeyValue *dataP = malloc(4 * sizeof(struct jsonKeyValue));
                        parsedObj->data = dataP;
                        parsedObj->dataCapacity = 4;
                        parsedObj->dataCount = 0;
                    }
                }
                parsedObj->data[parsedObj->dataCount++] = keyValue;
                parseState = 2;
                i = nameEnd;
            }
            else
            {
                printf("Error: did not find closing \" of \" at i=%d\n", i);
            }
        }
        else if (parseState == 2)
        {
            if(currentChar == ' '){
                //ignore
            } else if(currentChar == ':'){
                parseState = 3;
                printf("Found \":\" parsing value next\n");
            }else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        }
        else if (parseState == 3)
        {
            if(currentChar == ' '){
                //ignore
            } else if(currentChar == '{'){
                objs[depth]->data[objs[depth]->dataCount-1].type = OBJ;

                struct jsonObj* objP = malloc(sizeof(struct jsonObj));
                objP->dataCapacity = 0;
                objP->dataCount = 0;

                objs[depth]->data[objs[depth]->dataCount-1].objValue = objP;
                objs[depth]->data[objs[depth]->dataCount-1].stringValue = 0;

                depth++;
                objs[depth] = objP;
                
                parseState=0;
                i--;
            } else if(currentChar == '['){
                printf("Found array start at %d\n", i);
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
                printf("Found array: %s\n", string);
                printf("Array ends at %d\n", arrayEnd);
                inArray = 1;
                for (int j = i; j < arrayEnd; j++)
                {
                    if (json[i] == '{')
                    {
                        arrayEnd = j;
                    }
                }
                i = arrayEnd;
                parseState = 4;
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
                        printf("Found escape character \\ \n");
                    }
                    else if (json[j] == '"')
                    {
                        stringEnd = j;
                        printf("Found string value from %d to %d\n", stringStart, stringEnd);
                    }
                }
                if(stringStart != -1 && stringEnd != -1){
                    int size = stringEnd - stringStart;
                    char *string = malloc(sizeof(char) * size);
                    char *jsonP = json;
                    cpChars(stringStart + 1, stringEnd, jsonP, string);
                    printf("Found string: %s\n", string);
                    struct jsonObj* parsedObj = objs[depth];
                    parsedObj->data[parsedObj->dataCount - 1].type = STRING;
                    parsedObj->data[parsedObj->dataCount - 1].stringValue = string;
                    parsedObj->data[parsedObj->dataCount - 1].objValue = 0;
                    parseState = 4;
                    i = stringEnd;
                }else {
                    printf("Error: did not find closing \" of \" at i=%d\n", i);
                }
            } else if(currentChar >= '0' && currentChar <= '9'){
                printf("Parsing number at %d\n", i);
                int numEnd = -1;
                for(int j = i; j < objEnds[depth] && numEnd == -1; j++){
                    if(json[j] < '0' || json[j] > '9'){
                        numEnd = j;
                    }
                }
                if(numEnd == -1){
                    printf("number extends to obj end\n");
                    numEnd = objEnds[depth];
                }
                printf("Number ends at %d\n", numEnd);
                char string[numEnd-i];
                char *stringP = string;
                char *jsonP = json;
                cpChars(i, numEnd, jsonP, stringP);
                printf("Found number: %s\n", string);
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
                printf("Parsed number is: %d\n", parsedNumber);
                struct jsonObj* currentObj = objs[depth];
                currentObj->data[currentObj->dataCount].intValue = parsedNumber;
                parseState = 4;
                i = numEnd-1;
            } else if(currentChar == 't'){
                int hasR = json[i+1] != '\0' && json[i+1] == 'r';
                int hasU = json[i+2] != '\0' && json[i+2] == 'u';
                int hasE = json[i+3] != '\0' && json[i+3] == 'e';
                if( hasR + hasU + hasE < 3){
                    printf("Error: unexpected bool fragmet\n");
                    return 1;
                }
                printf("Found boolean true\n");
                parseState = 4;
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
                printf("Found boolean false\n");
                parseState = 4;
                i = i+4;
            }else if(currentChar == 'n'){
                int hasU = json[i+1] != '\0' && json[i+1] == 'u';
                int hasL = json[i+2] != '\0' && json[i+2] == 'l';
                int hasL2 = json[i+3] != '\0' && json[i+3] == 'l';
                if( hasU + hasL + hasL2 < 3){
                    printf("Error: unexpected null fragmet\n");
                    return 1;
                }
                printf("Found null\n");
                parseState = 4;
                i = i+3;
            } else {
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        }
        else if (parseState == 4)
        {
            if(currentChar == ' '){
                //ignore
            } 
            else if(currentChar == '}'){
                printf("Found object end at i=%d\n", i);
                if(depth > 0){
                    objEnds[depth] =-1;
                    depth--;
                }
            }else if(currentChar == ','){
                printf("Found \",\" parsing next key value\n");
                parseState = 1;
            } else{
                printf("Error: unexpected char %c at i=%d\n", currentChar, i);
                return 1;
            }
        }
    }
    if(parsedObj.data != 0){
        printf("Printing parsed json\n");
        printObj(&parsedObj, 0);
    }

    return 0;
}
