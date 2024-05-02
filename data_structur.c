#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "data_table.h"
#include "data_structur.h"


typedef enum DirectiveType {
    NO_DIRECTIVE = -1,
    DATD_DIRECTIVE = 0,
    STRING_DIRECTIVE, 
    ENTRY_DIRECTIVE,
    EXTERN_DIRECTIVE,
    DEFINE
}DirectiveType;

/*Function that reaturn the directive type*/
DirectiveType get_directive_type(char* word);

/*Function that check the spelling of the label*/
boolean check_word_spelling(char *word, int size);

/*Function that gets a string and checks it for the .data directive, then converts it to an integer*/
boolean extract_integer(char* str, line_structur* line_s, table_list* table_head, int* result );

/*Function that handles a data line, reads the array and check its integrity*/
void data_hendel(char*line, line_structur *line_s, int *point, table_list* table_head );

/*Function to reads a string form the line and stor it in the line structur*/
void read_string(char* line, int* point, line_structur *line_s);

/*Function that extracts a single integer from the line returns true on success and false on failure, stores the number in result*/
boolean read_num(char* line, int* point, int *result, line_structur *line_s, int *size_of_int);

/*Function that reads and checks parameters for the define line, and stores the information in line_structur*/
void handele_define(char* line, int* point, line_structur *line_s);

/*Function that reads the name of constant in a define line*/
char *read_define(char *line, int *point);

/*Function to check if a given string is one of the instructions*/
boolean read_instruction(char* word, line_structur *line_s);

/*Function that checks what type of operand there is (register, immediate, direct, index, no_address), reads its relevant information and stores it in line_structur*/
boolean handele_address(char *line, int *point, line_structur *line_s, address_definition *address_d , table_list *table_head);

/*Function that checks for extra characters after an address declaration*/
boolean check_unnecessary_char(char *line, int *point, line_structur *line_s);

/*Function that reads a word from the line, it will stop reading if it reached '[' ']' ',' , white character or maximum buffer hase been reached */
char *read_address(char *line, int *point, line_structur *line_s);

/*Function that receives a word and checks if it is a predefined constant or an integer, casts it to an integer and returns it in num* */
boolean read_12_bit_number(char *address, int *num, line_structur *line_s, table_list* table_head );

/*Function that checks if the type of the operand is legal in a specific instruction*/
void check_operand(line_structur* line_s);

