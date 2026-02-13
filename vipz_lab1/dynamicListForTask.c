#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdbool.h> 
#include "dynamicListForTask.h" 

Book* start = NULL;

/* Append node to list pointed by head (head can be &start or another list) */
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

/* Read until fscanf fails; append into provided list */
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