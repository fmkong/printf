//
// Created by fanming on 19-8-8.
//
#include <stdio.h>
#include "printf.h"

uint8_t string_buffer[150] = { 0 };


void testLog(const char *format, ...)
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
    testLog("vsnprintf %lld\n", (uint64_t)-12348777222);

    testLog("vsnprintf %f\n", -123456.1234567);
    testLog("vsnprintf %f\n", -0.1234567);
    testLog("vsnprintf %f\n", 0.01234567);

    testLog("vsnprintf %f\n", -0.9999995);
    printf("printf %f\n", -0.9999999);

    testLog("vsnprintf %.4f\n", 0.01234567);
    printf("printf %.4f\n", 0.01234567);

    testLog("vsnprintf %.5f\n", 0.012);
    printf("printf %.5f\n", 0.012);

    testLog("vsnprintf %.05d\n", 32);
    printf("printf %.05d\n", 32);

    testLog("vsnprintf .*s %.*s\n", 6, "abcdefghi");
    printf("printf .*s %.*s\n", 6, "abcdefghi");

    testLog("vsnprintf .8s %.8s\n", "abcdefghi");
    printf("printf .8s %.8s\n", "abcdefghi");

    testLog("vsnprintf 8.4s %8.4s\n", "abcdefghi");
    printf("printf 8.4s %8.4s\n", "abcdefghi");

    testLog("vsnprintf *.*s %*.*s\n", 8,4,"abcdefghi");
    printf("printf *.*s %*.*s\n", 8, 4, "abcdefghi");

    int test = 0;
    testLog("vsnprintf %d, %f, %p, %lld, %ld, %u, %x, %X, 8.4s %8.4s\n",
            100, -12.34, &test, 1234568973LL, 1234567L, 2345122, 0x84756af, 0x876543ab, "abcdefghi");
    printf("printf %d, %f, %p, %lld, %ld, %u, %x, %X, 8.4s %8.4s\n",
               100, -12.34, &test, 1234568973LL, 1234567L, 2345122, 0x84756af, 0x876543ab, "abcdefghi");
}