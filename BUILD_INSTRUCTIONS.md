# How to Build and Run This Project on Windows

This project requires a Linux/Unix environment because it uses `pthread.h` which is not available in standard Windows. Here are several options:

## Option 1: Windows Subsystem for Linux (WSL) - RECOMMENDED

### Step 1: Install WSL (if not already installed)
Open PowerShell as Administrator and run:
```powershell
wsl --install
```

This will install Ubuntu by default. Restart your computer when prompted.

### Step 2: Install Build Tools in WSL
After WSL is installed, open Ubuntu (or your Linux distribution) and run:
```bash
sudo apt update
sudo apt install build-essential
```

This installs `gcc`, `make`, and other essential build tools.

### Step 3: Navigate to Project Directory
In your WSL terminal:
```bash
cd /mnt/d/concurrent-hash-table
```

(Windows D: drive is accessible as `/mnt/d/` in WSL)

### Step 4: Build and Run
```bash
make clean
make
./chash
```

## Option 2: Use MSYS2 (Alternative for Windows)

1. Download and install MSYS2 from: https://www.msys2.org/
2. Open MSYS2 terminal
3. Install build tools:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S make
   ```
4. Navigate to your project and build

## Option 3: Manual Compilation (without make)

If you have `gcc` installed but not `make`, you can compile manually:

```bash
gcc -Wall -Wextra -std=c99 -pthread -g -o chash chash.c hash.c rwlock.c hash_table.c logger.c
```

Then run:
```bash
./chash
```

## Option 4: Use a Linux Virtual Machine

You can use VirtualBox or VMware to run a Linux virtual machine and build the project there.

## Quick Test Commands

Once built, you can test with different command files:

```bash
# Use the comprehensive test file
cp commands_comprehensive_test.txt commands.txt
./chash

# Check the output
cat hash.log
```

## Troubleshooting

- **"pthread.h: No such file or directory"**: You need to use WSL or a Linux environment
- **"make: command not found"**: Install build-essential in WSL: `sudo apt install build-essential`
- **Permission denied**: Make sure the executable has permissions: `chmod +x chash`

