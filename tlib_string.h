#ifndef TLIB_STRING_H
#define TLIB_STRING_H

#include "tlib_types.h"
#include <string.h>
#include <stdio.h>

typedef struct
{
    I8 *data;
    U32 size;
} String;

String _String(char *cstring);

void StringPrint(String string);

String StringTrimLeft(String string);
String StringTrimRight(String string);
String StringTrim(String string);
String StringSplit(String *string, I8 delim);

B32 StringCompare(String string, char *cstring);

B32 StringIsDigit(char digit);
I32 StringToInt(String string);
F32 StringToFloat(String string);

#endif // TLIB_STRING_H
