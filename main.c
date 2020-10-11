#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdl.h"
#include "app.h"

int main() {
    
    object_db_t *object_db = create_object_db_configured();
    
    student_t *abhishek = create_student(object_db, "abhishek", 30, 33, 50.5, NULL);
    student_t *shivani = create_student(object_db, "shivani", 2, 18, 81, abhishek);
    employee_t *joseph = create_employee(object_db, "joseph", 1, 25, 7500.0, NULL);

    student_t *arthur = create_student(object_db, "arthur", 30, 33, 50.5, NULL);

    print_object_db(object_db);

    to_string(object_db, arthur);

    return 0;
}


