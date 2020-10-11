/*
 * =====================================================================================
 *
 *       Filename:  mld.c
 *
 *    Description:  This file implements the functions and routines for mld library
 *
 *        Version:  1.0
 *        Created:  Thursday 28 February 2019 06:17:28  IST
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Networking Developer (AS), sachinites@gmail.com
 *        Company:  Brocade Communications(Jul 2012- Mar 2016), Current : Juniper Networks(Apr 2017 - Present)
 *        
 *        This file is part of the MLD distribution (https://github.com/sachinites).
 *        Copyright (c) 2017 Abhishek Sagar.
 *        This program is free software: you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by  
 *        the Free Software Foundation, version 3.
 *
 *        This program is distributed in the hope that it will be useful, but 
 *        WITHOUT ANY WARRANTY; without even the implied warranty of 
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *        General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License 
 *        along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "css.h"
#include "mdl.h"

char *DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
                     "CHAR", "OBJ_PTR", "FLOAT",
                     "DOUBLE", "OBJ_STRUCT"};

/* Dumping Function */

void
print_structure_rec(struct_db_rec_t *struct_rec){
    if(!struct_rec) {
        return;
    }
    
    int j = 0;
    field_info_t *field = NULL;
    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW "| %-20s | size = %-8d | #flds = %-3d |\n" ANSI_COLOR_RESET, struct_rec->struct_name, struct_rec->ds_size, struct_rec->n_fields);
    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    for(j = 0; j < struct_rec->n_fields; j++){
        field = &struct_rec->fields[j];
        if (NULL == field) {
            printf("There are NULL field_info_t, in struct %s", struct_rec->struct_name);
            continue;
        }
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d|  nstructname = %-20s  |\n",
                j, field->fname, DATA_TYPE[field->dtype], field->size, field->offset, field->nested_str_name);
        printf("  %-20s |", "");
        printf(ANSI_COLOR_CYAN "--------------------------------------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    }
}

void
print_structure_db(struct_db_t *struct_db){
    
    if(!struct_db) {
        puts("struct_db cannot be null!");
        return;
    }
    
    printf("printing structure db\n");
    int i = 0;
    struct_db_rec_t *struct_rec = NULL;
    struct_rec = struct_db->head;
    printf("No of Structures Registered = %d\n", struct_db->count);
    while(struct_rec){
        printf("structure No : %d (%p)\n", i++, struct_rec);
        print_structure_rec(struct_rec);
        struct_rec = struct_rec->next;
    }
}

int
add_structure_to_struct_db(struct_db_t *struct_db, 
                           struct_db_rec_t *struct_rec) {

    if(!struct_db) {
        puts("struct_db cannot be null!");
        return -1;
    }

    struct_db_rec_t *head = struct_db->head;

    if(!head){
        struct_db->head = struct_rec;
        struct_rec->next = NULL;
        struct_db->count++;
        return 0;
    }

    struct_rec->next = head;
    struct_db->head = struct_rec;
    struct_db->count++;
    return 0;
}

struct_db_rec_t*
struct_db_look_up(struct_db_t *struct_db, char *struct_name) {

    if (!struct_db) {
        puts("struct_db cannot be null!");
        return NULL;
    }

    if (!struct_name || strcmp(struct_name, EMPTY_STR) == 0) {
        puts("struct_name cannot be null or empty!");
        return NULL;
    }

    struct_db_rec_t* record = struct_db->head;
    while (record) {
        if (strcmp(record->struct_name, struct_name) == 0) {
            return record;
        }

        record = record->next;
    }

    return NULL;
}

static object_db_rec_t *
object_db_look_up(object_db_t *object_db, void *ptr) {

    object_db_rec_t *head = object_db->head;
    if(!head) return NULL;
    
    for(; head; head = head->next){
        if(head->ptr == ptr)
            return head;
    }
    return NULL;
}

/*Working with objects*/
static void
add_object_to_object_db(object_db_t *object_db, 
                     void *ptr, 
                     int units,
                     struct_db_rec_t *struct_rec,
                     mld_boolean_t is_root) {
     
    object_db_rec_t *obj_rec = object_db_look_up(object_db, ptr);
    /*Dont add same object twice*/
    assert(!obj_rec);

    obj_rec = calloc(1, sizeof(object_db_rec_t));

    obj_rec->next = NULL;
    obj_rec->ptr = ptr;
    obj_rec->units = units;
    obj_rec->struct_rec = struct_rec;
    obj_rec->is_visited = MDL_FALSE;
    obj_rec->is_root = is_root;

    object_db_rec_t *head = object_db->head;
        
    object_db->count++;
    if(!head) {
        object_db->head = obj_rec;
        obj_rec->next = NULL;
        return;
    }

    obj_rec->next = head;
    object_db->head = obj_rec;
}


