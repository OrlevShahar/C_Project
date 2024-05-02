/*list of line in a specific macro*/
typedef struct line_list
{
	char *line;						/*a line in the macro */
	struct line_list *next_line; 	/*pointer for the next line*/
}line_list;

/*List of Macros Each macro has its own list of macros*/
typedef struct macro_list
{
	char *name;						/* name of the macro */
	struct line_list *line_head; 	/*head of the lines*/
	struct macro_list *next_macro; 	/*pointer for the next macro*/
}macro_list;


/*Function that returns a macro with a given name if it does not exist returns NULL*/
macro_list* get_macro(macro_list* mh, char* name);

/*Function that write a given macro to file*/
void write_macro_to_file(macro_list *macro, FILE *file);

/*Function that add a new line for a given macro*/
boolean add_line_to_macro(macro_list* macro, char* line);

/*Function that add a new macro to the macro list*/
macro_list* add_new_macro(macro_list* mh, char* name);

/*Function that frees all dynamically allocated memory in the macro*/
void free_macro(macro_list* head);

/*Function that check if the name of the macro is legal*/
boolean check_macro_name(macro_list* head,char *word);
