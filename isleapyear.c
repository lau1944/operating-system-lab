#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isLeapYear(int year);

int main(int argc, char *argv[]) {
    int year = argc;
    char output_str[30];
    int isLeap = isLeapYear(year);
    sprintf(output_str, isLeap == 0 ? "%d was not a leap year" : "%d was a leap year", year);
    printf("%s", output_str);
}

int isLeapYear(int year) {
    if (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0)) return 0;

    return 1;
}