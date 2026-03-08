N = 1000;
M = 1000;
sum = 0;

i = 0;
while ((i += 1) <= N)
{
    j = 0;
    while ((j += 1) <= M)
    {
        if (i % 2)
        {
            if (j % 2)
            {
                sum = sum + i * j;
            }
            else
            {
                sum = sum + i + j;
            }
        }
        else
        {
            if (j % 3 == 0)
            {
                sum = sum + i - j;
            }
            else if (j % 3 == 1)
            {
                sum = sum + i * i;
            }
            else
            {
                sum = sum + j * j;
            }
        }
    }
}
print sum;
