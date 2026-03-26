<h1 align=center>Chapter 05: Writing our First Parser</h1>

In the last chapter we covered parsers, how they work, and the specific structure of recursive descent parsers like the one we will be building here in this chapter. Now it's time to put that theory into practice and actually get building.

Before we start, I mentioned last chapter that we will need a way to attempt to parse a token and then backtrack if it fails. For that we will be defining a couple of new macros in the header section of our helpers module:
```c
#define Option(T) struct {bool present; T value}
#define Nothing {.present = false}
#define Just(V) {.present = true; .value = V}
```
This is just a simple, generic, optional 'type' that allows us to return just the AST node if we were able to parse it and nothing if it failed then check the returned value to branch on if the node was present or not.

We are also going to define a macro that expects a statement to be true and returns Nothing otherwise (we wrap it in a do loop so that it doesn't interfere with control flow we can terminate it with a semicolon):
```c
#define Expect(T, E) do {if (!(E)) return (Option(T))Nothing;} while(false)
```

We also need to create our parser.h module. This will be exactly the same as we did before for helpers.h and tokeniser.h:
```c
/************************
* File: parser.h
************************/

#ifndef PARSER_H
#define PARSER_H

//header goes here

#endif

#ifdef PARSER_C
#undef PARSER_C

//implementation goes here

#endif
```

With that complete let's remind ourselves of our parser grammar (the grammar from above the divider in chapter 2):
```ebnf
<program> ::= <object>*;

<object> ::= <definition>;

<definition> ::= <proc_definition>;

<proc_definition> ::= <l_paren> <keyword_define> <keyword_proc>
                 <proc_definition_inner> <r_paren>;

<proc_definition_inner> ::= <l_paren> <identifier> <proc_type>
                      <proc_body> <r_paren>;

<proc_body> ::= <l_paren> <keyword_body> <statement>*
                <r_paren>;

<statement> ::= <return_statement>

<return_statement> ::= <l_paren> <keyword_return> <value_literal>
                <r_paren>;

<type> ::= <proc_type> | <base_type>;

<proc_type> ::= <l_paren> <type> <l_paren> <declaration>*
                <r_paren> <r_paren>;

<base_type> ::= <keyword_bool> | <keyword_i8> | <keyword_i16> |
                <keyword_i32> | <keyword_i64> | <keyword_u8> |
                <keyword_u16> | <keyword_u32> | <keyword_u64> |
                <keyword_f32> | <keyword_f64> | <keyword_f80> |
                <keyword_string>;

<declaration> ::= <l_paren> (<keyword_i8> | <keyword_i16>
                  | <keyword_i32> | <keyword_i64> | <keyword_u8> |
                  <keyword_u16> | <keyword_u32> | <keyword_u64>)
                  <identifier> <r_paren>;
<declaration> ::= <l_paren> (<keyword_f32> | <keyword_f64> |
                  <keyword_f80>) <identifier> <r_paren>;
<declaration> ::= <l_paren> <keyword_bool> <identifier>
                  <r_paren>;
<declaration> ::= <l_paren> <keyword_string> <identifier>
                  <r_paren>;

<value_literal> ::= <l_paren> (<keyword_i8> | <keyword_i16>
                  | <keyword_i32> | <keyword_i64> | <keyword_u8> |
                  <keyword_u16> | <keyword_u32> | <keyword_u64>)
                  <integer_literal> <r_paren>;
<value_literal> ::= <l_paren> (<keyword_f32> |
                    <keyword_f64> | <keyword_f80>)
                   (<integer_literal> | <decimal_literal>)
                   <r_paren>;
<value_literal> ::= <l_paren> <keyword_bool> <bool_literal>
                    <r_paren>;
<value_literal> ::= <l_paren> <keyword_string>
                    <string_literal> <r_paren>;
```
So we will need a function to parse each of these symbols. Let's, in fact, re-write it so that we treat the tokens emitted by our tokeniser as terminals:
```ebnf
<program> ::= <object>* "eof";

<object> ::= <definition>;

<definition> ::= <proc_definition>;

<proc_definition> ::= "l_paren" "keyword_define" "keyword_proc"
                 <proc_definition_inner> "r_paren";

<proc_definition_inner> ::= "l_paren" "identifier" <proc_type>
                      <proc_body> "r_paren";

<proc_body> ::= "l_paren" "keyword_body" <statement>*
                "r_paren";

<statement> ::= <return_statement>

<return_statement> ::= "l_paren" "keyword_return" <value_literal>
                "r_paren";

<type> ::= <proc_type> | <base_type>;

<proc_type> ::= "l_paren" <type> "l_paren" <declaration>*
                "r_paren" "r_paren";

<base_type> ::= "keyword_bool" | "keyword_i8" | "keyword_i16" |
                "keyword_i32" | "keyword_i64" | "keyword_u8" |
                "keyword_u16" | "keyword_u32" | "keyword_u64" |
                "keyword_f32" | "keyword_f64" | "keyword_f80" |
                "keyword_string";

<declaration> ::= "l_paren" ("keyword_i8" | "keyword_i16"
                  | "keyword_i32" | "keyword_i64" | "keyword_u8" |
                  "keyword_u16" | "keyword_u32" | "keyword_u64")
                  "identifier" "r_paren";
<declaration> ::= "l_paren" ("keyword_f32" | "keyword_f64" |
                  "keyword_f80") "identifier" "r_paren";
<declaration> ::= "l_paren" "keyword_bool" "identifier"
                  "r_paren";
<declaration> ::= "l_paren" "keyword_string" "identifier"
                  "r_paren";

<value_literal> ::= "l_paren" ("keyword_i8" | "keyword_i16"
                  | "keyword_i32" | "keyword_i64" | "keyword_u8" |
                  "keyword_u16" | "keyword_u32" | "keyword_u64")
                  "integer_literal" "r_paren";
<value_literal> ::= "l_paren" ("keyword_f32" |
                    "keyword_f64" | "keyword_f80")
                   ("integer_literal" | "decimal_literal")
                   "r_paren";
<value_literal> ::= "l_paren" "keyword_bool" "bool_literal"
                    "r_paren";
<value_literal> ::= "l_paren" "keyword_string"
                    "string_literal" "r_paren";
```
Note as well how we added in the eof token to the end of the program to represent the end of the file.

I would also recommend reading over the include header file for libchibi although don't worry if you don't fully understand everything; I'll cover what we need as we go (plus, there are a few couple of not-super-important-and-slightly-esoteric functions I really haven't documented all that well).

