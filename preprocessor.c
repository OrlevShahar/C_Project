#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "utils.h"
#include "macro.h"

boolean preprocessor_step(char *file){
    char *file_am_name = add_file_ending(file, ".am");/*new file*/
    char *file_as_name = add_file_ending(file, ".as");/*original file*/
    FILE *file_as = fopen(file_as_name,"r");
    FILE *file_am = fopen(file_am_name,"w");
    char line [MAX_LINE_SIZE + 2]; 
    int line_pointer;
    char *macro_name=NULL;
    boolean is_mcr_found = false;
    boolean preprocessor_success = true;
    macro_list *head = NULL;
    macro_list *macro = NULL;

    /*Checking if the fopen of the .as file was successful. If not, returning false*/
    if (file_as ==NULL){
        printf("problem in opening %s",file_as_name);
        free(file_am_name);
        free(file_as_name);
        fclose(file_as);
        fclose(file_am);
        return false;
    }

    /*Checking if the fopen of the .am file was successful. If not, returning false*/
    if (file_am ==NULL){
        printf("problem in creating %s",file_am_name);
        free(file_am_name);
        free(file_as_name);
        fclose(file_as);
        fclose(file_am);
        return false;
    }

    /*Runs through the file_as line by line.*/
    while(fgets(line,MAX_LINE_SIZE + 2,file_as)!=NULL){
        char *first_word=NULL;
        line_pointer = 0;
        first_word = read_next_word(line,&line_pointer);
        
        /*macro write procedure*/
        if(!is_mcr_found && (macro = get_macro(head,first_word)) != NULL){
            write_macro_to_file(macro,file_am);
            macro = NULL;
            free(first_word);
            continue;
        }

        /*start reading a new macro*/
        if(strcmp(first_word,"mcr")==0){
            is_mcr_found = true;
            macro_name = read_next_word(line, &line_pointer);
            if (!check_macro_name(head,macro_name) || is_garbeg(line,&line_pointer)){
                printf("incorect macro name/characters after the name");
                free(macro_name);
                free(first_word);
                preprocessor_success = false;
                break;
            }
            head = add_new_macro(head, macro_name);
            free(macro_name);
            free(first_word);
            continue;
        }

        /*reading a new line to the macro*/
        if (is_mcr_found && strcmp(first_word,"endmcr")!=0){
            if(add_line_to_macro(head,line)==false){
                free(first_word);
                preprocessor_success = false;
                break;
            }
            free(first_word);
            continue;            
        }

        /*ending reading a macro*/
        if (strcmp(first_word,"endmcr")==0){
            if(is_garbeg(line, &line_pointer)){
                printf("characters after the endmcr");
                free(first_word);
                preprocessor_success = false;
                break;
            }
            is_mcr_found = false;
            free(first_word);
            continue;
        }
        
        fprintf(file_am,"%s", line);
        free(first_word);
    }
    
    fclose(file_as); 
    fclose(file_am); 
    /*if problem in preprocessor delete the am file*/
    if(preprocessor_success == false){
        remove(file_am_name);
    }

     /*free all the dynamic memory*/
    free(file_am_name);
    free(file_as_name);
    free_macro(head);
    return preprocessor_success;
}
