class Main inherits IO {
  main(): Int {
    {
      let x: Int <- 5, y: Int in
      {
        out_string("Value of x is ");
        out_int(x);
        out_string("\n");
        out_string("Sum of x and 3 is ");
        out_int(x + 3);
        out_string("\n");
      };
      0;
    }
  };
};

class A {
};

class B inherits A {  
  count: Int <- 0;
  
  reset(): Int {
    count <- 0
  };
  
  inc_by(n: Int): Int {
    count <- count + n
  };
  
  add_and_return(a: Int, b: Int): Int {
    a + b
  };

  call_add(): Int {
    self.add_and_return(2, 3)
  };
};