line_structur build_line_structur (char* line, table_list* table_head){
    int point = 0;
    line_structur line_s = {0};
    char *word;

    line_s.warning_str[0] = '\0';
    line_s.label_name[0] = '\0';



    /*Check if the line length exceeds the maximum length(80)*/
    if(strlen(line) > MAX_LINE_SIZE){
        strcpy(line_s.warning_str,"Line length exceeds the maximum line length");
        return line_s;
    }

    skip_leading_spaces(line, &point);
    /*comment_line*/
    if(line[point]==';'){
        if(point != 0){
            strcpy(line_s.warning_str,"There should be no white characters befor a comment line"); 
        }
        line_s.line_type = comment_line;
        return line_s;
    }
    
    /*blank_line*/
    if(line == NULL || line[point] == '\n'){
        line_s.line_type = blank_line;
        return line_s;
    }

    word = read_next_word(line, &point);

    /*check the size of the word*/
    if(strlen(word) > MAX_LABEL_SIZE){
        if(line[point] != '\0' && line[point] != ' ' && line[point] != '\t' && line[point] != '\n'){
            strcpy(line_s.warning_str,"Maximum buffer for label, directive is 31");
            free(word);
            return line_s;
        }
    }
    /*the line starts in label*/
    if( word[strlen(word) - 1] == ':'){
        if (check_word_spelling(word, strlen(word)-1) == false){
            strcpy(line_s.warning_str,word);
            strcat(line_s.warning_str," is not a legal label name can only start with a letter and only letters and numbers are allowed");
            free(word);
            return line_s;
        }
        word[strlen(word)-1] = '\0';
        strcpy(line_s.label_name,word);
        free(word);
        word = read_next_word(line, &point);
    }

    /*directive line*/
    if (word[0] == '.'){
        DirectiveType directive = get_directive_type(word);
        
        switch (directive)
        {
        /*define*/
        case 4: /*define*/
            line_s.line_type = define_line;
            handele_define(line, &point, &line_s);
            free(word);
            return line_s;
            break;

        /*data line*/
        case 0:
            line_s.line_type = directive_line;
            line_s.line_data.directive.directive_type = directive;
            data_hendel(line, &line_s, &point, table_head);
            free(word);
            return line_s;
            break;

        /*string line*/
        case 1:
            line_s.line_type = directive_line;
            line_s.line_data.directive.directive_type = directive;
            read_string(line, &point, &line_s);
            free(word);
            return line_s;
            break;
                
            /*entry and extern line*/
        case 2:
        case 3:
            line_s.line_type = directive_line;
            line_s.line_data.directive.directive_type = directive;
            free(word);
            word = read_next_word(line, &point);
                    
            /*check for missing label*/
            if (word[0]=='\0'){
                strcpy(line_s.warning_str,"mising label after: entry/ label");
                free(word);
                return line_s;
            }
            /*Check if the label size exceeds 31 characters*/
            if(strlen(word) > MAX_LABEL_SIZE){
                strcpy(line_s.warning_str,"Maximum size of label is 31");
                free(word);
                return line_s;
            }
            /*check if there is garbage after label*/
            if(is_garbeg(line, &point) == true){
                strcpy(line_s.warning_str,"Unwanted characters after label declaration");
                free(word);
                return line_s;
            }
            /*check spelling and assign the label to the struct*/
            if(check_word_spelling(word, strlen(word))){
                strcpy(line_s.line_data.directive.directive_info.label, word);
            }else{
                strcpy(line_s.warning_str,word);
                strcat(line_s.warning_str," is not a legal label name can only start with a letter, and only letters and numbers are allowed");
            }
                
            free(word);
            return line_s;
            break; 
        case -1: 
            strcpy(line_s.warning_str,"undefing name of directive: ");
            strcat(line_s.warning_str,word);
            free(word);
            return line_s; 
            break;     
        }
    }

    
    /*instruction line*/
    if(read_instruction(word, &line_s) == true){
        free(word);

        if(handele_address(line, &point, &line_s, &line_s.line_data.instruction.first_operand, table_head) == false){
            return line_s;
        }
        
        skip_leading_spaces(line, &point);

        /*missing comma between operands*/
        if(line[point] != '\0' && line[point] != '\n' && line[point] != ','){
            strcpy(line_s.warning_str,"missing comma between source operand and target operand");
            return line_s;
        }
        if (line[point] == ','){
            point++;
            if(handele_address(line, &point, &line_s, &line_s.line_data.instruction.second_operand, table_head) == false){
                return line_s;
            }
            /*missing target operand after comma*/
            if (line_s.line_data.instruction.second_operand.address_type == no_address){
                strcpy(line_s.warning_str,"missing target operand after comma");
                return line_s;
            }
        }else{
            line_s.line_data.instruction.second_operand.address_type = no_address;
        }
        check_operand(&line_s);
        return line_s;
    }

    free(word);
    return line_s;
}

DirectiveType get_directive_type(char* word) {
    
    if (strcmp(".data", word) == 0) {
        return DATD_DIRECTIVE;
    }
    if (strcmp(".string", word) == 0) {
        return STRING_DIRECTIVE;
    }
    if (strcmp(".entry", word) == 0) {
        return ENTRY_DIRECTIVE;
    }
    if (strcmp(".extern", word) == 0) {
        return EXTERN_DIRECTIVE;
    }
     if (strcmp(".define", word) == 0) {
        return DEFINE;
    }
    return NO_DIRECTIVE;
}

boolean check_word_spelling(char *word, int size) {
    int i;
   
    /*check if the word start in a characters*/
    if(!isalpha(word[0])){
        return false;
    }

    /*check the word name only numbers and characters are allowed*/
    for (i=0;i < size; i++){
        if(!isdigit(word[i]) && !isalpha(word[i])){
            return false;
        }
    }
    return true;
}

void read_string(char* line, int* point, line_structur *line_s){
    int end_pointer = strlen(line)-1;
    int counter = 0;
    char string[MAX_CHAR_IN_STRING+1] = {'\0'};

   /*finding the first " in the line*/
   skip_leading_spaces(line, point);
    if (line[*point] != 34){
            strcpy(line_s->warning_str,"Missing quotation marks before characters in .string directive");
		    return;    
    } 
    (*point)++;
    /*finding the last " in the line*/
    while (line[end_pointer] != 34){
        if(line[end_pointer] != '\0' && line[end_pointer] != ' ' && line[end_pointer] != '\t' && line[end_pointer] != '\n'){
            strcpy(line_s->warning_str,"charcters not inside quotation marks in .string directive");
		    return;    
        }
        end_pointer--;
    }
    
    /*run through the string check if it can be printed and insert it the the struct*/
    for(; *point < end_pointer; (*point)++){
        if(isprint(line[*point]) == 0){
            strcpy(line_s->warning_str,"character can't be printed");
            return;
        }
        string[counter] = line[*point];
        counter++;
    }
    strcpy(line_s->line_data.directive.directive_info.string, string);
    return;
}

