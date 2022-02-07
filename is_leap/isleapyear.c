#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isLeapYear(int year);

int main(int argc, char *argv[]) {
    char output_str[30];
    int isLeap = isLeapYear(atoi(argv[1]));
    printf(isLeap == 0 ? "%s was not a leap year" : "%s was a leap year", argv[1]);
}

int isLeapYear(int year) {
    if (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0)) return 0;

    return 1;
}