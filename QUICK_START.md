# Quick Start Guide

## On Windows: Use WSL (Windows Subsystem for Linux)

### 1. Check if WSL is installed:
```powershell
wsl --list
```

If you see a Linux distribution listed, skip to step 3.

### 2. Install WSL (if needed):
Open PowerShell **as Administrator** and run:
```powershell
wsl --install
```

This installs Ubuntu. Restart your computer when prompted.

### 3. Open WSL/Ubuntu Terminal:
- Open Ubuntu from Start Menu, OR
- Run `wsl` in PowerShell

### 4. Install build tools in WSL:
```bash
sudo apt update
sudo apt install build-essential
```

### 5. Navigate to your project:
```bash
cd /mnt/d/concurrent-hash-table
```

Note: Windows `D:\` drive = `/mnt/d/` in WSL

### 6. Build the project:
```bash
make clean
make
```

### 7. Run it:
```bash
./chash
```

Or test with the comprehensive test:
```bash
cp commands_comprehensive_test.txt commands.txt
./chash
```

---

## Alternative: Manual Compilation (if you have gcc but not make)

If you have gcc installed (e.g., via MinGW), compile manually:

```bash
gcc -Wall -Wextra -std=c99 -pthread -g -o chash chash.c hash.c rwlock.c hash_table.c logger.c
```

---

## Need Help?

- See BUILD_INSTRUCTIONS.md for more detailed options
- This project requires pthread.h which is only available on Linux/Unix systems
- WSL is the easiest way to get a Linux environment on Windows

