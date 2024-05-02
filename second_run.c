#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "data_table.h"
#include "data_structur.h"
#include "second_run.h"

/*Function that checks if the direct label has been set correctly*/
boolean check_direct_label(table_list* table_head, line_structur *line_s, char *label);

/*Function that fills in the gaps in memory if the label is extern puts A.R.E = 01 if not  A.R.E = 10*/
boolean set_label(table_list* table_head, label_list **extern_head, int *memory_image, int counter, char *label);

/*Function that checks if the index label has been set correctly*/
boolean check_index_label(table_list* table_head, line_structur *line_s, char *label);



boolean second_run(char *file_name, table_list* table_head, int* IC, int* DC, int *memory_image, label_list **extern_head, label_list **entry_head){
    char *file_am_name = add_file_ending(file_name, ".am");
    FILE *file_am = fopen(file_am_name,"r");
    char line[MAX_LINE_SIZE + 2];
    line_structur line_s;
    boolean success = true;
    int counter = 0, line_counter = 0;

    while(fgets(line,MAX_LINE_SIZE + 2,file_am)!=NULL){
        line_counter++;
        line_s = build_line_structur(line, table_head);

        /*fills in the gaps in memory, need to check each option of operand to maintain counter of IC*/
        if(line_s.line_type == instruction_line){
            counter++;
            /*if first operand is direct*/
            if(line_s.line_data.instruction.first_operand.address_type == direct){
                if(line_s.warning_str[0] == '\0'){
                    if (check_direct_label(table_head, &line_s, line_s.line_data.instruction.first_operand.address_data.label_name) == false){
                        success = false;
                        print_warning_str(file_am_name, line_counter, &line_s);
                    }else{
                        set_label(table_head, extern_head, memory_image, counter, line_s.line_data.instruction.first_operand.address_data.label_name);
                    }
                }
                counter++;
            }
            /*if first operand is index*/
            if(line_s.line_data.instruction.first_operand.address_type == index){
                if(line_s.warning_str[0] == '\0'){
                    if (check_index_label(table_head, &line_s, line_s.line_data.instruction.first_operand.address_data.index_info.constant) == false){
                        success = false;
                        print_warning_str(file_am_name, line_counter, &line_s);
                    }else{
                        set_label(table_head, extern_head, memory_image, counter, line_s.line_data.instruction.first_operand.address_data.index_info.constant);
                    }
                }
                counter+=2;
            }
            /*if source and target operands are register*/
            if(line_s.line_data.instruction.first_operand.address_type == register_address && line_s.line_data.instruction.second_operand.address_type == register_address){
                counter++;
                continue;
            }
            /*if first operand is register or immediate*/
            if(line_s.line_data.instruction.first_operand.address_type == register_address || line_s.line_data.instruction.first_operand.address_type == immediate){
                counter++;
            }
            /*if second operand is direct*/
            if(line_s.line_data.instruction.second_operand.address_type == direct){
                if(line_s.warning_str[0] == '\0'){
                    if (check_direct_label(table_head, &line_s, line_s.line_data.instruction.second_operand.address_data.label_name) == false){
                        success = false;
                        print_warning_str(file_am_name, line_counter, &line_s);
                    }else{
                        set_label(table_head, extern_head, memory_image, counter, line_s.line_data.instruction.second_operand.address_data.label_name);
                    }
                }
                counter++;
            }
            /*if second operand is index*/
            if(line_s.line_data.instruction.second_operand.address_type == index){
                if(line_s.warning_str[0] == '\0'){
                    if (check_index_label(table_head, &line_s, line_s.line_data.instruction.second_operand.address_data.index_info.constant) == false){
                        success = false;
                        print_warning_str(file_am_name, line_counter, &line_s);
                    }else{
                        set_label(table_head, extern_head, memory_image, counter, line_s.line_data.instruction.second_operand.address_data.index_info.constant);
                    }
                }
                counter+=2;
            }
            /*if second operand is register or immediate*/
            if(line_s.line_data.instruction.second_operand.address_type == register_address || line_s.line_data.instruction.second_operand.address_type == immediate){
                counter++;
            }
        }

        if(line_s.line_type == directive_line && line_s.line_data.directive.directive_type == entry_directive && line_s.warning_str[0] == '\0'){
            if(is_in_list(line_s.line_data.directive.directive_info.label, table_head) == false || 
            (get_lebel_type(line_s.line_data.directive.directive_info.label, table_head) != CODE_LABEL && get_lebel_type(line_s.line_data.directive.directive_info.label, table_head) != DATA_LABEL)){
                strcpy(line_s.warning_str, "Entry label must be declerd in the program as a code label or a data label");
            }else{
                int label_location;
                get_constant_num(line_s.line_data.directive.directive_info.label, table_head, &label_location);
                add_new_label(entry_head, line_s.line_data.directive.directive_info.label, label_location);
            }
        }
    }

    free(file_am_name);
    fclose(file_am);
    return success;
}


boolean set_label(table_list* table_head, label_list **extern_head, int *memory_image, int counter, char *label){
    int label_location;

    if(get_lebel_type(label, table_head) == EXTERN_LABEL){
        memory_image[counter] = EXTERNAL;
        add_new_label(extern_head, label, counter);
    }else{
        get_constant_num(label, table_head, &label_location);
        label_location+=MEMORY_START;
        memory_image[counter] = label_location << 2 | RELOCATABLE;
    }
    return true;    
}

boolean check_index_label(table_list* table_head, line_structur *line_s, char *label){
    /*if not in table_head, and not data label or extern */
    if(is_in_list(label, table_head) == false || (get_lebel_type(label, table_head) != DATA_LABEL && get_lebel_type(label, table_head) != EXTERN_LABEL)){
        strcpy(line_s->warning_str, label);
        strcat(line_s->warning_str," This word should be defined as a data label or extern");
        return false;
    }
    return true;    
}

boolean check_direct_label(table_list* table_head, line_structur *line_s, char *label){
    /*if not in table_head*/
    if(is_in_list(label, table_head) == false){
        strcpy(line_s->warning_str, label);
        strcat(line_s->warning_str," This word should be defined as a label or extern");
        return false;
    }
    return true;    
}


