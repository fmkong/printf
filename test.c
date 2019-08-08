//
// Created by fanming on 19-8-8.
//
#include <stdio.h>
#include "printf.h"

uint8_t string_buffer[100] = { 0 };


//static bool dumpBinaryPutC(void* p, char c) {
//    *(*(char**)p)++ = c;
//    return true;
//}

void osUsartLog(const char *format, ...)
{
    va_list args;
    char* p = string_buffer;

    va_start(args, format);
    int dataLength = vsnprintf( string_buffer, sizeof(string_buffer), format, args);
    va_end(args);
    printf("%s", string_buffer);
}


void main()
{
    osUsartLog("vsnprintf %lld\n", (uint64_t)-12348777222);

    osUsartLog("vsnprintf %f\n", -123456.1234567);
    osUsartLog("vsnprintf %f\n", -0.1234567);
    osUsartLog("vsnprintf %f\n", 0.01234567);

    osUsartLog("vsnprintf %f\n", -0.9999995);
    printf("printf %f\n", -0.9999999);

    osUsartLog("vsnprintf %.4f\n", 0.01234567);
    printf("printf %.4f\n", 0.01234567);


    osUsartLog("vsnprintf %.*s\n", 6, "abcdefghi");
    printf("printf %.*s\n", 6, "abcdefghi");

    osUsartLog("vsnprintf %.8s\n", "abcdefghi");
    printf("printf %.8s\n", "abcdefghi");

    osUsartLog("vsnprintf %8s\n", "abcdefghi");
    printf("printf %8s\n", "abcdefghi");
}