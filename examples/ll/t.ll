; ModuleID = 'my cool jit'
source_filename = "my cool jit"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

%struct.Value = type { i8, i8* }
%opaque.FiniteWord = type opaque
%opaque.RationalWord = type opaque

@finiteWordString = private constant [2 x i8] c"1\00", align 1
@errorString = private constant [23 x i8] c"Expected a Finite Word\00", align 1
@errorString.1 = private constant [35 x i8] c"Expected a Finite Word with size 1\00", align 1
@integerString = private constant [2 x i8] c"2\00", align 1
@integerString.2 = private constant [2 x i8] c"3\00", align 1

define %struct.Value* @__anon_expr(%struct.Value**, i64) {
entry:
  %createFiniteWord = call %opaque.FiniteWord* @FiniteWord_createFromBinaryString(i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @finiteWordString, i64 0, i64 0))
  %createValue = call %struct.Value* @Value_createFromFiniteWord(%opaque.FiniteWord* %createFiniteWord)
  %2 = getelementptr %struct.Value, %struct.Value* %createValue, i32 0, i32 0
  %3 = load i8, i8* %2
  %4 = icmp eq i8 %3, 2
  br i1 %4, label %type_good, label %type_bad

type_bad:                                         ; preds = %entry
  %createValue1 = call %struct.Value* @Value_createFromError(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @errorString, i64 0, i64 0))
  br label %merge

type_good:                                        ; preds = %entry
  %5 = getelementptr %struct.Value, %struct.Value* %createValue, i32 0, i32 1
  %6 = load i8*, i8** %5
  %7 = bitcast i8* %6 to %opaque.FiniteWord*
  %size = call i64 @FiniteWord_size(%opaque.FiniteWord* %7)
  %8 = icmp eq i64 %size, 1
  br i1 %8, label %size_good, label %size_bad

size_bad:                                         ; preds = %type_good
  %createValue2 = call %struct.Value* @Value_createFromError(i8* getelementptr inbounds ([35 x i8], [35 x i8]* @errorString.1, i64 0, i64 0))
  br label %merge

size_good:                                        ; preds = %type_good
  %bit = call i8 @FiniteWord_getBit(%opaque.FiniteWord* %7, i64 0)
  %9 = icmp eq i8 %bit, 1
  br i1 %9, label %cond_true, label %cond_false

cond_true:                                        ; preds = %size_good
  %calltmp = call %opaque.RationalWord* @RationalWord_createFromDecimalString(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @integerString, i64 0, i64 0))
  %calltmp3 = call %struct.Value* @Value_createFromRationalWord(%opaque.RationalWord* %calltmp)
  br label %merge

cond_false:                                       ; preds = %size_good
  %calltmp4 = call %opaque.RationalWord* @RationalWord_createFromDecimalString(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @integerString.2, i64 0, i64 0))
  %calltmp5 = call %struct.Value* @Value_createFromRationalWord(%opaque.RationalWord* %calltmp4)
  br label %merge

merge:                                            ; preds = %cond_false, %cond_true, %size_bad, %type_bad
  %iftmp = phi %struct.Value* [ %createValue1, %type_bad ], [ %createValue2, %size_bad ], [ %calltmp3, %cond_true ], [ %calltmp5, %cond_false ]
  ret %struct.Value* %iftmp
}

declare %opaque.FiniteWord* @FiniteWord_createFromBinaryString(i64, i8*)

declare %struct.Value* @Value_createFromFiniteWord(%opaque.FiniteWord*)

declare %struct.Value* @Value_createFromError(i8*)

declare i64 @FiniteWord_size(%opaque.FiniteWord*)

declare i8 @FiniteWord_getBit(%opaque.FiniteWord*, i64)

declare %opaque.RationalWord* @RationalWord_createFromDecimalString(i8*)

declare %struct.Value* @Value_createFromRationalWord(%opaque.RationalWord*)
