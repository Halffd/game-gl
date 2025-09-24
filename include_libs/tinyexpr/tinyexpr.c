/*
 * TINYEXPR - Tiny recursive descent parser and evaluation engine for math expressions
 *
 * Copyright (c) 2015-2018 Lewis Van Winkle
 *
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgement in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/* COMPILE TIME OPTIONS */

/* Exponentiation associativity:
For right associative, this will be defined.
For left associative, this will be undefined. */
#define TE_RIGHT_ASSOCIATIVE

/* Logarithms:
For base 10, this will be defined.
For base e, this will be undefined. */
#define TE_NAT_LOG

#include "tinyexpr.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#ifndef NAN
#define NAN (0.0/0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif


typedef double (*te_fun2)(double, double);

enum {
    TOK_NULL = 0, TOK_ERROR, TOK_END, TOK_SEP,
    TOK_OPEN, TOK_CLOSE, TOK_NUMBER, TOK_VARIABLE, TOK_FUNCTION,
    TOK_INFIX
};

enum {TE_CONSTANT = 0, TE_FUNCTION = 1, TE_VARIABLE = 2};


typedef struct state {
    const char *start;
    const char *next;
    int type;
    union {double value; const double *bound; const void *function;};
    te_variable *context;

    const te_variable *lookup;
    int lookup_len;
} state;


#define TYPE_MASK(TYPE) ((TYPE)&0x0000001F)

#define IS_FUNCTION(TYPE) (((TYPE) & 0x00000020))
#define IS_CLOSURE(TYPE) (((TYPE) & 0x00000040))
#define ARITY(TYPE) ( ((TYPE) & 0x00000FE0) >> 5 )
#define SET_ARITY(F, A) ( ((F) & ~0x00000FE0) | (((A) << 5) & 0x00000FE0) )



static te_expr *new_expr(const int type, const int arity) {
    te_expr *e = malloc(sizeof(te_expr) + arity * sizeof(te_expr*));
    e->type = type;
    e->arity = arity;
    return e;
}


static void te_free_parameters(te_expr *n) {
    if (!n) return;
    int i;
    for (i = 0; i < n->arity; ++i) {
        te_free(n->parameters[i]);
    }
    free(n);
}


void te_free(te_expr *n) {
    if (!n) return;
    te_free_parameters(n);
}


static double pi(void) {return 3.14159265358979323846;}
static double e(void) {return 2.71828182845904523536;}
static double fac(double a) {/*Recursion.*/
    if (a < 0.0)
        return NAN;
    if (a > UINT_MAX)
        return INFINITY;
    unsigned int ua = (unsigned int)(a);
    unsigned long int result = 1, i;
    for (i = 1; i <= ua; i++) {
        if (i > ULONG_MAX / result)
            return INFINITY;
        result *= i;
    }
    return (double)result;
}
static double ncr(double n, double r) {
    if (n < 0.0 || r < 0.0 || n < r) return NAN;
    if (n > UINT_MAX || r > UINT_MAX) return INFINITY;
    unsigned long int un = (unsigned int)(n), ur = (unsigned int)(r), i;
    unsigned long int result = 1;
    if (ur > un / 2) ur = un - ur;
    for (i = 1; i <= ur; i++) {
        if (result > ULONG_MAX / (un - i + 1))
            return INFINITY;
        result = result * (un - i + 1) / i;
    }
    return result;
}
static double npr(double n, double r) {return ncr(n, r) * fac(r);}

static const te_variable functions[] = {
    /* must be in alphabetical order */
    {"abs", fabs,     TE_FUNCTION1, 0},
    {"acos", acos,    TE_FUNCTION1, 0},
    {"asin", asin,    TE_FUNCTION1, 0},
    {"atan", atan,    TE_FUNCTION1, 0},
    {"atan2", atan2,  TE_FUNCTION2, 0},
    {"ceil", ceil,    TE_FUNCTION1, 0},
    {"cos", cos,      TE_FUNCTION1, 0},
    {"cosh", cosh,    TE_FUNCTION1, 0},
    {"e", e,          TE_FUNCTION0, 0},
    {"exp", exp,      TE_FUNCTION1, 0},
    {"fac", fac,      TE_FUNCTION1, 0},
    {"floor", floor,  TE_FUNCTION1, 0},
#ifdef TE_NAT_LOG
    {"ln", log,       TE_FUNCTION1, 0},
#else
    {"log", log10,    TE_FUNCTION1, 0},
#endif
    {"log10", log10,  TE_FUNCTION1, 0},
    {"ncr", ncr,      TE_FUNCTION2, 0},
    {"npr", npr,      TE_FUNCTION2, 0},
    {"pi", pi,        TE_FUNCTION0, 0},
    {"pow", pow,      TE_FUNCTION2, 0},
    {"sin", sin,      TE_FUNCTION1, 0},
    {"sinh", sinh,    TE_FUNCTION1, 0},
    {"sqrt", sqrt,    TE_FUNCTION1, 0},
    {"tan", tan,      TE_FUNCTION1, 0},
    {"tanh", tanh,    TE_FUNCTION1, 0},
    {0, 0, 0, 0}
};

