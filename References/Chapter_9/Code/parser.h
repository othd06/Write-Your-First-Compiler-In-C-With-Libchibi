/************************
* File: parser.h
************************/

#ifndef PARSER_H
#define PARSER_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "helpers.h"
#include "tokeniser.h"
#include "libchibi.h"

ObjectList parse_program(Token_Seq tokens);

DeclareOption(Type, Type_Option);
DeclareOption(Node, Node_Option);
DeclareOption(Object, Object_Option);

DeclareSeq(char*, char_ptr_Seq);
DeclareSeq(Object, Object_Seq);
DeclareSeq(Type, Type_Seq);
DeclareSeq(UniqueLabel, UniqueLabel_Seq);
DeclareSeq(Node, Node_Seq);

#endif

#ifdef PARSER_C
#undef PARSER_C

Token_Seq tokens;
int idx;

Object_Seq global_objects = {0};
Type_Seq global_object_types = {0};
char_ptr_Seq global_object_names = {0};

Object_Seq objects_to_add = {0};

UniqueLabel_Seq break_labels = {0};
UniqueLabel_Seq continue_labels = {0};

Type_Option parse_base_type();
Type_Option parse_type();
Node_Option parse_expression();
Node_Option parse_statement(char_ptr_Seq* local_names, TypeList* local_types);

Object free_declaration;
Object calloc_declaration;

Node_Option parse_value_literal() {
    Node output_node;
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    switch (tokens.data[idx++].kind) {
        case (TOK_KEYWORD_I8): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_char_literal_node((char)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_I16): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_short_literal_node((short)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_I32): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_int_literal_node((int)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_I64): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_long_literal_node((long)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U8): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_char_literal_node((char)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U16): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_short_literal_node((short)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U32): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_int_literal_node((int)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U64): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL);
            output_node = create_long_literal_node((long)tokens.data[idx++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_F32): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL || tokens.data[idx].kind == TOK_DECIMAL_LITERAL);
            if (tokens.data[idx].kind == TOK_INTEGER_LITERAL) {
                output_node = create_float_literal_node((float)tokens.data[idx++].data.integer_literal, 0, 0);
                //again, no debug info yet
            } else {
                output_node = create_float_literal_node((float)tokens.data[idx++].data.decimal_literal, 0, 0);
                //again, no debug info yet
            }
            break;
        } case (TOK_KEYWORD_F64): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL || tokens.data[idx].kind == TOK_DECIMAL_LITERAL);
            if (tokens.data[idx].kind == TOK_INTEGER_LITERAL) {
                output_node = create_double_literal_node((double)tokens.data[idx++].data.integer_literal, 0, 0);
                //again, no debug info yet
            } else {
                output_node = create_double_literal_node((double)tokens.data[idx++].data.decimal_literal, 0, 0);
                //again, no debug info yet
            }
            break;
        } case (TOK_KEYWORD_F80): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_INTEGER_LITERAL || tokens.data[idx].kind == TOK_DECIMAL_LITERAL);
            if (tokens.data[idx].kind == TOK_INTEGER_LITERAL) {
                output_node = create_long_double_literal_node((long double)tokens.data[idx++].data.integer_literal, 0, 0);
                //again, no debug info yet
            } else {
                output_node = create_long_double_literal_node((long double)tokens.data[idx++].data.decimal_literal, 0, 0);
                //again, no debug info yet
            }
            break;
        } case (TOK_KEYWORD_BOOL): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_BOOL_LITERAL);
            output_node = create_int_literal_node((int)tokens.data[idx++].data.bool_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_STRING): {
            Expect(Node_Option, tokens.data[idx].kind == TOK_STRING_LITERAL);
            
            //create the name for the global object storing the literal
            int buff_size = snprintf(NULL, 0, "string_literal_%d", objects_to_add.len);
            char* name = calloc(sizeof(char), buff_size+1);
            snprintf(name, buff_size+1, "string_literal_%d", objects_to_add.len);
            
            //create the global object to store the literal
            Object literal_object = create_global_variable_definition(
                name,
                create_array_type(
                    create_base_type(BTY_CHAR),
                    strlen(tokens.data[idx].data.string_literal)+1
                ),
                create_array_initialiser(
                    create_array_type(
                        create_base_type(BTY_CHAR),
                        strlen(tokens.data[idx].data.string_literal)+1
                    ),
                    tokens.data[idx++].data.string_literal
                )
            );

            //append said object to the objects_to_add seq
            append(Object_Seq, objects_to_add, literal_object);
            
            output_node = create_addr_node(create_global_var_node(literal_object, name, 0, 0), 0, 0);
            break;
        } default: Expect(Node_Option, false);
    }
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option, output_node);
    return output;
}

