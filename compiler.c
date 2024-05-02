#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "utils.h"
#include "macro.h"
#include "data_table.h"
#include "data_structur.h"
#include "preprocessor.h"
#include "first_run.h"
#include "second_run.h"

/*Function that call the funtion that creates the .ob .ent .ext files, the ent and the ext files will only be created if they have values*/
void create_files(char *file_name, table_list* table_head, int* IC, int* DC, int *memory_image, label_list *extern_head, label_list *entry_head);

/*Function that creates the .ob file*/
void ob_file_create(char *file_ob_name,  int IC, int DC, int *memory_image);

/*Function that takes the 14 binary sequence and turns them into 4 base encoded ssequence*/
void base_4_encode(int bin, char *encode);

/*Function that gets 2 bit and return the encode char*/
char bim_to_code(int tow_bit);

int main (int argc, char *argv[]){ // DS-REVIEW: Missing space before curly brace
    int i, IC, DC; // DS-REVIEW: Not clear what IC and DC are. Give them a meaningful name even if it's longer.
    /*run through the files and compile them*/
    for (i = 1; i < argc; i++) {
        int memory_image[MEMORY_SIZE + 2]  = {0}; // DS-REVIEW: it is not clear what the significance of 2 is. Maybe define a constant that explains it or add a comment.
        table_list* table_head = NULL;
        label_list* extern_head = NULL;
        label_list* entry_head = NULL;

        if(!preprocessor_step(argv[i])){
            printf("problem in %s", argv[i]);
            continue;
        }
        IC = 0; DC = 0;
        if(first_run(argv[i], &table_head, &IC, &DC, memory_image, &extern_head, &entry_head) == true && second_run(argv[i], table_head, &IC, &DC, memory_image, &extern_head, &entry_head) == true){
            /*If no errors were found in the first run and the second run*/
            create_files(argv[i], table_head, &IC, &DC, memory_image, extern_head, entry_head);
            
        } // DS-REVIEW: Missing `else` statement for error handling. In this case you should print a failure message and return a non-zero exit code.
        /*free all link lists*/
        free_table_list(table_head);
        free_label_list(extern_head);
        free_label_list(entry_head);

    }
    return 0;
}

void create_files(char *file_name, table_list* table_head, int* IC, int* DC, int *memory_image, label_list *extern_head, label_list *entry_head){
    char *file_ob_name = add_file_ending(file_name, ".ob");
    char *file_ent_name = add_file_ending(file_name, ".ent");
    char *file_ext_name = add_file_ending(file_name, ".ext");
    
    ob_file_create(file_ob_name, *IC, *DC, memory_image);

    if(extern_head != NULL){
        label_file_create(file_ext_name, extern_head);
    }

    if(entry_head != NULL){
        label_file_create(file_ent_name, entry_head);
    }
    /*free the names of the files*/
    free(file_ob_name);
    free(file_ent_name);
    free(file_ext_name);
}

void ob_file_create(char *file_ob_name,  int IC, int DC, int *memory_image){
    FILE *file_ob = fopen(file_ob_name,"w");
    char encode[BASE_4_WORD_SIZE+1];
    int i;
    fprintf(file_ob,"\t%d  %d\n", IC, DC);
    for(i = 0; i < IC+ DC; i++){
        base_4_encode(memory_image[i], encode);
        fprintf(file_ob,"%04d  %s\n", (i+MEMORY_START), encode);
    }
    fclose(file_ob);
}

void base_4_encode(int bin, char *encode){
    int tow_bit;
    int i;

    for(i = 0; i < BASE_4_WORD_SIZE; i++){
        tow_bit = (bin >> (i*2)) & 3;
        encode[BASE_4_WORD_SIZE-1-i] = bim_to_code(tow_bit);
    }
    encode[BASE_4_WORD_SIZE] = '\0';

}

char bim_to_code(int tow_bit){
    switch (tow_bit)
    {
    case 0: 
        return '*';
        break;
    case 1: 
        return '#';
        break;
    case 2: 
        return '%';
        break;
    case 3: 
        return '!';
        break;
    default:
        break;
    }
    return '\0';
}

