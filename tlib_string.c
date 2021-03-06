#include "tlib_string.h"

String _String(char *cstring)
{
    String result = {0};
    result.data = (I8 *)cstring;
    result.size = (U32)strlen(cstring);
    return result;
}

void StringPrint(String string)
{
    printf("%.*s\n", string.size, string.data);
}

String StringTrimLeft(String string)
{
    String result = string;
    while(result.size > 0 && *result.data == ' ')
    {
        result.data++;
        result.size--; 
    }
    return result;
}

String StringTrimRight(String string)
{
    String result = string;
    while(result.size > 0 && *(result.data + (result.size-1)) == ' ')
    {
        result.size--;
    }
    return result;
}

String StringTrim(String string)
{
    String result = StringTrimLeft(StringTrimRight(string));
    return result;
}

String StringSplit(String *string, I8 delim)
{
    String result = *string;

    for(U32 i = 0; i < string->size; ++i)
    {
        if(*(string->data + i) == delim)
        {
            string->data += (i + 1);
            string->size -= (i + 1);

            result.size = i;
            return result;
        }
    }
    return result;
}

B32 StringCompare(String string, char *cstring)
{
    if(string.size != (U32)strlen(cstring))
    {
        return false;
    }
    
    for(U32 i = 0; i < string.size; ++i)
    {
        if(string.data[i] != cstring[i])
        {
            return false;
        }
    }
    return true;
}

B32 StringIsDigit(char digit)
{
    B32 result = (('0' <= digit) && ('9' >= digit));
    return result;
}

I32 StringToInt(String string)
{
    I32 integerNum = 0;
    B32 negative = false;
    I32 base = 10;
    
    for(U32 i = 0; i < string.size; ++i)
    {
        char *number= (char *)string.data + i;
        if(StringIsDigit(*number))
        {
            integerNum = integerNum*base+(*number-'0');
        }
        else if(*number == '-')
        {
            negative = true;
        }
        
    }
    return negative ? -integerNum : integerNum;
}

F32 StringToFloat(String string)
{
    F32 floatNum = 0;
    B32 negative = false;
    I32 base = 10;

    U32 startDecimal = 0;
    
    for(U32 i = 0; i < string.size; ++i)
    {
        char *number = (char *)string.data + i;
        if(StringIsDigit(*number))
        {
            floatNum = floatNum*base+(*number-'0');
        }
        else if(*number == '-')
        {
            negative = true;
        }
        else if(*number == '.')
        {
            startDecimal = i + 1;
            break;
        }
    }

    if(startDecimal)
    {
        F32 added = 0;
        F32 exponent = 1;
        for(U32 i = startDecimal; i < string.size; ++i)
        {
            char *number = (char *)string.data + i;
            if(StringIsDigit(*number))
            {
                added = added*base+(*number-'0');
                exponent *= base;
            }
        }
        floatNum = floatNum + (added / exponent);
    }
    return negative ? -floatNum : floatNum;
}
