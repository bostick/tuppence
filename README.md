# The Tuppence Programming Language

A language for 2-adic arithmetic and bit-twiddling.

[![Build Status](https://travis-ci.org/bostick/tuppence.svg?branch=master)](https://travis-ci.org/bostick/tuppence)

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

When concatenating words, only 1 Rational Word is allowed, and it must be on the left.

```
>>> `1` # ...`01`
Error: Expected FiniteWord: -1/3
```

## Philosophy

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

Traditionally in other programming langauges, when adding 32-bit or 64-bit integers, the carry bit is lost. Tuppence propagates the carry bit for later use.

```
>>> `1` + `1`
(`1`, `0`)
```

Similarly with multiplication, when multiplying 2 n-bit words, the result is a list with 2 n-bit elements.


## Why the name *Tuppence*?

Tuppence is British slang for "two pence" <a href="https://en.wikipedia.org/wiki/Two_pence_(British_decimal_coin)">Two pence</a>. You frequently hear the coin referred to as "2p". I was looking for a name that captured the sense that p-adic numbers, for p=2, were the foundation of the language.








