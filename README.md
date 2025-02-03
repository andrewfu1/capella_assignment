# LaTeX-Like Macro Processor

## Project Overview

This project implements a **TeX-like macro processor** in C. The program reads an input text file (or standard input), processes **macro definitions and expansions**.

### Features
- Macro Definition & Expansion: Supports defining and using macros with arguments.
- Built-in Macros: Implements common TeX-like commands such as \def, \undef, \if, \ifdef, \include, and \expandafter.
- Recursive Expansion: Handles nested macro calls and substitutions correctly.
- File Inclusion: Allows including external files using \include.

### Example Execution

#### Input
```tex
\def{GREETING}{Hello, #!}
\GREETING{World}
```
#### Output
```tex
Hello, World!
```
Here, \def{GREETING}{Hello, #!} defines GREETING as Hello, #!, where # gets replaced by the macro's argument. When \GREETING{World} is encountered, it expands to Hello, World!.

## Solution Walkthrough

#### 1. Preprocessing Phase
The preprocessor reads the input file(s), removes comments (% to newline), and processes escape characters (\, {, }, #, %). The cleaned-up text is stored in a buffer (pre_buffer) before macro expansion begins.

#### 2. Processing Phase
The macro processor:
Scans the pre_buffer character by character.
Identifies and extracts macro names and their arguments.
Expands macros using a linked list (macro_list_t) that stores defined macros.
Recursively replaces macro calls with their expanded values.
Handles built-in macros like \if, \include, and \expandafter.
### 3. Macro Expansion
When encountering a macro, the processor:
Checks if it is built-in or user-defined.
Retrieves its stored value from macro_list_t.
Expands the macro’s value while substituting its arguments (# placeholders).
If the macro has nested macros inside its expansion, they are processed recursively.