Node_Option parse_sizeof() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_SIZEOF);
    Type_Option type = parse_type();
    Expect(Node_Option, type.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    long size = get_type_size(type.value);
    Node_Option output = Just(Node_Option,
        create_long_literal_node(size, 0, 0)
    );
    return output;
}

Node_Option parse_le_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_LE);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option,
        create_le_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_lt_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_LT);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option,
        create_lt_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_ge_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_GE);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option,
        create_ge_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_gt_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_GT);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option,
        create_gt_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_pointer_value() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_PTR);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_TO);
    Node_Option expression = parse_expression();
    Expect(Node_Option, expression.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_addr_node(expression.value, 0, 0)
    );
    return output;
}

Node_Option parse_deref_value() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_FROM);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_PTR);
    Node_Option ptr = parse_expression();
    Expect(Node_Option, ptr.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_deref_node(ptr.value, 0, 0)
    );
    return output;
}

Node_Option parse_variable_literal() {
    Expect(Node_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    char* name = tokens.data[idx++].data.identifier;
    for (int i = 0; i < global_object_names.len; i++) {
        if (strcmp(global_object_names.data[i], name) == 0) {
            Node output_node = create_global_var_node(
                global_objects.data[i],
                name,
                0,
                0
            );
            Node_Option output = Just(Node_Option, output_node);
            return output;
        }
    }
    Node output_node = create_local_var_node(name, 0, 0);
    Node_Option output = Just(Node_Option, output_node);
    return output;
}

Node_Option parse_eq_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_EQ);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option,
        create_eq_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_neq_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_NEQ);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option,
        create_ne_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_shl_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_SHL);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    Node_Option output = Just(Node_Option,
        create_shl_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_shr_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_SHR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    Node_Option output = Just(Node_Option,
        create_shr_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_bit_and_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_AND);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_bit_and_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_bit_or_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_OR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_bit_or_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_bit_not_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_NOT);
    Node_Option expression = parse_expression();
    Expect(Node_Option, expression.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_bit_not_node(expression.value, 0, 0)
    );
    return output;
}

Node_Option parse_bit_xor_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_XOR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_bit_xor_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_log_and_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_LOG);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_AND);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_logic_and_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_log_or_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_LOG);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_OR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_logic_or_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_log_not_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_LOG);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_NOT);
    Node_Option expression = parse_expression();
    Expect(Node_Option, expression.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_not_node(expression.value, 0, 0)
    );
    return output;
}

Node_Option parse_log_xor_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_LOG);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_XOR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);


    //libchibi doesn't have a logic xor node so we need to create it using a xor b = (a or b) and (not (a and b))
    Node or_node = create_logic_or_node(lhs.value, rhs.value, 0, 0);
    Node and_node = create_logic_and_node(lhs.value, rhs.value, 0, 0);
    Node nand_node = create_not_node(and_node, 0, 0);
    Node output_node = create_logic_and_node(or_node, nand_node, 0, 0);
    Node_Option output = Just(Node_Option, output_node);
    return output;
}

Node_Option parse_sub_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_MINUS);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_sub_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_mul_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_TIMES);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_mul_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_div_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_DIVIDE);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_div_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_mod_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_MODULO);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_mod_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_add_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_PLUS);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node_Option output = Just(Node_Option,
        create_add_node(lhs.value, rhs.value, 0, 0)
    );
    return output;
}

Node_Option parse_allocation() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_ALLOCATE);
    Node_Option number = parse_expression();
    Expect(Node_Option, number.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_OF);
    Type_Option type = parse_type();
    Expect(Node_Option, type.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    TypeList argument_types = empty_list;
    append_type(argument_types, create_base_type(BTY_LONG));
    append_type(argument_types, create_base_type(BTY_LONG));
    Type calloc_type = create_function_type(
        create_ptr_type(create_base_type(BTY_VOID)),
        argument_types
    );
    Node proc_node = create_function_node(calloc_declaration, 0, 0);
    NodeList arguments = empty_list;
    append_node(arguments, number.value);
    append_node(arguments, create_long_literal_node(get_type_size(type.value), 0, 0));
    Node output_node = create_funcall_node(
        proc_node,
        calloc_type,
        arguments,
        0,
        0
    );
    Node_Option output = Just(Node_Option, output_node);
    return output;
}

