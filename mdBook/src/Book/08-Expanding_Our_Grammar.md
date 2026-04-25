
<div>
<small>
  Write Your First Compiler In C With Libchibi<br>
  <em>Part 2: Making ChibiLisp Useful</em>
</small>
</div>
<h1 align=center>Chapter 08: Expanding Our Grammar</h1>

### Overview:

Last chapter we decided on the featureset we wanted to implement for this part of the book. This chapter we will begin to design our syntax for it. This is also where you will get to put into practice the knowledge and skills learned in part 1 since you are going to be writing the grammar yourself. You should have been following along and already have your own copy of the grammar but if not (and don't worry if not) you can take the grammar files from the references for chapter 6. In the references for this chapter you can also find the updated grammar you should hopefully have also written yourself after this chapter.

### Designing Our Syntax

#### Procedure Calls:

Thinking back to last chapter, the first thing we need to implement is our procedure calls. For that I'm going to suggest we just use the procedure's name followed by it's arguments, of course surrounded by brackets. While we could add a keyword like `call` first to specify that we are calling a procedure, since procedure names can't be keywords, it's actually not ambiguous so long as we never have two colliding identifier names. It's also worth considering where procedures can be called. Since many procedures can return a value we definitely want to be able to use them as expressions. Procedures also, however, can have side effects such as printing or altering a global variable and sometimes we want to be able to call a procedure purely for those side effects without using the returned value. This is particularly the case with procedures that return void (as we're about to discuss) since they literally _don't_ return a value that can be used in an expression (and when we add type checking in a later part, we will be able to disallow them). One solution is to have a statement like `discard` that expects an expression and does nothing with the output. Another notation for this might be `_ =` (or in our language `(assign _ <our procedure> )`). That's actually how Libchibi works, requiring such expressions to be wrapped in an `expression_statement` node. For ChibiLisp, however, I'm going to suggest that since we already know whether we expect a statement or an expression from our grammar, we just allow procedures to be called in both places and automatically construct the relevant node when it comes to implementation.

#### External Procedure Declarations:

For external procedure declarations it makes sense for them to be as structurally similar to procedure declarations as possible. For this I'm going to suggest we make only two changes to our syntax: firstly we remove the procedure body since we are, obviously, only declaring the procedure and not implementing it. Secondly, I'm going to suggest that we add the word extern before the word proc. This isn't technically necessary; since the two objects are structurally different we could just not do this and rely on backtracking to determine which one was present. The reason I am suggesting we force the use of the extern keyword is so that it is immediately obvious from looking at a file which FFI C functions it imports (since we intend to add our own module system in part 4 other ChibiLisp functions will be automatically imported from the module if they're public). It does also mean that we will need to add a token for this new keyword as well.

For now, I am also not going to suggest we introduce any way to rename the function when we import it. This is mostly out of convenience for implementation now and may be something we might want to change after we implement modules to, for instance, replace name prefixing with module namespacing (if we choose to implement this feature) however since we don't yet know if this will be something our language implements it doesn't yet make sense to start designing our functionality around it. For external procedure declarations I'm also going to suggest we treat them as, well, declarations, rather than as definitions like, well, procedure definitions.

#### Void Return Types:

This is easy enough. I'm going to suggest we use the actual keyword `void` for this. We could use something else such as `_` and it still wouldn't be ambiguous, even if we also allow this for values since we pretty much always know when we expect a value vs a type; it might even be more idiomatically lispy. The reason I'm suggesting `void` however is that we want out procedural code (the only code where a void type makes sense) to mirror C-style code and so choosing the void type just makes it easier to see that correspondence when working with mixed language codebases, especially for FFI boundaries (in both directions).

#### Variable Declarations And Definition:

(I'm skipping string literals since we already have the grammar, we just lack the implementation)

For global variable declarations I'm going to suggest up front that we actually don't distinguish between global and local declarations in our syntax. This, again, diverges from the internals of Libchibi where they are represented with different constructs (whole objects vs properties of the function object) but since we always know if we are expecting a ChibiLisp object or statement we can automatically handle that difference without any ambiguity. The same applies to variable definitions. For variable declarations I'm going to suggest we have the keyword `var`, the variable name and then a declaration like we already use for procedure arguments all, of course, enclosed in parens. For definitions we can actually just do the same and but with a value literal at the end as well. Much like with external procedure declarations though I'm going to suggest we add an extra keyword to the start: `define`. The reason for this is mostly vibes and we could just as validly make either choice but I want to make the difference between variable definitions and declarations as explicit as possible especially since, at the moment, we're not doing any sort of 0-initialisation of declared but undefined variables. The reason we provide a value literal instead of just any expression is because global variable's get defined at compile time and all of our expression evaluation is currently happening only at runtime. We could allow expressions for local variable definitions however this seems like introducing asymmetry where it isn't needed when the option to separately declare and then assign to a runtime-known value is available.

#### Variable Assignment:

For variable assignment I've kind of given away the notation we will be using in discussing procedure calls but we will be introducing the `assign` keyword to our tokens and then using it, followed by the variable name and the expression we're assigning it to all enclosed in brackets. This, however, is actually technically ambiguous. In particular what's allowing us to differentiate assigning to a local variable vs a global variable, two things which are different nodes in Libchibi. Or, for that matter, to stop us assigning to a function (although that's malformed input anyway so for now we can call that undefined behaviour and hopefully just provide an error). This is an area where we could make our grammar unambiguous by forcing the user to specify whether they are assigning to a local or a global however that is a lot of additional friction for the programmer so, instead, we're going to require our parser implementation to handle this ambiguity for us much like our tokeniser did for identifiers vs keywords. In this particular case that's easy because we can simply maintain a list of the global variables and if it's not there assume it's local (or, indeed, maintain lists of the global variables and the local variables in scope too).

#### If/If-Elif-Else Statements And Block Statements:

For if and if-elif-else statements I'm going to suggest that we simply have the keyword `if` followed by the expression that must be true then the statement that is executed if it is, then, as many times as we want, the keyword `elif` followed by the same two things, and finally, optionally, the keyword `else` followed by the statement that must be executed. When you're implementing this in our grammar, don't forget to also add the keywords themselves and also, you may find the [square brackets] that represent optionality useful for the final else-statement clause.

For block statements I'm going to suggest simply reusing our proc-body symbol and renaming the symbol to a block statement. The reason for this is that it preserves the same symmetry that we find in C (and therefore in the underlying Libchibi AST) that both a block statement and a procedure body are just blocks that, in C, are enclosed by {curly braces}. This may seem slightly odd nonetheless since we're using the keyword `body` even where the block just represents an ordinary block statements however in many cases, such as loops and control flow, this still makes sense as the block forms the body of the loop or if-statement clause etc... There are, of course, places where block statements are used as just a free-standing statement however I contend that these will just feel like a natural extension of the cases where the `body` keyword is appropriate rather than a truly distinct case where they keyword will feel out of place.

#### While Loops And Break/Continue Statements:

For while loops I'm going to suggest we maintain symmetry with if statements and introduce a keyword `while` then just follow it up with the expression that must remain true while looping and the statement that will be executed in that loop. For break and continue, since they don't require any arguments at all, I'm going to suggest we just introduce and use the keywords `break` and `continue` raw without any enclosing parens.

#### Addition/Subtraction/Multiplication/Division/Modulo

For binary mathematical operations I'm going to suggest we add 5 new symbol tokens for `+`, `-`, `*`, `/`, and `%`. We can then simply have these operations be expressed by the relevant symbol followed by the two expressions being operated on and then, of course, all enclosed in parens.

#### Equality, Non-Equality, And Inequality Testing:

For equality, non-equality, and inequality I'm going to suggest the exact same approach but introducing the symbols `=` for equality, `/=` for inequality, and `<`, `<=`, `>`, and `>=` for inequalities. The only things here that might surprise you are `=` and `/=` where you are likely more used to `==` and `!=`. The reason I'm not using `==` is simply because assignment is handled by the keyword `assign` rather than `=` leaving the symbol `=` free to be used, however feel free to modify your grammar to include `==` as an alternative representation of the same token. For `/=` this is to more closely represent the mathematical `≠` symbol as is more in-line with the tradition of functional languages such as other lisps, haskell, etc...

#### Logical And Bitwise AND, OR, XOR, NOT:

For logical operations I'm going to suggest the keyword `log` followed by one of the keywords `and`, `or`, `xor`, or `not` and then the operand expressions all, of course again, enclosed in parens. My choice to use the keywords rather than sticking with symbolic representation is that the formal symbol systems of boolean algebra (`·`, `+`, `⊕`, and `‾`), and logic (`∧`, `∨`, `⊻`, and `¬`) are not easy to type in a plaintext document (not to mention that the boolean algebra system clashes with addition). We could use C-like symbols of `&`, `|`, `^`, and `!` however this diverges from the more mathematically aligned tradition of lisps that I am attempting to direct our language, ChibiLisp, towards (not to mention that it would clash with `^` for exponentiation as we may later choose to use it). The choice of the extra `log` keyword then becomes obvious however, even if we were using symbolic representation I would still suggest using this keyword as it avoids having to remember which version of the symbol is for logical vs bitwise operators.

It should not come as a surprise that for the bitwise versions of these operations I'm going to suggest we simply replace the keyword `log` with a new keyword `bit`.

#### Bitshifts:

For bitshifts I'm going to suggest we use the keyword `shl` and `shr` for shift left and shift right then follow them up with an expression indicating the number of bits to shift and, finally, the expression being shifted. This means that bitshifts will act as expressions that return a shifted value rather than an in-place shift. Of course, however, an in-place shift is easy to do by combining this with an assign statement.

#### Variable Literals:

For variable literals we can just use the identifier of the variable name on it's own much like we use the keywords for break and continue statements. This is both possible because variables don't take any sort of arguments and useful because it avoids ambiguity with procedure calls since they will always be enclosed in parens. We still have the ambiguity between local and global variables from before however this is resolved in exactly the same manner as before also.

#### C-Casts:

For C casts I'm going to propose using the keyword `cast` followed by the type to cast to and then the value to cast, again, all enclosed in parens.

#### Pointers:

For pointers we need a few things:
  *  pointer types
  *  pointer addressing
  *  pointer dereferencing

For pointer types I'm going to suggest just the keyword `ptr` followed by the type the pointer points to (again, all enclosed in parens). Notably, since void is not a type but rather a keyword that can only be used in procedure types, this means we don't have void pointers. This is a bit of a double-edged sword but it will improve type safety once we introduce a proper type system.

For pointer addressing I'm going to suggest the keyword `ptr` followed by the keyword `to` followed by the value being addressed, and again, all enclosed in parens.

For dereferencing I'm going to suggest the keyword `from` followed by the keyword `ptr` followed by the expression of the pointer being dereferenced.

#### Memory allocation:

For memory allocation I'm going to suggest that we use the keyword `allocate`. I'm going to also (slightly pre-emptively) suggest that our allocate be based on C's calloc since that offers slightly better safety than malloc for very little runtime cost. As you know, calloc has two arguments, a number and a size. I am going to suggest that since the size is generally fed by sizeof() anyway that we instead define allocate in ChibiLisp to take a number and a type (this also means that when we're type checking later, we can know the exact type of the pointer returned which is a benefit we would not otherwise have). Our syntax will therefore become, in parens of course, the keyword `allocate` followed by an expression for the number of elements to allocate, then the keyword `of` followed by the type of those elements.

