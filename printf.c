
/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <limits.h>
#include <stdarg.h>
#include <sys/types.h>
#include <printf.h>
#include <string.h>
#include <math.h>
#include "stdio.h"

#define LONGFLAG     0x00000001
#define LONGLONGFLAG 0x00000002
#define HALFFLAG     0x00000004
#define HALFHALFFLAG 0x00000008
#define SIZETFLAG    0x00000010
#define ALTFLAG      0x00000020
#define CAPSFLAG     0x00000040
#define SHOWSIGNFLAG 0x00000080
#define SIGNEDFLAG           0x00000100
#define LEFTFORMATFLAG       0x00000200
#define LEADZEROFLAG         0x00000400
#define LONGDOUBLEGLAG       0x00000800
#define STRINGOUTPUTFLAG     0x00001000
#define HASPREFORMATNUM      0x00002000


#define DEFAULT_NUM_DECIMALS (6)

static char *longlong_to_string(char *buf, unsigned long long n, int len, uint flag)
{
    int pos = len;
    int negative = 0;
    if((flag & SIGNEDFLAG) && (long long)n < 0) {
        negative = 1;
        n = -n;
    }
    buf[--pos] = 0;

    /* only do the math if the number is >= 10 */
    while(n >= 10) {
        int digit = n % 10;
        n /= 10;
        buf[--pos] = digit + '0';
    }
    buf[--pos] = n + '0';

    if(negative)
        buf[--pos] = '-';
    else if((flag & SHOWSIGNFLAG))
        buf[--pos] = '+';
    return &buf[pos];
}

static char *double_to_string(char *buf, long double n, int len, uint flag, int num_decimals)
{
    int pos = len;
    int negative = 0;
    long int integer_part;
    long double decimal_part;

    if((flag & SIGNEDFLAG) && n < 0.0) {
        negative = 1;
        n = -n;
    }
    buf[--pos] = 0;

    /* do decimals to round up and round down */
    long double temp = (n * pow(10, num_decimals));
    if (temp - (long long int)temp > 0.5f) {
        n += pow(10, -num_decimals);
    }

    integer_part = (long int)n;
    decimal_part = n - (long double)integer_part;

    /* decimal part */
    pos = pos - num_decimals;
    int tmp_pos = pos;
    while(num_decimals-- > 0) {
        int digit = (int)(decimal_part * 10);
        buf[tmp_pos++] = digit + '0';
        decimal_part = decimal_part * 10 - digit;
    }

    buf[--pos] = '.';
    /* integer part */
    while(integer_part >= 10) {
        int digit = integer_part % 10;
        integer_part /= 10;
        buf[--pos] = digit + '0';
    }
    buf[--pos] = integer_part + '0';

    if(negative)
        buf[--pos] = '-';
    else if((flag & SHOWSIGNFLAG))
        buf[--pos] = '+';
    return &buf[pos];
}

