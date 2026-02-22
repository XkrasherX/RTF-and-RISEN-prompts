#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 
#include "dynamicListForTask.h" 

Book* start = NULL;

/* helper: trim leading/trailing whitespace */
static void trim(char* s) {
    if (!s) return;
    char* end;
    while (*s && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')) s++;
    /* shift left if needed */
    char* dst = s;
    if (dst != s) memmove(dst, s, strlen(s) + 1);
    /* now do trailing */
    end = s + strlen(s) - 1;
    while (end >= s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        *end = '\0';
        --end;
    }
}

/* helper: remove surrounding quotes if present, then trim */
static void strip_quotes_and_trim(char* s) {
    if (!s) return;
    trim(s);
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '\"' && s[len - 1] == '\"') {
        /* move inner content */
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
        trim(s);
    }
}

/* helper: replace underscores with spaces */
static void underscores_to_spaces(char* s) {
    if (!s) return;
    for (; *s; ++s) if (*s == '_') *s = ' ';
}

void addElementToList(Book** head, const char* input_text_author, const char* input_text_book_title, int input_year, int input_pages, int input_price) {
    Book* ptr = (Book*)malloc(sizeof(Book));
    if (ptr == NULL) {
        printf("Memory allocation error\n");
        return;
    }
    /* safe copy */
    strncpy(ptr->author, input_text_author ? input_text_author : "", sizeof(ptr->author) - 1);
    ptr->author[sizeof(ptr->author) - 1] = '\0';
    strncpy(ptr->book_title, input_text_book_title ? input_text_book_title : "", sizeof(ptr->book_title) - 1);
    ptr->book_title[sizeof(ptr->book_title) - 1] = '\0';

    ptr->year = input_year;
    ptr->pages = input_pages;
    ptr->price = input_price;
    ptr->next = NULL;

    if (!*head) {
        *head = ptr;
    }
    else {
        Book* current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = ptr;
    }
}

void readingDataFromFile(Book** head, FILE* name_of_file) {
    char tmp_author[100];
    char tmp_book_title[200];
    int tmp_year;
    int tmp_pages;
    int tmp_price;

    while (fscanf(name_of_file, " %99[^,], %199[^,], %d, %d, %d",
        tmp_author,
        tmp_book_title,
        &tmp_year,
        &tmp_pages,
        &tmp_price) == 5) {
        trim(tmp_author);
        trim(tmp_book_title);
        addElementToList(head, tmp_author, tmp_book_title, tmp_year, tmp_pages, tmp_price);
    }
}

double countAvgPriceOfBook(const Book* first_node) {
    const Book* node = first_node;
    int sum_of_book_price = 0;
    int num_of_elements = 0;
    while (node) {
        sum_of_book_price += node->price;
        num_of_elements++;
        node = node->next;
    }
    if (num_of_elements == 0) return 0.0;
    return (double)sum_of_book_price / num_of_elements;
}

void print_list(const Book* first) {
    const Book* ptr = first;
    printf("%-25s %-50s %-6s %-6s %-8s\n", "Author", "Book title", "Year", "Pages", "Price");
    printf("-------------------------------------------------------------------------------------------\n");
    while (ptr) {
        printf("%-25s %-50s %-6d %-6d %8d\n",
            ptr->author,
            ptr->book_title,
            ptr->year,
            ptr->pages,
            ptr->price);
        ptr = ptr->next;
    }
}

/* Delete nodes with price <= input_avg_price so only strictly greater remain */
void deleteElementFromList(Book** first_node, double input_avg_price) {
    if (!first_node) return;
    Book* tmp_list = *first_node;
    Book* prev = NULL;

    /* Remove head nodes that meet the delete condition */
    while (tmp_list != NULL && tmp_list->price <= input_avg_price) {
        *first_node = tmp_list->next;
        free(tmp_list);
        tmp_list = *first_node;
    }

    if (tmp_list == NULL) {
        return;
    }

    prev = tmp_list;
    tmp_list = tmp_list->next;

    while (tmp_list != NULL) {
        if (tmp_list->price <= input_avg_price) {
            prev->next = tmp_list->next;
            free(tmp_list);
            tmp_list = prev->next;
        }
        else {
            prev = tmp_list;
            tmp_list = tmp_list->next;
        }
    }
}

