N = 150;
M = 150;
P = 150;
sum = 0;

i = 0;
while ((i += 1) <= N)
{
    j = 0;
    while ((j += 1) <= M)
    {
        k = 0;
        while ((k += 1) <= P)
        {
            sum = sum + i * j * k;
            sum = sum - i + j - k;
        }
    }
}
print sum;
