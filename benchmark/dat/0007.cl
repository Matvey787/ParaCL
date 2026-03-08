terms = 10000000;
pi4 = 0;
sign = 1;
i = 1;
n = 0;

while ((n += 1) <= terms)
{
    pi4 = pi4 + sign / i;
    sign = 0 - sign;
    i = i + 2;
}
pi = pi4 * 4;
print pi;
