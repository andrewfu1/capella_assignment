#include "proj1.h"

int main(int argc, char *argv[])
{
    // Text buffer for preprocessed input
    line_t *pre_buffer = create_line();
    
    // Perform intial preprocessing on input files
    preprocessor(argc, argv, pre_buffer);
    
    // Buffers for for text processing and macro storage
    line_t *text_buffer = create_line();
    macro_list_t *macro_list = create_macro_list();

    // Main process loop to handle macro expansions
    processor_startloop(pre_buffer, text_buffer, macro_list);

    // Outputs processed text
    if (text_buffer != NULL && text_buffer->array != NULL) 
    {
        printf("%s", text_buffer->array);
        free_line(pre_buffer);
        free_line(text_buffer);
        free_macro_list(macro_list);
    }
    else
        DIE("%s", "error");
        
}

// Handles file input and forwards it to the preprocessor loop
void preprocessor(int argc, char *argv[], line_t *pre_buffer) {

    FILE *input;
    if(argc == 1)
    {
        input = stdin;
        preprocessor_startloop(input, pre_buffer);
    }
    else
    {
        // Iterate through input files and process each
        for (int i = 1; i<argc; i++) 
        {
            input = fopen(argv[i], "r");
            if(input == NULL)
                DIE("%s", "error could not open");
            preprocessor_startloop(input, pre_buffer);
        }
    }
}

// Reads input character by character and forwards it to the preprocessor
void preprocessor_startloop(FILE *input, line_t *pre_buffer)
{
    char c = fgetc(input);
    while (c != EOF) {
        preprocessor_tick(c, pre_buffer);
        c = fgetc(input);
    }
}

// Processes characters based on preprocessing state
void preprocessor_tick(char c, line_t *pre_buffer)
{
    static pre_state_t state = pre_state_plaintext;
    switch(state) {
        // Handles escape sequences and comment detection
        case pre_state_plaintext:
            if (c == '\\')
            {
                string_putchar(pre_buffer, c);
                state = pre_state_escape;
            }
            else if(c == '%')
                state = pre_state_comment_start;
            else
            {
                string_putchar(pre_buffer, c);
            }
        break;
        // Makes sure escaped characters are preserved
        case pre_state_escape:
            string_putchar(pre_buffer, c);
            state = state_plaintext;
        break;
        // Ignores comments until a newline is encountered
        case pre_state_comment_start:
            if (c == '\n')
            {
                state = pre_state_comment_end;
            }
        break;
        // Resumes processing after a comment block
        case pre_state_comment_end:
            if (!isblank(c))
            {
                string_putchar(pre_buffer, c);
                state = pre_state_plaintext;
            }
        break;
    }
}

// Main processor loop for each character
void processor_startloop(line_t *pre_buffer, line_t *text_buffer, macro_list_t *macro_list)
{
    line_t *macro_name_buffer = create_line();

    char c;
    int current_position = 0;

    while(current_position <= pre_buffer->length)
    {
        c = pre_buffer->array[current_position];
        tick(c, &current_position, pre_buffer, text_buffer, macro_name_buffer, macro_list);
        current_position ++;
    }
    free_line(macro_name_buffer);
}

// State machine that processes characters and expands macros
void tick(char c, int *current_position, line_t *pre_buffer, line_t *text_buffer, line_t *macro_name_buffer, macro_list_t *macro_list) 
{
    static state_t state = state_plaintext;
    
    switch(state) {
        // Detects escape sequences and starts macro processing
        case state_plaintext:
            if (c == '\\')
                state = state_escape;
            else
                string_putchar(text_buffer, c);
        break;
        // Determines if a macro name follows an escape sequence
        case state_escape:
            if (isalpha(c) || isdigit(c))
            {
                erase_line(macro_name_buffer); // ADDED
                string_putchar(macro_name_buffer, c);
                state = state_macro_name;
            }
            else
            {
                string_putchar(text_buffer, '\\'); // ADDED
                string_putchar(text_buffer, c);
                state = state_plaintext;
            }
                
        break;
        // Parses macro names and validates if syntax is correct
        case state_macro_name:
            if (c == '{')
                state = state_macro_args;
            else if (isalpha(c) || isdigit(c))
                string_putchar(macro_name_buffer, c);
            else
            {
                printf("%s", macro_name_buffer->array);
                DIE("%s", "Macro name is not alphanumeric");
            }
        break;
        // Handles macro argument parsing
        case state_macro_args: {
            int brace_counter = 1; // Counter to make sure the braces are balanced (should never be negative)
            int arguments = how_many_arguments(macro_list, macro_name_buffer);
            int current_arg = 1;
            line_t *arg1 = create_line();
            line_t *arg2 = create_line();
            line_t *arg3 = create_line();
            line_t* macro_buffer = create_line();

            char prev_c = ' ';
            // Loops until all arguments are parsed
            while(arguments != 0)
            {

                if(*current_position > pre_buffer->length)
                    DIE("%s", "ERROR SHOULD NEVER HAPPEN");

                c = pre_buffer->array[*current_position];
                if(*current_position >= 1)
                    prev_c = pre_buffer->array[*current_position - 1];

                // Tracks braces and arguments
                if(brace_counter == 0)
                {
                    if (prev_c == '}' && c != '{')
                        DIE("%s", "error, character in between braces");
                    else if(c == '{')
                    {
                        brace_counter++;
                    }
                }
                else if(brace_counter != 0)
                {
                    if(prev_c == '\\')
                        arg_putchar(c, current_arg, arg1, arg2, arg3);
                    
                    else if(c == '{') {
                        brace_counter++;
                        arg_putchar(c, current_arg, arg1, arg2, arg3);
                    }
                    else if(c == '}') {
                        brace_counter--;
                        if(brace_counter == 0)
                        {
                            arguments--;
                            current_arg++;
                        }
                        else
                            arg_putchar(c, current_arg, arg1, arg2, arg3);
                    }
                    else
                        arg_putchar(c, current_arg, arg1, arg2, arg3);
                }
                (*current_position)++;
            }
            (*current_position)--;

            if(brace_counter != 0)
                DIE("%s", "brace error");

            // Expands macro and integrates it into the output
            state = state_plaintext;
            expand_macro(macro_buffer, macro_list, macro_name_buffer, arg1, arg2, arg3, text_buffer);

            // Cleans up macro buffers so they can be reused
            erase_line(macro_name_buffer);
            free_line(arg1);
            free_line(arg2);
            free_line(arg3);
            
            // Recursively expands the macro
            for(int i=0; i<macro_buffer->length; i++)
            {
                tick(macro_buffer->array[i], &i, macro_buffer, text_buffer, macro_name_buffer, macro_list);
            }
            
            free_line(macro_buffer);
        break;
        }
    }
}

