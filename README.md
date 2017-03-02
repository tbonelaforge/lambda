Getting Started:

1. git clone git@github.com:tbonelaforge/lambda.git
2. cd lambda
3. make
4. ./lambda ./examples/my_first_namespace.lc > output.html
5. Open output.html in a web browser.

The interpreter was built for educational purposes, and produces a very detailed representation of the program, before and after evaluation.
If the lambda executable is given a filename as command-line argument, e.g.

lambda ./examples/my_first_namespace.lc

it will parse the specified file ( if it conforms to the required grammar ), 
and execute the resulting program.  
This process will produce html output which, when viewed in a web browser, 
shows a graphical representation of the operator trees before evaluation of the program,
and the final operator trees after evaluation is all finished.

Programs written in the language resemble something like the following:

two:((plus,one),one);
one:\f\x(f,x);
plus:\m\n\f\x((m,f),((n,f),x));

Each semicolon-terminated line of input establishes an entry in a namespace ( symbol table ). For example, after parsing the first line above, 
the operator tree for "((plus,one),one)" will be 
stored in the namespace under the name "two".

The backslashes in the second and third lines each mark a bound variable in a lambda (function) definition.
The parentheses show the application of one expression to another.
For example, the expression:

\f\x(f,x)

Defines a function which takes two arguments ( 'f' and 'x', in that order ), 
and applies the first argument to the second.

The program above defines the "Church numeral" for the number 1, 
and a function to add two church numerals together. 
Then, it calculates the church numeral for the number 2 by adding 1 + 1.
