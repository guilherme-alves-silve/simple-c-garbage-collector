#ifndef __APP__
#define __APP__

#include "mdl.h"

typedef struct employee_ {
    char name[30];
    unsigned int id;
    unsigned int age;
    float salary;
    struct employee_ *mgr;
} employee_t;

typedef struct student_ {
    char name[32];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    struct student_ *best_colleage;
} student_t;

object_db_t* create_object_db_configured();

student_t* create_student(object_db_t *object_db, char* name, int rollno, int age, float aggregate, student_t *best_colleage);

employee_t* create_employee(object_db_t *object_db, char* name, int id, int age, float salary, employee_t *mgr);

student_t* create_students(object_db_t *object_db, int units);

employee_t* create_employees(object_db_t *object_db, int units);

#endif