void handele_define(char* line, int* point, line_structur *line_s){
    char *constant_name = read_define(line, point);
    int size_of_int = 0;
    int num;

    /*check if there is a label*/
    if(line_s->label_name[0] != '\0'){
        strcpy(line_s->warning_str,"label is not allowed before .define");
        free(constant_name);  
        return;  
    }
    /*check for missing data after define*/
    if (constant_name[0]=='\0'){
        strcpy(line_s->warning_str,"mising constant name and integer after .define");
        free(constant_name);  
        return;                  
    }
    if(strlen(constant_name) > MAX_LABEL_SIZE){
        strcpy(line_s->warning_str,"Maximum size of constant name is 31");
        free(constant_name); 
        return;
    }

    /*check spelling of constant name*/
    if(check_word_spelling(constant_name, strlen(constant_name)) == false){
        strcpy(line_s->warning_str,constant_name);
        strcat(line_s->warning_str," is not a legal constant name can only start with a letter, and only letters and numbers are allowed");
        free(constant_name);  
        return; 
    }

    strcpy(line_s->line_data.define.constant, constant_name);
    free(constant_name);   

    /*check for = sign*/
    skip_leading_spaces(line, point);
    if(line[*point] != '='){
        strcpy(line_s->warning_str,"missing the equal sign between the constant name and the number");
    }
    (*point)++;
    skip_leading_spaces(line, point);

    /*reading a number and checking it*/
    if(read_num(line, point, &num, line_s, &size_of_int) == false){
        return;
    }
    if(size_of_int == 0){
        strcpy(line_s->warning_str,"missing integer in define line");
        return;
    }

    /*look for unwanted character after the number*/
    if(is_garbeg(line,point) == true){
        strcpy(line_s->warning_str,"undefined characters after integer in define line");
        return;
    }

    line_s->line_data.define.varuble = num;
    return;
     
}

char *read_define(char *line, int *point){
    char* constant = (char*)malloc((MAX_LABEL_SIZE+2) * sizeof(char));
    int size_of_constant = 0;

    if(constant == NULL){
        printf("Memory allocation failed in read next word\n");
        exit(EXIT_FAILURE);
    }

    /*run through the line and stops at and of word, '=' has been detected or maximum size of word*/
    skip_leading_spaces(line, point);
    while (line[*point] != '=' && line[*point] != '\0' && line[*point] != ' ' && line[*point] != '\t' && line[*point] != '\n' ){
        if(size_of_constant <= MAX_LABEL_SIZE){
            constant[size_of_constant] = line[*point];
            size_of_constant++;
            (*point)++;
            
        }else{
            break;
        }
    }
    constant[size_of_constant] = '\0';
    return constant;
}

boolean read_num(char* line, int* point, int *result, line_structur *line_s, int *size_of_int){
    char str[MAX_INTEGER_SIZE] = {'\0'};
    
    /*running on line to scan the number*/
	while (line[*point] != '\0' && line[*point] != ' ' && line[*point] != '\t' && line[*point] != '\n' && line[*point] != ',') {

		/*Scans and checks if the number is not too big*/
		if (*size_of_int < MAX_INTEGER_SIZE){

            /*If the first char is + or - add them and don't give a warning*/
            if(*size_of_int==0 && (line[*point] == '+' || line[*point] == '-')){
                str[*size_of_int] = line[*point];
			    (*size_of_int)++;
			    (*point)++;
                continue;
            }

            /*Check if the character is a number*/
            if (!isdigit(line[*point])){
                strcpy(line_s->warning_str, "integer can receive only numbers");
                return false;
            }

			str[*size_of_int] = line[*point];
			(*size_of_int)++;
			(*point)++;
		}else {
			strcpy(line_s->warning_str, "The integer size exceeds the limit -8192 - +8191");
			return false;
		}
	}
    /*casting str to integer*/
    str[*size_of_int] = '\0';
	*result = atoi(str);


    /*check the size of the integer*/
    if (*result > MAX_14_BIT_INTEGER ||*result < MIN_14_BIT_INTEGER){
        strcpy(line_s->warning_str, "The integer size exceeds the limit -8192 - +8191");
		return false; 
    }
    return true;
}