Node_Option parse_proc_call() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    char* name = tokens.data[idx++].data.identifier;
    NodeList args = empty_list;
    while (tokens.data[idx].kind != TOK_R_PAREN) {
        Node_Option arg = parse_expression();
        Expect(Node_Option, arg.present);
        append_node(args, arg.value);
    }
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    for (int i = 0; i < global_object_names.len; i++) {
        if (strcmp(global_object_names.data[i], name) == 0) {
            Object proc = global_objects.data[i];
            Type proc_type = global_object_types.data[i];
            Node proc_node = create_function_node(proc, 0, 0);
            Node output_node = create_funcall_node(
                proc_node, proc_type, args, 0, 0
            );
            return Just(Node_Option, output_node);
        }
    }
    //The function call was correctly parsed but the function doesn't exist so we will produce a fatal error
    fprintf(stderr, "Error: implicit declaration of function %s\n", name);
    exit(1);
}

Node_Option parse_cast() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_CAST);
    Type_Option target_type = parse_type();
    Expect(Node_Option, target_type.present);
    Node_Option expression = parse_expression();
    Expect(Node_Option, expression.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node output_node = create_c_cast_node(
        target_type.value,
        expression.value,
        0,
        0
    );
    Node_Option output = Just(Node_Option, output_node);
    return output;
}

Node_Option parse_expression() {
    int current_idx = idx;
    Node_Option output;

    output = parse_value_literal();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_add_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_sub_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_mul_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_div_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_mod_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_log_and_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_log_or_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_log_not_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_log_xor_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_bit_and_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_bit_or_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_bit_not_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_bit_xor_expr();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_shl_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_shr_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_eq_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_neq_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_gt_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_ge_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_lt_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_le_expression();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_variable_literal();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_proc_call();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_pointer_value();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_deref_value();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_allocation();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_sizeof();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_cast();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Node_Option)Nothing;
}

Type_Option parse_pointer_type() {
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Type_Option, tokens.data[idx++].kind == TOK_KEYWORD_PTR);
    Type_Option ptr_type_type = parse_type();
    Expect(Type_Option, ptr_type_type.present);
    Expect(Type_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Type_Option output = Just(Type_Option,
        create_ptr_type(ptr_type_type.value)
    );
    return output;
}

