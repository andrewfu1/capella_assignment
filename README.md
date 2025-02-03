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
% This defines a simple macro for greetings
\def{GREET}{Hello, #!} 

% Now, we use the macro with different arguments
\GREET{Alice}
\GREET{Bob}
```
#### Output
```tex
Hello, Alice!
Hello, Bob!
```
Here, \def{GREET}{Hello, #!} defines GREET as Hello, #!, where # gets replaced by the macro's argument. When \GREET{Alice} is encountered, it expands to Hello, Alice!. Comments are defined using %, and are not outputted.

## Solution Walkthrough

### 1. Preprocessing Phase
The preprocessor reads the input file(s), removes comments (% to newline), and processes escape characters (\, {, }, #, %). The cleaned-up text is stored in a buffer (pre_buffer) before macro expansion begins.

Key Functions:
* preprocessor(argc, argv, pre_buffer)
  * Determines whether input comes from stdin or files.
  * Calls the function to process input files line-by-line.
preprocessor_startloop(input, pre_buffer)
Reads characters from the input file and passes them for further processing.
preprocessor_tick(c, pre_buffer)
Implements a state machine to process escape sequences and remove comments.
Ensures that escape characters like \, {, }, #, and % are handled properly.
Strips out comments while preserving the remaining content.

#### 2. Processing Phase
The macro processor:
Scans the pre_buffer character by character.
Identifies and extracts macro names and their arguments.
Expands macros using a linked list (macro_list_t) that stores defined macros.
Recursively replaces macro calls with their expanded values.
#### 3. Macro Expansion
When encountering a macro, the processor:
Checks if it is built-in or user-defined.
Retrieves its stored value from macro_list_t.
Expands the macro’s value while substituting its arguments (# placeholders).


