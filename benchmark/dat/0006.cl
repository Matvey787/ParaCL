N = 300;
a = 0;
b = 1;
i = 2;

print a;
print b;

while ((i += 1) <= N)
{
    c = a + b;
    print c;
    a = b;
    b = c;
}
