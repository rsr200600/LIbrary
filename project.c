#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "library.txt"
#define TEMP_FILE "library.tmp"
#define BACKUP_FILE "library_backup.txt"
#define TITLE_LEN 100
#define AUTHOR_LEN 100
#define PERSON_LEN 100
#define DATE_LEN 20
#define LINE_LEN 512

typedef struct {
    int id;
    char title[TITLE_LEN];
    char author[AUTHOR_LEN];
    int year;
    int issued;
    char borrower[PERSON_LEN];
    char due_date[DATE_LEN];
} Book;

static void trim_newline(char *text) {
    size_t n = strlen(text);
    while (n > 0 && (text[n - 1] == '\n' || text[n - 1] == '\r')) {
        text[--n] = '\0';
    }
}

static void read_text(const char *prompt, char *out, size_t size) {
    for (;;) {
        printf("%s", prompt);
        if (!fgets(out, (int)size, stdin)) {
            out[0] = '\0';
            return;
        }
        if (strchr(out, '\n') == NULL) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            puts("Input is too long. Please try again.");
            continue;
        }
        trim_newline(out);
        if (out[0] == '\0') {
            puts("This field cannot be empty.");
            continue;
        }
        if (strchr(out, ',')) {
            puts("Commas are not allowed because the data file uses comma-separated fields.");
            continue;
        }
        return;
    }
}

static int read_int(const char *prompt, int min, int max) {
    char line[64], *end;
    long value;
    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof line, stdin)) return min;
        trim_newline(line);
        value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end)) end++;
        if (end != line && *end == '\0' && value >= min && value <= max)
            return (int)value;
        printf("Enter a whole number from %d to %d.\n", min, max);
    }
}

static int read_book(FILE *file, Book *book) {
    char line[LINE_LEN];
    char *fields[7];
    int count = 0;

    while (fgets(line, sizeof line, file)) {
        trim_newline(line);
        if (line[0] == '\0') continue;
        fields[count++] = strtok(line, ",");
        while (count < 7 && (fields[count] = strtok(NULL, ",")) != NULL) count++;
        if (count != 7) {
            count = 0;
            continue;
        }
        book->id = atoi(fields[0]);
        snprintf(book->title, sizeof book->title, "%s", fields[1]);
        snprintf(book->author, sizeof book->author, "%s", fields[2]);
        book->year = atoi(fields[3]);
        book->issued = atoi(fields[4]) != 0;
        snprintf(book->borrower, sizeof book->borrower, "%s", strcmp(fields[5], "-") == 0 ? "" : fields[5]);
        snprintf(book->due_date, sizeof book->due_date, "%s", strcmp(fields[6], "-") == 0 ? "" : fields[6]);
        return 1;
    }
    return 0;
}

static void write_book(FILE *file, const Book *book) {
    fprintf(file, "%d,%s,%s,%d,%d,%s,%s\n", book->id, book->title,
            book->author, book->year, book->issued,
            book->borrower[0] ? book->borrower : "-",
            book->due_date[0] ? book->due_date : "-");
}

