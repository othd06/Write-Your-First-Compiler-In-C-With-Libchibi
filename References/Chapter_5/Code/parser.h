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

ObjectList parse_program(Seq(Token) tokens);

#endif

#ifdef PARSER_C
#undef PARSER_C

Seq(Token) tokens;
int index;


Option(Type) parse_base_type();
Option(Type) parse_type();

Option(Node) parse_value_literal() {
    Node output_node;
    Expect(Node, tokens[index++].kind == TOK_L_PAREN);
    switch (tokens[index++].kind) {
        case (TOK_KEYWORD_I8): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_char_literal_node((char)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_I16): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_short_literal_node((short)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_I32): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_int_literal_node((int)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_I64): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_long_literal_node((long)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U8): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_char_literal_node((char)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U16): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_short_literal_node((short)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U32): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_int_literal_node((int)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_U64): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL);
            output_node = create_long_literal_node((long)tokens[index++].data.integer_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_F32): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL || tokens[index].kind == TOK_DECIMAL_LITERAL);
            if (tokens[index].kind == TOK_INTEGER_LITERAL) {
                output_node = create_float_literal_node((float)tokens[index++].data.integer_literal, 0, 0);
                //again, no debug info yet
            } else {
                output_node = create_float_literal_node((float)tokens[index++].data.decimal_literal, 0, 0);
                //again, no debug info yet
            }
            break;
        } case (TOK_KEYWORD_F64): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL || tokens[index].kind == TOK_DECIMAL_LITERAL);
            if (tokens[index].kind == TOK_INTEGER_LITERAL) {
                output_node = create_double_literal_node((double)tokens[index++].data.integer_literal, 0, 0);
                //again, no debug info yet
            } else {
                output_node = create_double_literal_node((double)tokens[index++].data.decimal_literal, 0, 0);
                //again, no debug info yet
            }
            break;
        } case (TOK_KEYWORD_F80): {
            Expect(Node, tokens[index].kind == TOK_INTEGER_LITERAL || tokens[index].kind == TOK_DECIMAL_LITERAL);
            if (tokens[index].kind == TOK_INTEGER_LITERAL) {
                output_node = create_long_double_literal_node((long double)tokens[index++].data.integer_literal, 0, 0);
                //again, no debug info yet
            } else {
                output_node = create_long_double_literal_node((long double)tokens[index++].data.decimal_literal, 0, 0);
                //again, no debug info yet
            }
            break;
        } case (TOK_KEYWORD_BOOL): {
            Expect(Node, tokens[index].kind == TOK_BOOL_LITERAL);
            output_node = create_bool_literal_node(tokens[index++].data.bool_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_STRING): {
            fprintf(stderr, "Error: string literals not yet supported\n");
            exit(1);
            //supporting string literals requires declaring global variables which we haven't implemented yet
            break;
        } default: Expect(Node, false);
    }
    Expect(Node, tokens[index++].kind == TOK_R_PAREN);
    Option(Node) output = Just(output_node);
    return output;
}

Option(Type) parse_declaration(char** name) {
    Expect(Type, tokens[index++].kind == TOK_L_PAREN);
    Option(Type) decl_type = parse_base_type();
    Expect(Type, decl_type.present);
    Expect(Type, tokens[index].kind == TOK_IDENTIFIER);
    //don't consume the token yet as we still need the identifier string
    char* name_temp = tokens[index++].data.identifier;
    //don't modify name until we know we won't fail
    Expect(Type, tokens[index++].kind == TOK_R_PAREN);
    *name = name_temp;
    return decl_type;
}

Option(Type) parse_base_type() {
    TokenKind base_type_tokens[] = {
        TOK_KEYWORD_F80, TOK_KEYWORD_F64,
        TOK_KEYWORD_F32, TOK_KEYWORD_U64,
        TOK_KEYWORD_U32, TOK_KEYWORD_U16,
        TOK_KEYWORD_U8, TOK_KEYWORD_I64,
        TOK_KEYWORD_I32, TOK_KEYWORD_I16,
        TOK_KEYWORD_I8, TOK_KEYWORD_BOOL};
    BaseType base_types[] = {
        BTY_LDOUBLE, BTY_DOUBLE,
        BTY_FLOAT, BTY_LONG,
        BTY_INT, BTY_SHORT,
        BTY_CHAR, BTY_LONG,
        BTY_INT, BTY_SHORT,
        BTY_CHAR, BTY_INT};
    //BaseType is a type provided by Libchibi to represent base types
    Option(Type) output = Nothing;
    for (int i = 0; i < 12; i++) {
        if (tokens[index].kind == base_type_tokens[i]) {
            output = Just(create_base_type(base_types[i]));
            index++;
        }
    }
    if (!output.present && tokens[index++].kind == TOK_KEYWORD_STRING) 
        output = Just(create_ptr_type(create_base_type(BTY_CHAR)));
    
    return output;
}

