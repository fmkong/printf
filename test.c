//
// Created by fanming on 19-8-8.
//
#include <stdio.h>
#include "printf.h"

uint8_t string_buffer[100] = { 0 };


static bool dumpBinaryPutC(void* p, char c) {
    *(*(char**)p)++ = c;
    return true;
}

void osUsartLog(const char *format, ...)
{
    va_list args;
    char* p = string_buffer;

    va_start(args, format);
    int dataLength = (uint16_t)cvprintf(dumpBinaryPutC, PRINTF_FLAG_CHRE|PRINTF_FLAG_SHORT_DOUBLE, &p, format, args);
    va_end(args);
    printf("%s", string_buffer);
}


void main()
{
    osUsartLog("test %d\n", -1234);
}