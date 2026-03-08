limit = 1000000;
count = 0;
n = 2;

while (n <= limit)
{
    isPrime = 1;
    d = 2;
    while (d * d <= n)
    {
        if (n % d == 0)
        {
            isPrime = 0;
        }
        d = d + 1;
    }
    count = count + isPrime;
    n = n + 1;
}
print count;
