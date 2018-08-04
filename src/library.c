#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "library.h"

int peek_char(void);

int get_date(char s[]);

int get_description(char s[]);

typedef struct Inputs {
    char prompt[100];
    char error[100];

    int (*f)(char s[]);
} Input;

typedef struct Facts {
    char date[20];
    char description[1000];
} Fact;

typedef struct Words {
    char word[25];
    int count;
} Word;

typedef struct FactsOrWords {
    Fact fact;
    Word word;
} FactOrWord;

void input_wrapper(Input input);

void merge_sort(FactOrWord *arr, int n, int t);

void merge(FactOrWord *arr, FactOrWord *left, int left_num, FactOrWord *right, int right_num, int t);

Word set_word(char *token);

void word_count(Fact facts[], int num_facts, Word words[]);

extern int word_accumulator;
word_accumulator = 0;

extern int word_count_max;
word_count_max = 0;

int main() {
    int i = 0;

    char buf[1000];

    Input date = {"Please enter date. (YYYY-MM-DD)\n", "Invalid format. Try again.\n", get_date};
    Input description = {"Please enter a description.\n", "Oops. Something went wrong. Try again.\n", get_description};

    while (i < PROGRAM_LIMIT) {
        ++i;
        printf("Fact %d of %d\n", i, PROGRAM_LIMIT);
        input_wrapper(date);
        input_wrapper(description);
    }

    printf("That's it. You're done!\n");

    //analysis
    Fact facts[PROGRAM_LIMIT];
    FactOrWord for_sort[PROGRAM_LIMIT];


    FILE *ptr_file;
    ptr_file = fopen(FILE_OPEN, "r");

    i = 0;

    while (fgets(buf, 1000, ptr_file) != NULL) {
        sscanf(buf, "%s%1000[^\n]", facts[i].date, facts[i].description);
        for_sort[i].fact = facts[i];
        ++i;
    }

    fclose(ptr_file);

    //sort the facts
    size_t n = sizeof(facts) / sizeof(facts[0]);

    merge_sort(for_sort, (int) n, 0);

    ptr_file = fopen(OUT_FILE, "w");

    fprintf(ptr_file, "FACT PATTERN ANALYSIS\n\n");
    fprintf(ptr_file, "Sorted Facts:\n\n");
    for (i = 0; i < PROGRAM_LIMIT; i++) {
        fprintf(ptr_file, "%s\t%s\n", for_sort[i].fact.date, for_sort[i].fact.description);
    }

    //count the words
    Word words[100];

    word_count(facts, (int) n, words);

    FactOrWord word_sort[word_accumulator];

    for (i = 0; i < word_accumulator; i++) {
        word_sort[i].word = words[i];
    }

    merge_sort(word_sort, word_accumulator, 1);

    int k;

    fprintf(ptr_file, "\nWord Count:\n\n");
    for (k = 0; k < word_accumulator; k++) {
        fprintf(ptr_file, "%s: %d\n", word_sort[k].word.word, word_sort[k].word.count + 1);
    }
    fclose(ptr_file);

    return 0;

}

int peek_char(void) {
    int c;

    c = getchar();

    if (c != EOF) ungetc(c, stdin);

    return c;
}

void input_wrapper(Input input) {
    int valid = 0;
    char line[100];
    int c;
    FILE *fp;

    do {
        printf("%s", input.prompt);
        valid = (*input.f)(line);

        if (valid == 1) {
            fp = fopen(FILE_OPEN, "a");
            fprintf(fp, "%s", line);
            fclose(fp);
        } else {
            printf("%s", input.error);
        }
        while ((c = getchar()) != '\n' && c != EOF) {}
    } while (valid == 0);

}


int get_description(char s[]) {
    int c;
    int i = 0;
    while ((c = peek_char()) != '\n' && c != EOF) {
        s[i++] = (char) getchar();
    }
    s[i++] = '\n';
    s[i] = '\0';
    return 1;
}