Type_Option parse_declaration(char** name) {
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);

    int current_idx = idx;
    Type_Option decl_type = parse_base_type();
    if (!decl_type.present){
        //the declaration type was not a base type
        idx = current_idx;
        //backtrack
        decl_type = parse_pointer_type();
        //try a pointer type
        //if this also fails the expect statement will catch it
    }
    
    Expect(Type_Option, decl_type.present);
    Expect(Type_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    //don't consume the token yet as we still need the identifier string
    char* name_temp = tokens.data[idx++].data.identifier;
    //don't modify name until we know we won't fail
    Expect(Type_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    *name = name_temp;
    return decl_type;
}

Type_Option parse_base_type() {
    TokenKind base_type_tokens[] = {
        TOK_KEYWORD_F80, TOK_KEYWORD_F64,
        TOK_KEYWORD_F32, TOK_KEYWORD_U64,
        TOK_KEYWORD_U32, TOK_KEYWORD_U16,
        TOK_KEYWORD_U8, TOK_KEYWORD_I64,
        TOK_KEYWORD_I32, TOK_KEYWORD_I16,
        TOK_KEYWORD_I8, TOK_KEYWORD_BOOL};
    BaseType base_types[] = {
        BTY_LDOUBLE, BTY_DOUBLE,
        BTY_FLOAT, BTY_ULONG,
        BTY_UINT, BTY_USHORT,
        BTY_UCHAR, BTY_LONG,
        BTY_INT, BTY_SHORT,
        BTY_CHAR, BTY_INT};
    //BaseType is a type provided by Libchibi to represent base types
    Type_Option output = Nothing;
    for (int i = 0; i < 12; i++) {
        if (tokens.data[idx].kind == base_type_tokens[i]) {
            output = Just(Type_Option, create_base_type(base_types[i]));
            idx++;
        }
    }
    if (!output.present && tokens.data[idx++].kind == TOK_KEYWORD_STRING) 
        output = Just(Type_Option, create_ptr_type(create_base_type(BTY_CHAR)));
    
    return output;
}

Type_Option parse_proc_type(char_ptr_Seq* arg_names) {
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Type_Option return_type;
    if (tokens.data[idx].kind == TOK_KEYWORD_VOID) {
        idx++;
        return_type = Just(Type_Option,
            create_base_type(BTY_VOID)
        );
    } else {
        return_type = parse_type();
    }
    Expect(Type_Option, return_type.present);
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);

    TypeList arg_types = empty_list;
    //TypeList is a type provided by Libchibi to store lists of types
    char_ptr_Seq arg_names_temp = {0};
    while (tokens.data[idx].kind != TOK_R_PAREN) {
        Type_Option arg_type;
        char* arg_name;
        arg_type = parse_declaration(&arg_name);
        Expect(Type_Option, arg_type.present);
        append(char_ptr_Seq, arg_names_temp, arg_name);
        append_type(arg_types, arg_type.value);
    }

    Expect(Type_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Expect(Type_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    for (int i = 0; i < arg_names_temp.len; i++) {
        append(char_ptr_Seq, *arg_names, arg_names_temp.data[i]);
    }
    //append to arg_names after all Expects so we know we won't fail
    
    Type output_type = create_function_type(return_type.value, arg_types);
    Type_Option output = Just(Type_Option, output_type);
    return output;
}

Type_Option parse_type() {
    int current_idx = idx;
    Type_Option output;

    char_ptr_Seq arg_names = {0};
    //this must be provided to parse_proc_type but will be unused

    output = parse_proc_type(&arg_names);
    if (output.present) {
        return output;
    } else idx = current_idx;

    output = parse_base_type();
    if (output.present) {
        return output;
    } else idx = current_idx;

    output = parse_pointer_type();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Type_Option)Nothing;
}

Node_Option parse_if_statement(char_ptr_Seq* local_names, TypeList* local_types) {
    Node_Seq expressions = {0};
    Node_Seq statements = {0};
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_IF);
    Node_Option new_expression = parse_expression();
    Expect(Node_Option, new_expression.present);
    append(Node_Seq, expressions, new_expression.value);
    Node_Option new_statement = parse_statement(local_names, local_types);
    Expect(Node_Option, new_statement.present);
    append(Node_Seq, statements, new_statement.value);
    while (tokens.data[idx].kind == TOK_KEYWORD_ELIF) {
        idx++;
        new_expression = parse_expression();
        Expect(Node_Option, new_expression.present);
        append(Node_Seq, expressions, new_expression.value);
        new_statement = parse_statement(local_names, local_types);
        Expect(Node_Option, new_statement.present);
        append(Node_Seq, statements, new_statement.value);
    }
    if (tokens.data[idx].kind == TOK_KEYWORD_ELSE) {
        idx++;
        new_statement = parse_statement(local_names, local_types);
        Expect(Node_Option, new_statement.present);
        append(Node_Seq, statements, new_statement.value);
    }
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    if (statements.len == 1 && expressions.len == 1) {
        Node output_node = create_if_node(
            expressions.data[0],
            statements.data[0],
            0,
            0
        );
        return Just(Node_Option, output_node);
    }

    int i = expressions.len-1;
    Node tail_statement;
    if (statements.len > expressions.len) {
        tail_statement = statements.data[statements.len-1];
    } else {
        i--;
        tail_statement = create_if_node(
           expressions.data[expressions.len-1],
           statements.data[statements.len-1],
           0,
           0 
        );
    }
    for (; i >= 0; i--) {
        tail_statement = create_if_else_node(
            expressions.data[i],
            statements.data[i],
            tail_statement,
            0,
            0
        );
    }
    return Just(Node_Option, tail_statement);
}

