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
\def{GREET}{Hello, #!} defines GREET as Hello, #!, where # gets replaced by the macro's argument. When \GREET{Alice} is encountered, it expands to Hello, Alice!. 
Comments are defined using %, and are not outputted.

## Approach and Design

To implement a solution to this project, I used several key programming techniques.

#### 1. State Machine for Input Processing
A finite state machine (FSM) was used to parse input, handle macros, and track context. The tick() function maintains state variables using an enum to determine whether a character belongs to:
- Plain text
- A macro name
- A macro argument
- A comment (to be ignored)
- An escaped character

#### 2. Struct-Based Buffer and Macro Management
The processor stores text using struct-based buffers (line_t) that allow dynamic resizing.

#### 3. Linked Lists for Macro Storage
A linked list-based symbol table (macro_list_t) is used to store user-defined macros. Each macro entry (macro_t) contains:
- Macro name
- Macro replacement text
- Pointer to the next macro

#### 1. Preprocessing Phase
The preprocessor reads the input file(s), removes comments (% to newline), and processes escape characters (\, {, }, #, %).
The cleaned-up text is stored in a buffer (pre_buffer) before macro expansion begins.
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


