#include "type.h"

bool is_type_array(type *type_to_check)
{
    type *curr = type_to_check;
    while (curr->next != NULL)
        curr = curr->next;
    // check if we have an array here
    if (curr->base == ARRAY)
        return true;
    return false;
}

size_t get_dimension_of_array(type *array_type)
{
    type *curr = array_type;
    while (curr->base != ARRAY)
        curr = curr->next;
    size_t count = 0;
    while (curr->base == ARRAY)
    {
        count++;
        curr = curr->next;
    }
    return count;
}

bool deduce_expression_type(type *type_of_var, expr_t *res)
{
    // we have to deduce the type of the expression and then based on that
    // proceed to create an expression
    if (is_type_array(type_of_var))
    {
        size_t dimension = get_dimension_of_array(type_of_var);
        if (dimension > 1)
        {
        }
    }
}

// bool is_type_primitive(type *type_to_check)
// {
//     if (type_to_check->base >= SIGNED_BYTE && type_to_check->base <= BOOLEAN)
//         return true;
//     return false;
// }

bool is_array_fit_for_string(type *array_type)
{
    // 'u8[]' can accept a string as the assignment.
    // 'u8[][]...' can also accept strings as the assignment(each string will be given the length of the longest one)
    /// NOTE: 'u8*[]' doesn't accept strings for assignments
    /// NOTE: 'u8**[]' also cannot accept strings for assignments
    if (array_type->base == UNSIGNED_BYTE)
        return true;
    return false;
}