Node_Option parse_var_assign_statement() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_ASSIGN);
    Expect(Node_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    char* var_name = tokens.data[idx++].data.identifier;
    Node_Option assignment = parse_expression();
    Expect(Node_Option, assignment.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node var_node;
    bool is_global = false;
    for (int i = 0; i < global_object_names.len; i++) {
        if (strcmp(global_object_names.data[i], var_name) == 0) {
            var_node = create_global_var_node(
                global_objects.data[i],
                var_name,
                0,
                0
            );
            is_global = true;
            break;
        }
    }
    if (!is_global) {
        var_node = create_local_var_node(
            var_name,
            0,
            0
        );
    }
    Node_Option output = Just(Node_Option,
        create_expression_stmt_node(
            create_ass_node(
                var_node,
                assignment.value,
                0,
                0
            ),
            0,
            0
        )
    );
    return output;
}

Node_Option parse_return_statement() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_RETURN);
    Node_Option value = parse_expression();
    Expect(Node_Option, value.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node output_node = create_return_node(value.value, 0, 0);
    //again, no debug info
    Node_Option output = Just(Node_Option, output_node);
    return output;
}

Node_Option parse_free_statement() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_FREE);
    Node_Option free_expression = parse_expression();
    Expect(Node_Option, free_expression.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    NodeList args = empty_list;
    append_node(args, free_expression.value);

    TypeList free_type_params = empty_list;
    append_type(
        free_type_params,
        create_ptr_type(create_base_type(BTY_VOID))
    );
    Type free_type = create_function_type(
        create_base_type(BTY_VOID),
        free_type_params
    );

    Node function_node = create_function_node(
        free_declaration,
        0,
        0
    );

    Node funcall_node = create_funcall_node(
        function_node,
        free_type,
        args,
        0, 0 //no debug info
    );

    return Just(Node_Option,
        create_expression_stmt_node(funcall_node, 0, 0)
    );
    //since a funcall can return a value it is an expression and needs to be wrapped in an expression statement node
}

Node_Option parse_while_loop(char_ptr_Seq* local_names, TypeList* local_types) {
    append(UniqueLabel_Seq, break_labels, create_unique_label());
    append(UniqueLabel_Seq, continue_labels, create_unique_label());
    bool succeeded = true;
    if (tokens.data[idx++].kind != TOK_L_PAREN) goto failed;
    if (tokens.data[idx++].kind != TOK_KEYWORD_WHILE) goto failed;
    Node_Option cond = parse_expression();
    if (!cond.present) goto failed;
    Node_Option body = parse_statement(local_names, local_types);
    if (!body.present) goto failed;
    if (tokens.data[idx++].kind != TOK_R_PAREN) goto failed;

    //success
    break_labels.len -= 1;
    continue_labels.len -= 1;
    Node output_node = create_while_node(cond.value, body.value, 0, 0, break_labels.data[break_labels.len], continue_labels.data[continue_labels.len]);
    //technically we're reading from beyond the end of the list but since all we did to remove the last elements was reduce the length value and we haven't yet overwritten the data it's fine.
    return Just(Node_Option, output_node);

    //failure
    failed:;
    //we create a label to jump to if we fail that pops the stacks and returns
    break_labels.len -= 1;
    continue_labels.len -= 1;
    return (Node_Option)Nothing;
}

Node_Option parse_break_statement() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BREAK);
    return Just(Node_Option,
        create_goto_node(break_labels.data[break_labels.len-1], 0, 0)
    );
}

Node_Option parse_continue_statement() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_CONTINUE);
    return Just(Node_Option,
        create_goto_node(continue_labels.data[continue_labels.len-1], 0, 0)
    );
}

Node_Option parse_body_statement(char_ptr_Seq* local_names, TypeList* local_types) {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BODY);
    NodeList body_list = empty_list;
    //NodeList is a type provided by Libchibi to represent lists of AST nodes
    while(tokens.data[idx].kind != TOK_R_PAREN) {
        Node_Option new_node = parse_statement(local_names, local_types);
        Expect(Node_Option, new_node.present);
        append_node(body_list, new_node.value);
        //append_node is a function provided by Libchibi to add nodes to node lists
    }
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    Node body_node = create_block_node(body_list, 0, 0);
    //again we have no debug info yet form the tokeniser
    Node_Option output = Just(Node_Option, body_node);
    return output;
}

