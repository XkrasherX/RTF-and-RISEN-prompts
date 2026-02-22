#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h> 
#include "dynamicListForTask.h" 

int main(void) {
	FILE* input_text_file = fopen("D:\\NYLP\\1k2s\\VIPZ\\lab1\\input_text.txt", "r");
	if (input_text_file == NULL) {
		printf("Error opening file\n");
		return 1;
	}

	/* read into list start */
	readingDataFromFile(&start, input_text_file);
	printf("Initial list from file:\n\n");
	print_list(start);

	/* Let user input N books (adds to list) */
	inputBooksFromUser(&start);
	printf("\n\nList after adding user-input books: \n\n");
	print_list(start);

	/* compute average after additions */
	double avg_of_price = countAvgPriceOfBook(start);
	printf("\nAverage price (after additions): %.2f\n", avg_of_price);

	/* remove books with price <= average so only strictly greater remain */
	deleteElementFromList(&start, avg_of_price);
	printf("\n\nList after removing books with price <= avg: \n\n");
	print_list(start);

	/* sort remaining list alphabetically by title */
	sortingListByBookTitle(start);
	printf("\n\nList after sorting by book title: \n\n");
	print_list(start);

	printf("\n\nList after deleting book titles starting with P, K, L: \n\n");
	removeNodeStartingWith_P_K_L(&start);
	print_list(start);

	fclose(input_text_file);
	free_list(start);
	start = NULL;
	return 0;
}