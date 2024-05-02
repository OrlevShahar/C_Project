
/* This function takes as input the linked lists of extern and entry labels, along with the label table, IC, DC, memory array, and the file name.
    It opens the file, reads it line by line, and for each line, it uses a line_structur to determine the type of line.
    Based on the line type, it updates IC and DC, as well as the memory array and the tables accordingly.
    Additionally, it generates a temporary array for the DC memory block, which is later combined at the end of the memory array. */
boolean first_run(char *file_name, table_list **table_head, int* IC, int* DC, int *memory_image, label_list **extern_head, label_list **entry_head);

