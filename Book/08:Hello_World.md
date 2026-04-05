<h1 align=center>Chapter 08: Hello World</h1>

Last chapter we outlined the features we wanted in order to make our language 'hello world capable' (in fact, we went a fair bit beyond merely being able to compile hello world but that's how we chose to frame the next milestone). This chapter, we are going to actually go through and implement them.

The first thing to do is to update our tokeniser, so let's remind ourselves of our new tokens from chapter 7:
```EBNF
<symbol_plus> ::= "+";
<symbol_minus> ::= "-";
<symbol_times> ::= "*";
<symbol_divide> ::= "/";
<symbol_modulo> ::= "%";
<symbol_eq> ::= "=";
<symbol_neq> ::= "/=";
<symbol_gt> ::= ">";
<symbol_ge> ::= ">=";
<symbol_lt> ::= "<";
<symbol_le> ::= "<=";

<keyword_extern> ::= "extern";
<keyword_var> ::= "var";
<keyword_assign> ::= "assign";
<keyword_if> ::= "if";
<keyword_elif> ::= "elif";
<keyword_else> ::= "else";
<keyword_while> ::= "while";
<keyword_break> ::= "break";
<keyword_continue> ::= "continue";
<keyword_log> ::= "log";
<keyword_and> ::= "and";
<keyword_or> ::= "or";
<keyword_not> ::= "not";
<keyword_xor> ::= "xor";
<keyword_bit> ::= "bit";
<keyword_shl> ::= "shl";
<keyword_shr> ::= "shr";
<keyword_void> ::= "void";
<keyword_ptr> ::= "ptr";
<keyword_to> ::= "to";
<keyword_from> ::= "from";
<keyword_allocate> ::= "allocate";
<keyword_of> ::= "of";
<keyword_free> ::= "free";
<keyword_sizeof> ::= "sizeof";
```
So let's add each of these to our token kind enum:
```c
typedef enum {
    TOK_STRING_LITERAL,
    TOK_BOOL_LITERAL,
    TOK_INTEGER_LITERAL,
    TOK_DECIMAL_LITERAL,
    TOK_IDENTIFIER,
    TOK_KEYWORD_STRING,
    TOK_KEYWORD_F80,
    TOK_KEYWORD_F64,
    TOK_KEYWORD_F32,
    TOK_KEYWORD_U64,
    TOK_KEYWORD_U32,
    TOK_KEYWORD_U16,
    TOK_KEYWORD_U8,
    TOK_KEYWORD_I64,
    TOK_KEYWORD_I32,
    TOK_KEYWORD_I16,
    TOK_KEYWORD_I8,
    TOK_KEYWORD_BOOL,
    TOK_KEYWORD_BODY,
    TOK_KEYWORD_PROC,
    TOK_KEYWORD_DEFINE,
    TOK_KEYWORD_RETURN,
    TOK_KEYWORD_EXTERN,
    TOK_KEYWORD_VAR,
    TOK_KEYWORD_ASSIGN,
    TOK_KEYWORD_IF,
    TOK_KEYWORD_ELIF,
    TOK_KEYWORD_ELSE,
    TOK_KEYWORD_WHILE,
    TOK_KEYWORD_BREAK,
    TOK_KEYWORD_CONTINUE,
    TOK_KEYWORD_LOG,
    TOK_KEYWORD_AND,
    TOK_KEYWORD_OR,
    TOK_KEYWORD_NOT,
    TOK_KEYWORD_XOR,
    TOK_KEYWORD_BIT,
    TOK_KEYWORD_SHL,
    TOK_KEYWORD_SHR,
    TOK_KEYWORD_VOID,
    TOK_KEYWORD_PTR,
    TOK_KEYWORD_TO,
    TOK_KEYWORD_FROM,
    TOK_KEYWORD_ALLOCATE,
    TOK_KEYWORD_OF,
    TOK_KEYWORD_FREE,
    TOK_KEYWORD_SIZEOF,
    TOK_L_PAREN,
    TOK_R_PAREN,
    TOK_SYMBOL_PLUS,
    TOK_SYMBOL_MINUS,
    TOK_SYMBOL_TIMES,
    TOK_SYMBOL_DIVIDE,
    TOK_SYMBOL_MODULO,
    TOK_SYMBOL_EQ,
    TOK_SYMBOL_NEQ,
    TOK_SYMBOL_GT,
    TOK_SYMBOL_GE,
    TOK_SYMBOL_LT,
    TOK_SYMBOL_LE,
    TOK_EOF,
} TokenKind;
```
For each of the keywords we can then simply add them to our dictionary of keywords:
```c
//keyword dictionary
char* keyword_strings[] = {"string", "f80", "f64",
    "f32", "u64", "u32", "u16", "u8", "i64", "i32",
    "i16", "i8", "bool", "body", "proc", "define",
    "return", "extern", "var", "assign", "if", "elif",
    "else", "while", "break", "continue", "log", "and",
    "or", "not", "xor", "bit", "shl", "shr", "void",
    "ptr", "to", "from", "allocate", "of", "free",
    "sizeof"};
TokenKind keyword_kinds[] = {TOK_KEYWORD_STRING,
    TOK_KEYWORD_F80, TOK_KEYWORD_F64,
    TOK_KEYWORD_F32, TOK_KEYWORD_U64,
    TOK_KEYWORD_U32, TOK_KEYWORD_U16,
    TOK_KEYWORD_U8, TOK_KEYWORD_I64,
    TOK_KEYWORD_I32, TOK_KEYWORD_I16,
    TOK_KEYWORD_I8, TOK_KEYWORD_BOOL,
    TOK_KEYWORD_BODY, TOK_KEYWORD_PROC,
    TOK_KEYWORD_DEFINE, TOK_KEYWORD_RETURN,
    TOK_KEYWORD_EXTERN, TOK_KEYWORD_VAR,
    TOK_KEYWORD_ASSIGN, TOK_KEYWORD_IF,
    TOK_KEYWORD_ELIF, TOK_KEYWORD_ELSE,
    TOK_KEYWORD_WHILE, TOK_KEYWORD_BREAK,
    TOK_KEYWORD_CONTINUE, TOK_KEYWORD_LOG,
    TOK_KEYWORD_AND, TOK_KEYWORD_OR,
    TOK_KEYWORD_NOT, TOK_KEYWORD_XOR,
    TOK_KEYWORD_BIT, TOK_KEYWORD_SHL,
    TOK_KEYWORD_SHR, TOK_KEYWORD_VOID,
    TOK_KEYWORD_PTR, TOK_KEYWORD_TO,
    TOK_KEYWORD_FROM, TOK_KEYWORD_ALLOCATE,
    TOK_KEYWORD_OF, TOK_KEYWORD_FREE,
    TOK_KEYWORD_SIZEOF};
```
And our existing keyword identification code will handle it all correctly (one of the many benefits of good code architecture up front).

For our new symbol tokens, it's a bit more tricky. Still, we have an established pattern that we can use for our single character symbols. Let's add these else if branches to the end of our after the parens and before the string literal branch:
```c
//after 'else if (plaintext[index] == ')') ...' but before 'else if (plaintext[index] == '"') ...'
else if (plaintext[index] == '+') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_PLUS;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '-') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_MINUS;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '*') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_TIMES;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '/') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_DIVIDE;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '%') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_MODULO;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '=') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_EQ;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '>') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_GT;
    append(Token_Seq, tokens, new_token);
} else if (plaintext[index] == '<') {
    index++;
    Token new_token;
    new_token.kind = TOK_SYMBOL_LT;
    append(Token_Seq, tokens, new_token);
}
```
Then, for the two character symbols, we can modify their respective branches:
```c
else if (plaintext[index] == '/') {
    index++;
    Token new_token;
    if (plaintext[index] == '=') {
        index++;
        new_token.kind = TOK_SYMBOL_NEQ;
    } else {
        new_token.kind = TOK_SYMBOL_DIVIDE;
    }
    append(Token_Seq, tokens, new_token);
}
```
```c
else if (plaintext[index] == '>') {
    index++;
    Token new_token;
    if (plaintext[index] == '=') {
        index++;
        new_token.kind = TOK_SYMBOL_GE;
    } else {
        new_token.kind = TOK_SYMBOL_GT;
    }
    append(Token_Seq, tokens, new_token);
}
```
```c
else if (plaintext[index] == '<') {
    index++;
    Token new_token;
    if (plaintext[index] == '=') {
        index++;
        new_token.kind = TOK_SYMBOL_LE;
    } else {
        new_token.kind = TOK_SYMBOL_LT;
    }
    append(Token_Seq, tokens, new_token);
}
```
And just like that our tokeniser is ready for all of the new features, without any significant architectural changes.

Now we can look at the modified (but already existing) grammar symbols for the parser:
```EBNF
<object> ::= <definition> | <object_declaration>;

<definition> ::= <proc_definition> | <var_definition>;

<proc_definition_inner> ::= "l_paren" "identifier" <proc_type>
                      <body_statement> "r_paren";

<body_statement> ::= "l_paren" "keyword_body" <statement>*
                "r_paren";

<statement> ::= <return_statement> | <proc_call>
              | <var_declaration> | <var_assign_statement>
              | <if_statement> | <body_statement>
              | <while_loop> | <break_statement>
              | <continue_statement> | <free_statement>;

<type> ::= <proc_type> | <base_type> | <pointer_type>;

<declaration> ::= "l_paren" (<base_type> | <pointer_type>)
                  "identifier" "r_paren";
```
And, of course, anything that referenced value_literal.

We will start by changing that. Replace every call to:
```c
parse_value_literal
```
with a call to:
```c
parse_expression
```
As we discussed in the grammar in previous chapter.

This should be relatively easy since basically all IDEs/text editors will provide functionality for find-and-replace.

We can then go through and modify each of the functions that correspond to our symbols accordingly:
```c
Object parse_object() {
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

    fprintf(stderr, "Error: failed to parse object\n");
    exit(1);
}
```
```c
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
```
```c
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
    return output;
}
```
Note that this is just a name change again and can therefore probably be done easily with your IDEs built in find-and-replace functionality.
```c
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
    Type_Option var_type = parse_declaration(&name);
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
```
As you can see the pattern of:
```c
if (output.present) {
    return output;
}else idx = current_idx;
```
appears a lot throughout our codebase. This might be something you would want to factor out into a macro rather than having to type it in full every time. For clarity I won't but I encourage you to do so if you feel like it; making small changes to the codebase like that as you follow along will help cement your understanding even better.

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

    output = parse_pointer_type();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Type_Option)Nothing;
}
```
```c
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
```
Notice again how we made virtually no architectural changes to our current nodes in order to accommodate the changes to our grammar.

Now all that is left is to implement the new symbols we introduced in chapter 7. First of all, let's remind ourselves of what all those new symbols are:
```EBNF
<object_declaration> ::= <proc_declaration> | <var_declaration>;

