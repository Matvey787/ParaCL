a = 10;
print "before block a = ", a; // expect: 10
{ 
    print "before new assign in block a = ", a; // expect: 10
    a = 15;
    print "after assign in block a = ", a; // expect: 15
}

print "after block witout new assign a = ", a; // expect: 15
