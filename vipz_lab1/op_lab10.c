#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h> 
#include "dynamicListForTask.h" 

int main(void) {
	char name_of_author[100] = "FChemerys Danylo";
	char name_of_book[100] = "PMy Own Book";
	int year_of_book = 2025;
	int num_of_pages_in_book = 13;
	int num_of_book_price = 398;
	FILE* input_text_file = fopen("D:\\NYLP\\1k2s\\VIPZ\\lab1\\input_text.txt", "r");
	if (input_text_file == NULL) {
		printf("Error opening file\n");
		return 1;
	}

	/* read into list start */
	readingDataFromFile(&start, input_text_file);
	printf("List: \n\n");
	print_list(start);

	double avg_of_price = countAvgPriceOfBook(start);
	printf("\nAverage price: %.2f\n", avg_of_price);

	/* remove books with price <= average so only strictly greater remain */
	deleteElementFromList(&start, avg_of_price);
	printf("\n\nList after removing books with price <= avg: \n\n");
	print_list(start);

	/* add two elements (use the new API) */
	addElementToList(&start, "Chemerys Danylo", "My Own Book", 2025, 13, 357);
	addElementToList(&start, name_of_author, name_of_book, year_of_book, num_of_pages_in_book, num_of_book_price);
	printf("\n\nList after adding 2 elements: \n\n");
	print_list(start);

	/* sort remaining list alphabetically by title (only nodes left in list are sorted) */
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