class A {};
class B inherits A {};

class Main {
  main() : Int {
    let x : A <- new B in
      case x of
        b : B => 0;
        a : A => 1;
      esac
  };
};