<var_declaration> ::= "l_paren" "keyword_var"
                       <declaration> "r_paren";

<proc_declaration> ::= <extern_proc_decl>;

<extern_proc_decl> ::= "l_paren" "keyword_extern" "keyword_proc"
                       <extern_proc_decl_inner> "r_paren";

<extern_proc_decl_inner> ::= "l_paren" "identifier"
                             <proc_type> "r_paren";

<var_definition> ::= "l_paren" "keyword_define" "keyword_var"
                     <declaration> <value_literal> "r_paren";

<free_statement> ::= "l_paren" "keyword_free" <expression>
                     "r_paren";

<break_statement> ::= "keyword_break";

<continue_statement> ::= "keyword_continue";

<while_loop> ::= "l_paren" "keyword_while" <expression>
                   <statement> "r_paren";

<if_statement> ::= "l_paren" "keyword_if" <expression>
                   <statement> ("keyword_elif"
                   <expression> <statement>)*
                   ["keyword_else" <statement>]
                   "r_paren";

<var_assign_statement> ::= "l_paren" "keyword_assign"
                           "identifier" <expression>
                           "r_paren";

<proc_call> ::= "l_paren" "identifier"
                <expression>* "r_paren";

<pointer_type> ::= "l_paren" "keyword_ptr" <type> "r_paren";