Type_Option parse_var_declaration(char** name){
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Type_Option, tokens.data[idx++].kind == TOK_KEYWORD_VAR);
    char* var_name;
    //save the name to a temporary variable until every expect is done
    Type_Option var_type = parse_declaration(&var_name);
    Expect(Type_Option, var_type.present);
    Expect(Type_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    *name = var_name;
    return var_type;
}

Node_Option parse_statement(char_ptr_Seq* local_names, TypeList* local_types) {
    int current_idx = idx;
    Node_Option output;

    output = parse_return_statement();
    //parse_return_statement doesn't need the local names/types Seqs since it can't declare local variables
    if (output.present) {
        return output;
    } else idx = current_idx;
    Node_Option proc_call = parse_proc_call();
    if (proc_call.present) {
        Node output_node = create_expression_stmt_node(proc_call.value, 0, 0);
        output = Just(Node_Option, output_node);
        return output;
    } else idx = current_idx;
    char* name;
    Type_Option var_type = parse_var_declaration(&name);
    if (var_type.present) {
        //we need to construct the variable here since parse_var_declaration is responsible for producing either a local var or a global var depending on where it is called from.
        append(char_ptr_Seq, *local_names, name);
        append_type(*local_types, var_type.value);
        output = Just(Node_Option,
            create_expression_stmt_node(
                create_null_expression_node(0, 0),
                0,
                0
            )
        );
        //since we are just creating a local variable and not doing anything else we return a null statement (an expression statement containing a null expression).
        //we can't return nothing since that indicates that the function failed and the parent needs to backtrack or error out.
        return output;
    } else idx = current_idx;
    output = parse_var_assign_statement();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_if_statement(local_names, local_types);
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_body_statement(local_names, local_types);
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_while_loop(local_names, local_types);
    if (output.present) {
        return output;
    }else idx = current_idx;
    output = parse_break_statement();
    if (output.present) {
        return output;
    }else idx = current_idx;
    output = parse_continue_statement();
    if (output.present) {
        return output;
    }else idx = current_idx;
    output = parse_free_statement();
    if (output.present) {
        return output;
    }else idx = current_idx;

    return (Node_Option)Nothing;
}

Object_Option parse_proc_definition_inner() {
    Expect(Object_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Object_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    //we don't consume this token yet because we still need to get the identifier string
    char* name = tokens.data[idx++].data.identifier;
    char_ptr_Seq arg_names = {0};
    Type_Option proc_type = parse_proc_type(&arg_names);
    //Type is a type provided by Libchibi to represent types
    Expect(Object_Option, proc_type.present);
    char_ptr_Seq local_names = {0};
    TypeList local_types = empty_list;
    Node_Option proc_body = parse_body_statement(&local_names, &local_types);
    //Node is a type provided by Libchibi to represent AST nodes
    Expect(Object_Option, proc_body.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Object_Option output = Just(Object_Option, create_function_definition(name, proc_type.value, arg_names.len, arg_names.data, local_names.len, local_types, local_names.data, proc_body.value));

    append(Object_Seq, global_objects, output.value);
    append(Type_Seq, global_object_types, proc_type.value);
    append(char_ptr_Seq, global_object_names, name);
    return output;
}

Object_Option parse_proc_definition() {
    Object_Option output;
    Expect(Object_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_DEFINE);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_PROC);
    output = parse_proc_definition_inner();
    Expect(Object_Option, output.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    return output;
}

Object_Option parse_var_definition() {
    Expect(Object_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_DEFINE);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_VAR);
    char* name;
    Type_Option var_type = parse_declaration(&name);
    Expect(Object_Option, var_type.present);
    Node_Option var_literal = parse_value_literal();
    Expect(Object_Option, var_literal.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    BaseType var_base_type = get_literal_node_type(var_literal.value);
    GlobalInit var_initialiser;
    if (var_base_type == BTY_CHAR) {
        char data = get_char_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(char));
    } else if (var_base_type == BTY_SHORT) {
        short data = get_short_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(short));
    } else if (var_base_type == BTY_INT) {
        int data = get_int_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(int));
    } else if (var_base_type == BTY_LONG) {
        long data = get_long_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(long));
    } else if (var_base_type == BTY_FLOAT) {
        float data = get_float_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(float));
    } else if (var_base_type == BTY_DOUBLE) {
        double data = get_double_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(double));
    } else if (var_base_type == BTY_LDOUBLE) {
        long double data = get_long_double_literal_from_node(var_literal.value);
        var_initialiser = create_base_type_initialiser(&data, sizeof(long double));
    } else {
        fprintf(stderr, "value literal was of an unsupported type for variable initialisation");
        exit(1);
    }
    //we need to go through this whole rigamarole because parse_value_literal returns a node option not a raw value and size which are what Libchibi's functions for creating initialisers require
    Object_Option output = Just(Object_Option,
        create_global_variable_definition(
            name,
            var_type.value,
            var_initialiser
        )
    );
    append(Object_Seq, global_objects, output.value);
    append(Type_Seq, global_object_types, create_base_type(var_base_type));
    append(char_ptr_Seq, global_object_names, name);
    return output;
}

