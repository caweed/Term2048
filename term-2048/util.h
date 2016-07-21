#ifndef TERM_2048_UTIL_H
#define TERM_2048_UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define STR(x) "x"

void swap(int *, int *);
void copy(int (*)[4], const int (*)[4]);
bool equal(const int (*)[4], const int (*)[4]);
int saveToFile(const int (*)[4], const char *, const char *);
int resumeFromFile(int (*)[4], const char *, const char *);

void rollRightNinety(int (*)[4]);
void rollAroundH(int (*)[4]);
void rollAroundV(int (*)[4]);

char *strcatFullFilePath(char **, const char *, const char *);
char *subString(const char *, char *, int, int);
char **getSubStrs(const char *, int);

WINDOW *createChoiceBox(const char *, const char *, const char *, chtype, chtype);


#endif