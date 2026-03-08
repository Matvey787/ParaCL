count = 0;
a = 1;
while ((a += 1) <= 2000)
{
    b = 1;
    while ((b += 1) <= 2000)
    {
        x = a * 100;
        y = b * 100;
        
        while (x != y)
        {
            if (x > y)
            {
                x = x - y;
            }
            else
            {
                y = y - x;
            }
        }
        
        if (x == 1)
        {
            count = count + 1;
        }
    }
}
print count;
