#ifndef _MUSIC_H
#define _MUSIC_H

#include "list.h"
#include "class_head.h"


music_node_t *Init_music_node(music_node_t *data);
extern void destroy_music_list(music_node_t* head);
extern void file_write_music_info(FILE *fp3, class_manage_t *manager);
extern void file_read_music_info(FILE *fp3, class_manage_t *manager);
extern void insert_music_node(class_manage_t *manager, FILE *fp3);
extern void foreach_music_node(class_manage_t *manager);
extern music_node_t *find_music_node(class_manage_t *manager, char *name);
extern void delete_music_node(class_manage_t *manager, FILE *fp3);
extern void list_bubble_music_info(class_manage_t *manager);
extern void change_music_node(class_manage_t *manager, FILE *fp3);
extern void file_load_music_info(FILE *fp3, class_manage_t *manager);

#endif