#include <stdio.h>
#include <stdlib.h>

int isLeapYear(int year);

int main(int argc, char *argv[]) {
    int year;
    printf("Enter an year: ");
    scanf("%d", &year);  
    int isLeap = isLeapYear(year);
    printf(isLeap == 0 ? "%d was not a leap year" : "%d was a leap year", year);
}

int isLeapYear(int year) {
    if (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0)) return 0;

    return 1;
}