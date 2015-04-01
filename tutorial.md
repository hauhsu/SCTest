#SCTest

##Tutorial
Assume we have simple adder module and we want to build a unit test for it.  
In order to build unit test for a SystemC module, you need to create a testbench module.   
First include `sc_test.h` and define your testbench that inherits from sc_testbench:  

~~~ c++
#include "sc_test.h"

struct AdderTestbench: sc_test::sc_testbench 
{

};
~~~



In this testbench, we have to put the module we want to test: the adder module in this example.  
Assume the adder is defined in file `adder.h`.   

~~~ c++
#include "sc_test.h"
#include "adder.h"

struct AdderTestbench: sc_test::sc_testbench 
{
public:
	//Constructor
	AdderTestbench(sc_module_name name):
		adder("adder")
	{
		
	}
	
public:
	Adder adder;
};
~~~