boolean read_instruction(char* word, line_structur *line_s){
    int i;
    /*if word is an instruction*/
    for(i=0; i < NUMBER_OF_COMENDLINE; i++){
        if(strcmp(comendline[i], word) == 0){
            line_s->line_type = instruction_line;
            strcpy(line_s->line_data.instruction.instruction_name, word);
            return true;
        }
    }
    strcpy(line_s->warning_str, "\"");
    strcat(line_s->warning_str, word);
    strcat(line_s->warning_str,"\" is not a reserved command in the system, A white character is needed after the command");
    return false;    
}

boolean handele_address(char *line, int *point, line_structur *line_s, address_definition *address_d , table_list *table_head){
    char *address;

    skip_leading_spaces(line, point);

    /*read word up until , or [ or blank or size 32, 31 for word and 1 for #*/
    if((address = read_address(line, point, line_s)) == NULL){
        return false;
    }
    /*if no address*/
    if(strlen(address) == 0){
        free(address);
        /*check if there is unwanted char after the register*/
        if(check_unnecessary_char(line, point, line_s) == false){
            return false;
        }
        address_d->address_type = no_address;
        return true;
    }

    /*if register*/
    if(is_register(address) == true){
        char register_num;
        /*check if there is unwanted char after the register*/
        if(check_unnecessary_char(line, point, line_s) == false){
            free(address);
            return false; 
        }
        register_num = address[1];
        address_d->address_type = register_address;
        address_d->address_data.register_number = register_num -'0';
        free(address);
        return true;
    }
   
    /*immediate address*/
    if(address[0] == '#'){
        int num = 0;
        /*Check for unwanted char after the address*/
        if(check_unnecessary_char(line, point, line_s) == false){
            free(address);
            return false;
        }

        /*Convert the address to a number using a constant or an integer*/
        if(read_12_bit_number(address, &num, line_s, table_head) == false){
            free(address);
            return false;
        }
        address_d->address_type = immediate;        
        address_d->address_data.immediate_int = num;
        free(address);
        return true;  
    }

    /*direct and index*/
    if(check_word_spelling(address,strlen(address)) == true){

        /*direct*/
        if(line[*point] != '['){
            /*Check for unwanted char after the address*/
            if(check_unnecessary_char(line, point, line_s) == false){
                free(address);
                return false;
            }
            address_d->address_type = direct;
            strcpy(address_d->address_data.label_name, address); 
            free(address);
            return true;           
        }

        /*index*/
        if(line[*point] == '['){
            int num = 0;
           
            address_d->address_type = index;
            strcpy(address_d->address_data.index_info.constant, address);
            (*point)++;
            free(address);

            /*Get the argument in []*/
            if((address = read_address(line, point, line_s)) == NULL){
                return false;
            }
            /*Convert the address to a number using a constant or an integer*/
            if(read_12_bit_number(address, &num, line_s, table_head) == false){
                free(address);
                return false;
            }
            address_d->address_data.index_info.jump = num;
            free(address);
            /*check for missing ]*/
            if(line[*point] != ']'){
                strcpy(line_s->warning_str, "missing ']' after the index declare no white characters are allowed");
                return false;
            }
            (*point)++;
            return check_unnecessary_char(line, point, line_s);
        }
    }
    strcpy(line_s->warning_str, "address is not a possible label, immediate, register or an array pointer");
    return false;
}

boolean check_unnecessary_char(char *line, int *point, line_structur *line_s){
    /*Check for characters after and if there is make sure it's a comma*/
    if(is_garbeg(line, point) == true){
        skip_leading_spaces(line, point);
        if(line[*point] !=','){
            strcpy(line_s->warning_str, "unwanted charecters after address declaration");
            return false;
        }
    }
    return true;
}

char *read_address(char *line, int *point, line_structur *line_s){
    char* address = (char*)malloc((MAX_LABEL_SIZE+2) * sizeof(char));
    int size_of_address = 0;

    if(address == NULL){
        printf("Memory allocation failed in read next word\n");
        exit(EXIT_FAILURE);
    }
    skip_leading_spaces(line, point);
    /*run through the line and stops at and of word, '[' ']' ',' has been detected or maximum size of word*/
    while (line[*point] != '[' && line[*point] != ']' && line[*point] != ',' && line[*point] != '\0' && line[*point] != ' ' && line[*point] != '\t' && line[*point] != '\n' ){
        if(size_of_address < MAX_LABEL_SIZE){
            address[size_of_address] = line[*point];
            size_of_address++;
            (*point)++;
            
        }else{
            strcpy(line_s->warning_str, "Maximum buffer for label, data is 31 and integer is limit to -2048 - +2047");
            free(address);
            return NULL;
        }
    }
    address[size_of_address] = '\0';
    return address;
}