int numOfElementHigherThanAvgPriceInList(const Book* first_node, double avg_price_of_book) {
    const Book* ptr_tmp = first_node;
    int count_higher_avg_price = 0;
    while (ptr_tmp) {
        if (ptr_tmp->price > avg_price_of_book) {
            count_higher_avg_price++;
        }
        ptr_tmp = ptr_tmp->next;
    }
    return count_higher_avg_price;
}

/* In-place bubble sort by book_title (alphabetical). Operates on list provided */
void sortingListByBookTitle(Book* first_node) {
    Book* node = NULL;
    char tmp_author[100];
    char tmp_book_title[100];
    int tmp_year;
    int tmp_pages;
    int tmp_price;
    bool isswapped;
    if (!first_node) return;

    do {
        isswapped = false;
        node = first_node;
        while (node != NULL && node->next != NULL) {
            if (strcmp(node->book_title, node->next->book_title) > 0) {
                /* swap contents */
                strcpy(tmp_author, node->author);
                strcpy(tmp_book_title, node->book_title);
                tmp_year = node->year;
                tmp_pages = node->pages;
                tmp_price = node->price;

                strcpy(node->author, node->next->author);
                strcpy(node->book_title, node->next->book_title);
                node->year = node->next->year;
                node->pages = node->next->pages;
                node->price = node->next->price;

                strcpy(node->next->author, tmp_author);
                strcpy(node->next->book_title, tmp_book_title);
                node->next->year = tmp_year;
                node->next->pages = tmp_pages;
                node->next->price = tmp_price;

                isswapped = true;
            }
            node = node->next;
        }
    } while (isswapped);
}

int bookTitleStartingWith_P_K_L(const char* name_book_title) {
    if (!name_book_title || name_book_title[0] == '\0') return 0;
    char c = name_book_title[0];
    return (c == 'P' || c == 'p' ||
        c == 'K' || c == 'k' ||
        c == 'L' || c == 'l');
}

void removeNodeStartingWith_P_K_L(Book** first_list) {
    if (!first_list) return;
    Book* tmp_list = *first_list;
    Book* prev = NULL;

    while (*first_list && bookTitleStartingWith_P_K_L((*first_list)->book_title)) {
        tmp_list = *first_list;
        *first_list = (*first_list)->next;
        free(tmp_list);
    }
    prev = *first_list;
    if (!prev) {
        return;
    }
    tmp_list = prev->next;

    while (tmp_list != NULL) {
        if (bookTitleStartingWith_P_K_L(tmp_list->book_title)) {
            prev->next = tmp_list->next;
            free(tmp_list);
            tmp_list = prev->next;
        }
        else {
            prev = tmp_list;
            tmp_list = tmp_list->next;
        }
    }
}

void free_list(Book* first_node) {
    while (first_node) {
        Book* tmp_list = first_node;
        first_node = first_node->next;
        free(tmp_list);
    }
}

