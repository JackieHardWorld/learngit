#ifndef _DOCTOR_H
#define _DOCTOR_H

#include "list.h"
#include "class_head.h"



doctor_node_t *Init_doctor_node(doctor_node_t *data);

extern void destroy_doctor_list(doctor_node_t* head);

extern void file_write_doctor_info(FILE *fp2, class_manage_t *manager);

extern void file_read_doctor_info(FILE *fp2, class_manage_t *manager);

extern void insert_doctor_node(class_manage_t *manager, FILE *fp2);


extern void foreach_doctor_node(class_manage_t *manager);

extern doctor_node_t *find_doctor_node(class_manage_t *manager, char *name);

extern void delete_doctor_node(class_manage_t *manager, FILE *fp2);

extern void list_bubble_doctor_info(class_manage_t *manager);

extern void change_doctor_node(class_manage_t *manager, FILE *fp2);
extern void file_load_doctor_info(FILE *fp2, class_manage_t *manager);




#endif