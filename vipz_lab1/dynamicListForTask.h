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

/* single global head (defined in .c) */
extern Book* start;

/* I/O */
int read_books_from_file(const char* path); /* returns number read, -1 on error */
void print_list(const Book* head);

/* basic operations */
void add_book(const char* author, const char* title, int year, int pages, int price);
double compute_average_price(void);
void remove_below_average(double avg);
void sort_books_by_title(void);
void remove_books_starting_with_pkl(void);
void free_list(void);