static const te_variable *find_builtin(const char *name, int len) {
    int imin = 0;
    int imax = sizeof(functions) / sizeof(te_variable) - 2;
    int imid;

    while (imax >= imin) {
        imid = (imin + imax) / 2;
        int c = strncmp(name, functions[imid].name, len);
        if (!c && functions[imid].name[len] == '\0') {
            return functions + imid;
        }
        if (c > 0) {
            imin = imid + 1;
        } else {
            imax = imid - 1;
        }
    }

    return 0;
}

static const te_variable *find_variable(const char *name, int len, const te_variable *variables, int var_count) {
    int i;
    for (i = 0; i < var_count; ++i) {
        if (strncmp(name, variables[i].name, len) == 0 && variables[i].name[len] == '\0') {
            return variables + i;
        }
    }
    return 0;
}


static double add(double a, double b) {return a + b;}
static double sub(double a, double b) {return a - b;}
static double mul(double a, double b) {return a * b;}
static double divide(double a, double b) {return a / b;}
static double negate(double a) {return -a;}
static double comma(double a, double b) {return b;}


static void next_token(state *s) {
    s->type = TOK_NULL;

    do {
        if (!*s->next){
            s->type = TOK_END;
            return;
        }

        /* Try reading a number. */
        if ((s->next[0] >= '0' && s->next[0] <= '9') || s->next[0] == '.') {
            s->value = strtod(s->next, (char**)&s->next);
            s->type = TOK_NUMBER;
        } else {
            /* Look for a variable or builtin function call. */
            if (s->next[0] >= 'a' && s->next[0] <= 'z') {
                const char *start;
                start = s->next;
                while ((s->next[0] >= 'a' && s->next[0] <= 'z') || (s->next[0] >= '0' && s->next[0] <= '9') || (s->next[0] == '_')) s->next++;

                const te_variable *var = find_variable(start, s->next - start, s->context, s->lookup_len);
                if (!var) var = find_builtin(start, s->next - start);

                if (var) {
                    switch(TYPE_MASK(var->type)) {
                        case TE_VARIABLE:
                            s->type = TOK_VARIABLE;
                            s->bound = var->address;
                            break;

                        case TE_FUNCTION:
                        case TE_CLOSURE:
                            s->type = TOK_FUNCTION;
                            s->function = var->address;
                            s->type |= var->type & ~TE_VARIABLE;
                            break;
                    }
                } else {
                    s->type = TOK_ERROR;
                }

            } else {
                /* Look for an operator or special character. */
                switch (s->next[0]) {
                    case '+': s->type = TOK_INFIX; s->function = add; break;
                    case '-': s->type = TOK_INFIX; s->function = sub; break;
                    case '*': s->type = TOK_INFIX; s->function = mul; break;
                    case '/': s->type = TOK_INFIX; s->function = divide; break;
                    case '^': s->type = TOK_INFIX; s->function = pow; break;
                    case '%': s->type = TOK_INFIX; s->function = fmod; break;
                    case '(': s->type = TOK_OPEN; break;
                    case ')': s->type = TOK_CLOSE; break;
                    case ',': s->type = TOK_SEP; break;
                    case ' ': case '\t': case '\n': case '\r': break;
                    default: s->type = TOK_ERROR; break;
                }
                s->next++;
            }
        }
    } while (s->type == TOK_NULL);
}


static te_expr *list(state *s);
static te_expr *expr(state *s);
static te_expr *power(state *s);

