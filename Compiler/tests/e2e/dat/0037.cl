n = 0;
a = 0;
b = 1;
x = 20;

while(n < x)
{
    n += 1;

    if (n == 1)
        print a;

    if (n == 2)
        print b;

    if (n > 2)
    {
        tmp = b;
        b = a + b;
        a = tmp;
        print b;
    }
}