<expression> ::= <value_literal> | <add_expression>
               | <sub_expression> | <mul_expression>
               | <div_expression> | <mod_expression>
               | <log_and_expr> | <log_or_expr>
               | <log_not_expr> | <log_xor_expr>
               | <bit_and_expr> | <bit_or_expr>
               | <bit_not_expr> | <bit_xor_expr>
               | <shl_expression> | <shr_expression>
               | <eq_expression> | <neq_expression>
               | <gt_expression> | <ge_expression>
               | <lt_expression> | <le_expression>
               | <variable_literal> | <proc_call>
               | <pointer_value> | <deref_value>
               | <allocation> | <sizeof>;

<gt_expression> ::= "l_paren" "symbol_gt" <expression>
                    <expression> "r_paren";

<ge_expression> ::= "l_paren" "symbol_ge" <expression>
                    <expression> "r_paren";

<lt_expression> ::= "l_paren" "symbol_lt" <expression>
                    <expression> "r_paren";

<le_expression> ::= "l_paren" "symbol_le" <expression>
                    <expression> "r_paren";

<sizeof> ::= "l_paren" "keyword_sizeof" <type> "r_paren";

<allocation> ::= "l_paren" "keyword_allocate" <expression>
                 "keyword_of" <type> "r_paren";

<pointer_value> ::= "l_paren" "keyword_ptr" "keyword_to"
                    <expression> "r_paren";