#### Sizeof:

For sizeof I'm going to simplify slightly here and say that sizeof can only accept a type, not a value. When we expand our type system later, we might choose to change this but for now let's keep our lives as simple as possible. We can therefore make our syntax just the keyword `sizeof` followed by a type and all enclosed in parens.

#### Comments:

And now we come to our final new feature: comments. For lisps it is common to use a semicolon to denote comments. I'm going to also suggest that, since it may be idiomatic in many cases to put many nested elements on a single line in a way that it is not for, say, C, that we use block comments rather newline-terminated comments. Our comments will therefore be composed of a semicolon followed by the comment followed by a terminating semicolon. One problem with this design as-is though is that it becomes very hard to comment out code that itself contains comments so I am going to suggest one final refinement which is that we allow runs of multiple semicolons to initialise a comment and require that a run of equal length of semicolons be used to terminate the comment. This allows us to use 'precedence-1' comments with single semicolons for ordinary comments and then comment out code containing them with double-semicolon 'precedence-2' comments without worrying about accidentally uncommenting our original comments and so on if the programmer has use for higher precedences as well for whatever reason.


#### Final Notes:

If you've been following along then you should hopefully have a complete grammar for all the features we implemented last chapter. Take a look over it, make sure there aren't any ambiguities other than the ones we already know about. Compare it, as well, to the grammar from the references of this chapter. Are there any differences? If so it is just the ordering or have you chosen to express something in a different way? If there are differences try to think about why and about what the advantages are of each representation. Try to think, also, about if your version and mine still produce the same language or if you can find any cases/edge-cases where they actually disagree. Learning how to answer these sorts of questions will really allow you to reason about the behaviour of any language you may be designing ahead of time at the design stage rather than building out a whole implementation in code only to then find that it doesn't quite behave as you intended. It also helps you to isolate design bugs from implementation bugs.

### Conclusion:

Now that we have designed ChibiLisp's new features and expanded the grammar to accommodate them, we will spend next chapter exploring how to implement each of these features.