Option(Type) parse_proc_type(Seq(char*)* arg_names) {
    Expect(Type, tokens[index++].kind == TOK_L_PAREN);
    Option(Type) return_type = parse_type();
    Expect(Type, return_type.present);
    Expect(Type, tokens[index++].kind == TOK_L_PAREN);

    TypeList arg_types = empty_list;
    //TypeList is a type provided by Libchibi to store lists of types
    Seq(char*) arg_names_temp = {0};
    while (tokens[index].kind != TOK_R_PAREN) {
        Option(Type) arg_type;
        char* arg_name;
        arg_type = parse_declaration(&arg_name);
        Expect(Type, arg_type.present);
        append(char*, arg_names_temp, arg_name);
        append_type(arg_types, arg_type.value);
    }

    Expect(Type, tokens[index++].kind == TOK_R_PAREN);
    Expect(Type, tokens[index++].kind == TOK_R_PAREN);

    for (int i = 0; i < arg_names_temp.len; i++) {
        append(char*, *arg_names, arg_names_temp.data[i]);
    }
    //append to arg_names after all Expects so we know we won't fail
    
    Type output_type = create_function_types(return_type.value, arg_types);
    Option(Type) output = Just(output_type);
    return output;
}

Option(Type) parse_type() {
    int current_idx = index;
    Option(Type) output;

    Seq(char*) arg_names = {0};
    //this must be provided to parse_proc_type but will be unused

    output = parse_proc_type(&arg_names);
    if (output.present) {
        return output;
    } else index = current_idx;

    output = parse_base_type();
    if (output.present) {
        return output;
    } else index = current_idx;

    return (Option(Type))Nothing;
}

Option(Node) parse_return_statement() {
    Expect(Node, tokens[index++].kind == TOK_L_PAREN);
    Expect(Node, tokens[index++].kind == TOK_KEYWORD_RETURN);
    Option(Node) value = parse_value_literal();
    Expect(Node, value.present);
    Expect(Node, tokens[index++].kind == TOK_R_PAREN);

    Node output_node = create_return_node(value.value, 0, 0);
    //again, no debug info
    Option(Node) output = Just(output_node);
    return output;
}

Option(Node) parse_statement(Seq(char*)* local_names, Seq(Type)* local_types) {
    int current_idx = index;
    Option(Node) output;

    output = parse_return_statement();
    //parse_return_statement doesn't need the local names/types Seqs since it can't declare local variables
    if (output.present) {
        return output;
    } else index = current_idx;

    return (Option(Node))Nothing;
}

Option(Node) parse_proc_body(Seq(char*)* local_names, Seq(Type)* local_types) {
    Expect(Node, tokens[index++].kind == TOK_L_PAREN);
    Expect(Node, tokens[index++].kind == TOK_KEYWORD_BODY);
    NodeList body_list = empty_list;
    //NodeList is a type provided by Libchibi to represent lists of AST nodes
    while(tokens[index].kind != TOK_R_PAREN) {
        Option(Node) new_node = parse_statement(local_names, local_types);
        Expect(Node, new_node.present);
        append_node(body_list, new_node.value);
        //append_node is a function provided by Libchibi to add nodes to node lists
    }
    Expect(Node, tokens[index++].kind == TOK_R_PAREN);
    
    Node body_node = create_block_node(body_list, 0, 0);
    //again we have no debug info yet form the tokeniser
    Option(Node) output = Just(body_node);
    return output;
}

Option(Object) parse_proc_definition_inner() {
    Expect(Object, tokens[index++].kind == TOK_L_PAREN);
    Expect(Object, tokens[index].kind == TOK_IDENTIFIER);
    //we don't consume this token yet because we still need to get the identifier string
    char* name = tokens[index++].data.identifier;
    Seq(char*) arg_names = {0};
    Option(Type) proc_type = parse_proc_type(&arg_names);
    //Type is a type provided by Libchibi to represent types
    Expect(Object, proc_type.present);
    Seq(char*) local_names = {0};
    Seq(Type) local_types = {0};
    Option(Node) proc_body = parse_proc_body(&local_names, &local_types);
    //Node is a type provided by Libchibi to represent AST nodes
    assert(local_names.len == local_types.len);
    //If these two Seqs are different lengths then something has gone wrong that can't be easily recovered from
    Expect(Object, proc_body.present);
    Expect(Object, tokens[index++].kind == TOK_R_PAREN);
    Option(Object) output = Just(create_function_definition(name, proc_type, arg_names.len, arg_names.data, local_names.len, local_types.data, local_names.data, 0, 0));
    return output;
}

Option(Object) parse_proc_definition() {
    Option(Object) output;
    Expect(Object, tokens[index++].kind == TOK_L_PAREN);
    Expect(Object, tokens[index++].kind == TOK_KEYWORD_DEFINE);
    Expect(Object, tokens[index++].kind == TOK_KEYWORD_PROC);
    output = parse_proc_definition_inner();
    Expect(Object, output.present);
    Expect(Object, tokens[index++].kind == TOK_R_PAREN);
    return output;
}

Option(Object) parse_definition() {
    int current_idx = index;
    Option(Object) output;

    output = parse_proc_definition();
    if (output.present) {
        return output;
    } else index = current_idx;

    return (Option(Object))Nothing;
}

Object parse_object() {
    int current_idx = index;
    Option(Object) output;

    output = parse_definition();
    if (output.present) {
        return output.value;
    } else index = current_idx;

    fprintf(stderr, "Error: failed to parse object\n");
    exit(1);
}

ObjectList parse_program(Seq(Token) tokens_arg) {
    tokens = tokens_arg;
    index = 0;
    ObjectList output = empty_list; //empty_list is a macro provided by Libchibi to represent an empty list
    while (tokens[index].kind != TOK_EOF) append_object(output, parse_object());
    //append_object is a function provided by Libchibi to append objects to object lists
    return output;
}

#endif