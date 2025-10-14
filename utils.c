// --- FILE: utils.c ---
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Get validated integer input
int getIntInput(const char* prompt, int min, int max) {
    int value, valid = 0;

    while (!valid) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            if (value >= min && value <= max) valid = 1;
            else printf("Enter a value between %d and %d.\n", min, max);
        } else {
            printf("Invalid input. Try again.\n");
            clearInputBuffer();
        }
        if (!valid) clearInputBuffer();
    }
    clearInputBuffer();
    return value;
}

// Get string input
void getStringInput(const char* prompt, char* buffer, int maxLen) {
    printf("%s", prompt);
    if (fgets(buffer, maxLen, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    }
}

// Get double input
double getDoubleInput(const char* prompt) {
    double value;
    printf("%s", prompt);
    while (scanf("%lf", &value) != 1) {
        printf("Invalid input. Try again.\n");
        clearInputBuffer();
        printf("%s", prompt);
    }
    clearInputBuffer();
    return value;
}

// Display banner
void displayBanner() {
    printf("\n-------------------------------------------------------------\n");
    printf("|        DISASTER RELIEF RESOURCE MANAGEMENT SYSTEM          |\n");
    printf("|    Efficient Resource Allocation via Graph Network         |\n");
    printf("-------------------------------------------------------------\n");
}

// Display menu
void displayMainMenu() {
    printf("\n============================== MAIN MENU ==============================\n");
    printf("1. Display City Network\n");
    printf("2. Add New City\n");
    printf("3. Add Road Between Cities\n");
    printf("4. Raise Disaster Request\n");
    printf("5. Allocate Resources (Process Next Request)\n");
    printf("6. Display Allocation Status\n");
    printf("7. View Allocation Logs\n");
    printf("8. Exit\n");
    printf("=======================================================================\n");
}

// Wait for user input
void pressEnterToContinue() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}

// Clear screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Display log file
void displayLogFile() {
    FILE* fp = fopen("allocation_logs.txt", "r");
    if (!fp) {
        printf("\nNo allocation logs found.\n");
        return;
    }

    printf("\n==================== ALLOCATION LOG FILE ====================\n\n");
    char line[256];
    while (fgets(line, sizeof(line), fp)) printf("%s", line);
    fclose(fp);
}

// Trim whitespace
void trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

// Case-insensitive compare
int stringCompare(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2))
            return 0;
        str1++;
        str2++;
    }
    return (*str1 == *str2);
}