// Expands macros by replacing them with their corresponding values
void expand_macro(line_t *macro_buffer, macro_list_t *macro_list, line_t *macro_name, line_t *arg1, line_t *arg2, line_t *arg3, line_t *text_buffer)
{
    // Defines a new macro
    if(strcmp(macro_name->array, "def") == 0)
    {
        macro_t *tail = macro_list->head;
        while(tail!= NULL)
        {
            if(strcmp(tail->name->array, arg1->array) == 0)
                DIE("%s", "macro already defined");
            else
                tail = tail->next;
        }

        macro_t *new = create_macro(arg1, arg2);

        if (macro_list->head == NULL)
        {
            macro_list->head = new;
        }
        else
        {
            new->next = macro_list->head;
            macro_list->head = new;
        }
    }
    // Undefines (removes) an existing macro
    else if(strcmp(macro_name->array, "undef") == 0)
    {
        macro_t *tail = macro_list->head;
        macro_t *prev = NULL;
        
        while(tail != NULL)
        {
            if(strcmp(tail->name->array, arg1->array) == 0)
            {
                if(prev == NULL)
                    macro_list->head = tail->next;
                else
                    prev->next = tail->next;
                
                free_macro(tail);
                break;
            }
            prev = tail;
            tail = tail->next;
        }
        if(tail == NULL)
            WARN("Cannot undef %s", arg1->array);
    }
    // Conditional expansion based on macro existence
    else if(strcmp(macro_name->array, "if") == 0)
    {
        if(arg1->array[0] != '\0')
        {
            string_putstring(macro_buffer, arg2->array);
        }
        else
        {
            string_putstring(macro_buffer, arg3->array);
        }
    }
    // Checks if a macro is defined and expands accordingly
    else if(strcmp(macro_name->array, "ifdef") == 0)
    {
        macro_t *tail = macro_list->head;
        while(tail != NULL)
        {
            if(strcmp(tail->name->array, arg1->array) == 0)
            {
                string_putstring(macro_buffer, arg2->array);
                break;
            }
            else
                tail = tail->next;
        }
        if(tail == NULL)
            string_putstring(macro_buffer, arg3->array);
    }
    // Handles file inclusion by reading the specified file
    else if(strcmp(macro_name->array, "include") == 0)
    {
        if (arg1 == NULL || arg1->length == 0)
            DIE("%s", "No file path provided for include");

        FILE *input = stdin;


        input = fopen(arg1->array, "r");
        if(input == NULL)
            DIE("%s", "error could not open");

        char c = fgetc(input);
        while (c != EOF) {
            string_putchar(macro_buffer, c);
            c = fgetc(input);
        }

        fclose(input);
    }
    // Delay expanding the before argument until the after argument has been expanded
    else if(strcmp(macro_name->array, "expandafter") == 0)
    {
        line_t *expanded_after = create_line();
        line_t *temp_macro_name = create_line();

        for(int i = 0; i < arg2->length; i++)
        {
            tick(arg2->array[i], &i, arg2, expanded_after, temp_macro_name, macro_list);
        }

        string_putstring(macro_buffer, arg1->array);
        string_putstring(macro_buffer, expanded_after->array);

        free_line(expanded_after);
        free_line(temp_macro_name);
    }
     // Expands user-defined macros by replacing instances with stored values
    else
    {
        macro_t *tail = macro_list->head;
        while(tail != NULL)
        {

            if(strcmp(tail->name->array, macro_name->array) == 0)
            {
                char prev = ' ';
                escape_t state = notescaped;
                for(int i=0; i<tail->arg1->length; i++)
                {
                    if(i >= 1)
                        prev = tail->arg1->array[i - 1];
                    char c = tail->arg1->array[i];
                    switch(state)
                    {
                        case escaped:
                            string_putchar(macro_buffer, tail->arg1->array[i]);
                            state = notescaped;
                        break;
                        case notescaped:
                            if(c == '#')
                            {
                                if(prev == '\\')
                                {
                                    string_putchar(macro_buffer, tail->arg1->array[i]);
                                }
                                else
                                    string_putstring(macro_buffer, arg1->array);
                            }
                            else if(c == '\\')
                            {
                                string_putchar(macro_buffer, tail->arg1->array[i]);
                                state = escaped;
                            }
                            else
                            {
                                string_putchar(macro_buffer, tail->arg1->array[i]);
                            }
                        break;
                    }
                    prev = c;
                }
                break;
            }
            else
                tail = tail->next;
        }
        if(tail == NULL)
            DIE("%s", "User defined macro does not exist");
    }
}

