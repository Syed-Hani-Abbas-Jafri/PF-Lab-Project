/**
 * Project: Batch Palindrome Checker with Line + Word Analysis
 * Language: C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 2000
#define MAX_WORD 256
#define OUTPUT_FILENAME "results_report.txt"

// Structure to hold statistics
typedef struct {
    int total_processed;
    int line_palindrome_count;
    int line_non_palindrome_count;
    int total_word_palindromes;
} Stats;


// Normalize function (lowercase + remove non-alphanumeric)
void normalizeString(const char *input, char *output) {
    int j = 0,i;
    for (i = 0; input[i] != '\0'; i++) {
        if (isalnum((unsigned char)input[i])) {
            output[j++] = tolower((unsigned char)input[i]);
        }
    }
    output[j] = '\0';
}


// Check palindrome + explanation
int analyzePalindrome(const char *cleanStr, char *reason) {
    int len = strlen(cleanStr);

    if (len == 0) {
        strcpy(reason, "No alphanumeric content");
        return 1;
    }

    int left = 0, right = len - 1;

    while (left < right) {
        if (cleanStr[left] != cleanStr[right]) {
            sprintf(reason,
                "Mismatch at positions %d and %d: '%c' vs '%c'",
                left + 1, right + 1, cleanStr[left], cleanStr[right]
            );
            return 0;
        }
        left++;
        right--;
    }

    strcpy(reason, "Perfect Match");
    return 1;
}


// Check individual words for palindromes
int isWordPalindrome(const char *word) {
    char clean[MAX_WORD];
    normalizeString(word, clean);

    int len = strlen(clean);
    int left = 0, right = len - 1;

    if (len < 2) return 0;

    while (left < right) {
        if (clean[left] != clean[right]) return 0;
        left++;
        right--;
    }

    return 1;
}


void processFile(const char *inputFilename) {
    FILE *inFile = fopen(inputFilename, "r");
    FILE *outFile = fopen(OUTPUT_FILENAME, "w");

    if (!inFile) {
        printf("Error: Cannot open file '%s'\n", inputFilename);
        return;
    }
    if (!outFile) {
        printf("Error: Cannot create output file.\n");
        fclose(inFile);
        return;
    }

    char line[MAX_LINE_LENGTH];
    char cleanLine[MAX_LINE_LENGTH];
    char reason[MAX_LINE_LENGTH];
    Stats stats = {0};

    // For global word-level palindromes
    char allWordPalindromes[5000] = "";

    // Output header
    fprintf(outFile,
        "%-30s | %-15s | %-40s | %s\n",
        "Original Text", "Line Result", "Reason", "Palindrome Words"
    );
    fprintf(outFile,
        "---------------------------------------------------------------------------------------------------------------\n"
    );

    printf("\nProcessing file: %s...\n", inputFilename);

    while (fgets(line, sizeof(line), inFile)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        stats.total_processed++;

        // Normalize line
        normalizeString(line, cleanLine);

        // Line-level palindrome check
        int isLinePal = analyzePalindrome(cleanLine, reason);

        if (isLinePal)
            stats.line_palindrome_count++;
        else
            stats.line_non_palindrome_count++;

        // ==========================================
        // WORD-LEVEL PALINDROME CHECK
        // ==========================================

        char wordCopy[MAX_LINE_LENGTH];
        strcpy(wordCopy, line);

        char *token = strtok(wordCopy, " ,.!?;:\"()[]{}-");
        char linePalWords[1000] = "";
        int foundWordPal = 0;

        while (token != NULL) {
            if (isWordPalindrome(token)) {
                foundWordPal = 1;
                strcat(linePalWords, token);
                strcat(linePalWords, " ");

                // Add to global palindrome list
                strcat(allWordPalindromes, token);
                strcat(allWordPalindromes, " ");

                stats.total_word_palindromes++;
            }
            token = strtok(NULL, " ,.!?;:\"()[]{}-");
        }

        // Write to output file
        fprintf(outFile,
            "%-30s | %-15s | %-40s | %s\n",
            line,
            isLinePal ? "PALINDROME" : "NOT PALINDROME",
            isLinePal ? "---" : reason,
            foundWordPal ? linePalWords : "None"
        );
    }

    // SUMMARY TO FILE
    fprintf(outFile,
        "---------------------------------------------------------------------------------------------------------------\n"
    );
    fprintf(outFile,
        "SUMMARY:\nLines: %d | Line Palindromes: %d | Line Non-Palindromes: %d | Palindrome Words Found: %d\n",
        stats.total_processed,
        stats.line_palindrome_count,
        stats.line_non_palindrome_count,
        stats.total_word_palindromes
    );

    fprintf(outFile,
        "\nALL PALINDROME WORDS IN FILE:\n%s\n", 
        stats.total_word_palindromes ? allWordPalindromes : "None found"
    );

    fclose(inFile);
    fclose(outFile);

    // ==========================================
    // CONSOLE OUTPUT (CLI)
    // ==========================================
    printf("\n--- Processing Complete ---\n");
    printf("Total Lines Processed:    %d\n", stats.total_processed);
    printf("Line Palindromes Found:   %d\n", stats.line_palindrome_count);
    printf("Word Palindromes Found:   %d\n", stats.total_word_palindromes);
    
    printf("\n--- Exact Palindromes Found ---\n");
    if (stats.total_word_palindromes > 0) {
        printf("%s\n", allWordPalindromes);
    } else {
        printf("None found.\n");
    }

    printf("\n--- Done! ---\nReport saved to: %s\n\n", OUTPUT_FILENAME);
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
                printf("Enter filename: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0;
                processFile(filename);
                break;

            case 2:
                printf("\nThis program checks:\n");
                printf("- If the ENTIRE LINE is a palindrome\n");
                printf("- Palindrome WORDS inside the line\n");
                printf("- Lists ALL palindrome words found in the file\n");
                break;

            case 3:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}