<deref_value> ::= "l_paren" "keyword_from" "keyword_ptr"
                  <expression> "r_paren";

<variable_literal> ::= "identifier";

<eq_expression> ::= "l_paren" "symbol_eq" <expression>
                     <expression> "r_paren";

<neq_expression> ::= "l_paren" "symbol_neq" <expression>
                     <expression> "r_paren";

<shl_expression> ::= "l_paren" "keyword_shl" <expression>
                     <expression> "r_paren";

<shr_expression> ::= "l_paren" "keyword_shr" <expression>
                     <expression> "r_paren";

<bit_and_expr> ::= "l_paren" "keyword_bit" "keyword_and"
                   <expression> <expression> "r_paren";

<bit_or_expr> ::= "l_paren" "keyword_bit" "keyword_or"
                  <expression> <expression> "r_paren";

<bit_not_expr> ::= "l_paren" "keyword_bit" "keyword_not"
                   <expression> "r_paren";

<bit_xor_expr> ::= "l_paren" "keyword_bit" "keyword_xor"
                   <expression> <expression> "r_paren";

<log_and_expr> ::= "l_paren" "keyword_log" "keyword_and"
                   <expression> <expression> "r_paren";

<log_or_expr> ::= "l_paren" "keyword_log" "keyword_or"
                  <expression> <expression> "r_paren";

<log_not_expr> ::= "l_paren" "keyword_log" "keyword_not"
                   <expression> "r_paren";

<log_xor_expr> ::= "l_paren" "keyword_log" "keyword_xor"
                   <expression> <expression> "r_paren";

<sub_expression> ::= "l_paren" "symbol_minus" <expression>
                     <expression> "r_paren";

<mul_expression> ::= "l_paren" "symbol_times" <expression>
                     <expression> "r_paren";

<div_expression> ::= "l_paren" "symbol_divide" <expression>
                     <expression> "r_paren";

<mod_expression> ::= "l_paren" "symbol_modulo" <expression>
                     <expression> "r_paren";

<add_expression> ::= "l_paren" "symbol_plus" <expression>
                     <expression> "r_paren";
