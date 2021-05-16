
%void_p = type i8* ; error: pointers to void are invalid - use i8* instead
%struct.FiniteWordImpl = type opaque
%struct.Value = type { i8, %void_p }

declare %struct.FiniteWordImpl* @FiniteWordImpl_createFromVal(i64, i64) ; unsigned numBits, uint64_t val
declare void @FiniteWordImpl_release(%struct.FiniteWordImpl*)

declare void @FiniteWordImpl_newString(%struct.FiniteWordImpl*, i8**)
declare void @FiniteWordImpl_releaseString(i8*)

declare %struct.FiniteWordImpl* @FiniteWordImpl_add(%struct.FiniteWordImpl*, %struct.FiniteWordImpl*)
declare i64* @FiniteWordImpl_getRawData(%struct.FiniteWordImpl*)



;@.str = internal constant [14 x i8] c"Hello, world!\00"

define i32 @Value_NewString(%struct.Value %val, i8** %Str_p) {

  %tag = extractvalue %struct.Value %val, 0
  %payload = extractvalue %struct.Value %val, 1

  switch i8 %tag, label %label.otherwise [ i8 FINITEWORDTAG, label %label.finiteWordImpl ]

label.otherwise:
  unreachable

label.finiteWordImpl:
  %payload_finiteWordImpl = bitcast %void_p %payload to %struct.FiniteWordImpl*
  call void @FiniteWordImpl_newString(%struct.FiniteWordImpl* %payload_finiteWordImpl, i8** %Str_p)
  br label %return

return:
  ret i32 0
}

define void @Value_ReleaseString(%struct.Value %val, i8* %Str) {

  %tag = extractvalue %struct.Value %val, 0
  %payload = extractvalue %struct.Value %val, 1

  switch i8 %tag, label %label.otherwise [ i8 FINITEWORDTAG, label %label.finiteWordImpl ]

label.otherwise:
  unreachable

label.finiteWordImpl:
  %payload_finiteWordImpl = bitcast %void_p %payload to %struct.FiniteWordImpl*
  call void @FiniteWordImpl_releaseString(i8* %Str)
  br label %return

return:
  ret void
}

define void @Value_ReleaseValue(%struct.Value %val) {

  %tag = extractvalue %struct.Value %val, 0
  %payload = extractvalue %struct.Value %val, 1

  switch i8 %tag, label %label.otherwise [ i8 FINITEWORDTAG, label %label.releaseFiniteWordImpl ]

label.releaseFiniteWordImpl:
  %payload_finiteWordImpl = bitcast %void_p %payload to %struct.FiniteWordImpl*
  call void @FiniteWordImpl_release(%struct.FiniteWordImpl* %payload_finiteWordImpl)
  br label %doneClearing

label.otherwise:
  unreachable

doneClearing:
  ;store i8 0, i8* %tmp1_0
  ;store %void_p null, %void_p* %tmp1_1
  br label %return

return:
  ret void
}




define %struct.Value @Value_Plus(%struct.Value*, i32 %count) {

  ;first, determine if any are FiniteWords
entry:
  br label %loop

loop:       ; preds = %loop, %entry
  %i = phi i32 [ 0, %entry ], [ %nextvar, %loop ]
  ; body
  %val_p = getelementptr %struct.Value, %struct.Value* %0, i32 %i
  %val = load %struct.Value, %struct.Value* %val_p
  %tag = extractvalue %struct.Value %val, 0
  switch i8 %tag, label %label.otherwise [ i8 FINITEWORDTAG, label %label.releaseFiniteWordImpl ]

  ; increment
  %nextvar = add i32 %i, 1

  ; termination test
  %cmptmp = icmp ult i32 %i, %count
  br i1 %cmptmp, label %loop, label %afterloop

afterloop:      ; preds = %loop
  ; 





}

