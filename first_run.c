#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "data_table.h"
#include "data_structur.h"
#include "first_run.h"

#define TARGET_OPERAND  0
#define SOURCE_OPERAND  1 

/*Function that checks if a label name is valid, not a reserved word in the system and has not already defined*/
boolean is_legal_word(char *word, line_structur *line_s, table_list* table_head);

/*Function that translate data array and string to binary array and put it in DC memory image*/
void translate_array(line_structur *line_s, int *DC, int **DC_memory_image);

/*Function that check if entry and extern are legal and put the in the correct table*/
void translate_extern_entry(line_structur *line_s, int IC, table_list** table_head, label_list **extern_head, label_list **entry_head, boolean *success);

/*Function that translates the instruction line into the memory image and depending on the operands calls the operand analysis function*/
void translate_instruction(line_structur line_s,int *IC,int *memory_image);

/*Function that analyze the operand store its binary code into memory image, if its direct or index place -1 for its label code*/
void analyze_operand(int *IC, int *memory_image, address_definition operand,int type);

/*allocat memory for DC array*/
void allocat_DC_memory (int *DC, int **DC_memory_image, int count);

boolean first_run(char *file_name, table_list **table_head, int* IC, int* DC, int *memory_image, label_list **extern_head, label_list **entry_head){
    char *file_am_name = add_file_ending(file_name, ".am");
    FILE *file_am = fopen(file_am_name,"r");
    char line[MAX_LINE_SIZE + 2];
    line_structur line_s;
    boolean success = true;
    int *DC_memory_image = NULL;
    int i, line_counter = 0;

    while(fgets(line,MAX_LINE_SIZE + 2,file_am)!=NULL){
        line_counter++;
        line_s = build_line_structur(line, *table_head);
        
        if(line_s.warning_str[0] != '\0'){
            success = false;
            print_warning_str(file_am_name, line_counter, &line_s);
            continue;
        }


        switch (line_s.line_type)
        {
        /*comment and blank lines*/
        case comment_line:
        case blank_line:
            continue;
            break;

        case directive_line:
            
            /*data and string*/
            if(line_s.line_data.directive.directive_type == data_directive || line_s.line_data.directive.directive_type == string_directive){
                if(line_s.label_name[0] !='\0' && is_legal_word(line_s.label_name, &line_s, *table_head) == true){
                    add_new_table_mamber(DATA_LABEL, line_s.label_name, *DC,  table_head);
                }        

                if(line_s.warning_str[0] != '\0'){
                    success = false;
                    print_warning_str(file_am_name, line_counter, &line_s);
                    continue;
                }

                translate_array(&line_s, DC, &DC_memory_image);
            }
            
            /*entry and extern*/
            if(line_s.line_data.directive.directive_type == entry_directive || line_s.line_data.directive.directive_type == extern_directive){
                
                if(line_s.warning_str[0] != '\0'){
                    success = false;
                    print_warning_str(file_am_name, line_counter, &line_s);
                continue;
                }
                
                translate_extern_entry(&line_s, *IC, table_head, extern_head, entry_head, &success);
            }
            break;

        case instruction_line:

            if(line_s.label_name[0] !='\0'){
                if(is_legal_word(line_s.label_name, &line_s, *table_head) == true){
                    add_new_table_mamber(CODE_LABEL, line_s.label_name, *IC,  table_head);
                }        
            }
            if(line_s.warning_str[0] != '\0'){
                success = false;
                print_warning_str(file_am_name, line_counter, &line_s);
                continue;
            }
            /*if no more room in memory image*/
            if(*IC < MEMORY_SIZE ){
                translate_instruction(line_s, IC, memory_image);
            }else{
                printf("WARNING - Out of space in program memory");
                return false;
            }
            break;

        case define_line:
            
            if(line_s.warning_str[0] != '\0' || is_legal_word(line_s.line_data.define.constant, &line_s, *table_head) == false){
                success = false;
                print_warning_str(file_am_name, line_counter, &line_s);
                continue;
            }

            add_new_table_mamber(CONSTANT, line_s.line_data.define.constant, line_s.line_data.define.varuble, table_head);
            break;

        }
    }

    if((*IC) + (*DC) > MEMORY_SIZE){
        printf("WARNING - Out of space in program memory");
        return false;
    }

    /*move data and string to the back of memory image*/
    for(i=0; i < *DC; i++){
        memory_image[(*IC)+i] = DC_memory_image[i];
    }
    set_label_values(*table_head, *IC);

    free(DC_memory_image);
    free(file_am_name);
    fclose(file_am);

    return success;

}

void translate_array(line_structur *line_s, int *DC, int **DC_memory_image){
    int i;

    /*building a memory block for data*/
    if(line_s->line_data.directive.directive_type == data_directive){

        /*allocating dynamic memory*/
        allocat_DC_memory (DC, DC_memory_image, line_s->line_data.directive.directive_info.data_info.data_count);

        /*Go through the array and put it in a DC memory image*/
        for(i=0; i < line_s->line_data.directive.directive_info.data_info.data_count; i++){
            (*DC_memory_image)[*DC] = line_s->line_data.directive.directive_info.data_info.data_array[i];
            (*DC)++;
        }
    }

    /*building a memory block for string*/
    if(line_s->line_data.directive.directive_type == string_directive){
        int string_length = strlen(line_s->line_data.directive.directive_info.string);
        
        /*allocating dynamic memory*/
        allocat_DC_memory (DC, DC_memory_image, string_length+1);
        
        /*Go through the string and put it in a DC memory image*/
        for(i=0; i < string_length; i++){
            (*DC_memory_image)[*DC] = (int)line_s->line_data.directive.directive_info.string[i];
            (*DC)++;
        }
        /*add '\0' in the end*/
        (*DC_memory_image)[*DC] = 0;
        (*DC)++;
    }
}