int get_date(char s[]) {
    int res = 1;
    int c;
    int pos[] = {4, 7};
    int pos_index = 0;
    int i = 0;
    int digit_accumulator = 0;

    while ((c = peek_char()) != '\n' && c != EOF) {
        if (isdigit(c)) {
            if (i == pos[pos_index]) {
                res = 0;
                break;
            } else {
                ++digit_accumulator;
            }
        } else if (c == '-') {
            if (i != pos[pos_index]) {
                res = 0;
                break;
            } else {
                ++pos_index;
            }
        } else {
            res = 0;
            break;
        }
        s[i++] = (char) getchar();

        //year check
        if (res == 1 && i == 4) {
            if (strcmp(s, MIN_YEAR) < 0) {
                res = 0;
            }
            if (strcmp(s, MAX_YEAR) > 0) {
                res = 0;
            }
        }

        //month check
        //day check

    }

    if (res == 1) {
        if (pos_index == 2) {
            if (digit_accumulator == 8) {
                s[i++] = '\t';
                s[i] = '\0';
            } else {
                res = 0;
            }
        } else {
            res = 0;
        }
    }

    return res;
}

void merge(FactOrWord *arr, FactOrWord *left, int left_num, FactOrWord *right, int right_num, int t) {
    int i = 0, j = 0, k = 0;

    //whoever finishes first
    while (j < left_num && k < right_num) {
        switch (t) {
            case 0:
                if (strcmp(left[j].fact.date, right[k].fact.date) < 0) {
                    arr[i++] = left[j++];
                } else {
                    arr[i++] = right[k++];
                }
                break;
            case 1:
                if (left[j].word.count > right[k].word.count) {
                    arr[i++] = left[j++];
                } else {
                    arr[i++] = right[k++];
                }
                break;
            default:
                break;
        }


    }

    //if didn't finish first get leftover
    while (j < left_num) {
        arr[i++] = left[j++];
    }

    //if didn't finish first get leftover
    while (k < right_num) {
        arr[i++] = right[k++];
    }

}

//O(n log n)
/*
 * recursively divide the array to left and right
 * when you reach 1 element in array return
 * when you have left and right 1 element each, then merge
 * merge by left 1 check right 1, then left 1 check right 2 or left 2 check right 1, and so on
 * consume leftovers in left or right
 * now the merged array is returned up the tree and process continues
 */
void merge_sort(FactOrWord *arr, int n, int t) {
    int mid, i;
    if (n < 2) return;

    //calculate mid (e.g., 5/2=2)
    mid = n / 2;

    //allocate memory for num of items times Fact struct size
    FactOrWord *left = malloc(mid * sizeof(FactOrWord));
    FactOrWord *right = malloc((n - mid) * sizeof(FactOrWord));

    //fill the left and right arrays
    for (i = 0; i < mid; i++) {
        left[i] = arr[i];
    }
    //move the index up to mid and limit to array length
    for (i = mid; i < n; i++) {
        //  e.g. (i = 2, mid = 2)
        // right[0] = arr[2]
        right[i - mid] = arr[i];
    }

    merge_sort(left, mid, t);
    merge_sort(right, (n - mid), t);
    merge(arr, left, mid, right, (n - mid), t);
    //free allocated memory from malloc because left and right not needed anymore
    free(left);
    free(right);

}


Word set_word(char *token) {
    Word word;
    word.count = 0;
    strcpy(word.word, token);
    return word;
}

void word_count(Fact facts[], int num_facts, Word words[]) {
    char *token;
    int k;
    int w = 0;
    int found = 0;
    for (int i = 0; i < num_facts; i++) {
        token = strtok(facts[i].description, " ,.-\t");
        while (token != NULL) {
            if (w == 0) {
                words[w] = set_word(token);
                found++;
            } else {
                //loop over words array
                for (k = 0; k < w; k++) {
                    if (strcasecmp(words[k].word, token) == 0) {
                        words[k].count++;
                        found++;
                        w--;
                        if (words[k].count > word_count_max) {
                            word_count_max = words[k].count;
                        }
                        break;
                    }
                }
            }
            if (found == 0) {
                word_accumulator++;
                words[w] = set_word(token);
            } else {
                found = 0;
            }
            w++;
            token = strtok(NULL, " ,.-\t");
        }
    }
}
