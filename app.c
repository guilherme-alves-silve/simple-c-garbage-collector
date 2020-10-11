#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"

object_db_t* create_object_db_configured() {

    struct_db_t *struct_db = calloc(1, sizeof(struct_db_t));
    
    static field_info_t employee_fields[] = {
        FIELD_INFO(employee_t, name,   CHAR,    0),
        FIELD_INFO(employee_t, id,     UINT32,  0),
        FIELD_INFO(employee_t, age,    UINT32,  0),
        FIELD_INFO(employee_t, salary, FLOAT, 0),
        FIELD_INFO(employee_t, mgr,    OBJ_PTR, employee_t)
    };

    REG_STRUCT(struct_db, employee_t, employee_fields);

    static field_info_t student_fiels[] = {
        FIELD_INFO(student_t, name, CHAR, 0),
        FIELD_INFO(student_t, rollno,    UINT32, 0),
        FIELD_INFO(student_t, age,       UINT32, 0),
        FIELD_INFO(student_t, aggregate, FLOAT, 0),
        FIELD_INFO(student_t, best_colleage, OBJ_PTR, student_t)
    };
    REG_STRUCT(struct_db, student_t, student_fiels);

    print_structure_db(struct_db);

    object_db_t *object_db = calloc(1, sizeof(object_db_t));
    object_db->struct_db = struct_db;
    
    return object_db;
}

student_t* create_student(object_db_t *object_db, char* name, int rollno, int age, float aggregate, student_t *best_colleage) {
    
    student_t *student = xcalloc(object_db, "student_t", 1);
    strncpy(student->name, name, strlen(name));
    student->rollno = rollno;
    student->age = age;
    student->aggregate = aggregate;
    student->best_colleage = best_colleage;
    return student;
}

employee_t* create_employee(object_db_t *object_db, char* name, int id, int age, float salary, employee_t *mgr) {

    employee_t *employee = xcalloc(object_db, "employee_t", 1);
    strncpy(employee->name, name, strlen(name));
    employee->id = id;
    employee->age = age;
    employee->salary = salary;
    employee->mgr = mgr;
    return employee;
}

student_t* create_students(object_db_t *object_db, int units) {
    student_t *student = xcalloc(object_db, "student_t", units);
    return student;
}

employee_t* create_employees(object_db_t *object_db, int units) {
    employee_t *employee = xcalloc(object_db, "employee_t", units);
    return employee;
}