// proj1.h                                          Stan Eisenstat (09/17/15)
//
// System header files and macros for proj1

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char *array;
    size_t length;
    size_t capacity;
} line_t;

typedef struct _macro_t macro_t;
struct _macro_t {
    line_t *name;
    line_t *arg1;
    macro_t *next;
};

typedef struct {
    macro_t *head;
} macro_list_t;

typedef enum {
    escaped,
    notescaped,
} escape_t;

typedef enum {
    pre_state_plaintext,
    pre_state_comment_start,
    pre_state_comment_end,
    pre_state_escape,
} pre_state_t;

typedef enum {
    state_plaintext,
    state_escape,
    state_macro_name,
    state_macro_args, // read '}', store buffer, start new buffer, expecting '{' on next
    //how to keep track of number of arguments
} state_t;

// Write message to stderr using format FORMAT
#define WARN(format,...) fprintf (stderr, "proj1: " format "\n", __VA_ARGS__)

// Write message to stderr using format FORMAT and exit.
#define DIE(format,...)  WARN(format,__VA_ARGS__), exit (EXIT_FAILURE)

// Double the size of an allocated block PTR with NMEMB members and update
// NMEMB accordingly.  (NMEMB is only the size in bytes if PTR is a char *.)
#define DOUBLE(ptr,nmemb) realloc (ptr, (nmemb *= 2) * sizeof(*ptr))

void preprocessor(int argc, char *argv[], line_t *pre_buffer);
void preprocessor_startloop(FILE *input, line_t *pre_buffer);
void preprocessor_tick(char c, line_t *pre_buffer);

void processor_startloop(line_t *pre_buffer, line_t *text_buffer, macro_list_t *macro_list);
void tick(char c, int *current_position, line_t *pre_buffer, line_t *text_buffer, line_t *macro_name_buffer, macro_list_t *macro_list);
void arg_putchar(char c, int current_arg, line_t* arg1, line_t* arg2, line_t* arg3);
void expand_macro(line_t *macro_buffer, macro_list_t *macro_list, line_t *macro_name, line_t *arg1, line_t *arg2, line_t *arg3, line_t *text_buffer);
int how_many_arguments(macro_list_t *macro_list, line_t *macro_name);

line_t* create_line();
macro_list_t* create_macro_list();
macro_t* create_macro(line_t *macro_name, line_t *input_arg1);

void erase_line(line_t *line);
void free_line(line_t *line);
void free_macro(macro_t *macro);
void free_macro_list(macro_list_t *macro_list);

void string_putchar(line_t *line, char c);
void string_putstring(line_t *line, char *string);
void expand_buffer_if_needed(line_t *line);