static te_expr *base(state *s) {
    /* <base>      =    <constant> | <variable> | <function-0> | <function-1> <base> | <function-X> (<list>) | (<expr>) */
    te_expr *n;
    int arity;

    switch (TYPE_MASK(s->type)) {
        case TOK_NUMBER:
            n = new_expr(TE_CONSTANT, 0);
            n->value = s->value;
            next_token(s);
            break;

        case TOK_VARIABLE:
            n = new_expr(TE_VARIABLE, 0);
            n->bound = s->bound;
            next_token(s);
            break;

        case TOK_FUNCTION:
            arity = ARITY(s->type);

            if (arity == 0) {
                n = new_expr(s->type, 0);
                n->function = s->function;
                next_token(s);
            } else if (arity == 1) {
                n = new_expr(s->type, 1);
                n->function = s->function;
                next_token(s);
                n->parameters[0] = base(s);
            } else {
                n = new_expr(s->type, arity);
                n->function = s->function;
                next_token(s);
                if (s->type != TOK_OPEN) s->type = TOK_ERROR;
                else {
                    int i;
                    for(i = 0; i < arity; i++) {
                        next_token(s);
                        n->parameters[i] = expr(s);
                        if(s->type != TOK_SEP) break;
                    }
                    if(s->type != TOK_CLOSE || i != arity - 1) {
                        s->type = TOK_ERROR;
                    } else {
                        next_token(s);
                    }
                }

            }

            break;

        case TOK_OPEN:
            next_token(s);
            n = list(s);
            if (s->type != TOK_CLOSE) {
                s->type = TOK_ERROR;
            } else {
                next_token(s);
            }
            break;

        default:
            n = new_expr(0, 0);
            s->type = TOK_ERROR;
            n->value = NAN;
            break;
    }

    return n;
}


static te_expr *power(state *s) {
    /* <power>     =    {<base> ^} <base> */
    int sign = 1;
    while (s->type == TOK_INFIX && (s->function == add || s->function == sub)) {
        if (s->function == sub) sign = -sign;
        next_token(s);
    }

    te_expr *n;

    if (sign == -1) {
        n = new_expr(TE_FUNCTION1, 1);
        n->function = negate;
        n->parameters[0] = power(s);
    } else {
        n = base(s);
    }

#ifdef TE_RIGHT_ASSOCIATIVE
    if (s->type == TOK_INFIX && s->function == pow) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = s->function;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = power(s);
        return p;
    }
#else
    while (s->type == TOK_INFIX && s->function == pow) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = s->function;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = base(s);
        n = p;
    }
#endif

    return n;
}

#ifdef TE_OLD_EVAL
static te_expr *factor(state *s) {
    /* <factor>    =    <power> {* <power> | / <power>} */
    te_expr *n = power(s);

    while (s->type == TOK_INFIX && (s->function == mul || s->function == divide)) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = s->function;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = power(s);
        n = p;
    }

    return n;
}

static te_expr *term(state *s) {
    /* <term>      =    <factor> {+ <factor> | - <factor>} */
    te_expr *n = factor(s);

    while (s->type == TOK_INFIX && (s->function == add || s->function == sub)) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = s->function;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = factor(s);
        n = p;
    }

    return n;
}
#else
static te_expr *term(state *s) {
    /* <term>      =    <power> {* <power> | / <power> | % <power>} */
    te_expr *n = power(s);

    while (s->type == TOK_INFIX && (s->function == mul || s->function == divide || s->function == fmod)) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = s->function;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = power(s);
        n = p;
    }

    return n;
}

static te_expr *expr(state *s) {
    /* <expr>      =    <term> {+ <term> | - <term>} */
    te_expr *n = term(s);

    while (s->type == TOK_INFIX && (s->function == add || s->function == sub)) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = s->function;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = term(s);
        n = p;
    }

    return n;
}
#endif

static te_expr *list(state *s) {
    /* <list>      =    <expr> {',' <expr>} */
    te_expr *n = expr(s);

    while (s->type == TOK_SEP) {
        te_expr *p = new_expr(TE_FUNCTION2, 2);
        p->function = comma;
        p->parameters[0] = n;
        next_token(s);
        p->parameters[1] = expr(s);
        n = p;
    }

    return n;
}


#define TE_FUN(...) ((double(*)(__VA_ARGS__))n->function)
#define M(e) te_eval(n->parameters[e])

