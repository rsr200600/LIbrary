# Library Management System

A command-line library management program written in C. It stores book records in a local text file and supports catalogue management, book loans, statistics, and data backup.

## Features

- Add books with a unique ID, title, author, and publication year.
- List all books and see whether each is available or issued.
- Search book titles and authors.
- Update a book's title, author, or publication year.
- Delete a book after confirmation.
- Issue an available book to a borrower and record its due date.
- Return an issued book and mark it available again.
- View total, available, and issued book counts.
- Back up the catalogue and restore it from a backup.
- Validate menu choices and numeric values.

## Requirements

- A C compiler, such as GCC or Clang.
- A terminal or command prompt.

## Build

Save the source file as `library_management.c`, then compile it with GCC:

```bash
gcc -std=c99 -Wall -Wextra -pedantic library_management.c -o library_management
```

On Windows with MinGW, run the executable from PowerShell:

```powershell
gcc -std=c99 -Wall -Wextra -pedantic library_management.c -o library_management.exe
./library_management.exe
```

On macOS or Linux:

```bash
./library_management
```

## Run

Start the program from the directory where you want the data files stored:

```bash
./library_management
```

On Windows PowerShell:

```powershell
./library_management.exe
```

The program displays a menu. Enter the number for the action you want, then follow the prompts. Choose `0` to exit.

## Data and Backup Files

The program automatically creates `library.txt` in its current working directory if the file does not already exist. It stores one book per line using comma-separated fields:

```text
id,title,author,year,issued,borrower,due_date
```

The value `1` in the `issued` field means the book is on loan; `0` means it is available. A hyphen (`-`) is used for borrower and due-date fields when a book is available.

Selecting the backup option writes a copy to `library_backup.txt`. Restoring replaces the current `library.txt` contents with the backup after confirmation.

Keep both files in a safe location. The program stores data locally and does not encrypt it.

## Input Notes

- Book IDs must be positive and unique.
- Enter `0` for an unknown publication year.
- Do not use commas in titles, author names, borrower names, or due dates because commas separate fields in the data file.
- Enter due dates as text, for example `2026-12-31`. The program records the date but does not validate calendar correctness or calculate overdue status.

## Limitations

- This is a single-user, local command-line application.
- The data file is plain text and is not encrypted.
- Search is case-sensitive and matches title or author text.
- The program does not calculate fines, enforce loan periods, or track multiple physical copies of the same title.
- Avoid editing `library.txt` while the program is running. Keep a backup before making manual changes.

## Source Code

The program source is available in [`library_management.c`](library_management.c).
