# Simple Code Inserter

Insert print statements in every function in a given source file. 

## Usage 

1. Single File

```bash
./insert-print-stmts main.cpp
```

2. Multiple File 

```bash
./insert-print-stmts main.cpp foo.cpp bar.cpp 
```

or 

```bash
find <abs-path-to-src> -name "*.cpp" | xargs ./insert-print-stmts
```

