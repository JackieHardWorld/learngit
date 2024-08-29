#ifndef _PATIENT_H
#define _PATIENT_H

#include "list.h"
#include "class_head.h"


patient_node_t *Init_patient_node(patient_node_t *data);

extern void destroy_patient_list(patient_node_t* head);

extern void file_write_patient_info(FILE *fp, class_manage_t *manager);


extern void file_read_patient_info(FILE *fp, class_manage_t *manager);

extern void insert_patient_node(class_manage_t *manager, FILE *fp);

extern void foreach_patient_node(class_manage_t *manager);

extern patient_node_t *find_patient_node(class_manage_t *manager, char *name);

extern void delete_patient_node(class_manage_t *manager, FILE *fp);

extern void list_bubble_patient_info(class_manage_t *manager);
extern void change_patient_node(class_manage_t *manager, FILE *fp);

extern void file_load_patient_info(FILE *fp, class_manage_t *manager);
extern char *read_patient_head_node(class_manage_t *manager);

#endif