#ifndef UTILS_H
#define UTILS_H

#define MAX_MACRO_NAME_SIZE 31
#define MAX_LINE_SIZE 80
#define NUMBER_OF_REGISTERS 8
#define NUMBER_OF_COMENDLINE 16
#define NUMBER_OF_SAVE_WORD 7
#define MEMORY_SIZE 3996
#define MEMORY_START 100
#define BASE_4_WORD_SIZE 7

extern const char *registers[];
extern const char *comendline[];
extern const char *save_word[];

/*Creating a new type "boolean" where false = 0 and true = 1*/
typedef enum {
   false ,
   true
} boolean;

/*Function that allocate and assign the file name and the desired ending*/
char *add_file_ending(char *file, char* end);

/*Function to skip blank spaces in line*/
void skip_leading_spaces(char* line, int* point);

/*Function that reads the next word in the line if no word or symbol is left return NULL*/
char *read_next_word(char *line, int *point);

/*Function that check if the given word is one of the registers*/
boolean is_register(char *word);

/*Function that check if the given word is one of the comendline*/
boolean is_comendline(char *word);

/*Function that returns the number of the command line (its binary number to int)*/
int get_comendline_number(char* str);

/*Function that check if the given word is one of the save words*/
boolean is_save_word(char *word);

/*Function that check if the given word is starting in a leter*/
boolean is_start_letter (char *word);

/*Function to find any unwanted characters at the end of the command return true if there is unwanted characters */
boolean is_garbeg(char* line, int* point);


#endif 
