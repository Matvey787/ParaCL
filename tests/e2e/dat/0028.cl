
// expect 1
if (1) {
    print 1;
} else {
    print 0;
}

// expect 0
if (0) {
    print 1;
} else {
    print 0;
}

// expect 3
if      (0) print 1;
else if (0) print 2;
else if (1) print 3;
else if (0) print 4;
else        print 5;

// expect 1
if      (1) print 1;
else if (!0)print 2;
else if (1) print 3;
else if (0) print 4;
else        print 5;

// expect 5
if      (0)  print 1;
else if (0)  print 2;
else if (0)  print 3;
else if (!1) print 4;
else         print 5;


// expect 3
if      (0) print 1;
else if (0) print 2;
else if (1) print 3;
else if (1) print 4;
else        print 4;
