
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




