FIND_FIBONACCI_NUMBER_INDEX = 45;
ITERATIONS = 10000;

print "0001.cl: find ", FIND_FIBONACCI_NUMBER_INDEX, " fibonacci number, ", ITERATIONS, " times";

it = -1;
ite = ITERATIONS;

while ((it += 1) < ite)
{
    F_nm2 = 0; /* F(n-2) */
    F_nm1 = 1; /* F(n-1) */
    F_n = F_nm2 + F_nm1; /* F(n) */

    i = 2; /* Fibonacci number iterator, i = 2 cause skip F(0) and F(1) */

    while ((i += 1) < FIND_FIBONACCI_NUMBER_INDEX)
    {
        F_nm2 = F_nm1;
        F_n = F_n + F_nm1;
        F_nm1 = F_n - F_nm1;
    }

    print "[", it, "]: F(", i, ") = ", F_n;
}
