
<div>
<small>
  Write Your First Compiler In C With Libchibi<br>
  <em>Part 1: Compiling Our First Program</em>
</small>
</div>
<h1 align=center>Chapter 05: Writing our First Parser</h1>

### Overview:

In the last chapter we covered parsers, how they work, and the specific structure of recursive descent parsers like the one we will be building here in this chapter. Now it's time to put that theory into practice and actually get building.

### Defining Our New Helpers:

Before we start, I mentioned last chapter that we will need a way to attempt to parse a symbol and then backtrack if it fails. For that we will be defining a couple of new macros in the header section of our helpers module:
```c
#define DeclareOption(T, Name) typedef struct{bool present; T value;} Name
#define Nothing {.present = false}
#define Just(Name, V) (Name){.present = true, .value = V}
```
This is just a simple, generic, optional 'type' that allows us to return just the AST node if we were able to parse it and nothing if it failed then check the returned value to branch on if the node was present or not.

We are also going to define a macro that expects a statement to be true and returns Nothing otherwise (we wrap it in a do loop so that it doesn't interfere with control flow and we can terminate it with a semicolon):
<a id="Expect_Macro"><\a>
```c
#define Expect(Name, E) do {if (!(E)) return (Name)Nothing;} while(false)
```
This will be what allows us to conveniently consume symbols and fail if they weren't what we expected in the grammar, allowing us to follow the grammar more closely in structure.

### Setting Up Our Module:

After defining the new helpers we will need, the next step is to set up the skeleton of our parser module. This will be exactly the same as we did before for helpers.h and tokeniser.h:
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

As with the tokeniser before, I recommend opening the grammar and following along with that too. Again, you can find it in the section above the line of the grammar reference for chapter 2 but I recommend using the reference for the parser grammar for this chapter as I've made a few changes: I added in the eof token from last chapter, and I rewrote the tokens to be treated as terminals rather than non-terminals since they are the input string to our parser. This is the grammar I will be quoting throughout the chapter.

I would also recommend opening back up the [include header](https://github.com/othd06/libchibi/blob/main/libchibi.h) file for libchibi although don't worry if you don't fully understand everything; I'll cover what we need as we go (plus, there are a few couple of not-super-important-and-slightly-esoteric functions I really haven't documented all that well). It's still a very useful reference when following along.

### Defining Our Header Section:

In the header section of parser.h we just need to import helpers.h, tokeniser.h and libchibi.h then provide a single function declaration:
```c
//header section
#include "helpers.h"
#include "tokeniser.h"
#include "libchibi.h"

ObjectList parse_program(Token_Seq tokens);
```
Every other function will be private to the implementation of our parser. Note as well that the parse_program function returns the type ObjectList. This is a type provided by Libchibi and is what Libchibi expects the program to be provided as. The objects in question are function and global variable declarations/definitions (Note: for those of you intimately familiar with C's backend terminology, libchibi treats a tentative function definition as a kind of declaration not a kind of definition. If you don't know what that meant, good; you really don't need to). As you can see, we already designed our grammar to have an object symbol that mirrors this. This is one of those areas where you can hopefully start to see how the design of our grammar begins to correspond with the eventual AST.

We are also going to need a few Option types and one more Seq type in this module beyond what we declared in tokeniser.h:
```c
DeclareOption(Type, Type_Option);
DeclareOption(Node, Node_Option);
DeclareOption(Object, Object_Option);

DeclareSeq(char*, char_ptr_Seq);
```

### Implementing Our Parser:

Now that we've set up the header section correctly, we can start to define our parse_program function in the body section. If you remember back to last chapter, I mentioned that we need to store our index and token string in global variables within the parser module so the first thing parse_program has to do is initialise them.

#### Programs, Objects, and Definitions:

Then it simply has to initialise an ObjectList to return and fill it according to our grammar:
```EBNF
<program> ::= <object>* "eof";
```
Which we can see means we will need to parse objects in a loop (because of the '*') until we hit an eof token:
```c
Token_Seq tokens;
int idx;

ObjectList parse_program(Token_Seq tokens_arg) {
    tokens = tokens_arg;
    idx = 0;
    ObjectList output = empty_list; //empty_list is a macro provided by Libchibi to represent an empty list
    while (tokens.data[idx].kind != TOK_EOF) append_object(output, parse_object());
    //append_object is a function provided by Libchibi to append objects to object lists
    return output;
}
```

For parse object we know this is our grammar:
```EBNF
<object> ::= <definition>;
```
Meaning we could simply return the value of parse_definition but, since we will be adding new alternative symbols that can be an object I will suggest we do this instead:
```c
Object parse_object() {
    int current_idx = idx;
    Object_Option output;

    output = parse_definition();
    if (output.present) {
        return output.value;
    } else idx = current_idx;

    fprintf(stderr, "Error: failed to parse object\n");
    exit(1);
}
```
This accepts an Object_Option from parse_definition and returns an Object if parse definition is present. Note how we store the original index and reset it if a definition wasn't present. This is so that we can attempt to parse other, alternative, symbols when we start to introduce alternatives. This is the pattern we will use repeatedly as our corresponding code to '|' in our EBNF.

Similarly for parse_definition, the grammar is:
```EBNF
<definition> ::= <proc_definition>;
```
So we could just return the result of parse_proc_definition but since, again the whole point of definitions being a separate symbol is to have different symbol alternatives we will follow the same structure as for parse_object:
```c
Object_Option parse_definition() {
    int current_idx = idx;
    Object_Option output;

    output = parse_proc_definition();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Object_Option)Nothing;
}
```
Notice how, this time, since we are returning an option, we no longer produce an error when parsing fails but instead simply return nothing and allow the parent function to handle it by trying the next case.

#### Proc_Definitions:

Unlike the last few symbols, proc_definition has a slightly different structure that is more linear rather than a set of alternatives:
```EBNF
<proc_definition> ::= "l_paren" "keyword_define" "keyword_proc"
                 <proc_definition_inner> "r_paren";
```
So here we can consume the tokens and non-terminals and fail by returning nothing if the token was not what we expected or the non-terminal itself using our [Expect](#Expect_Macro) macro from above. That looks like this:
```c
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
```

We can do the same for parse_proc_definition_inner:
```EBNF
<proc_definition_inner> ::= "l_paren" "identifier" <proc_type>
                            <proc_body> "r_paren";
```
except here we need to also generate a corresponding AST Node with the information we parse (the name in the identifier, the type, and the body). We can do that using the function create_function_definition provided to us by Libchibi. We also need to pass some values by reference to some of the child parsers. This is because Libchibi expects some information to reside in the function definition AST node such as local variable names and types and argument names that in our grammar are found in symbols corresponding to its child nodes. We therefore need to pass them up separately by allowing those functions to modify these lists directly. Overall, however, the code should still look very familiar to our grammar:
```c
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
    Node_Option proc_body = parse_proc_body(&local_names, &local_types);
    //Node is a type provided by Libchibi to represent AST nodes
    Expect(Object_Option, proc_body.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Object_Option output = Just(Object_Option, create_function_definition(name, proc_type.value, arg_names.len, arg_names.data, local_names.len, local_types, local_names.data, proc_body.value));
    return output;
}
```

#### Proc_Bodies:

We can now implement the next non-terminal parser: proc_body. The grammar for this is:
```EBNF
<proc_body> ::= "l_paren" "keyword_body" <statement>*
                "r_paren";
```
Which means we're going to need to consume some tokens then consume in a loop (again, due to the '*') until we see an r_paren then consume it. We also need to construct an AST node and in this case it's a block node constructed with create_block_node that contains a NodeList constructed from appending the results of parse_statement in our loop:
```c
Node_Option parse_proc_body(char_ptr_Seq* local_names, TypeList* local_types) {
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
    //The two 0s here represent a file and line number for debug info in the executable.
    //Since we didn't store any debug info in our tokens, we can just provide 0s.
    Node_Option output = Just(Node_Option, body_node);
    return output;
}
```

#### Statements and Return_Statements:

The grammar for a statement looks a lot like the grammar for an object or a definition in that it is defined as just one symbol but will later have many alternatives:
```EBNF
<statement> ::= <return_statement>;
```
So it shouldn't surprise you that the code looks incredibly similar too:
```c
Node_Option parse_statement(char_ptr_Seq* local_names, TypeList* local_types) {
    int current_idx = idx;
    Node_Option output;

    output = parse_return_statement();
    //parse_return_statement doesn't need the local names/types Seqs since it can't declare local variables
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Node_Option)Nothing;
}
```

By now you should be seeing the pattern. The grammar for return_statements is very simple, just some tokens we have to consume and a single non-terminal we need to expect. We can then create a return node using the create_return_node function from Libchibi:
```c
Node_Option parse_return_statement() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_RETURN);
    Node_Option value = parse_value_literal();
    Expect(Node_Option, value.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Node output_node = create_return_node(value.value, 0, 0);
    //again, no debug info
    Node_Option output = Just(Node_Option, output_node);
    return output;
}
```
Note that we don't actually check that the type of the return statement matches the return type of the function. This is something, like debug info, that we will go back and implement in later chapters.

#### Types:

The type symbol is another one that follows the structure of multiple alternatives each of only one symbol:
```EBNF
<type> ::= <proc_type> | <base_type>;
```
So the structure should look very familiar. Here there are actually 2 alternatives though so you can see how we handle that second case (not that there's much to see if you understood how the backtracking worked in our earlier 1-case examples):
```c
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

    return (Type_Option)Nothing;
}
```

The grammar for proc_type is slightly more involved but nothing we haven't seen before:
```EBNF
<proc_type> ::= "l_paren" <type> "l_paren" <declaration>*
                "r_paren" "r_paren";
```
Some tokens, some non-terminals, and a loop. All stuff we've already seen how to implement so this code shouldn't be too surprising. The node is then constructed with the Libchibi function create_function_node. The only slight complexity is that the argument names need to be extracted from the declarations where they are provided in the grammar and added to the list provided by the parent function (parse_proc_definition_inner):
```c
Type_Option parse_proc_type(char_ptr_Seq* arg_names) {
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Type_Option return_type = parse_type();
    //note: we will have to forward declare parse_type to make this work
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
```

For the base_type symbol it's just a bunch of single-terminal alternatives. We could handle this like we did the series of terminal-alternatives but since each terminal can directly be read from the tokens it's easier to simply use a dictionary to find the corresponding Libchibi base type and call create_base_type with that (or, if the terminal is keyword_string, call create_ptr_type on a char base type created as above):
```c
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
```

#### Declarations:

The grammar for declarations is, again, very standard:
```EBNF
<declaration> ::= "l_paren" <base_type> "identifier"
                  "r_paren";
```
Some terminals to consume, one of which is the name, and a base_type non-terminal. We just return the base_type and set the name through the parameter passed by reference (as called by parse_proc_type above (and others, later)):
```c
Type_Option parse_declaration(char** name) {
    Expect(Type_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Type_Option decl_type = parse_base_type();
    //note: we will have to forward declare parse_base_type to make this work
    Expect(Type_Option, decl_type.present);
    Expect(Type_Option, tokens.data[idx].kind == TOK_IDENTIFIER);
    //don't consume the token yet as we still need the identifier string
    char* name_temp = tokens.data[idx++].data.identifier;
    //don't modify name until we know we won't fail
    Expect(Type_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    *name = name_temp;
    return decl_type;
}
```

#### Expressions:

The grammar for expression is again, the same pattern as for object, type, etc...:
```EBNF
<expression> ::= <value_literal>;
```
And implemented accordingly:
```c
Node_Option parse_expression() {
    int current_idx = idx;
    Node_Option output;

    output = parse_value_literal();
    if (output.present) {
        return output.value;
    } else idx = current_idx;

    return (Node_Option)Nothing;
}
```

The grammar for value_literal:
```EBNF
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
is probably the most complex we've seen so far since it involves multiple alternatives that are all longer than one token however we can see they all start and end with l_paren and r_paren so we can consume the l_paren terminal, branch on the next token to find the specific case (and fail if no case matches), generate our node using the correct create_\<whichever type is being created>_literal_node function from Libchibi, and finally consume the final r_paren. So again, our code still matches closely with the structure of the grammar (although somewhat longer and more repetitive as we individually handle the many similar cases):
```c
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
            output_node = create_bool_literal_node(tokens.data[idx++].data.bool_literal, 0, 0);
            //again, no debug info yet
            break;
        } case (TOK_KEYWORD_STRING): {
            fprintf(stderr, "Error: string literals not yet supported\n");
            exit(1);
            //supporting string literals requires declaring global variables which we haven't implemented yet
            break;
        } default: Expect(Node_Option, false);
    }
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    Node_Option output = Just(Node_Option, output_node);
    return output;
}
```

#### Stepping Back:

We can now parse every symbol in our parser_grammar and build a corresponding AST node using the correct Libchibi function. We saw that there are various patterns that respond to different features of our EBNF grammar and that, once we have designed and defined our syntax in EBNF, we can usually write the code to mirror it in a very straightforward manner.

In particular, we saw three major patterns:
  *  '*' and '+' correspond to while (or do while) loops that end when the next token is whatever comes next.
  *  Linear non-terminals can be parsed by just expecting and consuming terminals and expecting that consumed non-terminals be present.
  *  Non-terminals that are defined by many, one non-terminal alternatives can be parsed by speculatively attempting various alternatives and backtracking if they don't work out.

Much like with our tokeniser, however, we can also see that this code is a lot more verbose than our grammar is on it's own demonstrating exactly why we design in EBNF where we can see and manipulate the whole thing at once, before attempting to also juggle building a much larger codebase and trying to identify the specific patterns we need in each parser out of thin air.

### Final Notes:

And just like that our parse is done. That's really how simple recursive descent parsers are. Of course, as we expand our language some features are going to become more complex but from a grammar perspective, that's getting quite close to as difficult as it's ever going to be.

Don't forget, of course, to import the right header files in the header section of parser.h:
  *  \<assert.h>
  *  \<stdbool.h>
  *  \<stdio.h>
  *  "tokeniser.h"
  *  "helpers.h"
  *  "libchibi.h"

We can now take our entire program and parse it into an AST. In the next chapter we will write our main file to bring together our tokeniser and parser and compile a full program from a text file input into a working executable with libchibi.

