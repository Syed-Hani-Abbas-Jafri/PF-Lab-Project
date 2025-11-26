/**
 * Project: Simple Palindrome Checker for Beginners
 * Language: C
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>   // For tolower, isalnum

#define MAX_LINE 2000
#define MAX_WORD 256
#define REPORT_FILE "results_report.txt"
#define REPORT_BUF 20000

/* ----------------------------------------------------
   FIXED: DECLARATIONS ADDED
---------------------------------------------------- */
int getLength(const char *str);
int getNextWord(const char *line, int *pos, char *word);
void appendString(char *dest, const char *src);

/* ----------------------------------------------------
   Helper: Get string length (replacement for strlen)
---------------------------------------------------- */
int getLength(const char *str) {
    int i = 0;
    while (str[i] != '\0') i++;
    return i;
}

/* ----------------------------------------------------
   Helper: Append string safely
---------------------------------------------------- */
void appendString(char *dest, const char *src) {
    while (*dest) dest++;   // move to end
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

/* ----------------------------------------------------
   Extract next word (A–Z, a–z, 0–9)
---------------------------------------------------- */
int getNextWord(const char *line, int *pos, char *word) {
    int i = *pos;
    int j = 0;

    // skip non-word chars
    while (line[i] != '\0' && !isalnum((unsigned char)line[i])) {
        i++;
    }

    // no more words
    if (line[i] == '\0') return 0;

    // read word
    while (line[i] != '\0' && isalnum((unsigned char)line[i])) {
        word[j++] = line[i++];
    }

    word[j] = '\0';
    *pos = i;
    return 1;
}

/* ----------------------------------------------------
   Normalize string (lowercase + remove non-alphanumeric)
---------------------------------------------------- */
void normalizeString(const char *in, char *out) {
    int j = 0;
    int i;
    for ( i = 0; in[i] != '\0'; i++) {
        if (isalnum((unsigned char)in[i])) {
            out[j] = tolower((unsigned char)in[i]);
            j++;
        }
    }
    out[j] = '\0';
}

/* ----------------------------------------------------
   Check if a string is a palindrome
---------------------------------------------------- */
int checkPalindrome(const char *str, char *reason) {
    int len = getLength(str);
    int left = 0;
    int right = len - 1;

    if (len == 0) {
        if (reason != NULL) strcpy(reason, "No alphanumeric content");
        return 1;
    }

    while (left < right) {
        if (str[left] != str[right]) {
            if (reason != NULL) {
                sprintf(reason, "Mismatch: position %d & %d ('%c' vs '%c')",
                        left + 1, len - right, str[left], str[right]);
            }
            return 0;
        }
        left++;
        right--;
    }

    if (reason != NULL) strcpy(reason, "Perfect Match");
    return 1;
}

/* ----------------------------------------------------
   MAIN FILE-PROCESSING FUNCTION (UNCHANGED)
---------------------------------------------------- */
void processFile(const char *filename) {
    FILE *in = fopen(filename, "r");
    FILE *out = fopen(REPORT_FILE, "w");

    if (in == NULL || out == NULL) {
        printf("Error accessing files.\n");
        return;
    }

    char line[MAX_LINE];
    char clean[MAX_LINE];
    char reason[MAX_LINE];
    char allPals[REPORT_BUF];
    allPals[0] = '\0';

    int totalLines = 0;
    int linePalindromes = 0;
    int lineNonPalindromes = 0;
    int wordPalindromes = 0;

    fprintf(out, "%-30s | %-15s | %-40s | %s\n",
            "Original", "Result", "Reason", "Palindrome Words");
    fprintf(out, "---------------------------------------------------------------------------------------------------\n");
    printf("\nProcessing: %s...\n", filename);

    while (fgets(line, sizeof(line), in) != NULL) {

        int lineLen = getLength(line);
        if (lineLen > 0 && line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';
        }

        if (line[0] == '\0') continue;

        totalLines++;
        normalizeString(line, clean);

        int isPalindrome = checkPalindrome(clean, reason);
        if (isPalindrome) linePalindromes++;
        else lineNonPalindromes++;

        char linePals[1000];
        linePals[0] = '\0';
        int position = 0;
        char word[MAX_WORD];
        int foundWordPalindrome = 0;

        while (getNextWord(line, &position, word)) {
            char cleanWord[MAX_WORD];
            normalizeString(word, cleanWord);

            if (getLength(cleanWord) >= 2 && checkPalindrome(cleanWord, NULL)) {
                foundWordPalindrome = 1;

                appendString(linePals, word);
                appendString(linePals, " ");

                if (getLength(allPals) + getLength(word) + 2 < REPORT_BUF) {
                    appendString(allPals, word);
                    appendString(allPals, ", ");
                }
                wordPalindromes++;
            }
        }

        fprintf(out, "%-30s | %-15s | %-40s | %s\n",
                line,
                isPalindrome ? "PALINDROME" : "NOT PALINDROME",
                isPalindrome ? "---" : reason,
                foundWordPalindrome ? linePals : "None");
    }

    fprintf(out, "---------------------------------------------------------------------------------------------------\n");
    fprintf(out, "SUMMARY: Lines: %d | Palindromes: %d | Failures: %d | Word Pals: %d\n",
            totalLines, linePalindromes, lineNonPalindromes, wordPalindromes);

    fprintf(out, "\nALL WORD PALINDROMES:\n%s\n", wordPalindromes ? allPals : "None");

    fclose(in);
    fclose(out);

    printf("\n--- Complete ---\n");
    printf("Total Lines: %d\n", totalLines);
    printf("Line Palindromes: %d\n", linePalindromes);
    printf("Word Palindromes: %d\n", wordPalindromes);
    printf("\n--- Exact Words Found ---\n");
    printf("%s\n", wordPalindromes ? allPals : "None found.");
    printf("\nDetailed report: %s\n", REPORT_FILE);
}

/* ----------------------------------------------------
   MAIN MENU (UNCHANGED)
---------------------------------------------------- */
int main() {
    char filename[100];
    int choice;

    printf("========================================\n");
    printf("   Simple Palindrome Checker (CLI)\n");
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

        if (choice == 1) {
            printf("Enter filename (e.g., input.txt): ");
            fgets(filename, sizeof(filename), stdin);

            int len = getLength(filename);
            if (len > 0 && filename[len - 1] == '\n') {
                filename[len - 1] = '\0';
            }

            processFile(filename);
        }
        else if (choice == 2) {
            printf("\nThis program checks:\n");
            printf("- Whole line palindromes\n");
            printf("- Individual words\n");
            printf("- Failure reasons\n");
        }
        else if (choice == 3) {
            printf("Exiting...\n");
            return 0;
        }
        else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
