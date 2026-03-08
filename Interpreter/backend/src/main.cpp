#warning "Using version with stupid ooptions parsing"

import interpreter;

int main(int argc, char* argv[])
{
    interpreter::interpret(argv[1]);
    return 0;
}
