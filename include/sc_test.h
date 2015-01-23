#ifndef SC_TEST_H_TXT5SQCI
#define SC_TEST_H_TXT5SQCI

#include <map>
#include <string>
#include <algorithm>

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>

namespace sc_test
{

enum result {UNKNOWN, PASSED, FAILED};

#define SC_TEST(func) {\
  sc_test::test_helper __t(#func, [=]{ return this->func(); }, sc_test::UNKNOWN); \
  m_test_list.list.push_back(__t);}

#define ASSERT_EQ(val1, val2) if((val1) != (val2)) set_fail(__func__);

#define EXPECT_DELAY(operation, time, unit) \
  m_test_list[__func__].is_expired = true; \
  m_test_list[__func__].expire_event.notify((time)*m_wait_expired_factor, (unit)); \
  ::sc_core::sc_time __tmp = ::sc_core::sc_time_stamp(); \
  (operation);\
  m_test_list[__func__].is_expired = false; \
  if (::sc_core::sc_time_stamp() - __tmp != ::sc_core::sc_time((time), (unit))) { \
    set_fail(__func__); \
  }

#define EXPECT_EXCEPTION(operation, exception_type) \
  try { \
    (operation); \
    set_fail(__func__); \
  } catch (exception_type) {\
    ;\
  }

struct test_helper {
  test_helper(::std::string n, ::std::function <void ()> f, result r);
  test_helper(const test_helper& t);

  void run();

  ::std::string name;
  ::std::function <void ()> func;
  result result;
  ::sc_core::sc_event expire_event;
  bool is_expired;
};

struct test_list {

  test_helper& operator[](::std::string const test_name) {
    auto it = find_if( begin(list), end(list), 
        [&](test_helper test){ 
          return test.name == test_name;
        });
    return *it;
  }

  size_t num_with_result(const result r) {
    return count_if(begin(list), end(list), [&](test_helper test) {
          return test.result == r; 
        });
  }

  size_t num_total() {
    return list.size();  
  }

  void print_test_with_result(const result r) {
    for (auto &test: list) {
      if (test.result == r) {
        ::std::cout << ::std::string("    ") << test.name << ::std::endl;
      }
    }
  }

  ::std::vector <test_helper> list;

};





class sc_testbench: ::sc_core::sc_module
{
public:
  sc_testbench(::sc_core::sc_module_name name):
    ::sc_core::sc_module(name),
    m_wait_expired_factor(5)
  {
    SC_HAS_PROCESS(sc_testbench);
    SC_THREAD(run_tests); 
  }

  ~sc_testbench() {
    analysis();
  }

  virtual void reset() = 0;

  void run_tests() {
    for (auto &test: m_test_list.list) {
      reset(); 
      ::std::cout << "Testing: " << test.name<< "...";

      ::sc_core::sc_process_handle h = ::sc_core::sc_spawn(sc_bind(test.func));
      wait(h.terminated_event() | test.expire_event);

      //update result
      if( test.is_expired ) {
        ::std::cout << "  FAILED..." << ::std::endl;
        test.result = FAILED;
      }
      else if (test.result == UNKNOWN) {
        ::std::cout << "  PASSED!" << ::std::endl;
        test.result = PASSED;
      }

      else if( test.result == FAILED) {
        ::std::cout << "  FAILED..." << ::std::endl;
      }

      else {
        ::std::cout << "  Some problem occures..." << ::std::endl;
      }
    }
  }

  void analysis() {
    ::std::cout << ::std::endl;
    ::std::cout << "Total number of tests: " << m_test_list.num_total() << ::std::endl;
    ::std::cout << "Passed tests (" << m_test_list.num_with_result(PASSED) << "): " << ::std::endl;
    m_test_list.print_test_with_result(PASSED);

    ::std::cout << ::std::endl;
    ::std::cout << "Failed tests (" << m_test_list.num_with_result(FAILED) << "): " << ::std::endl;
    m_test_list.print_test_with_result(FAILED);

    ::std::cout << ::std::endl;
    ::std::cout << "Not-finished tests (" << m_test_list.num_with_result(UNKNOWN) << "): " << ::std::endl;
    m_test_list.print_test_with_result(UNKNOWN);
  }



protected:
  void set_fail(::std::string func_name) {
    m_test_list[func_name].result = FAILED; 
  }
protected:
  test_list m_test_list;
  int m_wait_expired_factor;
};

} /* sc_test */ 

#endif /* end of include guard: SC_TEST_H_TXT5SQCI */

