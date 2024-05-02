#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"
#include "data_table.h"

void add_new_table_mamber(int type, char *name, int value, table_list **table_head){
    table_list *pointer = *table_head;
    
    table_list *new_list = (table_list*)malloc(sizeof(table_list));
    if(new_list == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }

    new_list->name = (char*)malloc(strlen(name)+1);
    if(new_list->name == NULL){
        printf("Memory allocation failed\n");
        free(new_list);
        exit(1);
    }
    /*apply the values to new_list*/
    new_list->table_type = type;
    strcpy(new_list->name, name);
    new_list->value = value;
    new_list->next_element = NULL;

    if(pointer == NULL){
        *table_head = new_list;
        return;
    }
    /*add new_list to the end of the list*/
    while(pointer->next_element != NULL){
        pointer = pointer->next_element;
    }

    pointer->next_element = new_list;    
}

boolean is_in_list(char *name, table_list *table_head){
    table_list *pointer = table_head;
    while (pointer != NULL){
        if(strcmp(pointer->name, name) == 0){
            return true;
        }
        pointer = pointer->next_element;
    }
    return false;
}

int get_lebel_type(char *name, table_list *table_head){
    table_list *pointer = table_head;
    while (pointer != NULL){
        if(strcmp(pointer->name, name) == 0){
            return pointer->table_type;
        }
        pointer = pointer->next_element;
    }
    return -1;
}

boolean get_constant_num(char *name, table_list *table_head, int *num){
    table_list *point = table_head;
    if(point == NULL){
        return false;
    }
    while(point != NULL){
        if(strcmp(name,point->name)==0){
            *num = point->value;
			return true;
		}
        point = point->next_element;
    }
    return false;
}

void free_table_list(table_list *table_head){
    table_list *point = table_head;
    table_list *point_next;

    while(point != NULL){
        point_next = point->next_element;
        free(point->name);
        free(point);
        point = point_next;
    }
}

void add_new_label(label_list **label_head, char *name, int value){
    label_list *pointer = *label_head;
    label_list *new = (label_list*)malloc(sizeof(label_list));
    
    if(new == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
    /*apply the values to new*/
    new->addres = value;
    new->next_label = NULL;
    new->name = (char*)malloc(strlen(name)+1);
    if(new->name == NULL){
        printf("Memory allocation failed\n");
        free(new);
        exit(1); 
    }
    strcpy(new->name,name);

    if(*label_head == NULL){
        *label_head = new;
        return;
    }
    /*add new to the end of the list*/
    while(pointer->next_label != NULL){
        pointer = pointer->next_label;
    }

    pointer->next_label = new;

}

boolean is_in_label(char *name, label_list *label_head){
    label_list *pointer = label_head;
    while (pointer != NULL){
        if(strcmp(pointer->name, name) == 0){
            return true;
        }
        pointer = pointer->next_label;
    }
    return false;
}

void set_label_values(table_list *label_head, int IC){
    table_list *pointer = label_head;
    if (label_head == NULL){
        return;
    }
    /*run through the list and add the IC to data label this will fix their value*/
    while(pointer != NULL){
        if(pointer->table_type == data_label){
            pointer->value += IC;
        }
        pointer = pointer->next_element;
    }
}

void free_label_list(label_list *table_head){
    label_list *point = table_head;
    label_list *point_next;

    while(point != NULL){
        point_next = point->next_label;
        free(point->name);
        free(point);
        point = point_next;
    }
}

void label_file_create(char *file_name, label_list *label_head){
    FILE *file_label = fopen(file_name,"w");
    label_list *point = label_head;
    /*run through the list and send it to the file*/
    while(point != NULL){
        fprintf(file_label,"%s  %04d\n",point->name, (point->addres + MEMORY_START));
        point = point->next_label;
    }
    fclose(file_label);
}
