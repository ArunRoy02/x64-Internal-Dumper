# x64 Internal Dumper

This project is a x64-based Windows memory dumper, implemented as a dynamic link library (DLL). The core functionality of this dumper revolves around bypassing commonly hooked user-space functions by utilizing syscalls directly. This project serves as a proof of concept, demonstrating how to effectively invoke syscalls to avoid detection and interference from user-space hooks.

## Features

- **DLL-Based Injection**: The dumper is implemented as a DLL, which means it needs to be injected into the target process. You can achieve this through various methods, such as using an injector or leveraging `LoadLibrary`—the choice is yours.

- **Bypass User-Space Hooks**: The primary appeal of this dumper is its ability to bypass hooks placed on functions like `VirtualAlloc`, `VirtualFree`, and `QueryVirtualMemoryInformation`. Instead of relying on these potentially hooked functions, the dumper directly invokes the syscalls through alternative functions:
  - `ZwAllocateVirtualMemory`
  - `ZwFreeVirtualMemory`
  - `ZwQueryVirtualMemory`

- **Direct Syscall Invocation**: By copying the assembly instructions of the original functions and adding external declarations, the dumper makes direct syscall invocations, effectively bypassing any user-space hooks that might interfere with standard API calls.

- **Process and Module Dumping**: The dumper provides two modes of operation:
  1. **Current Process Dumping**: Dump the memory of the process it is injected into.
  2. **Module Iteration and Dumping**: Iterate through all loaded modules in the current process and dump each one individually.
  
- **Output Folder**: Generates a folder on the desktop with a timestamp containing all the dumped binaries.

## Usage

1. **Injection**: Since this project is a DLL, you must inject it into the target process. You can use any preferred method of DLL injection, such as a custom injector or the `LoadLibrary` function.

2. **Selecting the Dump Mode**: Once injected, the dumper can either:
   - Dump the memory of the current process.
   - Iterate through and dump each loaded module.

   The mode of operation can be selected from the Initialize() function inside DllMain.

3. **Syscall-Based Memory Management**: The dumper's use of syscalls ensures that operations like memory allocation, deallocation, and querying bypass user-space hooks, providing a cleaner and more reliable dumping process.

## Disclaimer

This project is intended as a proof of concept for syscall usage in bypassing user-space hooks. It is designed for educational and research purposes only. Use it responsibly and only on systems and software you have explicit permission to analyze.