double te_eval(const te_expr *n) {
    if (!n) return NAN;

    switch(TYPE_MASK(n->type)) {
        case TE_CONSTANT: return n->value;
        case TE_VARIABLE: return *n->bound;

        case TE_FUNCTION0: case TE_FUNCTION1: case TE_FUNCTION2: case TE_FUNCTION3:
        case TE_FUNCTION4: case TE_FUNCTION5: case TE_FUNCTION6: case TE_FUNCTION7:
            switch(ARITY(n->type)) {
                case 0: return TE_FUN(void)();
                case 1: return TE_FUN(double)(M(0));
                case 2: return TE_FUN(double, double)(M(0), M(1));
                case 3: return TE_FUN(double, double, double)(M(0), M(1), M(2));
                case 4: return TE_FUN(double, double, double, double)(M(0), M(1), M(2), M(3));
                case 5: return TE_FUN(double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4));
                case 6: return TE_FUN(double, double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4), M(5));
                case 7: return TE_FUN(double, double, double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4), M(5), M(6));
                default: return NAN;
            }

        case TE_CLOSURE0: case TE_CLOSURE1: case TE_CLOSURE2: case TE_CLOSURE3:
        case TE_CLOSURE4: case TE_CLOSURE5: case TE_CLOSURE6: case TE_CLOSURE7:
            switch(ARITY(n->type)) {
                case 0: return TE_FUN(void*)(n->parameters[0]);
                case 1: return TE_FUN(void*, double)(n->parameters[0], M(1));
                case 2: return TE_FUN(void*, double, double)(n->parameters[0], M(1), M(2));
                case 3: return TE_FUN(void*, double, double, double)(n->parameters[0], M(1), M(2), M(3));
                case 4: return TE_FUN(void*, double, double, double, double)(n->parameters[0], M(1), M(2), M(3), M(4));
                case 5: return TE_FUN(void*, double, double, double, double, double)(n->parameters[0], M(1), M(2), M(3), M(4), M(5));
                case 6: return TE_FUN(void*, double, double, double, double, double, double)(n->parameters[0], M(1), M(2), M(3), M(4), M(5), M(6));
                case 7: return TE_FUN(void*, double, double, double, double, double, double, double)(n->parameters[0], M(1), M(2), M(3), M(4), M(5), M(6), M(7));
                default: return NAN;
            }

        default: return NAN;
    }
}

#undef TE_FUN
#undef M

static void optimize(te_expr *n) {
    /* Evaluates as much as possible. */
    if (n->type == TE_CONSTANT) return;
    if (n->type == TE_VARIABLE) return;

    /* Only optimize out functions that have all constant parameters. */
    int i;
    for (i = 0; i < n->arity; ++i) {
        optimize(n->parameters[i]);
        if (n->parameters[i]->type != TE_CONSTANT) return;
    }

    /* All parameters are constant. Evaluate the function. */
    const double value = te_eval(n);
    te_free_parameters(n);
    n->type = TE_CONSTANT;
    n->value = value;
}

te_expr *te_compile(const char *expression, const te_variable *variables, int var_count, int *error) {
    state s;
    s.start = expression;
    s.next = expression;
    s.lookup = variables;
    s.lookup_len = var_count;
    s.context = (te_variable*)functions;

    next_token(&s);
    te_expr *root = list(&s);

    if (s.type != TOK_END) {
        te_free(root);
        if (error) {
            *error = s.next - s.start;
            if (*error == 0) *error = 1;
        }
        return 0;
    } else {
        optimize(root);
        if (error) *error = 0;
        return root;
    }
}

double te_interp(const char *expression, int *error) {
    te_expr *n = te_compile(expression, 0, 0, error);
    double ret;
    if (n) {
        ret = te_eval(n);
        te_free(n);
    } else {
        ret = NAN;
    }
    return ret;
}

static void pn (const te_expr *n, int depth) {
    int i;
    for (i = 0; i < depth; ++i) {
        printf("  ");
    }

    switch(TYPE_MASK(n->type)) {
        case TE_CONSTANT: printf("%f\n", n->value); break;
        case TE_VARIABLE: printf("bound %p\n", n->bound); break;

        case TE_FUNCTION0: case TE_FUNCTION1: case TE_FUNCTION2: case TE_FUNCTION3:
        case TE_FUNCTION4: case TE_FUNCTION5: case TE_FUNCTION6: case TE_FUNCTION7:
        case TE_CLOSURE0: case TE_CLOSURE1: case TE_CLOSURE2: case TE_CLOSURE3:
        case TE_CLOSURE4: case TE_CLOSURE5: case TE_CLOSURE6: case TE_CLOSURE7:
            printf("f%d", ARITY(n->type));
            for(i=0; i<ARITY(n->type); i++) {
                printf(" %p", n->parameters[i]);
            }
            printf("\n");
            for(i=0; i<ARITY(n->type); i++) {
                pn(n->parameters[i], depth + 1);
            }
            break;
    }
}


void te_print(const te_expr *n) {
    pn(n, 0);
}
