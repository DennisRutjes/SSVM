(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (func (;0;) (type 0) (param i32) (result i32)
    (block (result i32)  ;; label = @1
      (if  ;; label = @2
        (i32.eq
          (local.get 0)
          (i32.const 0))
        (then
          (br 1 (;@1;)
            (i32.const 1))))
      (i32.const 2)))
  (func (;1;) (type 1) (result i32)
    (call 0
      (i32.const 0)))
  (func (;2;) (type 1) (result i32)
    (call 0
      (i32.const 1)))
  (export "test1" (func 1))
  (export "test2" (func 2)))