void *
xcalloc(object_db_t *object_db, 
        char *struct_name, 
        int units) {

    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);
    void *ptr = calloc(units, struct_rec->ds_size);
    add_object_to_object_db(object_db, ptr, units, struct_rec, MDL_FALSE);
    return ptr;
}


/*Dumping Functions for Object database*/
void 
print_object_rec(object_db_rec_t *obj_rec, int i){
    
    if(!obj_rec) return;
    printf(ANSI_COLOR_MAGENTA"-----------------------------------------------------------------------------------|\n");
    printf(ANSI_COLOR_YELLOW "%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s |\n", 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name); 
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------|\n");
}

void
print_object_db(object_db_t *object_db){

    object_db_rec_t *head = object_db->head;
    unsigned int i = 0;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n");
    for(; head; head = head->next){
        print_object_rec(head, i++);
    }
}

void
mld_dump_object_rec_detail(object_db_rec_t *obj_rec) {
    if (!obj_rec) {
        puts("obj_rec cannot be null!");
        return;
    }

    int i = 0;
    int n = 0;
    field_info_t *field_info = NULL;
    struct_db_rec_t *struct_rec = NULL;
    void *current_object_ptr = NULL;

    for (i = 0; i < obj_rec->units; i++) {
        struct_rec = obj_rec->struct_rec;
        for (n = 0; n < struct_rec->n_fields; n++) {
            
            current_object_ptr = (i * struct_rec->ds_size) + obj_rec->ptr;
            
            field_info = &struct_rec->fields[n];
            void *current_field = (field_info->offset + current_object_ptr);
            switch (field_info->dtype) {
                case UINT32:
                case UINT8: {
                    printf("%s[%d]->%s = %u\n", struct_rec->struct_name, i, field_info->fname,  *(unsigned int*) current_field);
                    break;
                }
                case INT32: {
                    printf("%s[%d]->%s = %d\n", struct_rec->struct_name, i, field_info->fname, *(int*) current_field);
                    break;
                }
                case CHAR: {
                    printf("%s[%d]->%s = %s\n", struct_rec->struct_name, i, field_info->fname, (char*) current_field);
                    break;
                }
                case FLOAT: {
                    printf("%s[%d]->%s = %f\n", struct_rec->struct_name, i, field_info->fname, *(float*) current_field);
                    break;
                }
                case DOUBLE: {
                    printf("%s[%d]->%s = %f\n", struct_rec->struct_name, i, field_info->fname, *(double*) current_field);
                    break;
                }
                case OBJ_PTR: {
                    printf("%s[%d]->%s = %p\n", struct_rec->struct_name, i, field_info->fname, (void*) *(size_t*) current_field);
                    break;
                }
                case OBJ_STRUCT: {
                    break;
                }
            }
        }
        
        puts(""); //Line break
    }
}

void
to_string(object_db_t *object_db, void *ptr) {
    object_db_rec_t *obj_rec = object_db_look_up(object_db, ptr);
    mld_dump_object_rec_detail(obj_rec);
}

static object_db_rec_t *
object_db_look_up_and_remove(object_db_t *object_db, const void const *ptr) {

    object_db_rec_t *actual = object_db->head;
    if(!actual) {
        return NULL;
    }
    
    object_db_rec_t *before = NULL;
    if (actual->ptr == ptr) {
        object_db->head = actual->next;
        object_db->count--;
        return actual;
    }
    
    before = actual;
    actual = actual->next;
    
    while (actual) {
        
        if(actual->ptr == ptr) {
            before->next = actual->next;
            object_db->count--;
            return actual;  
        }
        
        before = actual;
        actual = actual->next;
    }
    
    return NULL;
}

void
xfree(object_db_t *object_db, void *ptr) {

    object_db_rec_t *object_db_rec = object_db_look_up_and_remove(object_db, ptr);
    if (!object_db_rec) {
        printf("Object %p was not found!", object_db_rec);
        assert(!object_db_rec);
    }

    free(object_db_rec->ptr);
    free(object_db_rec);
}

void 
mld_register_root_object(object_db_t *object_db, 
                          void *objptr, 
                          char *struct_name, 
                          unsigned int units) {
    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    if (!struct_db) {
        printf("Struct %s was not found!", struct_name);
        assert(!struct_db);
    }
    
    add_object_to_object_db(object_db, objptr, units, struct_db_rec_t *struct_rec, MDL_TRUE);
}

void
mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr) {
    
    object_db_rec_t *object_db_rec = object_db_look_up(object_db, obj_ptr);
    if (!object_db_rec) {
        puts("Object was not found!");
        assert(!object_db_rec);
    }
    
    object_db_rec->is_root = MDL_TRUE;
}