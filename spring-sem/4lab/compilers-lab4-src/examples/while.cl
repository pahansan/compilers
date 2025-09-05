class A {
  print_n_times(number : Int) : Int {
    {
      (let i : Int <- 0 in
        while i < number loop
          {
            print(i);
            i <- i + 1;
          }
        pool
      );
      i;
    }
  };
};