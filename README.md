# Tiny CPU emulator

A C++ emulator for a small instruction set. It runs 16-bit instructions from a binary file, works with eight 8-bit registers and 256 bytes of program memory, and reads and writes bytes through files.

## Build and run

Use a C++17 compiler and CMake 3.16 or later. Run these commands from the repository root.

```sh
cmake -S . -B build
cmake --build build
./build/tiny_cpu_emulator examples/echo.bin examples/input.txt output.txt
cmp examples/input.txt output.txt
```

The sample copies the input byte for byte. A successful `cmp` prints nothing. To run it in CLion, set the working directory to the repository root and pass `examples/echo.bin examples/input.txt output.txt` as program arguments.

## Example program

The [sample binary](examples/echo.bin) reads one byte into register `r0`, checks the end-of-file flag, and either writes the byte and repeats or halts.

| Byte address | Hex word | Operation |
| --- | --- | --- |
| 0 | `0800` | Read a byte into `r0` |
| 2 | `5006` | Jump to address 8 at end of input |
| 4 | `1000` | Write the byte in `r0` |
| 6 | `28FA` | Jump back to address 0 |
| 8 | `5800` | Halt |

Instructions are stored high byte first. The top five bits select the opcode. Branches use signed byte offsets relative to the current instruction. The emulator also supports register moves, arithmetic, bitwise operations, and logical shifts.