boolean read_12_bit_number(char *address, int *num, line_structur *line_s, table_list* table_head ){
    int point = 0;
    int size_of_int = 0;
    if(address[point]== '#'){
        point++;
    }

    /*input is a number*/
    if(isdigit(address[point]) != 0 || address[point] == '-' || address[point] == '+'){
        while(point < strlen(address)){

            /*if the address start with + or - it's ok*/
            if(size_of_int == 0 && (address[point] == '-' || address[point] == '+')){
                point++;
                size_of_int++;
                continue;
            }

            /*check that all the characters are numbers*/
            if(isdigit(address[point]) == 0){
                strcpy(line_s->warning_str, "undefind charcter in number");
                return false;
            }
            point++;
            size_of_int++;
        }
        if(size_of_int > MAX_INTEGER_SIZE){
            strcpy(line_s->warning_str, "The language supports a 12-bit number +2023 - -2024");
            return false;
        }
        /*casting str to integer*/
        if(address[0]== '#'){
            char *endptr;
            *num = strtol(address + 1, &endptr, 10);
        }else{
	        *num = atoi(address);
        }

        /*check if the number in the 12 bit limit*/
        if (*num > MAX_12_BIT_INTEGER ||*num < MIN_12_BIT_INTEGER){
            strcpy(line_s->warning_str, "The language supports a 12-bit number +2047 - -2048");
		    return false; 
        }
        return true;
    }

    /*if address is not a preedefind constant*/
    if(get_constant_num(address + point, table_head, num) == false || get_lebel_type(address + point, table_head) != CONSTANT){
        strcpy(line_s->warning_str, "undefind constant name");
        return false;
    }

    /*check if the number in the 12 bit limit*/
    if (*num > MAX_12_BIT_INTEGER ||*num < MIN_12_BIT_INTEGER){
        strcpy(line_s->warning_str, "The language supports a 12-bit number +2047 - -2048");
	    return false; 
    }
    return true;
}

void data_hendel(char*line, line_structur *line_s, int *point, table_list* table_head ){
    char *str = NULL;
    int num = 0, count = 0;

    line_s->line_data.directive.directive_info.data_info.data_count = count;
    skip_leading_spaces(line, point);
    /*if there is a comma between the definition and the first integer*/
    if(line[*point] == ','){
        strcpy(line_s->warning_str,"There should not be a comma between the definition and the first integer/constant");
        return;
    }
    /*run throw the line and from comma to comma */
    str = strtok(line + *point, ",");
    while(str != NULL && count <= MAX_INTEGER_NUMBER){
        /*if its a number or predefined constant*/
        if(extract_integer(str, line_s, table_head, &num) == false){
            return;
        }
        line_s->line_data.directive.directive_info.data_info.data_array[count] = num;
        count++;
        str = strtok(NULL, ",");
    }
    line_s->line_data.directive.directive_info.data_info.data_count = count;
    return;
}

