#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "utils.h"
#include "macro.h"


const char *registers[NUMBER_OF_REGISTERS] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

const char *comendline[NUMBER_OF_COMENDLINE] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", 
"dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};

const char *save_word[NUMBER_OF_SAVE_WORD] = {"define", "extern", "entry", "string", "data", "endmcr", "mcr"};

char *add_file_ending(char *file, char* end) {
	char *str = (char *)malloc(strlen(file) + strlen(end) + 1);
	strcpy(str, file);
	strcat(str, end);
	return str;
}

void skip_leading_spaces(char* line, int* point) {
    while (line[*point] != '\0' && (line[*point] == ' ' || line[*point] == '\t')) {
        (*point)++;
    }
}

boolean is_register(char *word){
	int i;
	for(i=0; i < NUMBER_OF_REGISTERS; i++){
		if (strcmp(registers[i],word)==0){
			return true;
		}
	}
	return false;
}

boolean is_comendline(char *word){
	int i;
	for(i=0; i < NUMBER_OF_COMENDLINE; i++){
		if (strcmp(comendline[i],word)==0){
			return true;
		}
	}
	return false;
}

boolean is_save_word(char *word){
	int i;
	for(i=0; i < NUMBER_OF_SAVE_WORD; i++){
		if (strcmp(save_word[i],word)==0){
			return true;
		}
	}
	return false;
}

boolean is_start_letter (char *word){
	return isalpha(*word) != 0;
}

char *read_next_word(char *line, int *point){
	int length = 0;
    /*allocate memory for word*/
    char* word = (char*)malloc((MAX_MACRO_NAME_SIZE+2) * sizeof(char));
	if (word == NULL) {
		printf("Memory allocation failed in read next word\n");
		return NULL;
	}
	
	skip_leading_spaces(line, point);

	while (line[*point] != '\0' && line[*point] != ' ' && line[*point] != '\t' && line[*point] != '\n' &&length < MAX_MACRO_NAME_SIZE+1) {		
		word[length] = line[*point];
		length++;
		(*point)++;
		
	}
	word[length] = '\0';
	return word;
}

boolean is_garbeg(char* line, int* point){
	skip_leading_spaces(line, point);
	if(line[*point] == '\0'||line[*point] == '\n'){
		return false;
	}
	return true;
}

int get_comendline_number(char* str){
	int i;
	/*check if str == NULL*/
	if(strlen(str)==0){
		return -1;
	}
	for (i = 0; i < NUMBER_OF_COMENDLINE; i++){
		if(strcmp(comendline[i], str) == 0){
			return i;
		}
	}
	return -1;
	
}
