#pragma once 
#include <stdio.h> 

typedef struct Book {
	char author[100];
	char book_title[100];
	int year;
	int pages;
	int price;
	struct Book* next;
} Book;

/* Single external declaration (definition in .c) */
extern Book* start;

/* Mutating functions accept Book** head so they operate on arbitrary lists */
void addElementToList(Book** head, const char* input_text_author, const char* input_text_book_title, int input_year, int input_pages, int input_price);
void readingDataFromFile(Book** head, FILE* name_of_file);

double countAvgPriceOfBook(const Book* first_node);
void print_list(const Book* head);
void deleteElementFromList(Book** first_node, double input_avg_price);
int numOfElementHigherThanAvgPriceInList(const Book* first_node, double avg_price_of_book);
void sortingListByBookTitle(Book* first_node);
int bookTitleStartingWith_P_K_L(const char* name_book_title);
void removeNodeStartingWith_P_K_L(Book** first_list);
void free_list(Book* head);