```
We can now implement parsers for each symbol:

Note that here we will be making more architectural changes as different problems come up. The basic structural equivalence between the grammar and the structure of each function from chapter 5 still very much holds but as we encounter new kinds of node we need to construct there are more choices we find ourselves needing to make. If you feel like making your own choices for how to implement certain parsers I encourage you to do so as this is a great opportunity to test your understanding of the method while still preserving your ability to follow along with everything else. If not don't worry, the implementations you see here will be complete and guaranteed (once I finish writing the book) to work.

For object declarations/definitions we will want to reference global objects for, say, global variable assignments and function calls. We therefore need to create an object dictionary that we can reference against later. For this we will have to define a couple of new Seq type in the header section:
```c
DeclareSeq(Object, Object_Seq);
DeclareSeq(Type, Type_Seq);
```
We can then define our dictionary:
```c
Object_Seq global_objects = {0};
Type_Seq global_object_types = {0};
char_ptr_Seq global_object_names = {0};
```
We then will want to adjust our parse_proc_definition_inner function:
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
```
```c
//object_declaration
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
```
```c
//var_declaration
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
```
Note that since this could be responsible for parsing either a local variable declaration or a global variable declaration we return the name and type as is and let the parent function construct the actual variable (as they are represented differently).
```c
//proc_declaration
Object_Option parse_proc_declaration(){
    int current_idx = idx;
    Object_Option output;

    output = parse_extern_proc_declaration();
    if (output.present) {
        return output;
    } else idx = current_idx;

    return (Object_Option)Nothing;
}
```
```c
Object_Option parse_extern_proc_declaration(){
    Expect(Object_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_EXTERN);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_KEYWORD_PROC);
    Object_Option output = parse_extern_proc_decl_inner();
    Expect(Object_Option, output.present);
    Expect(Object_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    return output;
}
```
```c
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
```
```c
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
```
for a free-statement we need to create a globally accessible function declaration at the top of our implementation section:
```c
Object free_declaration;
```
and edit our parse_program function to initialise this declaration and add it at the start of our program:
```c
ObjectList parse_program(Token_Seq tokens_arg) {
    TypeList free_args = empty_list;
    append_type(free_args, create_ptr_type(create_base_type(BTY_VOID)));
    Type free_type = create_function_type(create_base_type(BTY_VOID), free_args);
    free_declaration = create_function_declaration("free", free_type);
    tokens = tokens_arg;
    idx = 0;
    ObjectList output = empty_list; //empty_list is a macro provided by Libchibi to represent an empty list
    append_object(output, free_declaration);
    while (tokens.data[idx].kind != TOK_EOF) append_object(output, parse_object());
    //append_object is a function provided by Libchibi to append objects to object lists
    return output;
}
```
Then we can implement parse_free_statement as follows:
```c
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
```
For the break and continue statements we will be defining a new Seq type in the header section:
```c
DeclareSeq(UniqueLabel, UniqueLabel_Seq);
```
Then at the top of our implementation section we wil declare two new variables:
```c
UniqueLabel_Seq break_labels = {0};
UniqueLabel_Seq continue_labels = {0};
```
Which we will use as stacks that we append to at the start of our loop parsing and pop off at the end of our loop parsing.

