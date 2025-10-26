#include "parser.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>

static char *cursor;

static inline bool parse(bool(*token_function)(double *), double *result) {
    while (isspace(*cursor))
        cursor++;
    char *cursor_copy = cursor;
    double result_copy = *result;
    if (token_function(result))
        return true;
    cursor = cursor_copy;
    *result = result_copy;
    return false;
}

static inline bool token(const char *literal) {
    while (isspace(*cursor))
        cursor++;
    char *cursor_copy = cursor;
    while (literal && *literal) {
        if (*literal != *cursor) {
            cursor = cursor_copy;
            return false;
        }
        literal++;
        cursor++;
    }
    return true;
}

static inline bool number(double *result) {
    const char *anchor = cursor;
    if (token("pi"))
        *result = 3.1415926535897931;
    else if (token("e"))
        *result = 2.7182818284590452;
    else if (isdigit(*cursor)) {
        cursor++;
        while (isdigit(*cursor))
            cursor++;
        if (*cursor == '.') {
            cursor++;
            while (isdigit(*cursor))
                cursor++;
        }
        *result = strtod(anchor, &cursor);
    } 
    else if (*cursor == '.') {
        cursor++;
        while (isdigit(*cursor))
            cursor++;
        *result = strtod(anchor, &cursor);
    }
    else
        return false;
    return true;
}

enum Operation { NONE, ADD, SUB, MULT, DIV, MOD, POW };

static inline double evaluate(double left, enum Operation op, double right) {
    switch (op)
    {
    case ADD:
        return left + right;
    case SUB:
        return left - right;
    case MULT:
        return left * right;
    case DIV:
        return left / right;
    case MOD:
        return fmod(left, right);
    case POW:
        return pow(left, right);
    default:
        return NAN;
    }
}

static bool expression(double *result);

static inline bool primary_expression(double *result) {
    if (parse(number, result)) {}
    else if (token("(")) {
        if(!parse(expression, result))
            return false;
        if (!token(")"))
            return false;
    }
    else
        return false;
    return true;
}

static inline bool trigonometric_expression(double *result) {
    if (token("sin") && parse(expression, result))
        *result = sin(*result);
    else if (token("cos") && parse(expression, result))
        *result = cos(*result);
    else if (token("tan") && parse(expression, result))
        *result = tan(*result);
    else if (token("arcsin") && parse(expression, result))
        *result = asin(*result);
    else if (token("arccos") && parse(expression, result))
        *result = acos(*result);
    else if (token("arctan") && parse(expression, result))
        *result = atan(*result);
    else
        return false;
    return true;
}

static inline bool unary_expression(double *result) {
    if (token("-") && parse(primary_expression, result))
    *result = -(*result);
    else if (token("+") && parse(primary_expression, result)) {}
    else if (parse(primary_expression, result)) {}
    else if (parse(trigonometric_expression, result)) {}
    else
        return false;
    return true;
}

static inline bool power_expression(double *result) {
    if (parse(unary_expression, result)) {
        if (!token("^"))
            return true;
        double exponent;
        if (!parse(unary_expression, &exponent))
            return false;
        *result = evaluate(*result, POW, exponent);
    }
    else return false;
    return true;
}

static inline bool multiplicative_expression(double *result) {
    if (!parse(power_expression, result))
        return false;
    for (;;) {
        enum Operation op = NONE;
        if (token("*"))
            op = MULT;
        else if (token("/"))
            op = DIV;
        else if (token("mod"))
            op = MOD;
        else
            return true;
        double right;
        if (!parse(power_expression, &right))
            return false;
        *result = evaluate(*result, op, right);
    }
}

static inline bool additive_expression(double *result) {
        if (!parse(multiplicative_expression, result))
            return false;
    for (;;) {
        enum Operation op = NONE;
        if (token("+"))
            op = ADD;
        else if (token("-"))
            op = SUB;
        else
            return true;
        double right;
        if (!parse(power_expression, &right))
            return false;
        *result = evaluate(*result, op, right);
    }
}

static bool expression(double *result) {
    return parse(additive_expression, result);
}

bool parse_and_evaluate_expression(const char *expression_string, double *result) {
    cursor = (char *)expression_string;
    (void)expression(result);
    return *cursor == '\0';
}