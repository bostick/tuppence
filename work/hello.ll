
; http://www.wilfred.me.uk/blog/2015/02/21/my-first-llvm-compiler/
;
; llc -filetype=obj hello.ll
; clang++ hello.o ../build/math/Debug/libmath.a -L/Users/brenton/llvm/build/Debug/lib/ `llvm-config --system-libs --libs core executionengine runtimedyld native` -o hello
; ./hello
; echo $?
;
;
;


; my_malloc

;define internal i8* @my_malloc(i64 %size) {
;  %ret = call noalias i8* @GC_malloc(i64 %size)
;  ret i8* %ret
;}


;%void = type opaque
%void_p = type i8* ; error: pointers to void are invalid - use i8* instead
%struct.FiniteWordImpl = type opaque
%struct.Value = type { i8, %void_p }

declare %struct.FiniteWordImpl* @FiniteWordImpl_createFromVal(i64, i64) ; unsigned numBits, uint64_t val
declare void @FiniteWordImpl_destroy(%struct.FiniteWordImpl*)

declare %struct.FiniteWordImpl* @FiniteWordImpl_add(%struct.FiniteWordImpl*, %struct.FiniteWordImpl*)
declare i64* @FiniteWordImpl_getRawData(%struct.FiniteWordImpl*)

;declare i64 @FiniteWordImpl_sleep(i64, i64)


; @GC_gc_no = external global i64, align 8
;declare void @GC_init()
;declare void @GC_gcollect()


define void @Value_clear(%struct.Value* %val_arg) {

  %tmp1_0 = getelementptr %struct.Value, %struct.Value* %val_arg, i32 0, i32 0
  %tmp1_1 = getelementptr %struct.Value, %struct.Value* %val_arg, i32 0, i32 1

  %tmp2 = load i8, i8* %tmp1_0
  switch i8 %tmp2, label %label.otherwise [ i8 2, label %label.clearFiniteWordImpl ]

label.clearFiniteWordImpl:
  %tmp5 = load %void_p, %void_p* %tmp1_1
  %arg0 = bitcast %void_p %tmp5 to %struct.FiniteWordImpl*
  call void @FiniteWordImpl_destroy(%struct.FiniteWordImpl* %arg0)
  br label %doneClearing

label.otherwise:
  unreachable


doneClearing:
  store i8 0, i8* %tmp1_0
  store %void_p null, %void_p* %tmp1_1
  ret void
}


define i64 @main() {

  ;call void @GC_init()

  call i64 @FiniteWordImpl_sleep(i64 23, i64 34)

  %a_impl = call %struct.FiniteWordImpl* @FiniteWordImpl_createFromVal(i64 1744674407370955166, i64 5)
  %a_val = alloca %struct.Value
  %a_impl_cast = bitcast %struct.FiniteWordImpl* %a_impl to %void_p
  %tmp16 = getelementptr %struct.Value, %struct.Value* %a_val, i32 0, i32 0
  %tmp1 = getelementptr %struct.Value, %struct.Value* %a_val, i32 0, i32 1
  store i8 2, i8* %tmp16
  store %void_p %a_impl_cast, %void_p* %tmp1


  %b_impl = call %struct.FiniteWordImpl* @FiniteWordImpl_createFromVal(i64 1744674407370955166, i64 2)
  %b_val = alloca %struct.Value
  %b_impl_cast = bitcast %struct.FiniteWordImpl* %b_impl to %void_p
  %tmp17 = getelementptr %struct.Value, %struct.Value* %b_val, i32 0, i32 0
  %tmp2 = getelementptr %struct.Value, %struct.Value* %b_val, i32 0, i32 1
  store i8 2, i8* %tmp17
  store %void_p %b_impl_cast, %void_p* %tmp2



  ; do the add
  %tmp3 = getelementptr %struct.Value, %struct.Value* %a_val, i32 0, i32 1
  %tmp4 = getelementptr %struct.Value, %struct.Value* %b_val, i32 0, i32 1
  %tmp5 = load %void_p, %void_p* %tmp3
  %tmp6 = load %void_p, %void_p* %tmp4
  %arg0 = bitcast %void_p %tmp5 to %struct.FiniteWordImpl*
  %arg1 = bitcast %void_p %tmp6 to %struct.FiniteWordImpl*
  %c_impl = call %struct.FiniteWordImpl* @FiniteWordImpl_add(%struct.FiniteWordImpl* %arg0, %struct.FiniteWordImpl* %arg1)
  %c_val = alloca %struct.Value
  %c_impl_cast = bitcast %struct.FiniteWordImpl* %c_impl to %void_p
  %tmp18 = getelementptr %struct.Value, %struct.Value* %c_val, i32 0, i32 0
  %tmp7 = getelementptr %struct.Value, %struct.Value* %c_val, i32 0, i32 1
  store i8 2, i8* %tmp18
  store %void_p %c_impl_cast, %void_p* %tmp7

  
  ; get the result as a machine integer and finish
  %tmp8 = getelementptr %struct.Value, %struct.Value* %c_val, i32 0, i32 1
  %tmp9 = load %void_p, %void_p* %tmp8
  %arg2 = bitcast %void_p %tmp9 to %struct.FiniteWordImpl*
  %dataP = call i64* @FiniteWordImpl_getRawData(%struct.FiniteWordImpl* %arg2)
  %data = load i64, i64* %dataP

  call void @Value_clear(%struct.Value* %a_val)
  call void @Value_clear(%struct.Value* %b_val)
  call void @Value_clear(%struct.Value* %c_val)

  ;%X = trunc i64 %data to i32
  ;ret i32 %X
  ret i64 %data
}