Object_Option parse_definition() {
    int current_idx = idx;
    Object_Option output;

    output = parse_proc_definition();
    if (output.present) {
        return output;
    } else idx = current_idx;
    output = parse_var_definition();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Object_Option)Nothing;
}

Object_Option parse_extern_proc_decl_inner(){
    Expect(Object_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Object_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    char* name = tokens.data[idx++].data.identifier;
    char_ptr_Seq arg_names = {0};
    //unused but we have to pass it to parse_proc_type anyway
    Type_Option proc_type = parse_proc_type(&arg_names);
    Expect(Object_Option, proc_type.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    Object_Option output = Just(Object_Option,
        create_function_declaration(name, proc_type.value)
    );
    append(Object_Seq, global_objects, output.value);
    append(Type_Seq, global_object_types, proc_type.value);
    append(char_ptr_Seq, global_object_names, name);
    return output;
}

Object_Option parse_extern_proc_declaration(){
    Expect(Object_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_EXTERN);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_PROC);
    Object_Option output = parse_extern_proc_decl_inner();
    Expect(Object_Option, output.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    return output;
}

Object_Option parse_proc_declaration(){
    int current_idx = idx;
    Object_Option output;

    output = parse_extern_proc_declaration();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Object_Option)Nothing;
}

Object_Option parse_object_declaration(){
    int current_idx = idx;
    Object_Option output;

    output = parse_proc_declaration();
    if (output.present) {
        return output;
    } else idx = current_idx;
    char* name;
    Type_Option var_type = parse_var_declaration(&name);
    if (var_type.present) {
        output = Just(Object_Option,
            create_global_variable_declaration(name, var_type.value)
        );
        append(Object_Seq, global_objects, output.value);
        append(Type_Seq, global_object_types, var_type.value);
        append(char_ptr_Seq, global_object_names, name);
        return output;
    } else idx = current_idx;

    return (Object_Option)Nothing;
}

Object parse_object(int num) {
    int current_idx = idx;
    Object_Option output;

    output = parse_definition();
    if (output.present) {
        return output.value;
    } else idx = current_idx;
    output = parse_object_declaration();
    if (output.present) {
        return output.value;
    } else idx = current_idx;

    fprintf(stderr, "Error: failed to parse object %d\n", num);
    exit(1);
}

ObjectList parse_program(Token_Seq tokens_arg) {
    TypeList free_args = empty_list;
    append_type(free_args, create_ptr_type(create_base_type(BTY_VOID)));
    Type free_type = create_function_type(create_base_type(BTY_VOID), free_args);
    free_declaration = create_function_declaration("free", free_type);
    TypeList calloc_args = empty_list;
    append_type(calloc_args, create_base_type(BTY_LONG));
    append_type(calloc_args, create_base_type(BTY_LONG));
    Type calloc_type = create_function_type(create_ptr_type(create_base_type(BTY_VOID)), calloc_args);
    calloc_declaration = create_function_declaration("calloc", calloc_type);
    tokens = tokens_arg;
    idx = 0;
    ObjectList output = empty_list;
    //empty_list is a macro provided by Libchibi to represent an empty list
    append_object(output, free_declaration);
    append_object(output, calloc_declaration);
    int num = 1;
    while (tokens.data[idx].kind != TOK_EOF) append_object(output, parse_object(num++));
    //append_object is a function provided by Libchibi to append objects to object lists
    
    for (int i = 0; i < objects_to_add.len; i++) {
        append_object(output, objects_to_add.data[i]);
    }
    
    return output;
}

#endif