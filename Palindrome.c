/**
 * Project: Batch Palindrome Checker (Simplified)
 * Language: C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> // Added for cleaner logic

#define MAX_LINE 2000
#define MAX_WORD 256
#define REPORT_FILE "results_report.txt"
#define REPORT_BUF 20000 

typedef struct {
    int total, line_pals, line_non_pals, word_pals;
} Stats;

// Normalize: lowercase + keep only alphanumeric
void normalizeString(const char *in, char *out) {
    int j = 0;
    for (int i = 0; in[i]; i++) 
        if (isalnum((unsigned char)in[i])) out[j++] = tolower((unsigned char)in[i]);
    out[j] = '\0';
}

// Unified Palindrome Checker
// If 'reason' is NULL, it performs a silent check (for words).
// If 'reason' is provided, it fills it with failure details (for lines).
bool checkPalindrome(const char *str, char *reason) {
    int len = strlen(str), l = 0, r = len - 1;
    
    if (len == 0) {
        if (reason) strcpy(reason, "No alphanumeric content");
        return true;
    }

    while (l < r) {
        if (str[l] != str[r]) {
            if (reason) sprintf(reason, "Mismatch: %d & %d ('%c' vs '%c')", l+1, len-r, str[l], str[r]);
            return false;
        }
        l++; r--;
    }
    
    if (reason) strcpy(reason, "Perfect Match");
    return true;
}

void processFile(const char *filename) {
    FILE *in = fopen(filename, "r");
    FILE *out = fopen(REPORT_FILE, "w");
    if (!in || !out) { printf("Error accessing files.\n"); return; }

    char line[MAX_LINE], clean[MAX_LINE], reason[MAX_LINE], allPals[REPORT_BUF] = "";
    Stats s = {0};

    fprintf(out, "%-30s | %-15s | %-40s | %s\n", "Original", "Result", "Reason", "Palindrome Words");
    fprintf(out, "---------------------------------------------------------------------------------------------------\n");
    printf("\nProcessing: %s...\n", filename);

    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\n")] = 0;
        if (!*line) continue;

        s.total++;
        normalizeString(line, clean);
        
        // Check Line
        bool isPal = checkPalindrome(clean, reason);
        (isPal) ? s.line_pals++ : s.line_non_pals++;

        // Check Words
        char copy[MAX_LINE], linePals[1000] = "";
        strcpy(copy, line);
        char *tok = strtok(copy, " ,.!?;:\"()[]{}-");
        bool foundWord = false;

        while (tok) {
            char cleanWord[MAX_WORD];
            normalizeString(tok, cleanWord);
            if (strlen(cleanWord) >= 2 && checkPalindrome(cleanWord, NULL)) {
                foundWord = true;
                strcat(linePals, tok); strcat(linePals, " ");
                if (strlen(allPals) + strlen(tok) + 2 < REPORT_BUF) {
                    strcat(allPals, tok); strcat(allPals, ", ");
                }
                s.word_pals++;
            }
            tok = strtok(NULL, " ,.!?;:\"()[]{}-");
        }

        fprintf(out, "%-30s | %-15s | %-40s | %s\n", 
                line, isPal ? "PALINDROME" : "NOT PALINDROME", isPal ? "---" : reason, foundWord ? linePals : "None");
    }

    // Summary
    fprintf(out, "---------------------------------------------------------------------------------------------------\n");
    fprintf(out, "SUMMARY: Lines: %d | Palindromes: %d | Failures: %d | Word Pals: %d\n", 
            s.total, s.line_pals, s.line_non_pals, s.word_pals);
    fprintf(out, "\nALL WORD PALINDROMES:\n%s\n", s.word_pals ? allPals : "None");

    fclose(in); fclose(out);

    // CLI Output
    printf("\n--- Complete ---\nTotal Lines: %d\nLine Pals: %d\nWord Pals: %d\n", s.total, s.line_pals, s.word_pals);
    printf("\n--- Exact Words Found ---\n%s\n", s.word_pals ? allPals : "None found.");
    printf("\nDetailed report: %s\n", REPORT_FILE);
}

int main() {
    char filename[100];
    int choice;

    printf("========================================\n");
    printf("   Advanced Palindrome Checker (CLI)    \n");
    printf("========================================\n");

    while (1) {
        printf("\n1. Process a text file\n");
        printf("2. Help / About\n");
        printf("3. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf("Invalid input.\n");
            continue;
        }
        getchar(); 

        switch (choice) {
            case 1:
                printf("Enter filename (e.g., input.txt): ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0; 
                processFile(filename);
                break;
            case 2:
                printf("\nThis program checks:\n- Whole line palindromes\n- Individual words\n- Failure reasons\n");
                break;
            case 3:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}

