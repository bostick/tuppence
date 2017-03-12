
This Week in Tuppence

December 6, 2016

To my Math and CS friends: While I have been adjuncting at the local college, I have had time to sit down and codify some ideas for a language. I present Tuppence Programming Language 0.1, a language for doing 2-adic math.
Check it out, and if you have a HN account, please go to the discussion link.



March 11, 2017

Decided to give up with actually generating LLVM IR and making a compiler.

I was attracted to LLVM originally because of its huge market share, but also its specific implementation of finite words

I wanted to compile the finite words in Tuppence down to the LLVM IR integers. This means that the finite word `1` would compile down to a i1 LLVM IR integer and that finite word `01` would compile down to a i2 LLVM IR integer.

This implies that the constant 1/3 would have LLVM IR type {i1, i2}, which is a structure of an i1 integer and an i2 integer. Rational numbers would have radically different LLVM IR types based on their transients and periods, and it would be hard to predict ahead of time what that type would be.

LLVM IR is statically typed and it is not clear how to reconcile my goal with types in Tuppence with using LLVM code generation.

I have decided to slowly transition to a pure interpreter for now, but still using some features from LLVM that are nice.

I'm still going to be using:
APInt

dynamic casting

error handling

Copied the code LLVM that I needed.


