#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "macro.h"

macro_list* get_macro(macro_list* mh, char* name){
	macro_list *point = mh; // DS-REVIEW: not sure why it is called "point", more of "node" I would say.
	if(point==NULL){
		return NULL;
	}
	while (point!=NULL){
		if(strcmp(name,point->name)==0){
			return point;
		}
		point = point->next_macro;
	}
	return NULL;
}

void write_macro_to_file(macro_list *macro, FILE *file){
	line_list *line = macro->line_head;
	while (line != NULL){
		fprintf(file,"%s", line->line);
		line = line->next_line;
	}
}


boolean add_line_to_macro(macro_list* macro, char* line){
	line_list *head = macro->line_head;
	line_list *point = head;
	line_list *new = (line_list *)malloc(sizeof(line_list));
	if (new==NULL){
		printf("problam in alocaiting mamory for macro");
		return false;
	}

	new->next_line = NULL;
	new->line = malloc(strlen(line) + 1);
	if (new->line==NULL){
		printf("problam in alocaiting mamory for macro");
		free(new);
		return false;
	}
	strcpy(new->line,line);
	/*if first macro*/
	if (head==NULL){
		macro->line_head = new;
	}else{
		/*go to the end of the list*/
		while (point->next_line!=NULL){
			point = point->next_line;
		}
		point->next_line = new;
	}
	return true;
}

macro_list* add_new_macro(macro_list* mh, char* name){
	macro_list *head = (macro_list *)malloc(sizeof(macro_list));
	if (head == NULL){
		printf("problam in alocaiting mamory for macro");
		return NULL;
	}
	
	head->name = malloc(strlen(name) + 1);
	if (head->name==NULL){
		printf("problam in alocaiting mamory for macro");
		free(head);
		return NULL;
	}

	strcpy(head->name,name);
	head->line_head = NULL;
	head->next_macro = mh;

	return head;
}

boolean check_macro_name(macro_list* head,char *word){
	if(word==NULL || get_macro(head , word) != NULL){
		return false;
	}
	/*is_register(word)==false, is_comendline(word) == false, is_start_letter(word) = true than return true, strlen(word)<=MAX_MACRO_NAME_SIZE = true */ // DS-REVIEW: remove commented out code
	return !(is_register(word) || is_comendline(word) || !is_start_letter(word) || !(strlen(word)<=MAX_MACRO_NAME_SIZE));
}

void free_macro(macro_list* head){
	macro_list *current_macro = head;
	macro_list *next_macro;
	line_list *current_line;
	line_list *next_line;

	if(head == NULL){
		return;
	}

	/*free the outer link list the macros themself*/
	while (current_macro != NULL){
		next_macro = current_macro->next_macro;
		current_line = current_macro->line_head;

		/*free iner link list the lines if the macro*/
		while (current_line != NULL)
		{
			next_line = current_line->next_line;
			free(current_line->line);
			free(current_line);
			current_line = next_line;
		}
		free(current_macro->name);
		free(current_macro);
		current_macro = next_macro;		
	}
}

