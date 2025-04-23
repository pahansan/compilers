class Main inherits Aboba{
    aboba1 : Int <- 10;
    aboba2 : Int <- aboba1 / 10;

    main(): Int {
        {
            (let countdown : Int <- 20 in
                while 0 < countdown loop
                    {
                        cells.evolve();
                        cells.print();
                        countdown <- countdown - 1;
                    }
                pool
            );
            10;
        }
    };
};
