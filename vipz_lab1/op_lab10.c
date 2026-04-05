#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "dynamicListForTask.h"

int main(void) {
	const char* default_path_in = "D:\\NYLP\\1k2s\\VIPZ\\lab1\\input_text.txt";
	const char* path_out = "D:\\NYLP\\1k2s\\VIPZ\\lab1\\output_text.txt";

	char path_buffer[512];
	int read;

	/* First, let's try the default path */
	read = read_books_from_file(default_path_in);

	/* If the file doesn't open - try a different path */
	while (read < 0) {
		printf("\nDefault file path not found:\n%s\n", default_path_in);
		printf("Enter another file path or press ENTER to retry default: ");

		if (!fgets(path_buffer, sizeof(path_buffer), stdin))
			return 1;

		/* remove the newline character */
		path_buffer[strcspn(path_buffer, "\r\n")] = 0;

		/* If the user simply pressed ENTER, we try the default again */
		if (path_buffer[0] == '\0') {
			read = read_books_from_file(default_path_in);
		}
		else {
			read = read_books_from_file(path_buffer);
		}
	}

	printf("Read %d records from file.\n\nCurrent list:\n\n", read);
	print_list(start);

	/* Interactive user input: user may add N books */
	input_books_from_user();
	printf("\n\nList after user input (if any):\n\n");
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

	/* Write final list to output file */
	int written = write_books_to_file(path_out);
	if (written >= 0) {
		printf("\nWrote %d records to '%s'\n", written, path_out);
	} else {
		fprintf(stderr, "Failed to write output file.\n");
	}

	free_list();
	return 0;
}