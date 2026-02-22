#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
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
   Comments beginning with '#' are ignored.
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
		/* find first non-space */
		char* p = line;
		while (*p && isspace((unsigned char)*p)) p++;
		if (*p == '\0' || *p == '\n') continue;            /* blank line */
		if (*p == '#') continue;                          /* comment line */

		char author[100], title[100];
		int year, pages, price;
		/* Try comma format first */
		int matched = sscanf(line, " %99[^,], %99[^,], %d, %d, %d", author, title, &year, &pages, &price);
		if (matched == 5) {
			add_book(author, title, year, pages, price);
			count++;
		} else {
			/* Try quoted title format: Author "Title" Year Pages Price */
			char* q1 = strchr(line, '"');
			if (q1) {
				char* q2 = strchr(q1 + 1, '"');
				if (q2) {
					/* author is text before q1 */
					char a[100] = {0}, t[100] = {0};
					/* copy author (trim trailing spaces) */
					size_t lenA = q1 - line;
					if (lenA >= sizeof(a)) lenA = sizeof(a) - 1;
					strncpy(a, line, lenA);
					a[lenA] = '\0';
					/* trim trailing spaces from a */
					while (lenA && isspace((unsigned char)a[lenA - 1])) {
						a[--lenA] = '\0';
					}
					/* title */
					size_t lenT = q2 - (q1 + 1);
					if (lenT >= sizeof(t)) lenT = sizeof(t) - 1;
					strncpy(t, q1 + 1, lenT);
					t[lenT] = '\0';
					/* parse ints after q2 */
					int m2 = sscanf(q2 + 1, " %d %d %d", &year, &pages, &price);
					if (m2 == 3) {
						/* trim commas/spaces from author */
						char a_trim[100];
						snprintf(a_trim, sizeof(a_trim), "%s", a);
						/* remove possible trailing comma */
						char* last = a_trim + strlen(a_trim) - 1;
						while (last >= a_trim && isspace((unsigned char)*last)) { *last = '\0'; last--; }
						if (last >= a_trim && *last == ',') *last = '\0';
						add_book(a_trim, t, year, pages, price);
						count++;
						continue;
					}
				}
			}

			/* Last-resort: split by whitespace, expect last three tokens to be ints */
			char copy[512];
			strncpy(copy, line, sizeof(copy) - 1);
			copy[sizeof(copy) - 1] = '\0';
			/* tokenize */
			char* tokens[128];
			int tokc = 0;
			char* tk = strtok(copy, " \t\r\n");
			while (tk && tokc < (int)(sizeof(tokens) / sizeof(tokens[0]))) {
				tokens[tokc++] = tk;
				tk = strtok(NULL, " \t\r\n");
			}
			if (tokc >= 4) {
				char *p1 = tokens[tokc - 3], *p2 = tokens[tokc - 2], *p3 = tokens[tokc - 1];
				char *endptr;
				long v1 = strtol(p1, &endptr, 10);
				bool ok1 = (*p1 != '\0' && *endptr == '\0');
				long v2 = strtol(p2, &endptr, 10);
				bool ok2 = (*p2 != '\0' && *endptr == '\0');
				long v3 = strtol(p3, &endptr, 10);
				bool ok3 = (*p3 != '\0' && *endptr == '\0');
				if (ok1 && ok2 && ok3) {
					year = (int)v1; pages = (int)v2; price = (int)v3;
					/* remainder tokens form author+title but ambiguous; prompt user to split */
					/* build remainder string */
					char rem[256] = {0};
					size_t pos = 0;
					for (int i = 0; i < tokc - 3; ++i) {
						size_t tl = strlen(tokens[i]);
						if (pos + tl + 2 < sizeof(rem)) {
							if (pos) rem[pos++] = ' ';
							memcpy(rem + pos, tokens[i], tl);
							pos += tl;
						}
					}
					rem[pos] = '\0';
					/* try to split rem by comma if present */
					char* comma = strchr(rem, ',');
					if (comma) {
						*comma = '\0';
						char* a = rem;
						char* ttitle = comma + 1;
						/* trim both */
						while (*ttitle && isspace((unsigned char)*ttitle)) ttitle++;
						while (*a && isspace((unsigned char)*a)) a++;
						/* remove trailing spaces */
						char *ap = a + strlen(a) - 1;
						while (ap >= a && isspace((unsigned char)*ap)) { *ap = '\0'; ap--; }
						add_book(a, ttitle, year, pages, price);
						count++;
						continue;
					}
					/* ambiguous: interactively ask user for author and title */
					char author_in[100] = {0}, title_in[100] = {0}, buf[256];
					printf("Parsed numeric fields: Year=%d Pages=%d Price=%d\n", year, pages, price);
					printf("Could not unambiguously split author and title from: \"%s\"\n", rem);
					while (1) {
						printf("Enter author for this record: ");
						if (!fgets(buf, sizeof(buf), stdin)) break;
						/* trim newline */
						buf[strcspn(buf, "\r\n")] = 0;
						if (buf[0] == '\0') { printf("Author cannot be empty.\n"); continue; }
						snprintf(author_in, sizeof(author_in), "%s", buf);
						break;
					}
					while (1) {
						printf("Enter title for this record: ");
						if (!fgets(buf, sizeof(buf), stdin)) break;
						buf[strcspn(buf, "\r\n")] = 0;
						if (buf[0] == '\0') { printf("Title cannot be empty.\n"); continue; }
						snprintf(title_in, sizeof(title_in), "%s", buf);
						break;
					}
					add_book(author_in, title_in, year, pages, price);
					count++;
					continue;
				}
			}

			/* if we reach here it's a malformed line */
			char warn[128];
			strncpy(warn, line, sizeof(warn) - 1);
			warn[sizeof(warn) - 1] = '\0';
			char* nl = strchr(warn, '\n'); if (nl) *nl = '\0';
			fprintf(stderr, "Warning: skipping malformed line: %.120s\n", warn);
		}
	}
	fclose(f);
	return count;
}

