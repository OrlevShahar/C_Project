#define CONSTANT 0
#define CODE_LABEL 1
#define DATA_LABEL 2
#define EXTERN_LABEL 3

/*table for labels and constant*/
typedef struct table_list{
    enum{
        constant,
        code_label,     /*before command*/
        data_label,     /*before string and array*/
        extern_labal   /*can be used cannot be defined*/
    }table_type;
	char *name;
    int value;						
	struct table_list *next_element; 	/*pointer for the next constant*/
}table_list;

/*Function that add new entry in the table linked list*/
void add_new_table_mamber(int type, char *name, int value, table_list **table_head);

/*Fuction that reaturn the number of a constant in a specific name*/
boolean get_constant_num(char *name, table_list *table_head, int *num);

/*Function that check if a given name is in the table_list*/
boolean is_in_list(char *name, table_list *table_head);

/*Function that returns the type of the entry of the name in table_list*/
int get_lebel_type(char *name, table_list *table_head);

/*Function that set the values of the data type table to the correct values IC + his DC*/
void set_label_values(table_list *label_head, int IC);

/*Function that free all the memory for table_list*/
void free_table_list(table_list *table_head);

/*table for extern and entry form this table the ext, ent files are created*/
typedef struct label_list{
    char *name;
    int addres;
    struct label_list *next_label;
}label_list;

/*Function that add new label in the label linked list*/
void add_new_label(label_list **label_head, char *name, int value);

/*Function that check if a given name is in the label_list*/
boolean is_in_label(char *name, label_list *label_head);

/*Function that free all the memory for label_list*/
void free_label_list(label_list *label_head);

/*Function that create a file for the given file name and label_list*/
void label_file_create(char *file_name, label_list *label_head);

