#define EXTERNAL 1
#define RELOCATABLE 2

/* This function takes as input the linked lists of extern and entry labels, along with the label table, IC, DC, memory array, and the file name.
    It opens the file, reads it line by line, and for each line, it uses a line_structur to determine the type of line.
    The function keeps track of the IC location of each line, and if it's an instruction with operands of index or direct types, it puts the correct location of the label in the memory image.
    If the line type is entry, the function checks where it was declared and puts the declaration address in the entry list. */
boolean second_run(char *file_name, table_list* table_head, int* IC, int* DC, int *memory_image, label_list **extern_head, label_list **entry_head);
