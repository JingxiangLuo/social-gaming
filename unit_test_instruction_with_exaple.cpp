// part1 Unit Testing
// 1.In this project, we should use lots of the Unit Testing
// The definition of Unit Testing
// Unit Testing is a type of software testing where individual units or components of a software are tested. 
// The purpose is to validate that each unit of the software code performs as expected.
// Unit Testing is done during the development (coding phase) of an application by the developers. 
// Unit Tests isolate a section of code and verify its correctness. 
// A unit may be an individual function, method, procedure, module, or object.
// #refenrence to: https://www.guru99.com/unit-testing-guide.html


// 2. the advantage of Unit Testing
// (1) One of the main benefits of unit testing is that it makes the coding process more Agile. 
// (2) it can test small error and is prepared for the integration test.
// (3) we can find the bug ealy beacuase with the project going on, the bug will be more and more.
// (4) Unit testing reduces defects in the newly developed features or reduces bugs when changing the existing functionality. 
//reference to https://dzone.com/articles/top-8-benefits-of-unit-testing
// (5) Unit testing is a great way to ensure that your code is working as expected.
// (6) use some basic unit test can help us simplify the process of debugging.
// (7) unit test can save the cost of debugging relatively.
// (8) unit test can help other people to understand the code.

// there are also some disadvantages of Unit Testing

// 3. the disadvantage of Unit Testing
//1. it needs us to take more time to write the unit test.
//2. the unit test needs to be maintained.
//3. sometimes, it occupies the memory.
//4. At the end, we need to delete some of the unit test to make sure that the code can run correctly.


//example of Unit Testing
// 1. the example of Unit Testing
#include <iostream>

// Function to test
bool function1(int a) {
    return a > 5;   
}

// If parameter is not true, test fails
// This check function would be provided by the test framework
#define IS_TRUE(x) { if (!(x)) std::cout << __FUNCTION__ << " failed on line " << __LINE__ << std::endl; }

// Test for function1()
// You would need to write these even when using a framework
void test_function1()
{
    IS_TRUE(!function1(0));
    IS_TRUE(!function1(5));
    IS_TRUE(function1(10));
}

int main(void) {
    // Call all tests. Using a test framework would simplify this.
    test_function1();
}
reference to: https://stackoverflow.com/questions/52273110/how-do-i-write-a-unit-test-in-c



//part2: use the basic unit test-assertion
//assertion
assertion is a useful way to test impossible conditions in run time construct.
//example of assertion
#include <cassert>
#include <iostream>
using namespace std;
int main()
{
    int x = 1;
    int y = 2;
    assert(x > y);
    cout << "x < y";;
    return 0;
}
It is a simple example, becuase x is definately smaller than y, so the assert will be failed.

//example 2
#include <cassert>
#include <iostream>
using namespace std;
int add(int x, int y)
{
    return x + y;
}
int main()
{
    int x = 1;
    int y = 2;
    assert(add(x, y) == 3);
    cout << "x + y = 3";
    return 0;
}
In this example, the assert will be passed, so the cout will be printed.

//example 3
#include <cassert>
#include <iostream>
using namespace std;
bool isEven(int x)
{
    return x % 2 == 0;
}
int main()
{
    int x = 1;
    assert(isEven(x));
    cout << "This value is odd";
    return 0;
}

Conclusion: the unit test is an useful part in software development. It is a useful tool to prevent error.





