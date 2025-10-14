// --- FILE: utils.h ---
#ifndef UTILS_H
#define UTILS_H

// Input functions
int getIntInput(const char* prompt, int min, int max);
void getStringInput(const char* prompt, char* buffer, int maxLen);
double getDoubleInput(const char* prompt);
void clearInputBuffer();

// Menu/display
void displayBanner();
void displayMainMenu();
void pressEnterToContinue();
void clearScreen();
void displayLogFile();

// String utilities
void trim(char* str);
int stringCompare(const char* str1, const char* str2);

#endif
