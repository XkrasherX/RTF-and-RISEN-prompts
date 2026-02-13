#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dynamicListForTask.h"

Book* start = NULL;

/* Append a new book node to the end of the list */
void add_book(const char* author, const char* title, int year, int pages, int price) {
	Book* node = (Book*)malloc(sizeof(Book));
	if (!node) {
		fprintf(stderr, "Memory allocation failed in add_book\n");
		return;
	}
	strncpy(node->author, author, sizeof(node->author) - 1);
	node->author[sizeof(node->author) - 1] = '\0';
	strncpy(node->book_title, title, sizeof(node->book_title) - 1);
	node->book_title[sizeof(node->book_title) - 1] = '\0';
	node->year = year;
	node->pages = pages;
	node->price = price;
	node->next = NULL;

	if (!start) {
		start = node;
	} else {
		Book* cur = start;
		while (cur->next) cur = cur->next;
		cur->next = node;
	}
}

/* Read CSV-like file: Author, Book title, Year, Pages, Price per line
   Returns number of successfully parsed lines; -1 if file open failed */
int read_books_from_file(const char* path) {
	FILE* f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "Error: cannot open file '%s'\n", path);
		return -1;
	}

	char line[512];
	int count = 0;
	while (fgets(line, sizeof(line), f)) {
		/* trim newline */
		char* p = line;
		while (*p && (*p == ' ' || *p == '\t')) p++;
		if (*p == '\0' || *p == '\n') continue;

		char author[100], title[100];
		int year, pages, price;
		/* Use sscanf to parse; skip malformed lines */
		int matched = sscanf(line, " %99[^,], %99[^,], %d, %d, %d", author, title, &year, &pages, &price);
		if (matched == 5) {
			add_book(author, title, year, pages, price);
			count++;
		} else {
			fprintf(stderr, "Warning: skipping malformed line: %.80s\n", line);
		}
	}
	fclose(f);
	return count;
}

/* Compute average price (returns 0.0 if list is empty) */
double compute_average_price(void) {
	Book* cur = start;
	if (!cur) return 0.0;
	long sum = 0;
	int n = 0;
	while (cur) {
		sum += cur->price;
		n++;
		cur = cur->next;
	}
	return n ? ((double)sum) / n : 0.0;
}

/* Remove nodes with price < avg. Uses pointer-to-pointer for head updates. */
void remove_below_average(double avg) {
	Book** curptr = &start;
	while (*curptr) {
		if ((*curptr)->price < avg) {
			Book* tmp = *curptr;
			*curptr = tmp->next;
			free(tmp);
		} else {
			curptr = &((*curptr)->next);
		}
	}
}

/* Case-insensitive compare helper */
static int ci_compare(const char* a, const char* b) {
	while (*a && *b) {
		char ca = (char)tolower((unsigned char)*a);
		char cb = (char)tolower((unsigned char)*b);
		if (ca != cb) return (ca < cb) ? -1 : 1;
		a++; b++;
	}
	if (*a) return 1;
	if (*b) return -1;
	return 0;
}

/* Sort list by book_title (bubble sort swapping payload) */
void sort_books_by_title(void) {
	if (!start || !start->next) return;
	int swapped;
	do {
		swapped = 0;
		Book* cur = start;
		while (cur && cur->next) {
			if (ci_compare(cur->book_title, cur->next->book_title) > 0) {
				/* swap payload */
				char ta[100], tt[100];
				int ty, tp, tpr;
				strcpy(ta, cur->author);
				strcpy(tt, cur->book_title);
				ty = cur->year;
				tp = cur->pages;
				tpr = cur->price;

				strcpy(cur->author, cur->next->author);
				strcpy(cur->book_title, cur->next->book_title);
				cur->year = cur->next->year;
				cur->pages = cur->next->pages;
				cur->price = cur->next->price;

				strcpy(cur->next->author, ta);
				strcpy(cur->next->book_title, tt);
				cur->next->year = ty;
				cur->next->pages = tp;
				cur->next->price = tpr;

				swapped = 1;
			}
			cur = cur->next;
		}
	} while (swapped);
}

/* Check if title starts with 'P' or 'K' or 'L' (case-insensitive) */
static int starts_with_pkl(const char* title) {
	if (!title || !title[0]) return 0;
	char c = (char)tolower((unsigned char)title[0]);
	return (c == 'p' || c == 'k' || c == 'l');
}

/* Remove nodes whose book_title starts with P/K/L (case-insensitive) */
void remove_books_starting_with_pkl(void) {
	Book** curptr = &start;
	while (*curptr) {
		if (starts_with_pkl((*curptr)->book_title)) {
			Book* tmp = *curptr;
			*curptr = tmp->next;
			free(tmp);
		} else {
			curptr = &((*curptr)->next);
		}
	}
}

/* Print list with header */
void print_list(const Book* head) {
	printf("%-25s %-50s %-6s %-6s %-8s\n", "Author", "Book title", "Year", "Pages", "Price");
	printf("-------------------------------------------------------------------------------------------\n");
	const Book* cur = head;
	while (cur) {
		printf("%-25s %-50s %-6d %-6d %8d\n", cur->author, cur->book_title, cur->year, cur->pages, cur->price);
		cur = cur->next;
	}
}

/* Free full list */
void free_list(void) {
	Book* cur = start;
	while (cur) {
		Book* tmp = cur;
		cur = cur->next;
		free(tmp);
	}
	start = NULL;
}