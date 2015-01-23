#include "sc_test.h"

namespace sc_test {

test_helper::test_helper(::std::string n, ::std::function <void ()> f, enum result r):
    name(n), 
    func(f), 
    result(r),
    is_expired(false)
{;}


test_helper::test_helper(const test_helper& t):
    name(t.name),
    func(t.func),
    result(t.result),
    is_expired(t.is_expired)
{;}

void test_helper::run() 
{
  func();
}

}
