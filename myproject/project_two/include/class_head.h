#ifndef _CLASS_HEAD_H
#define _CLASS_HEAD_H

#include "list.h"


typedef struct patient_node{

    char name[20];
    int age;
    char gender[10];
    struct list_head list;

}patient_node_t;

typedef struct doctor_node{

    char name[20];
    int age;
    char position[20];
    struct list_head list;

}doctor_node_t;

typedef struct music_node{

    char name[20];
    char singer[20];
    char photo[20];
    struct list_head list;
}music_node_t;

typedef struct class_manage{

    patient_node_t *patient_list;
    doctor_node_t *doctor_list;
    music_node_t *music_list;
    int patient_cur_numb;
    int doctor_cur_numb;
    int music_cur_numb;

}class_manage_t;

#endif