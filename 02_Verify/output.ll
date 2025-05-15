; ModuleID = 'MyModule'
source_filename = "MyModule"

@0 = private unnamed_addr constant [5 x i8] c"%d\\n\00", align 1

declare i32 @printf(ptr, ...)

define i32 @my_func(i32 %0) {
entry:
  %1 = add i32 %0, 42
  ret i32 %1
}

define i32 @main() {
entry:
  %0 = call i32 @my_func(i32 22)
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 %0)
  ret i32 0
}
