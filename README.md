# Simple Code Inserter

Insert arbitrary code in every function in a given source file. 

## Usage 

1. Single File

### Insert code at top

```bash
./simple-code-inserter --top=<code> <input-file.cpp>
```

### Insert code at top, end and header file

```bash
./simple-code-inserter --top=<code> --end=<code> --include=<header> <input-file.cpp>
```

2. Multiple File 

```bash
./simple-code-inserter --top=<code> --end=<code> --include=<header> <input-file1.cpp> <input-file2.cpp> ... <input-filen.cpp>
```

## Limitations
1. Does not support functions expanded from macros. 
2. Does not work with constructors when using '{name}' in code. 
