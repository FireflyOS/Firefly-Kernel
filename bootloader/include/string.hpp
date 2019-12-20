#pragma once

extern void *memset(void *s, char c, int n);
extern int memcmp(const char *s1, const char *s2, int n);
extern void *memcpy(void *dest, const void *src, int n);
extern char *strcpy(char *dest, const char *src);
extern unsigned short strlen(const char *str);
extern int strcmp(const char *s1, const char *s2);
extern char *strchr(const char *str, int c);
extern char *strchrn(const char *str, int c, int n);
extern int toupper(char c);
extern char *strtok(char *s, const char *delimiters);