/* Attempts to parse a line into author, title and three ints.
   Accepts:
   - comma-separated: Author, Title, Year, Pages, Price
   - quoted: "Author Name" "Title Name" Year Pages Price
   - tokenized: Author_Title Title_With_Underscores Year Pages Price
   Returns 1 on success, 0 on failure.
*/
static int parse_book_line(const char* line_in, char* out_author, size_t auth_sz, char* out_title, size_t title_sz, int* out_year, int* out_pages, int* out_price) {
    if (!line_in || !out_author || !out_title) return 0;
    char buf[512];
    strncpy(buf, line_in, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    /* If line contains commas, split by comma */
    if (strchr(buf, ',') != NULL) {
        char* saveptr = NULL;
        char* tok = strtok_s(buf, ",", &saveptr);
        if (!tok) return 0;
        trim(tok);
        strncpy(out_author, tok, auth_sz - 1); out_author[auth_sz - 1] = '\0';
        tok = strtok_s(NULL, ",", &saveptr);
        if (!tok) return 0;
        trim(tok);
        strncpy(out_title, tok, title_sz - 1); out_title[title_sz - 1] = '\0';
        tok = strtok_s(NULL, ",", &saveptr);
        if (!tok) return 0;
        trim(tok); *out_year = atoi(tok);
        tok = strtok_s(NULL, ",", &saveptr);
        if (!tok) return 0;
        trim(tok); *out_pages = atoi(tok);
        tok = strtok_s(NULL, ",", &saveptr);
        if (!tok) return 0;
        trim(tok); *out_price = atoi(tok);
        strip_quotes_and_trim(out_author);
        strip_quotes_and_trim(out_title);
        return 1;
    }

    /* Try quoted parsing: "Author" "Title" year pages price */
    const char* p = buf;
    while (*p && (*p == ' ' || *p == '\t')) ++p;
    if (*p == '\"') {
        ++p;
        const char* q = strchr(p, '\"');
        if (!q) return 0;
        size_t len = (size_t)(q - p);
        if (len >= auth_sz) len = auth_sz - 1;
        strncpy(out_author, p, len); out_author[len] = '\0';
        p = q + 1;
        /* skip whitespace */
        while (*p && (*p == ' ' || *p == '\t')) ++p;
        if (*p == '\"') {
            ++p;
            q = strchr(p, '\"');
            if (!q) return 0;
            len = (size_t)(q - p);
            if (len >= title_sz) len = title_sz - 1;
            strncpy(out_title, p, len); out_title[len] = '\0';
            p = q + 1;
            /* now parse ints from p */
            if (sscanf_s(p, "%d %d %d", out_year, out_pages, out_price) == 3) {
                return 1;
            }
            return 0;
        }
    }

    /* Fallback tokenized parsing: author title year pages price (single-token author/title; underscores allowed) */
    char a[100] = {0}, t[200] = {0};
    int y = 0, pg = 0, pr = 0;
    if (sscanf_s(buf, "%99s %199s %d %d %d", a, (unsigned)_countof(a), t, (unsigned)_countof(t), &y, &pg, &pr) == 5) {
        strncpy(out_author, a, auth_sz - 1); out_author[auth_sz - 1] = '\0';
        strncpy(out_title, t, title_sz - 1); out_title[title_sz - 1] = '\0';
        underscores_to_spaces(out_author);
        underscores_to_spaces(out_title);
        *out_year = y; *out_pages = pg; *out_price = pr;
        return 1;
    }

    return 0;
}

void inputBooksFromUser(Book** head) {
    if (!head) return;
    int n = 0;
    printf("Enter number of books to input: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        printf("No valid number entered.\n");
        return;
    }
    /* consume leftover newline */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}

    char line[512];
    for (int i = 0; i < n; ) {
        printf("Enter book #%d (formats accepted):\n", i + 1);
        printf("  1) Comma-separated: Author, Title, Year, Pages, Price\n");
        printf("  2) Quoted: \"Author Name\" \"Book Title\" Year Pages Price\n");
        printf("  3) Tokenized: Author_Title Title_With_Underscores Year Pages Price\n> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("Input error.\n");
            break;
        }
        /* trim newline */
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n') line[len - 1] = '\0';

        char author[100] = {0};
        char title[200] = {0};
        int year = 0, pages = 0, price = 0;
        if (!parse_book_line(line, author, sizeof(author), title, sizeof(title), &year, &pages, &price)) {
            printf("Invalid format. Please try again.\n");
            continue; /* do not increment i; allow retry */
        }
        /* final trimming and normalization */
        strip_quotes_and_trim(author);
        strip_quotes_and_trim(title);
        underscores_to_spaces(author);
        underscores_to_spaces(title);

        addElementToList(head, author, title, year, pages, price);
        ++i;
    }
}