In the header section of parser.h we just need to import helpers.h, tokeniser.h and libchibi.h then provide a single function declaration:
```c
//header section
#include "helpers.h"
#include "tokeniser.h"
#include "libchibi.h"

ObjectList parse_program(Seq(Token) tokens);
```
Every other function will be private to the implementation of our parser. Note as well that the parse_program function returns the type ObjectList. This is a type provided by Libchibi and is what Libchibi expects the program to be provided as. The objects in question are function and global variable declarations/definitions (Note: for those of you intimately familiar with C's backend terminology, libchibi treats a tentative function definition as a kind of declaration not a kind of definition. If you don't know what that meant, good; you really don't need to). As you can see, we already have an object symbol that mirrors this in our grammar.

Now we can start to define our parse_program function in the body section. If you remember back to chapter 4 I mentioned that we need to store our index and token string in global variables within the parser module so the first thing parse_program has to do is initialise them. Then it simply has to initialise an ObjectList to return and call parse_object() (a function we have yet to implement to parse the object symbol) repeatedly and add that object to the ObjectList to be returned. We can work out when to stop calling parse_object() by checking when we have reached an eof token.
```c
Seq(Token) tokens;
int index;

ObjectList parse_program(Seq(Token) tokens_arg) {
    tokens = tokens_arg;
    index = 0;
    ObjectList output = empty_list; //empty_list is a macro provided by Libchibi to represent an empty list
    while (tokens[index].kind != TOK_EOF) append_object(output, parse_object());
    //append_object is a function provided by Libchibi to append objects to object lists
    return output;
}
```
You can see how this directly mirrors our original grammar with the while loop mirroring the '*' operator and the call to parse_object mirroring the object token.

We can do the same to implement parse_object. I will, however, make a slight adjustment since later we will be adding more alternative replacement rules for the object token.
```c
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
```
This allows us to add new blocks that parse other object types and correctly backtrack by resetting the index between them until we either find a valid object rule or, having found none, produce an error.

We can repeat the exact same pattern for parse_definition (since, again, we will later have new kinds of definitions as well):
```c
Option(Object) parse_definition() {
    int current_idx = index;
    Option(Object) output;

    output = parse_proc_definition();
    if (output.present) {
        return output;
    } else index = current_idx;

    return (Option(Object))Nothing;
}
```
Notice how, this time, since we are returning an option, we no longer produce an error when parsing fails but instead simply return nothing and allow the parent function to handle it by trying the next case.

We can now implement parse_proc_definition:
```c
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
```
Remember that we defined Expect to return Nothing if the condition fails so this allows us to consume a token and fail if it isn't what we expect by returning Nothing in exactly the same way we did for parse_definition. We don't need to backtrack since a proc definition will never have multiple different replacement rules.

Now we just follow the same pattern for parse_proc_definition_inner:
```c
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
```
This is a little bit different than the previous functions we defined. Firstly, it is the first time we are calling a Libchibi function to create, in this case, an object. As you can see we use the information from parsing the various different terminals and non-terminals together to create our procedure definition. (The two 0s at the end are for debug info representing the file and line number however since we haven't stored the line number in our tokens we just provide 0 for now; it won't stop the compiled binary from working). We also get pass pointer to Seqs to our parse_proc_body and parse_proc_type functions. This is so that the functions can modify these vectors and return the local variables/types and argument names of our procedure which need (in Libchibi) to be stored directly in the procedure definition rather than the type or body where they will be written in our language.

Note as well, how we do call assert at one point in this function. This is because we really do want to produce an error if the local names and local types have different lengths. In general, if something should never happen then we want to cause an error and either fprintf to stderr and quit with an error code whereas if something just doesn't match the grammar we want to return none so that the parent can backtrack and try something else.

We can now implement the next non-terminal parser: parse_proc_body:
```c
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
```
Here we create a block node that holds as it's body a list of the nodes returned from parse_statement and return this as the procedure's body node. Notice how, just like with parse_proc this directly mirrors the grammar of a proc_body, consuming the terminals and then evaluating the statements in a loop that matches the '*' operator in the grammar (like with parse_program).

Now we can implement parse_statement. You will see that it works very similarly to parse_definition because, again, we will be adding more statement types:
```c
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
```

And now we can implement parse_return_statement:
```c
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
```
Note that we don't actually check that the type of the return statement matches the return type of the function. This is something, like debug info, that we will go back and implement in later chapters.

And now we can implement parse_type:
```c
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
```
Here you can see how we can try multiple different replacement rules when one fails. Note that we had to reset index to current_idx between cases because parse_proc_type might partially consume some tokens which would cause parse_base_type to fail even if there had actually been a base type there.

Now we can implement parse_proc_type:
```c
Option(Type) parse_proc_type(Seq(char*)* arg_names) {
    Expect(Type, tokens[index++].kind == TOK_L_PAREN);
    Option(Type) return_type = parse_type();
    //note: we will have to forward declare parse_type to make this work
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
```

We can now implement parse_base_type:
```c
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
```
Note this looks a little different because base type is only a single terminal with many different rules for what that terminal could be but this should still feel fairly familiar to both the grammar and to what we wrote for the tokeniser in chapter 3.

Now we can implement parse_declaration (Note, we're going to slightly cheat and use parse_base_type to implement it. Even though this isn't exactly as written in the grammar, it is completely equivalent; take a second to reassure yourself of this as you follow along):
```c
Option(Type) parse_declaration(char** name) {
    Expect(Type, tokens[index++].kind == TOK_L_PAREN);
    Option(Type) decl_type = parse_base_type();
    //note: we will have to forward declare parse_base_type to make this work
    Expect(Type, decl_type.present);
    Expect(Type, tokens[index].kind == TOK_IDENTIFIER);
    //don't consume the token yet as we still need the identifier string
    char* name_temp = tokens[index++].data.identifier;
    //don't modify name until we know we won't fail
    Expect(Type, tokens[index++].kind == TOK_R_PAREN);
    *name = name_temp;
    return decl_type;
}
```

And finally we can implement parse_value_literal:
```c
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
```
And just like that our parse is done. That's really how simple recursive descent parsers are. Of course, as we expand our language some features our going to become more complex but from a grammar perspective, that's pretty much as difficult as it's ever going to be.

Don't forget, of course, to import the right header files in the header section of parser.h:
  *  \<assert.h>
  *  \<stdbool.h>
  *  \<stdio.h>
  *  "tokeniser.h"
  *  "helpers.h"
  *  "libchibi.h"

We can now take our entire program and parse it into an AST. In the next chapter we will write our main file to bring together our tokeniser and parser and compile a full program from a text file input into a working executable with libchibi.