We can then parse break and continue statements:
```c
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
```
This means we can't use our Expect macro in parse_while_loop since we need to make sure we pop from these stacks before every return.
```c
Node_Option parse_while_loop(char_ptr_Seq* local_names, TypeList* local_types) {
    append(UniqueLabel_Seq, break_labels, (UniqueLabel)empty_label);
    append(UniqueLabel_Seq, continue_labels, (UniqueLabel)empty_label);
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
```
For parsing if statements we are going to have to declare another Seq type in the header section:
```c
DeclareSeq(Node, Node_Seq);
```
```c
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
        new_expression = parse_expression(local_names, local_types);
        Expect(Node_Option, new_expression.present);
        append(Node_Seq, expressions, new_expression.value);
        new_statement = parse_statement(local_names, local_types);
        Expect(Node_Option, new_statement.present);
        append(Node_Seq, statements, new_statement.value);
    }
    if (tokens.data[idx].kind == TOK_KEYWORD_ELSE) {
        idx++;
        new_statement = parse_statement();
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
```
This was a bit more complex. Take a second to look it over. Reassure yourself that the loops and if statements match the corresponding '*' and '[]' in the grammar and check that you really follow how we construct the chain of if-else nodes.
```c
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
        create_ass_node(
            var_node,
            assignment.value,
            0,
            0
        )
    );
    return output;
}
```
```c
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
```
```c
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
```
```c
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

    return (Node_Option)Nothing;
}
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
For an allocation expression, much like for free statements we need to create a global object for the declaration of the calloc function:
```c
Object calloc_declaration;
```
Then we need to, again, adjust our parse_program function:
```c
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
    while (tokens.data[idx].kind != TOK_EOF) append_object(output, parse_object());
    //append_object is a function provided by Libchibi to append objects to object lists
    return output;
}
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
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
```
```c
Node_Option parse_shl_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_SHL);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);
    
    Type shl_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(shl_type))
        shl_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_shl_node(shl_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_shr_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_SHR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type shr_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(shr_type))
        shr_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_shr_node(shr_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_bit_and_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_AND);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type and_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(and_type))
        and_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_bit_and_node(and_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_bit_or_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_OR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type or_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(or_type))
        or_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_bit_or_node(or_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
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
```
```c
Node_Option parse_bit_xor_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_BIT);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_XOR);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type xor_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(xor_type))
        xor_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_bit_xor_node(xor_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
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
```
```c
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
```
```c
Node_Option parse_log_not_expr() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_LOG);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_KEYWORD_NOT);
    Node_Option expression = parse_expression();
    Expect(Node_Option, expression.present);
    Node_Option rhs = parse_expression();

    Node_Option output = Just(Node_Option,
        create_not_node(expression.value, 0, 0)
    );
    return output;
}
```
```c
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
```
```c
Node_Option parse_sub_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_MINUS);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type sub_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(sub_type))
        sub_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_sub_node(sub_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_mul_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_TIMES);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type mul_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(mul_type))
        mul_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_mul_node(mul_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_div_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_DIVIDE);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type div_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(div_type))
        div_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_div_node(div_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_mod_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_MODULO);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type mod_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(mod_type))
        mod_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_mod_node(mod_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
```c
Node_Option parse_add_expression() {
    Expect(Node_Option, tokens.data[idx++].kind == TOK_L_PAREN);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_SYMBOL_PLUS);
    Node_Option lhs = parse_expression();
    Expect(Node_Option, lhs.present);
    Node_Option rhs = parse_expression();
    Expect(Node_Option, rhs.present);
    Expect(Node_Option, tokens.data[idx++].kind == TOK_R_PAREN);

    Type add_type = get_expression_type(lhs.value);
    Type rhs_type = get_expression_type(rhs.value);
    if (get_type_size(rhs_type) > get_type_size(add_type))
        add_type = rhs_type;
    Node_Option output = Just(Node_Option,
        create_add_node(add_type, lhs.value, rhs.value, 0, 0)
    );
    return output;
}
```
We're almost finished but we have one last feature to implement, and that was the string literals we left unimplemented in chapter 5. We need to create a global Seq of objects that we can add to from our parse_value_literal function that parse_program will then append to the end of the program's ObjectList (we don't need to add this to our existing global object dictionary since it's just used for a single literal rather than treated as an actual global variable):
```c
Object_Seq objects_to_add = {0};
```
We then modify parse_program to add these:
```c
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
```
And we can finally replace our TOK_KEYWORD_STRING case in parse_value_literal:
```c
//string literal case
case (TOK_KEYWORD_STRING): {
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
}
```

And just like that our hello-world-capable compiler is complete. And, of course, we can do a lot more than just hello world. I highly recommend just trying out writing some code in ChibiLisp before continuing on to chapter 8 such as giving the first few problems on [Project Euler](https://projecteuler.net/archives) a go or trying to link with and write a simple project with [Raylib](https://www.raylib.com/) in order to get a feel for what we can already do and the extent of the milestone that this is.

For completeness's sake I should also show you Hello World:
```lisp
(extern proc
    (puts
        (i32 ((string input)))
    )
)

(define proc
    (main (i32 ())
        (body
            (puts (string "Hello World"))
            (return (i32 0))
        )
    )
)
```

In the next chapter we will layout the trajectory for the rest of the book and the path towards fleshing out our language into something that truly stands on it's own rather than being merely a lispy syntax over somewhat cut down C semantics.