boolean extract_integer(char* str, line_structur* line_s, table_list* table_head, int* result ){
    int left_point = 0, count = 0;
    int right_point = strlen(str)-1;
    char word[MAX_LINE_SIZE] = {'\0'};
    int i;

    skip_leading_spaces(str, &left_point);

    if(left_point > right_point){
        strcpy(line_s->warning_str, "Missing integer/constant after comma");
        return false;
    }
    /*Removing white characters from the end*/
    while(str[right_point] == ' ' || str[right_point] == '\t' || str[right_point] == '\n'){
        right_point--;
        count++;
    }

    strncpy(word, str+left_point, strlen(str) - left_point - count);

    /*If word = 0 an integer is missing*/
    if(strlen(word) == 0){
        strcpy(line_s->warning_str, "Missing integer/constant after comma");
        return false;
    }

    /*Look for white characters that indicate if commas are missing*/
    for(i = 0; i < strlen(word); i++){
        if(word[i] == ' ' || word[i] == '\t' || word[i] == '\n'){
            strcpy(line_s->warning_str, "Missing comma between integers/constants");
            return false;   
        }
    }

    /*the first char is a number or a + , -*/
    if(isdigit(word[0]) != 0 || word[0] == '+' || word[0] == '-'){

        /*Make sure all characters are numbers*/
        for(i = 1; i < strlen(word); i++){
            if (!isdigit(word[i])){
                strcpy(line_s->warning_str, "integer can receive only numbers");
                return false;
            }
        }

        /*casting word to integer*/
        if(strlen(word) > MAX_INTEGER_SIZE){
            strcpy(line_s->warning_str, "The integer size exceeds the limit -8192 - +8191");
		    return false; 
        }
        *result = atoi(str);

         /*check the size of the integer*/
        if (*result > MAX_14_BIT_INTEGER ||*result < MIN_14_BIT_INTEGER){
            strcpy(line_s->warning_str, "The integer size exceeds the limit -8192 - +8191");
		    return false; 
        }
        return true;
    }

    /*the input is a constant*/
    if(get_constant_num(word, table_head, result) == false || get_lebel_type(word, table_head) != CONSTANT){
        strcpy(line_s->warning_str,word);
        strcat(line_s->warning_str," - is not an integer and not predefined constant");
        return false;
    }
    return true;

}

void check_operand(line_structur* line_s){
    int instruction_name;

    /*get the number of the instruction*/
	for(instruction_name=0; instruction_name < NUMBER_OF_COMENDLINE; instruction_name++){
	    if (strcmp(comendline[instruction_name],line_s->line_data.instruction.instruction_name)==0){
		    break;
	    }
	}

    /*Each instruction has its legal operand*/
	switch (instruction_name)
    {
    case 0:/*mov*/
    case 2:/*add*/
    case 3:/*sub*/
        if(line_s->line_data.instruction.first_operand.address_type == no_address){
           strcpy(line_s->warning_str,"Missing source operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type == no_address || line_s->line_data.instruction.second_operand.address_type == immediate){
            strcpy(line_s->warning_str,"Invalid target operand after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;
    case 1:/*cmp*/
        if(line_s->line_data.instruction.first_operand.address_type == no_address){
           strcpy(line_s->warning_str,"Missing source operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type == no_address ){
            strcpy(line_s->warning_str,"Missing target operand after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;
    case 4:/*not*/
    case 5:/*clr*/
    case 7:/*inc*/
    case 8:/*dec*/
    case 11:/*red*/
        if(line_s->line_data.instruction.first_operand.address_type == no_address || line_s->line_data.instruction.first_operand.address_type == immediate){
           strcpy(line_s->warning_str,"Invalid target operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type != no_address ){
            strcpy(line_s->warning_str,"Source operand is not legal after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;

    case 6:/*lea*/
        if(line_s->line_data.instruction.first_operand.address_type != direct && line_s->line_data.instruction.first_operand.address_type != index){
           strcpy(line_s->warning_str,"Invalid source operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type == no_address || line_s->line_data.instruction.second_operand.address_type == immediate){
            strcpy(line_s->warning_str,"Invalid target operand after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;

    case 9:/*jmp*/
    case 10:/*bne*/
    case 13:/*jsr*/
        if(line_s->line_data.instruction.first_operand.address_type != direct && line_s->line_data.instruction.first_operand.address_type != register_address){
           strcpy(line_s->warning_str,"Invalid target operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type != no_address ){
            strcpy(line_s->warning_str,"Source operand is not legal after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;
    case 12:/*prn*/
        if(line_s->line_data.instruction.first_operand.address_type == no_address){
           strcpy(line_s->warning_str,"Missing target operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type != no_address ){
            strcpy(line_s->warning_str,"Source operand is not legal after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;
    case 14:/*rts*/
    case 15:/*hlt*/
        if(line_s->line_data.instruction.first_operand.address_type != no_address){
           strcpy(line_s->warning_str,"No target operand after instruction "); 
           strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
           return;
        }
        if(line_s->line_data.instruction.second_operand.address_type != no_address ){
            strcpy(line_s->warning_str,"Source operand is not legal after instruction "); 
            strcat(line_s->warning_str,line_s->line_data.instruction.instruction_name);
            return;
        }
        break;
    }
}
/*###################################################################################################*/


void print_warning_str(char *file_name, int line_counr, line_structur *line_s){
	printf("\nWARNING! file: %s line number: %d ,%s",file_name, line_counr, line_s->warning_str);
}