void translate_extern_entry(line_structur *line_s, int IC, table_list** table_head, label_list **extern_head, label_list **entry_head, boolean *success){

    /*the line is entry or extern and the same label name cannot contain both*/
    if(is_in_label(line_s->line_data.directive.directive_info.label, *extern_head) == true || is_in_label(line_s->line_data.directive.directive_info.label, *entry_head)){
            strcpy(line_s->warning_str, line_s->line_data.directive.directive_info.label);
            strcat(line_s->warning_str," This word has already been defined as an entry or an extern");
            success = false;
            return;
    }

    /*no need to build a memory block for entry will happen in second run*/
    
    /*building a memory block for extern*/
    if(line_s->line_data.directive.directive_type == extern_directive){

        /*extern cannot be defined as another label*/
        if(is_legal_word(line_s->line_data.directive.directive_info.label, line_s, *table_head) == false){
            success = false;
            return;
        }
        add_new_table_mamber(EXTERN_LABEL, line_s->line_data.directive.directive_info.label, -1, table_head);
    }    

}

void allocat_DC_memory (int *DC, int **DC_memory_image, int count){
    if(*DC == 0){
            *DC_memory_image = (int*)malloc(sizeof(int)*count);
            if(*DC_memory_image == NULL){
                printf("Memory allocation failed\n");
                exit(1);  
            }
        }else{
            *DC_memory_image = (int*)realloc(*DC_memory_image, sizeof(int) * (*DC + count));
            if(*DC_memory_image == NULL){
                printf("Memory allocation failed\n");
                exit(1);  
            }
        }
}

void translate_instruction(line_structur line_s,int *IC,int *memory_image){
    int comendline_code = get_comendline_number(line_s.line_data.instruction.instruction_name);
    int source_operand, target_operand;
   
    /*if target and source are no_address*/
    if(line_s.line_data.instruction.first_operand.address_type == no_address){
        source_operand = 0;
        target_operand = 0;
        memory_image[*IC] = (comendline_code << 6);
        (*IC)++;

    /*if only 1 operand and it is target*/
    }else if (line_s.line_data.instruction.second_operand.address_type == no_address){
        source_operand = 0;
        target_operand = line_s.line_data.instruction.first_operand.address_type;
        memory_image[*IC] = (comendline_code << 6) | (target_operand << 2);
        (*IC)++;
        analyze_operand(IC, memory_image, line_s.line_data.instruction.first_operand, TARGET_OPERAND);
    
    /*both operand exist*/
    }else{
        source_operand = line_s.line_data.instruction.first_operand.address_type;
        target_operand = line_s.line_data.instruction.second_operand.address_type;
        memory_image[*IC] = (comendline_code << 6) | (source_operand << 4) | (target_operand << 2);
        (*IC)++;

        /*is source and target are register that have the same line*/
        if(line_s.line_data.instruction.first_operand.address_type == register_address && line_s.line_data.instruction.second_operand.address_type ==register_address){
            memory_image[*IC] = line_s.line_data.instruction.first_operand.address_data.register_number << 5 | line_s.line_data.instruction.second_operand.address_data.register_number << 2;
            (*IC)++;
        }else{
            analyze_operand(IC, memory_image, line_s.line_data.instruction.first_operand, SOURCE_OPERAND);
            analyze_operand(IC, memory_image, line_s.line_data.instruction.second_operand, TARGET_OPERAND);
        }
    }
}

void analyze_operand(int *IC, int *memory_image, address_definition operand,int type){
    switch (operand.address_type)
    {
    case immediate:
        memory_image[*IC] = operand.address_data.immediate_int << 2;
        (*IC)++;
        break;

    case direct:
        memory_image[*IC] = -1;/*the label tabel isn't ready in this stage and will be filled in second pass*/
        (*IC)++;
        break;

    case index:
        memory_image[*IC] = -1;/*the label tabel isn't ready in this stage and will be filled in second pass*/
        (*IC)++;
        memory_image[*IC] = operand.address_data.index_info.jump << 2;
        (*IC)++;
    break;

    case register_address:
        if(type == TARGET_OPERAND){
            memory_image[*IC] = operand.address_data.register_number << 2;
        }else{
            memory_image[*IC] = operand.address_data.register_number << 5;
        }
        (*IC)++;
        break;
    default:
        return;
        break;
    }
}

boolean is_legal_word(char *word, line_structur *line_s, table_list* table_head){
    /*Check if already defined*/
    if(line_s->warning_str[0] == '\0' && is_in_list(word, table_head) == true){
        strcpy(line_s->warning_str, word);
        strcat(line_s->warning_str," has already been defined in the program as a label or as a constant");
        return false;
    }
    /*check if reserved word in the system*/
    if(line_s->warning_str[0] == '\0' && (is_comendline(word) == true || is_register(word) == true || is_save_word(word) == true)){
        strcpy(line_s->warning_str, word);
        strcat(line_s->warning_str," Is a reserved word in the language");
        return false;
    }
    return true;
}



