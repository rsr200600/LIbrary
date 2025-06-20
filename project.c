#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Book {
    int id;
    char title[100];
    char author[100];
    char status[20];
};

int is_duplicate_id(int id) {
    FILE *file = fopen("library.txt", "r");
    if (file == NULL) return 0;

    struct Book book;
    while (fscanf(file, "%d,%99[^,],%99[^,],%19[^\n]\n", &book.id, book.title, book.author, book.status) != EOF) {
        if (book.id == id) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void create_file() {
    FILE *file = fopen("library.txt", "w");
    if (file == NULL) {
        printf("\n? Error creating file!\n");
        return;
    }
    fclose(file);
    printf("\n? File 'library.txt' created successfully!\n");
}

void write_data() {
    FILE *file = fopen("library.txt", "w");
    if (file == NULL) {
        printf("\n? Error opening file for writing!\n");
        return;
    }

    struct Book book;
    char choice;
    do {
        printf("\n?? Enter Book Details:\n");
        printf("?? Book ID: ");
        scanf("%d", &book.id);
        if (is_duplicate_id(book.id)) {
            printf("? Duplicate ID! Please use a unique ID.\n");
            continue;
        }
        printf("?? Title: ");
        scanf(" %[^\n]", book.title);
        printf("?? Author: ");
        scanf(" %[^\n]", book.author);
        printf("?? Status (Available/Issued): ");
        scanf(" %[^\n]", book.status);

        fprintf(file, "%d,%s,%s,%s\n", book.id, book.title, book.author, book.status);
        printf("? Book added successfully!\n");

        printf("? Add another book? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    fclose(file);
}

void append_data() {
    FILE *file = fopen("library.txt", "a");
    if (file == NULL) {
        printf("\n? Error opening file for appending!\n");
        return;
    }

    struct Book book;
    char choice;
    do {
        printf("\n?? Enter Book Details:\n");
        printf("?? Book ID: ");
        scanf("%d", &book.id);
        if (is_duplicate_id(book.id)) {
            printf("? Duplicate ID! Please use a unique ID.\n");
            continue;
        }
        printf("?? Title: ");
        scanf(" %[^\n]", book.title);
        printf("?? Author: ");
        scanf(" %[^\n]", book.author);
        printf("?? Status (Available/Issued): ");
        scanf(" %[^\n]", book.status);

        fprintf(file, "%d,%s,%s,%s\n", book.id, book.title, book.author, book.status);
        printf("? Book appended successfully!\n");

        printf("? Add another book? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    fclose(file);
}

void read_data() {
    FILE *file = fopen("library.txt", "r");
    if (file == NULL) {
        printf("\n? File not found! Please create it first.\n");
        return;
    }

    struct Book book;
    int count = 0;
    printf("\n?? --- Library Records ---\n");
    printf("==========================================\n");
    while (fscanf(file, "%d,%99[^,],%99[^,],%19[^\n]\n", &book.id, book.title, book.author, book.status) != EOF) {
        printf("?? Book #%d\n", ++count);
        printf("?? ID: %d\n?? Title: %s\n?? Author: %s\n?? Status: %s\n", book.id, book.title, book.author, book.status);
        printf("------------------------------------------\n");
    }

    if (count == 0) {
        printf("?? No records found.\n");
    }

    fclose(file);
}

void search_by_id() {
    int search_id;
    int found = 0;
    printf("\n?? Enter Book ID to search: ");
    scanf("%d", &search_id);

    FILE *file = fopen("library.txt", "r");
    if (file == NULL) {
        printf("\n? File not found!\n");
        return;
    }

    struct Book book;
    while (fscanf(file, "%d,%99[^,],%99[^,],%19[^\n]\n", &book.id, book.title, book.author, book.status) != EOF) {
        if (book.id == search_id) {
            printf("\n? Book Found:\n");
            printf("?? ID: %d\n?? Title: %s\n?? Author: %s\n?? Status: %s\n", book.id, book.title, book.author, book.status);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("? Book with ID %d not found.\n", search_id);
    }

    fclose(file);
}

void backup_data() {
    FILE *src = fopen("library.txt", "r");
    FILE *dest = fopen("library_backup.txt", "w");

    if (src == NULL || dest == NULL) {
        printf("\n? Error during backup process!\n");
        if (src) fclose(src);
        if (dest) fclose(dest);
        return;
    }

    char ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("\n? Backup created as 'library_backup.txt'\n");
}

void restore_data() {
    FILE *src = fopen("library_backup.txt", "r");
    FILE *dest = fopen("library.txt", "w");

    if (src == NULL || dest == NULL) {
        printf("\n? Error during restore process!\n");
        if (src) fclose(src);
        if (dest) fclose(dest);
        return;
    }

    char ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("\n? Data restored from 'library_backup.txt'\n");
}

int main() {
    int choice;

    while (1) {
        printf("\n==========================================\n");
        printf("?? Welcome to Library Management System ??\n");
        printf("==========================================\n");
        printf("1??  Create New File\n");
        printf("2??  Write Data (Overwrite)\n");
        printf("3??  Read All Book Records\n");
        printf("4??  Append New Book\n");
        printf("5??  Search Book by ID\n");
        printf("6??  Backup Data\n");
        printf("7??  Restore from Backup\n");
        printf("8??  Exit\n");
        printf("==========================================\n");
        printf("?? Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: create_file(); break;
            case 2: write_data(); break;
            case 3: read_data(); break;
            case 4: append_data(); break;
            case 5: search_by_id(); break;
            case 6: backup_data(); break;
            case 7: restore_data(); break;
            case 8: printf("\n?? Exiting program. Thank you!\n"); exit(0);
            default: printf("\n? Invalid choice! Please select a valid option.\n");
        }
    }

    return 0;
}

