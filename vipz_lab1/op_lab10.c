#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "dynamicListForTask.h"

int main(void) {
	const char* path = "D:\\NYLP\\1k2s\\VIPZ\\lab1\\input_text.txt"; /* adapt to your environment */

	int read = read_books_from_file(path);
	if (read < 0) {
		return 1; /* file open error already printed by reader */
	}
	printf("Read %d records from file.\n\nCurrent list:\n\n", read);
	print_list(start);

	/* Add 2 elements (as required) */
	add_book("Chemerys Danylo", "My Own Book", 2025, 13, 357);
	add_book("FChemerys Danylo", "PMy Own Book", 2025, 13, 398);
	printf("\n\nList after adding 2 elements:\n\n");
	print_list(start);

	/* Compute average including the newly added ones */
	double avg = compute_average_price();
	printf("\nAverage book price: %.2f\n", avg);

	/* Remove elements with price < average (so remaining are >= avg), then sort them */
	remove_below_average(avg);
	printf("\n\nList after removing books with price < average:\n\n");
	print_list(start);

	sort_books_by_title();
	printf("\n\nList after sorting by book title (alphabetical):\n\n");
	print_list(start);

	/* Remove titles starting with P, K, L */
	remove_books_starting_with_pkl();
	printf("\n\nList after removing titles starting with P/K/L:\n\n");
	print_list(start);

	free_list();
	return 0;
}