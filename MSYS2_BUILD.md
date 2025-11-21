# Building with MSYS2

## Prerequisites

1. Make sure you have MSYS2 installed
2. Open **MSYS2 MinGW 64-bit** terminal (or MinGW 32-bit if you prefer)

## Install Required Tools (if needed)

In the MSYS2 terminal, run:

```bash
# Update package database
pacman -Syu

# Install build tools (if not already installed)
pacman -S make
pacman -S mingw-w64-x86_64-gcc  # For 64-bit
# OR
pacman -S mingw-w64-i686-gcc    # For 32-bit
```

## Build the Project

1. Navigate to your project directory in MSYS2:
   ```bash
   cd /d/concurrent-hash-table
   ```
   (Note: Windows D: drive is usually accessible as `/d/` in MSYS2)

2. Clean previous builds (optional):
   ```bash
   make clean
   ```

3. Build the project:
   ```bash
   make
   ```

4. Run the program:
   ```bash
   ./chash.exe
   ```

## Test with Comprehensive Test File

```bash
cp commands_comprehensive_test.txt commands.txt
./chash.exe
```

## Check Output

The program will:
- Output results to stdout (console)
- Create `hash.log` with diagnostic information

View the log:
```bash
cat hash.log
```

## Troubleshooting

- **"pthread.h not found"**: Make sure you're using the MinGW terminal, not the plain MSYS terminal
- **"make: command not found"**: Install with `pacman -S make`
- **Permission denied**: In MSYS2, use `./chash.exe` (include .exe extension)

