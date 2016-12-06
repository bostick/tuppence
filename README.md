# The Tuppence Programming Language

A language for 2-adic arithmetic and bit-twiddling.

Tuppence is an interpreted language.

```
>./tuppence
Tuppence 0.1
>>>
```

You can type expressions and evaluate them by hitting Enter.

```
>>> 1 + 1
2
```

Tuppence has exact rational arithmetic.

```
>>> 1/3 + 1/5
8/15
```

Integers and rationals are represented using **Rational Words**. The Rational Word data type in Tuppence is modeled after the Quote notation for p-adic numbers popularized by Hehner and Horspool [Quote notation](https://en.wikipedia.org/wiki/Quote_notation).

The 2-adic expansion of 1/3 is `...01010101010101011`, with the pattern extending infinitely to the left.

This is similar to the binary real expansion of 1/3, which is `0.0101010101010101...`, with the pattern extending infinitely to the right.

In the 2-adic expansion of 1/3, there is a single `1` bit that is not part of the repeating `01` pattern. The initial non-repeating part is called the `transient` and the repeating part is called the `period`.

```
>>> x = 1/3
>>> x.period
`01`
>>> x.transient
`1`
```

The period and transient are represented as **Finite Words**. Finite Words are binary strings delimited by `` ` `` (backtick) characters. You can do arithmetic and bitwise operations with Finite Words.

```
>>> `1110000000` | `0000000111`
`1110000111`
```

Arithmetic and bitwise operations must be done on Finite Words of all the same size.

You can create a Rational Word from a Finite Word.

```
>>> ...`1`
-1
```

The `...` operator extends its operand infinitely to the left. Here, you can see that an infinite string of `1` bits is equal to -1. If you are familiar with 2's complement representation of negative numbers, then you have seen how a string of all `1` bits can represent -1 for 32-bit or 64-bit words. The idea of 2's complement notation comes naturally from 2-adic arithmetic. The 2-adic representation of -1 is an infinite string of `1` bits.

You can concatenate Finite Words with other Finite Words.

```
>>> `11` # `000`
`11000`
```

And you can concatenate Rational Words with Finite Words.

```
>>> ...`01` # `1`
1/3
```


## Philospophy

Tuppence might seem strange to someone unfamiliar with 2-adic numbers. For example, there are no real numbers in Tuppence. And dividing numbers by 2 is an error.

Indexing occurs right-to-left. Evaluation of lists occurs right-to-left.

```
>>> (print(1), print(2), print(3))
3
2
1
(``, ``, ``)
```

Where it makes sense, information is processed right-to-left. Exceptions include the spelling of English keywords and functions of the language ( would `tnirp` be acceptable for a function name? ), function application ( `(1)print` would be too strange) and operators like `/` and `-` ( Writing `3\1` would be too strange for representing the fraction one-third. )

Traditionally, when adding 32-bit or 64-bit integers, the carry bit is lost. Tuppence propagates the carry bit for later use.

```
>>> `1` + `1`
(`1`, `0`)
```

Similarly with multiplication, when multiplying 2 n-bit words, the result is a list with 2 n-bit elements.

## Language

Values in Tuppence are immutable.

`%%` is the residue operator. It allows you to extract a finite number of bits from a rational number.

```
>>> 1/3 %% 20
`10101010101010101011`
```

#### Arithmetic operations

`+` and `-` return Lists with 2 elements. The first element of the result has the same bit width as the operands. The second element is the carry (or borrow) and is the size needed to represent the maximum possibly carry (or borrow).

`*` returns a List with 2 elements. Both elements are the same width as the operands, and both elements together represent the complete product of the operands.

`/` returns a List with 2 elements, i.e., (Remainder, Quotient)

#### Bitwise operations

The operators `&`, `|`, and `^` represent the traditional bitwise operators for `and`, `or`, and `xor`, respectively. The notation was popularized by the C programming language.

## Evaluation rules

Operations like `+` and `*` return a list of values.

```
>>> `1` + `1`
(`1`, `0`)
```

But, if lists are an intermediate result of evaluation, they are usually concatenated together.

```
>>> (`1` + `1`) * `11`
(`01`, `10`)
```
The intermediate result of `` `1` + `1` `` is `` (`1`, `0`) ``, but it is concatenated to `` `10` `` before being multiplied with `` `11` ``.


### Assignment

A list of identifiers can be a LHS and it will bind the corresponding values on the RHS list. Any extra elements on the RHS are not assigned to a variable. This allowed writing the code ``x = `1` + `1` `` to work as expected, i.e., `x` is assigned `0`. But you can also write ``(c, x) = `1` + `1` `` if you wish to also keep track of the carry.



### Examples

It is a property of 2-adic numbers that they must start with `001` in order to have a square root. Let's find the square root of 17, which is `10001` in binary.

```
; sqrt.2p
; Brenton Bostick
; Square Root of 17 in Tuppence
;
; initial guess of 1
guess = 1
; Newton's Method
guess = (guess + 17/guess) >> 1
print(guess)
guess = (guess + 17/guess) >> 1
print(guess)
guess = (guess + 17/guess) >> 1
print(guess)
guess = (guess + 17/guess) >> 1
print(guess)
guess = (guess + 17/guess) >> 1
print(guess)

; print guess as integer with 100 bits of precision
print(0 # (guess %% 100))

; print result as integer with 100 bits of precision
result = guess * guess
print(0 # (result %% 100))
```

And if we run the interpreter with this file:

```
./tuppence.exe sqrt.2p
9
49/9
1889/441
3437249/833049
Warning: Loop limit exceeded in RationalWord divide. Returning truncated result. Loop limit is: 1000
3526299271737049901142489188064762452771618484675801833747781042574412567139404581266443855599402418586554728091093092889940241552756633570817371360792679954999464735144547873671925392914561568751203486508447435984639721081511658824706088313577043771350752808234845312735541844167512127491844406685816091329/833049
Warning: Loop limit exceeded in RationalWord divide. Returning truncated result. Loop limit is: 1000
1990313625524614549424364567010171681910560013052581941814155397945006558578945406256220214929938179709836283596602702628927511002443863124929448389577151524995890102899552055223683766691867151573000720401828645206417256180613135585469867360297289228689905093033409343704317280587576819335923647992097436353/833049
1049862217672007560153128969961
Warning: Loop limit exceeded in RationalWord multiply. Returning truncated result. Loop limit is: 1000
17
```

The warnings indicate that it took more than 1000 iterations to get an answer inside of the code for dividing and multiplying. The guesses that are printed have been truncated to 1000 digits of precision. It is especially easy to generate fractions with enormous numerators and denominators in Tuppence and I will be working on methods for managing this complexity.

So we can see in terms of 100-bit unsigned integers, 1049862217672007560153128969961 is the square root of 17.

Pretty cool, huh?

## Why the name *Tuppence*?

Tuppence is British slang for "two pence" <a href="https://en.wikipedia.org/wiki/Two_pence_(British_decimal_coin)">Two pence</a>. You frequently hear the coin referred to as "2p". I was looking for a name that captured the sense that p-adic numbers, for p=2, were the foundation of the language.




## Building Instructions

Tuppence uses CMake to generate project files.

Make sure to have a build of LLVM. Follow the instructions for building LLVM with CMake: http://llvm.org/docs/CMake.html

Open CMake and set the source code location to tuppence/src. It is recommended to set the build directory to tuppence/build.

Set the CMake entries as follows:

Name | Value
------- | -------
LLVM_DIR | your-llvm-dir/build/lib/cmake/llvm
GTEST_INCLUDE_DIR | your-llvm-dir/llvm/utils/unittest/googletest/include
GTEST_LIBRARY | your-llvm-dir/build/Release/lib/gtest.lib
GTEST_LIBRARY_DEBUG | your-llvm-dir/build/Debug/lib/gtest.lib
GTEST_MAIN_LIBRARY | your-llvm-dir/build/Release/lib/gtest_main.lib
GTEST_MAIN_LIBRARY_DEBUG | your-llvm-dir/build/Debug/lib/gtest_main.lib

Then Configure and Generate. The tuppence project is the interpreter and the test project builds an executable named runUnitTests for running the GTest tests for Tuppence.

## Future work

Actual LLVM codegen, Jupyter kernel, Finite Field arithmetic, crit-bit trees, BDDs (binary decision diagrams).
