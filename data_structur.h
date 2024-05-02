#define MAX_INTEGER_NUMBER 40
#define MAX_CHAR_IN_STRING 80
#define MAX_LABEL_SIZE 31
#define MAX_WARNING_SIZE 150
#define MAX_INSTRUCTION_NAME 3
#define MAX_INTEGER_SIZE 5
#define MAX_14_BIT_INTEGER 8191
#define MIN_14_BIT_INTEGER -8192
#define MAX_12_BIT_INTEGER 2047
#define MIN_12_BIT_INTEGER -2048

/*A data structure containing all the information of the operand */
typedef struct address_definition
{
    enum{
        immediate,
        direct,
        index,
        register_address,
        no_address=-1
    }address_type;
    union 
    {
        int immediate_int;
        char constant[MAX_LABEL_SIZE+1];
        char label_name[MAX_LABEL_SIZE+1];
        struct 
        {
            char constant[MAX_LABEL_SIZE+1];
            int jump;
        }index_info;
        int register_number;
        
    }address_data;
}address_definition;


/*A data structure containing all the sentence information */
typedef struct line_structur
{
    /*If there is a warning this will be !=NULL*/
    char warning_str[MAX_WARNING_SIZE+1];

    /*if there is a label in the bigining this will be !=NULL*/
    char label_name[MAX_LABEL_SIZE+1];
    
    /*what type of line we got*/
    enum {
        comment_line,
        blank_line,
        directive_line,     /*.data, .string, .entry, .extern*/
        instruction_line,   /*add, bne, mov, ...*/
        define_line
    }line_type;
    union /*only 1 type of line so it will be better to do a union*/
    {
        /*handle the directive type*/
        struct
        {
            enum{
                data_directive=0,
                string_directive=1,
                entry_directive=2,
                extern_directive=3
            } directive_type;
            union /*only 1 type of info needed*/
            {
                struct{
                    int data_array[MAX_INTEGER_NUMBER];/* data */
                    int data_count;
                }data_info;
                char string[MAX_CHAR_IN_STRING+1];
                char label[MAX_LABEL_SIZE+1];         
            }directive_info;
        }directive;
    
        /*handel the type of instruction*/
        struct 
        {
            char instruction_name[MAX_INSTRUCTION_NAME+1];
            address_definition first_operand;
            address_definition second_operand;
        }instruction;
        
        /*handel the type of define*/
        struct 
        {
            char constant[MAX_LABEL_SIZE+1];
            int varuble;
        }define;
    }line_data;

}line_structur;

/*Function that receives a line, brakes it to the different line types. 
*   for each type make all the necessary checks, If an error is detected put it in the warning section in line_structur.
*   For each type reads all the necessary information and put it in the right place in line_structur, שdditionally label are stored in table_list
*/
line_structur build_line_structur (char* line, table_list* table_head);

/*Function that prints the warning for the line*/
void print_warning_str(char *file_name, int line_counr, line_structur *line_s);


