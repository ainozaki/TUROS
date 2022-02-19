#ifndef PRINT_H_
#define PRINT_H_

void putc(char c);

void puts(char *s);

void putd(unsigned long long val, unsigned char len);

void puth(unsigned long long val, unsigned char len);
void puth_without_0x(unsigned long long val, unsigned char len);
#endif// PRINT_H_