static char *longlong_to_hexstring(char *buf, unsigned long long u, int len, uint flag)
{
    int pos = len;
    static const char hextable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    static const char hextable_caps[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    const char *table;
    if((flag & CAPSFLAG))
        table = hextable_caps;
    else
        table = hextable;
    buf[--pos] = 0;
    do {
        unsigned int digit = u % 16;
        u /= 16;

        buf[--pos] = table[digit];
    } while(u != 0);
    return &buf[pos];
}

int vsnprintf(char *str, size_t len, const char *fmt, va_list ap)
{
    char c;
    unsigned char uc;
    const char *s;
    unsigned long long n;
    double dbl;
    long double ldbl;
    void *ptr;
    int flags;
    unsigned int format_num;
    unsigned int pre_format_num;
    size_t chars_written = 0;
    char num_buffer[32];
#define OUTPUT_CHAR(c) do { (*str++ = c); chars_written++; if (chars_written + 1 == len) goto done; } while(0)
#define OUTPUT_CHAR_NOLENCHECK(c) do { (*str++ = c); chars_written++; } while(0)
    for(;;) {
        /* handle regular chars that aren't format related */
        while((c = *fmt++) != 0) {
            if(c == '%')
                break; /* we saw a '%', break and start parsing format */
            OUTPUT_CHAR(c);
        }
        /* make sure we haven't just hit the end of the string */
        if(c == 0)
            break;
        /* reset the format state */
        flags = 0;
        format_num = 0;
        pre_format_num = 0;
        next_format:
        /* grab the next format character */
        c = *fmt++;
        if(c == 0)
            break;

        switch(c) {
            case '0'...'9':
                if (c == '0' && format_num == 0)
                    flags |= LEADZEROFLAG;
                if ((flags & HASPREFORMATNUM)) {
                    format_num *= 10;
                    format_num += c - '0';
                } else {
                    pre_format_num *= 10;
                    pre_format_num += c - '0';
                }
                goto next_format;
            case '.':
                /* XXX for now eat numeric formatting */
                flags |= HASPREFORMATNUM;
                goto next_format;
            case '%':
                OUTPUT_CHAR('%');
                break;
            case 'c':
                uc = va_arg(ap, unsigned int);
                OUTPUT_CHAR(uc);
                break;
            case 's':
                s = va_arg(ap, const char *);
                if(s == 0)
                    s = "<null>";
                flags |= STRINGOUTPUTFLAG;
                goto _output_string;
            case '-':
                flags |= LEFTFORMATFLAG;
                goto next_format;
            case '+':
                flags |= SHOWSIGNFLAG;
                goto next_format;
            case '#':
                flags |= ALTFLAG;
                goto next_format;
            case 'l':
                if(flags & LONGFLAG)
                    flags |= LONGLONGFLAG;
                flags |= LONGFLAG;
                goto next_format;
            case 'h':
                if(flags & HALFFLAG)
                    flags |= HALFHALFFLAG;
                flags |= HALFFLAG;
                goto next_format;
            case 'z':
                flags |= SIZETFLAG;
                goto next_format;
            case 'D':
                flags |= LONGFLAG;
                /* fallthrough */
            case 'i':
            case 'd':
                n = (flags & LONGLONGFLAG) ? va_arg(ap, long long) :
                    (flags & LONGFLAG) ? va_arg(ap, long) :
                    (flags & HALFHALFFLAG) ? (signed char)va_arg(ap, int) :
                    (flags & HALFFLAG) ? (short)va_arg(ap, int) :
                    (flags & SIZETFLAG) ? va_arg(ap, ssize_t) :
                    va_arg(ap, int);
                flags |= SIGNEDFLAG;
                s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags);
                goto _output_string;
            case 'U':
                flags |= LONGFLAG;
                /* fallthrough */
            case 'u':
                n = (flags & LONGLONGFLAG) ? va_arg(ap, unsigned long long) :
                    (flags & LONGFLAG) ? va_arg(ap, unsigned long) :
                    (flags & HALFHALFFLAG) ? (unsigned char)va_arg(ap, unsigned int) :
                    (flags & HALFFLAG) ? (unsigned short)va_arg(ap, unsigned int) :
                    (flags & SIZETFLAG) ? va_arg(ap, size_t) :
                    va_arg(ap, unsigned int);
                s = longlong_to_string(num_buffer, n, sizeof(num_buffer), flags);
                goto _output_string;
            case 'p':
                flags |= LONGFLAG | ALTFLAG;
                goto hex;

            case '*':
                if (flags & HASPREFORMATNUM) {
                    format_num = va_arg(ap, unsigned int);
                } else {
                    pre_format_num = va_arg(ap, unsigned int);
                }
                goto next_format;
            case 'F':
                flags |= LONGDOUBLEGLAG;
            case 'f':
                flags |= SIGNEDFLAG;
                if (flags & LONGDOUBLEGLAG) {
                    ldbl = va_arg(ap, long double);
                    s = double_to_string(num_buffer,
                            ldbl, sizeof(num_buffer),
                            flags,
                            format_num > 0 ? format_num : DEFAULT_NUM_DECIMALS);
                } else {
                    dbl = va_arg(ap, double);
                    s = double_to_string(num_buffer,
                            dbl, sizeof(num_buffer),
                            flags,
                            format_num > 0 ? format_num : DEFAULT_NUM_DECIMALS);
                }
                goto _output_string;
            hex:
            case 'X':
                flags |= CAPSFLAG;
                /* fallthrough */
            case 'x':
                n = (flags & LONGLONGFLAG) ? va_arg(ap, unsigned long long) :
                    (flags & LONGFLAG) ? va_arg(ap, unsigned long) :
                    (flags & HALFHALFFLAG) ? (unsigned char)va_arg(ap, unsigned int) :
                    (flags & HALFFLAG) ? (unsigned short)va_arg(ap, unsigned int) :
                    (flags & SIZETFLAG) ? va_arg(ap, size_t) :
                    va_arg(ap, unsigned int);
                s = longlong_to_hexstring(num_buffer, n, sizeof(num_buffer), flags);
                if(flags & ALTFLAG) {
                    OUTPUT_CHAR('0');
                    OUTPUT_CHAR((flags & CAPSFLAG) ? 'X': 'x');
                }
                goto _output_string;
            case 'n':
                ptr = va_arg(ap, void *);
                if(flags & LONGLONGFLAG)
                    *(long long *)ptr = chars_written;
                else if(flags & LONGFLAG)
                    *(long *)ptr = chars_written;
                else if(flags & HALFHALFFLAG)
                    *(signed char *)ptr = chars_written;
                else if(flags & HALFFLAG)
                    *(short *)ptr = chars_written;
                else if(flags & SIZETFLAG)
                    *(size_t *)ptr = chars_written;
                else
                    *(int *)ptr = chars_written;
                break;

            default:
                OUTPUT_CHAR('%');
                OUTPUT_CHAR(c);
                break;
        }
        /* move on to the next field */
        continue;
        /* shared output code */
        _output_string:
        if (flags & LEFTFORMATFLAG) {
            /* left justify the text */
            uint count = 0;
            while(*s != 0 &&((format_num > 0 && flags & STRINGOUTPUTFLAG) ? (count < format_num) : 1)) {
                OUTPUT_CHAR(*s++);
                count++;
            }
            /* pad to the right (if necessary) */
            for (; format_num > count; format_num--)
                OUTPUT_CHAR(' ');
        } else {
            /* right justify the text (digits) */
            size_t string_len = strlen(s);
            uint count = 0;
            char outchar = (flags & LEADZEROFLAG) ? '0' : ' ';
            if (pre_format_num > 0) {
                for (; pre_format_num > format_num; pre_format_num--)
                    OUTPUT_CHAR(outchar);
            }
            for (; format_num > string_len; format_num--)
                OUTPUT_CHAR(outchar);
            /* output the string */
            while(*s != 0 && ((format_num > 0 && flags & STRINGOUTPUTFLAG) ? (count < format_num) : 1)) {
                OUTPUT_CHAR(*s++);
                count++;
            }
        }
        continue;
    }
    done:
    /* null terminate */
    OUTPUT_CHAR_NOLENCHECK('\0');
    chars_written--; /* don't count the null */
#undef OUTPUT_CHAR
#undef OUTPUT_CHAR_NOLENCHECK
    return chars_written;
}

int vsprintf(char *str, const char *fmt, va_list ap)
{
    return vsnprintf(str, INT_MAX, fmt, ap);
}