/* Write list to file using the same CSV format and a small header.
   Returns number written; -1 on error. */
int write_books_to_file(const char* path) {
	FILE* f = fopen(path, "w");
	if (!f) {
		fprintf(stderr, "Error: cannot open output file '%s' for writing\n", path);
		return -1;
	}
	/* Header describing format */
	fprintf(f, "# File format: Author, Book title, Year, Pages, Price\n");
	fprintf(f, "# Each record on its own line. Lines starting with '#' are comments.\n");

	int written = 0;
	Book* cur = start;
	while (cur) {
		/* write exactly same order used for reading */
		fprintf(f, "%s, %s, %d, %d, %d\n",
			cur->author,
			cur->book_title,
			cur->year,
			cur->pages,
			cur->price);
		written++;
		cur = cur->next;
	}
	fclose(f);
	return written;
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

/* Interactive user input: user enters count then N book lines.
   Accepts multiple formats; falls back to interactive split if ambiguous. */
void input_books_from_user(void) {
	char buf[512];
	int n = 0;
	printf("\nEnter number of books to add (0 to skip): ");
	if (!fgets(buf, sizeof(buf), stdin)) return;
	/* parse integer robustly */
	char* endptr;
	long v = strtol(buf, &endptr, 10);
	if (buf == endptr || v < 0) {
		printf("Invalid number; skipping user input.\n");
		return;
	}
	n = (int)v;
	for (int i = 1; i <= n; ++i) {
		char line[512];
		while (1) {
			printf("\nEnter book #%d in one line.\n", i);
			printf("Formats accepted:\n");
			printf(" - Comma format: Author, Book title, Year, Pages, Price\n");
			printf(" - Quoted title: Author \"Book title\" Year Pages Price\n");
			printf(" - Space format: Author Title Year Pages Price (if ambiguous you'll be asked to provide Author and Title separately)\n");
			printf("Input: ");
			if (!fgets(line, sizeof(line), stdin)) return;
			/* trim newline */
			line[strcspn(line, "\r\n")] = 0;
			/* empty -> re-prompt */
			char* p = line;
			while (*p && isspace((unsigned char)*p)) p++;
			if (*p == '\0') { printf("Empty input; try again.\n"); continue; }

			/* try parsing using the same logic as file reader but applied to user input */
			/* Try comma format */
			char author[100], title[100];
			int year, pages, price;
			if (sscanf(line, " %99[^,], %99[^,], %d, %d, %d", author, title, &year, &pages, &price) == 5) {
				add_book(author, title, year, pages, price);
				break;
			}
			/* Try quoted title */
			char* q1 = strchr(line, '"');
			if (q1) {
				char* q2 = strchr(q1 + 1, '"');
				if (q2) {
					char a[100] = {0}, t[100] = {0};
					size_t lenA = q1 - line;
					if (lenA >= sizeof(a)) lenA = sizeof(a) - 1;
					strncpy(a, line, lenA);
					a[lenA] = '\0';
					size_t lenT = q2 - (q1 + 1);
					if (lenT >= sizeof(t)) lenT = sizeof(t) - 1;
					strncpy(t, q1 + 1, lenT);
					t[lenT] = '\0';
					if (sscanf(q2 + 1, " %d %d %d", &year, &pages, &price) == 3) {
						/* trim */
						char a_trim[100];
						snprintf(a_trim, sizeof(a_trim), "%s", a);
						char* last = a_trim + strlen(a_trim) - 1;
						while (last >= a_trim && isspace((unsigned char)*last)) { *last = '\0'; last--; }
						if (last >= a_trim && *last == ',') *last = '\0';
						add_book(a_trim, t, year, pages, price);
						break;
					}
				}
			}
			/* Last chance: space-split and expect last 3 tokens numbers */
			char tmp[512];
			strncpy(tmp, line, sizeof(tmp) - 1); tmp[sizeof(tmp) - 1] = '\0';
			char* tokens[128]; int tokc = 0;
			char* tk = strtok(tmp, " \t");
			while (tk && tokc < (int)(sizeof(tokens) / sizeof(tokens[0]))) {
				tokens[tokc++] = tk;
				tk = strtok(NULL, " \t");
			}
			if (tokc >= 4) {
				char *p1 = tokens[tokc - 3], *p2 = tokens[tokc - 2], *p3 = tokens[tokc - 1];
				char* e;
				long vv1 = strtol(p1, &e, 10); bool ok1 = (*p1 != '\0' && *e == '\0');
				long vv2 = strtol(p2, &e, 10); bool ok2 = (*p2 != '\0' && *e == '\0');
				long vv3 = strtol(p3, &e, 10); bool ok3 = (*p3 != '\0' && *e == '\0');
				if (ok1 && ok2 && ok3) {
					year = (int)vv1; pages = (int)vv2; price = (int)vv3;
					/* remainder is ambiguous; try to split by comma inside remainder */
					char rem[256] = {0}; size_t pos = 0;
					for (int j = 0; j < tokc - 3; ++j) {
						size_t l = strlen(tokens[j]);
						if (pos + l + 2 < sizeof(rem)) {
							if (pos) rem[pos++] = ' ';
							memcpy(rem + pos, tokens[j], l);
							pos += l;
						}
					}
					rem[pos] = '\0';
					char* comma = strchr(rem, ',');
					if (comma) {
						*comma = '\0';
						char* a = rem;
						char* ttitle = comma + 1;
						while (*ttitle && isspace((unsigned char)*ttitle)) ttitle++;
						while (*a && isspace((unsigned char)*a)) a++;
						char *ap = a + strlen(a) - 1;
						while (ap >= a && isspace((unsigned char)*ap)) { *ap = '\0'; ap--; }
						add_book(a, ttitle, year, pages, price);
						break;
					}
					/* ask user to enter author and title separately */
					char a_in[100] = {0}, t_in[100] = {0}, tmpbuf[256];
					printf("Parsed numbers: Year=%d Pages=%d Price=%d\n", year, pages, price);
					printf("Could not split author/title from: \"%s\"\n", rem);
					printf("Please enter author: ");
					if (!fgets(tmpbuf, sizeof(tmpbuf), stdin)) return;
					tmpbuf[strcspn(tmpbuf, "\r\n")] = 0;
					while (tmpbuf[0] == '\0') {
						printf("Author cannot be empty. Enter author: ");
						if (!fgets(tmpbuf, sizeof(tmpbuf), stdin)) return;
						tmpbuf[strcspn(tmpbuf, "\r\n")] = 0;
					}
					snprintf(a_in, sizeof(a_in), "%s", tmpbuf);
					printf("Please enter title: ");
					if (!fgets(tmpbuf, sizeof(tmpbuf), stdin)) return;
					tmpbuf[strcspn(tmpbuf, "\r\n")] = 0;
					while (tmpbuf[0] == '\0') {
						printf("Title cannot be empty. Enter title: ");
						if (!fgets(tmpbuf, sizeof(tmpbuf), stdin)) return;
						tmpbuf[strcspn(tmpbuf, "\r\n")] = 0;
					}
					snprintf(t_in, sizeof(t_in), "%s", tmpbuf);
					add_book(a_in, t_in, year, pages, price);
					break;
				}
			}
			printf("Input could not be parsed. Please try again using one of the accepted formats.\n");
		} /* end while for single record input */
	} /* for n */
}