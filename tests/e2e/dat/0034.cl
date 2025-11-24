a = 10;
print "before block a = ", a;
{ 
    print "before new assign in block a = ", a;
    a = 15;
    print "after assign in block a = ", a;
}

print "after block witout new assign a = ", a;
