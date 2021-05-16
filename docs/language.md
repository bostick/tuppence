
# Language

Tuppence might seem strange to someone unfamiliar with 2-adic numbers. For example, there are no real numbers in Tuppence. And dividing numbers by 2 is an error.






## Values

Values in Tuppence are immutable.



### FiniteWords



`` \`\` `` (the empty FiniteWord) is used as the null value in Tuppence.

`` `0` `` and `` `1` `` are used as boolean values in Tuppence.


FiniteWords have the methods:
`size`







### RationalWords


RationalWords have the methods:
`period`
`transient`
`numerator`
`denominator`




### Lists

Indexing occurs right-to-left. 

Lists have the methods:
`size`






### Builtin Symbols

infinity




### Builtin Functions

print(a)

print the value a


exit()

exit the process


rationalize(finiteWord)

return the RationalWord that is the closest value to finiteWord



size(finiteWord)
size(rationalWord)
size(list)


period(rationalWord)
transient(rationalWord)
numerator(rationalWord)
denominator(rationalWord)
















## Evaluation rules

Operations like `+` and `*` return a list of values.

```
>>> `1` + `1`
(`1`, `0`)
```

But, if lists are an intermediate result of evaluation, they are usually implicitly concatenated together.

```
>>> (`1` + `1`) * `11`
(`01`, `10`)
```
The intermediate result of `` `1` + `1` `` is `` (`1`, `0`) ``, but it is concatenated to `` `10` `` before being multiplied with `` `11` ``.


Implicit concatenation of intermediate results is a feature of the Tuppence language.



Evaluation of lists occurs right-to-left.

```
>>> (print(1), print(2), print(3))
3
2
1
(``, ``, ``)
```













## Operators

List of operators:
```
=
|
^
&
~
==
!=
%%
>>
>%
/%
+
unary -
binary -
*
/
#
**
.
...
```


### Assignment

=

A list of identifiers can be a LHS and it will bind the corresponding values on the RHS list. Any extra elements on the RHS are not assigned to a variable. This allowed writing the code ``x = `1` + `1` `` to work as expected, i.e., `x` is assigned `0`. But you can also write ``(c, x) = `1` + `1` `` if you wish to also keep track of the carry.





### Bitwise operations

The operators `&`, `|`, and `^` represent the traditional bitwise operators for `and`, `or`, and `xor`, respectively. The notation was popularized by the C programming language.

~

They are infix.





### Comparison operations

==
!=

They are infix operators.
== and != operators are special, if any word values are present, then only consider the first element of any lists
if all arguments are lists, then compare element-wise
functions are not comparable to anything

Returns `` `1` `` if the comparison is true, and returns `` `0` `` if the comparison is false.







### Residue operator

`%%` is the residue operator. It allows you to extract a finite number of bits from a rational number.

```
>>> 1/3 %% 20
`10101010101010101011`
```


### Shift operator

`>>` is the shift operator.



### Shift-Residue operator

`>%` is the shift-residue operator. Returns a list.







### Arithmetic operations

`+` and `-` return Lists with 2 elements. The first element of the result has the same bit width as the operands. The second element is the carry (or borrow) and is the size needed to represent the maximum possibly carry (or borrow).

`*` returns a List with 2 elements. Both elements are the same width as the operands, and both elements together represent the complete product of the operands.

`/` is the rational division operator.

/% is the quotient-remainder operator.


- is unary

+ is infix
- is binary
* is infix
/ is binary


With `a / b`, both `a` and `b` must be RationalWords

1/3 returns a RationalWord with period `` `01` `` and transient `` `1` ``




/% is binary

With `a /% b`, both `a` and `b` must be FiniteWords or both must be RationalWords.

1/%3 returns (0, 1) because 1/3 has quotient of 0 and remainder of 1

(2/3)/%(1/5) returns (3, 1/15)







### Concatenation

`#` is infix

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

When concatenating words, only 1 Rational Word is allowed, and it must be on the left.

```
>>> `1` # ...`01`
Error: Expected FiniteWord: -1/3
```








### Integer Repetition

`**` is binary


In `a ## b`, a must be an integer or infinity, and b must be a FiniteWord.







### Infinite Repetition

`...` is unary

You can create a Rational Word from a Finite Word.

```
>>> ...`1`
-1
```

The `...` operator extends its operand infinitely to the left. Here, you can see that an infinite string of `1` bits is equal to -1. If you are familiar with 2's complement representation of negative numbers, then you have seen how a string of all `1` bits can represent -1 for 32-bit or 64-bit words. The idea of 2's complement notation comes naturally from 2-adic arithmetic. The 2-adic representation of -1 is an infinite string of `1` bits.











