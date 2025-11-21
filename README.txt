Concurrent Hash Table Implementation
=====================================
System Requirements:
--------------------
- Linux/Unix system OR Windows with MSYS2/MinGW (recommended) OR WSL
- POSIX-compliant C environment (for pthread.h and strtok_r support)
- The code requires _GNU_SOURCE or _POSIX_C_SOURCE feature test macros to enable
  POSIX functions like strtok_r. This is automatically handled in the Makefile
  and source code.
- GCC compiler with pthread support
- Make utility

Important Build Notes:
----------------------
- This project uses POSIX functions (pthread.h, strtok_r) which are NOT available
  in standard Windows environments
- On Windows, use one of the following:
  * MSYS2/MinGW (recommended) - See MSYS2_BUILD.md
  * WSL (Windows Subsystem for Linux) - See QUICK_START.md
  * Linux virtual machine
- The Makefile includes -D_GNU_SOURCE flag to enable POSIX functions
- chash.c includes #define _GNU_SOURCE at the top for compatibility

Files:
------
- chash.c: Main program that reads commands.txt and processes commands with threads
- hash.c/h: Jenkins one-at-a-time hash function implementation
- rwlock.c/h: Reader-writer lock implementation using pthreads
- hash_table.c/h: Hash table data structure and operations (insert, delete, update, search, print)
- logger.c/h: Logging functionality for hash.log file
- Makefile: Build configuration (includes -D_GNU_SOURCE for POSIX support)
- commands.txt: Input file containing commands to execute

Building:
---------
On Linux/Unix/MSYS2/WSL:
  Run 'make' to build the executable 'chash' (or 'chash.exe' on Windows/MSYS2)

On MSYS2/MinGW (what i use):
  - Open MSYS2 MinGW 64-bit terminal
  - Navigate to project directory
  - Run: make clean && make

On WSL:
  - Open Ubuntu/WSL terminal
  - Navigate to: cd /mnt/d/concurrent-hash-table
  - Install build tools if needed: sudo apt install build-essential
  - Run: make clean && make

Running:
--------
Run './chash' (or './chash.exe' on Windows/MSYS2) to execute the program. It will:
1. Read commands from commands.txt
2. Process commands using threads with priority ordering
3. Output results to console (stdout)
4. Write diagnostic log to hash.log

Commands:
---------
- insert: <name>,<salary>,<priority> - Insert a new record
- delete: <name>,<priority> - Delete a record
- update: <name>,<new_salary> - Update salary (priority auto-assigned)
- search: <name>,<priority> - Search for a record
- print: <priority> - Print all records sorted by hash

The program will automatically execute a final PRINT at the end.

AI Use Citation:
---------------
This project was implemented with the assistance of an AI coding assistant
(Cursor AI/ChatGPT). Portions of this project were created, specifically the header files,
the make file, build scripts, and the above portion of this README.txt file. all AI generated content was reviewed
and ensured to the best of my ability to be correct. Partial modifications were made to ai content such as function signatures in header files
while work progressed and signatures changed. Additionally, logger.c was initially generated via AI, given the code from the assignement instructions
for the timestamp, and the information on output formatting.

The prompts used to get the results were situation based for the make and build file, ex: 'create a generic build file for this project structure' (screenshot of file structure)
Additionally, cursor AI was used to update the make and build files created by ChatGPT since it has scope of the project.

ex: "i will provide the sample input and sample log as a .txt file and .log file. Additionally, here is the sample output:
This document shows the expected stdout output for commands.txt (comprehensive test workload). Make the output formatting match without changing
any logic."