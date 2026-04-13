
<div>
<small>
  Write Your First Compiler In C With Libchibi<br>
  <em>Part 2: Making ChibiLisp Useful</em>
</small>
</div>
<h1 align=center>Chapter 07: Designing Our Featureset</h1>

### Overview:

Last chapter we compiled our very first program. But so far our language has been incredibly basic. If a programming language can't quit as soon as it's run it can't really do anything at all. And, of course, we have some pretty grand ambitions; we said all the way back in chapter 0 that we weren't just building a toy language but something that truly aims to be useful in production (or at least close enough that we can continue to develop it into one). I also promised you that we'll be building a lisp (albeit a slightly strange one) and that's more than just S-expressions. That's functional programming and meta-programming and all the features that come along with making a language truly live up to that name.

But we also have a much more immediate goal which is to compile Hello World.

Of course, we're also somewhat restricted by our AST from Libchibi. As you will see in a few chapters' time, this isn't a true limitation at all, and we will very soon be breaking well free of it. You may be asking why we're waiting to do so. Surely if we can break out of these limitations without breaking our compiler or needing to replace the backend with something else then it's better to do that as soon as possible. And I would see your argument. But stay with me. In fact, what we're going to build over the next couple of chapters is going to look a lot less like a lisp and a lot more like an S-expression flavoured subset of C.

I know. That sounds weird. But I promise there's a good reason. I mentioned at the start that ChibiLisp will be multi-paradigm. I also suggested that one of the key advantages of Libchibi is that we get first class support for C interoperability and there's no better way to capitalise on that than allowing our language to behave semantically similarly to C. This doesn't mean that we won't be implementing everything that makes lisps, well, lisps but if we intend to support procedural code anyway, we may as well model it on the classic of procedural languages. Crucially, none of this prevents us from adding pure functions, closures, macros, and everything else we need later. It also has the added bonus that we'll be able to write truly useful, C-level code much more quickly than if we tried to take a different approach.

### Our Featureset:

So, now we know broadly what we'll be building over the next few chapters, we still need to decide exactly which features we'll be supporting.

#### Procedures and FFI:

Since we're linking with libc already we don't need to create specific functionality for printing since we can wrap the libc function puts. This does mean, however, that we will need both the ability to call procedures and declare external procedures. Furthermore, we don't currently have a way for procedures to return nothing so we will need some kind of void return type for procedures as well.

#### Variables And String Literals:

Additionally, if you remember back to chapter 5, our code for parsing string literals was not yet implemented because we need to be able to declare a global variable for the literal so, since we're definitely going to need string literals in order to be able to print Hello World we should probably also add support for the user to declare and define global variables.

It would also be a pretty poor if our language couldn't work with local variables so we definitely need to be able to declare and assign to local variables. We won't yet add the ability to define local variables since, in C, this is actually syntax sugar over a declaration and an assignment so we will save this for when our language is less tied to Libchibi's C-shaped AST.

#### Control Flow:

By that same token, our language wouldn't even be turing complete without some sort of control flow. The most obvious of which being the if statement. It would also be nice to support if-else statements so let's add them to our checklist of features. The standard if ... else if ... pattern in C though might be a bit of a problem because of our S-expression syntax. This means that each if in the pattern will be enclosed in it's own brackets causing our control flow to quickly become very nested. As a result, I propose adding support for if ... elif ... else statements as well.

Since we now have control flow we will likely want something akin to C's block statements in order to allow us to execute multiple statements in our control flow.

We also would be quite held back in writing procedural code if we didn't have loops. Of course, for those of you familiar with functional programming you know that this isn't a strict necessity however since we want to properly support both paradigms loops are a must. We will, however, keep it simple and only support while loops. And if we're supporting loops, we should also support the control flow around them, namely, break and continue statements.

#### Expressions:

Our language also wouldn't be any good without support for arithmetic so we'll want to support:
  *  addition,
  *  subtraction,
  *  multiplication,
  *  division, and
  *  modulo

And same goes for logical operations so we'll also definitely want:
  *  equality testing
  *  non-equality testing
  *  inequality testing (greater than, less than, etc...)
  *  logical AND
  *  logical OR
  *  logical XOR
  *  logical NOT

And we would probably be remiss to not also include bitwise manipulations since they are so useful for so much practical code:
  *  bitwise AND
  *  bitwise OR
  *  bitwise XOR
  *  bitwise NOT
  *  bitshift left
  *  bitshift right

We also probably want have the capability to be able to use variables in our expressions so we're going to need variable literals and, for many real world use cases, also C-style casts so that we can correctly use those variables in expressions with other types.

#### Memory Management:

This last feature is maybe a bit more controversial but I'm going to say we probably want pointers, memory allocation, and frees as well. This isn't a particularly lispy feature since lisps typically provide garbage collection. The reason I am proposing we include it rather than wait and introduce garbage collection to our runtime later is, again, the fact that we want ChibiLisp to have first class interoperability with C code which is made much easier by having procedural code in ChibiLisp adopt C's memory model rather than attempting to rectify two completely different memory models across a boundary we only partially control. Plus, as you will see later in the book, we will still be able to guarantee memory safety for functional code. While we're at it, let's add C's sizeof as well since it can be useful for interop and allocation (even if we don't really need it since our types already have size names).

#### Comments:

With all of this we'll start to be able to write some serious, useful code. But no serious code is complete without comments. So, _I promise_ those are the actual last feature we need for this phase of ChibiLisp.


#### Review:

That was a lot. So let's review all of the features we've decided to introduce to our language (don't worry about remembering all of this, it's more just so you have a reference if you decide to look back over this chapter later):

  *  procedure calls
  *  external procedure declaration
  *  procedures with void return type

<br>

  *  string literals
  *  global variable declarations
  *  global variable definitions

<br>

  *  local variable declarations
  *  variable assignment

<br>

  *  if statements
  *  if-elif-else statements
  *  block statements

<br>

  *  while loops
  *  break and continue statements

<br>

  *  addition
  *  subtraction
  *  multiplication
  *  division
  *  modulo

<br>

  *  equality testing
  *  non-equality testing
  *  inequality testing (<, <=, >, >=)
  *  logical AND
  *  logical OR
  *  logical XOR
  *  logical NOT

<br>

  *  bitwise AND
  *  bitwise OR
  *  bitwise XOR
  *  bitwise NOT
  *  bitshift left
  *  bitshift right

<br>

  *  variable literals
  *  c-casts

<br>

  *  pointers
  *  memory allocation
  *  sizeof

<br>

  *  comments

And, again, this isn't yet very lispy but remember this is intentional as we build out a useful, procedural C-like subset from which to eventually develop the entire language into a true (again, albeit weird) lisp.

### Final Notes

Now that we have a solid idea of what we want to implement over the next few chapters, we can spend the next chapter planning out our syntax and designing our grammar for these new features.

