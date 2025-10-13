// --- FILE: utils.h ---
#ifndef UTILS_H
#define UTILS_H

// Input validation functions
int getIntInput(const char* prompt, int min, int max);
void getStringInput(const char* prompt, char* buffer, int maxLen);
double getDoubleInput(const char* prompt);
void clearInputBuffer();

// Menu utilities
void displayMainMenu();
void displayBanner();
void pressEnterToContinue();

// File logging
void displayLogFile();
void clearScreen();

// String utilities
void trim(char* str);
int stringCompare(const char* str1, const char* str2);

#endif // UTILS_H