// Helper functions

// Adds a character to the correct macro argument buffer based on the argument index
void arg_putchar(char c, int current_arg, line_t* arg1, line_t* arg2, line_t* arg3)
{
    if(current_arg == 1)
        string_putchar(arg1, c);
    else if(current_arg == 2)
        string_putchar(arg2, c);
    else if(current_arg == 3)
        string_putchar(arg3, c);
}

// Determines the number of arguments a macro requires based on its name
int how_many_arguments(macro_list_t *macro_list, line_t *macro_name)
{
    if(strcmp(macro_name->array, "def") == 0)
        return 2;
    else if(strcmp(macro_name->array, "undef") == 0)
        return 1;
    else if(strcmp(macro_name->array, "if") == 0)
        return 3;
    else if(strcmp(macro_name->array, "ifdef") == 0)
        return 3;
    else if(strcmp(macro_name->array, "include") == 0)
        return 1;
    else if(strcmp(macro_name->array, "expandafter") == 0)
        return 2;
    else
    {
        macro_t *tail = macro_list->head;
        while(tail != NULL)
        {
            if(strcmp(tail->name->array, macro_name->array) == 0)
                return 1;
            else
                tail = tail->next;
        }
        DIE("%s", "Macro does not exist");
    }
}

// Creates and initializes a new line buffer
line_t* create_line()
{
    line_t *line = malloc(sizeof(line_t));
    line->capacity = 32;
    line->length = 0;
    line->array = (char *)malloc(line->capacity * sizeof(char));
    memset(line->array, 0, line->capacity * sizeof(char));
    return line;
}

// Creates an empty macro list
macro_list_t* create_macro_list()
{
    macro_list_t *macro_list = malloc(sizeof(macro_list_t));
    macro_list->head = NULL;
    return macro_list;
}

// Creates a new macro with the given name and argument
macro_t* create_macro(line_t *macro_name, line_t *input_arg1)
{
    macro_t *macro = malloc(sizeof(macro_t));
    macro->name = create_line();
     string_putstring(macro->name, macro_name->array);
    macro->arg1 = create_line();
    string_putstring(macro->arg1, input_arg1->array);
    macro->next = NULL;
    return macro;
}

// Clears the contents of a line buffer
void erase_line(line_t *line)
{
    line->length = 0;
    memset(line->array, 0, line->capacity * sizeof(char));
}

// Frees memory allocated for a line buffer
void free_line(line_t *line)
{
    line->capacity = 32;
    line->length = 0;
    free(line->array);
    free(line);
}

// Frees memory allocated for a macro structure
void free_macro(macro_t *macro)
{
    free_line(macro->name);
    free_line(macro->arg1);
    free(macro);
}

// Frees all macros in the macro list
void free_macro_list(macro_list_t *macro_list)
{
    macro_t *current = macro_list->head;
    macro_t *next;

    while (current != NULL) {
        next = current->next;
        free_macro(current);
        current = next;
    }

    macro_list->head = NULL;
    free(macro_list);
}

// Appends a character to a line buffer, expanding if necessary
void string_putchar(line_t *line, char c)
{
    expand_buffer_if_needed(line);
    line->array[line->length] = c;
    line->length ++;

    line->array[line->length] = '\0';
}

// Appends an entire string to a line buffer
void string_putstring(line_t *line, char *string)
{
    while (*string != '\0') {
        string_putchar(line, *string);
        string++;
    }
}

// Expands the buffer size if needed to accommodate more characters
void expand_buffer_if_needed(line_t *line)
{
    if(line->length + 1 >= line->capacity)
    {
        line->capacity = line->capacity * 2;
        line->array = realloc(line->array, line->capacity * sizeof(char));
    }
}