static int id_exists(int id) {
    FILE *file = fopen(DATA_FILE, "r");
    Book book;
    if (!file) return 0;
    while (read_book(file, &book)) {
        if (book.id == id) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

static void show_book(const Book *book) {
    printf("\nID: %d\nTitle: %s\nAuthor: %s\nYear: %d\nStatus: %s\n",
           book->id, book->title, book->author, book->year,
           book->issued ? "Issued" : "Available");
    if (book->issued)
        printf("Borrower: %s\nDue date: %s\n", book->borrower, book->due_date);
    puts("----------------------------------------");
}

static int replace_book(int target_id, const Book *replacement, int deleting) {
    FILE *src = fopen(DATA_FILE, "r");
    FILE *tmp = fopen(TEMP_FILE, "w");
    Book book;
    int found = 0;
    if (!src || !tmp) {
        if (src) fclose(src);
        if (tmp) fclose(tmp);
        remove(TEMP_FILE);
        puts("Could not open the data files.");
        return 0;
    }
    while (read_book(src, &book)) {
        if (book.id == target_id) {
            found = 1;
            if (!deleting && replacement) write_book(tmp, replacement);
        } else {
            write_book(tmp, &book);
        }
    }
    fclose(src);
    if (fclose(tmp) != 0) {
        remove(TEMP_FILE);
        puts("Could not save changes.");
        return 0;
    }
    if (!found) {
        remove(TEMP_FILE);
        return 0;
    }
    if (remove(DATA_FILE) != 0 || rename(TEMP_FILE, DATA_FILE) != 0) {
        puts("Could not replace the library data file.");
        return 0;
    }
    return 1;
}

static void add_book(void) {
    Book book = {0};
    FILE *file;
    book.id = read_int("Book ID: ", 1, 2147483647);
    if (id_exists(book.id)) {
        puts("That ID is already in use.");
        return;
    }
    read_text("Title: ", book.title, sizeof book.title);
    read_text("Author: ", book.author, sizeof book.author);
    book.year = read_int("Publication year (0 if unknown): ", 0, 9999);
    file = fopen(DATA_FILE, "a");
    if (!file) {
        puts("Could not open the library file.");
        return;
    }
    write_book(file, &book);
    fclose(file);
    puts("Book added.");
}

static void list_books(void) {
    FILE *file = fopen(DATA_FILE, "r");
    Book book;
    int count = 0;
    if (!file) {
        puts("No library file found. It will be created when you add a book.");
        return;
    }
    puts("\n=== Library Catalogue ===");
    while (read_book(file, &book)) {
        show_book(&book);
        count++;
    }
    fclose(file);
    if (!count) puts("The catalogue is empty.");
    else printf("Total books: %d\n", count);
}

static void search_books(void) {
    char query[TITLE_LEN];
    FILE *file;
    Book book;
    int found = 0;
    read_text("Search title or author: ", query, sizeof query);
    file = fopen(DATA_FILE, "r");
    if (!file) { puts("No library file found."); return; }
    while (read_book(file, &book)) {
        if (strstr(book.title, query) || strstr(book.author, query)) {
            show_book(&book);
            found++;
        }
    }
    fclose(file);
    if (!found) puts("No matching books found.");
}

static void update_book(void) {
    Book book, updated;
    int id = read_int("ID of book to update: ", 1, 2147483647);
    FILE *file = fopen(DATA_FILE, "r");
    int found = 0;
    if (!file) { puts("No library file found."); return; }
    while (read_book(file, &book)) if (book.id == id) { found = 1; break; }
    fclose(file);
    if (!found) { puts("Book not found."); return; }
    updated = book;
    printf("Updating book %d (current title: %s)\n", id, book.title);
    read_text("New title: ", updated.title, sizeof updated.title);
    read_text("New author: ", updated.author, sizeof updated.author);
    updated.year = read_int("New publication year (0 if unknown): ", 0, 9999);
    puts(replace_book(id, &updated, 0) ? "Book updated." : "Update failed.");
}

static void delete_book(void) {
    int id = read_int("ID of book to delete: ", 1, 2147483647);
    char answer[8];
    read_text("Delete this book? Type yes to confirm: ", answer, sizeof answer);
    if (strcmp(answer, "yes") != 0) { puts("Deletion cancelled."); return; }
    puts(replace_book(id, NULL, 1) ? "Book deleted." : "Book not found or deletion failed.");
}

static void issue_book(void) {
    int id = read_int("ID of book to issue: ", 1, 2147483647);
    FILE *file = fopen(DATA_FILE, "r");
    Book book;
    int found = 0;
    if (!file) { puts("No library file found."); return; }
    while (read_book(file, &book)) if (book.id == id) { found = 1; break; }
    fclose(file);
    if (!found) { puts("Book not found."); return; }
    if (book.issued) { printf("Already issued to %s.\n", book.borrower); return; }
    read_text("Borrower's name: ", book.borrower, sizeof book.borrower);
    read_text("Due date (for example, 2026-12-31): ", book.due_date, sizeof book.due_date);
    book.issued = 1;
    puts(replace_book(id, &book, 0) ? "Book issued." : "Could not issue the book.");
}

static void return_book(void) {
    int id = read_int("ID of book to return: ", 1, 2147483647);
    FILE *file = fopen(DATA_FILE, "r");
    Book book;
    int found = 0;
    if (!file) { puts("No library file found."); return; }
    while (read_book(file, &book)) if (book.id == id) { found = 1; break; }
    fclose(file);
    if (!found) { puts("Book not found."); return; }
    if (!book.issued) { puts("This book is already marked available."); return; }
    book.issued = 0;
    book.borrower[0] = '\0';
    book.due_date[0] = '\0';
    puts(replace_book(id, &book, 0) ? "Book returned and marked available." : "Could not return the book.");
}

static void show_statistics(void) {
    FILE *file = fopen(DATA_FILE, "r");
    Book book;
    int total = 0, issued = 0;
    if (!file) { puts("No library file found."); return; }
    while (read_book(file, &book)) { total++; if (book.issued) issued++; }
    fclose(file);
    printf("\n=== Library Statistics ===\nTotal books: %d\nAvailable: %d\nIssued: %d\n",
           total, total - issued, issued);
}

static int copy_file(const char *from, const char *to) {
    FILE *src = fopen(from, "rb"), *dest = fopen(to, "wb");
    int ch, ok = 1;
    if (!src || !dest) ok = 0;
    if (src && dest) while ((ch = fgetc(src)) != EOF) if (fputc(ch, dest) == EOF) { ok = 0; break; }
    if (src) fclose(src);
    if (dest && fclose(dest) != 0) ok = 0;
    return ok;
}

static void backup_data(void) {
    puts(copy_file(DATA_FILE, BACKUP_FILE) ? "Backup saved to " BACKUP_FILE "." : "Backup failed. Check that the library file exists and is writable.");
}

static void restore_data(void) {
    char answer[8];
    read_text("Restore backup and replace current data? Type yes to confirm: ", answer, sizeof answer);
    if (strcmp(answer, "yes") != 0) { puts("Restore cancelled."); return; }
    puts(copy_file(BACKUP_FILE, DATA_FILE) ? "Library restored from backup." : "Restore failed. Check that the backup file exists.");
}

int main(void) {
    int choice;
    FILE *file = fopen(DATA_FILE, "a");
    if (!file) { puts("Cannot create or open library.txt."); return 1; }
    fclose(file);

    for (;;) {
        puts("\n========== Library Management System ==========");
        puts("1. Add a book");
        puts("2. List all books");
        puts("3. Search by title or author");
        puts("4. Update book details");
        puts("5. Delete a book");
        puts("6. Issue a book");
        puts("7. Return a book");
        puts("8. View library statistics");
        puts("9. Back up library data");
        puts("10. Restore from backup");
        puts("0. Exit");
        choice = read_int("Choose an option: ", 0, 10);
        switch (choice) {
            case 1: add_book(); break;
            case 2: list_books(); break;
            case 3: search_books(); break;
            case 4: update_book(); break;
            case 5: delete_book(); break;
            case 6: issue_book(); break;
            case 7: return_book(); break;
            case 8: show_statistics(); break;
            case 9: backup_data(); break;
            case 10: restore_data(); break;
            case 0: puts("Goodbye."); return 0;
        }
    }
}
