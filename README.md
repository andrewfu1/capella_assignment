# LaTeX-Like Macro Processor

## Project Overview

This project implements a **TeX-like macro processor** in C. The program reads an input text file (or standard input), processes **macro definitions and expansions**, and outputs the fully expanded text. It supports various built-in macros (`\def`, `\undef`, `\if`, `\ifdef`, `\include`, `\expandafter`) while also allowing **user-defined macros**.

## Example Execution

### Input
```tex
\def{GREETING}{Hello, #!}
\GREETING{World}
```
### Output
```tex
Hello, World!
```
Here, \def{GREETING}{Hello, #!} defines GREETING as Hello, #!, where # gets replaced by the macro's argument. When \GREETING{World} is encountered, it expands to Hello, World!.

### Features
Macro Definition & Expansion: Supports defining and using macros with arguments.
Built-in Macros: Implements common TeX-like commands such as \def, \undef, \if, \ifdef, \include, and \expandafter.
Recursive Expansion: Handles nested macro calls and substitutions correctly.
File Inclusion: Allows including external files using \include.
