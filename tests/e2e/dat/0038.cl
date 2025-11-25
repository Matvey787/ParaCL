

{
    a = 10;
    print a; // expect 10
    {
        print a; // expect 10
        a += 15;
        print a; // expect 25
        {
            a = a + 1;
            print a; // expect 26
            {
                a = 0;
                print a; // expect 0;
            }
            print a; // expect 0
            a = 10;
        }
        print a; // expect 10
    }
    print a; // expect 10
}
