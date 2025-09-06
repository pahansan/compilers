; ModuleID = 'calc.expr'
source_filename = "calc.expr"

@a.str = private constant [2 x i8] c"a\00"

define i32 @main(i32 %0, ptr %1) {
entry:
  %2 = call i32 @calc_read(ptr @a.str)
  call void @calc_write(i32 1)
  ret i32 0
}

declare i32 @calc_read(ptr)

declare void @calc_write(i32)
