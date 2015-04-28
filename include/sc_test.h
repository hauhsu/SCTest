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

#define ASSERT_EQ(val1, val2) if((val1) != (val2)) set_fail(__func__, __LINE__);

#define EXPECT_DELAY(operation, time, unit) \
{\
  m_test_list[__func__].is_expired = true; \
  m_test_list[__func__].expire_event.notify((time+1)*m_wait_expired_factor, (unit)); \
  set_fail(__func__, __LINE__); \
  ::sc_core::sc_time __start_operation = ::sc_core::sc_time_stamp(); \
  (operation);\
  m_test_list[__func__].is_expired = false; \
  m_test_list[__func__].expire_event.cancel();\
  if (::sc_core::sc_time_stamp() - __start_operation== ::sc_core::sc_time((time), (unit))) { \
    m_test_list[__func__].failed_lines.pop_back();\
    m_test_list[__func__].result = sc_test::UNKNOWN;\
  }\
}

#define EXPECT_EXCEPTION(operation, exception_type) \
  try { \
    (operation); \
    set_fail(__func__, __LINE__); \
  } catch (exception_type &e) {\
    ;\
  }

#define EXPECT_NO_EXCEPTION(operation) \
  try { \
    (operation); \
  } catch (...) {\
    set_fail(__func__, __LINE__); \
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
  ::std::vector<int> failed_lines;
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
        ::std::cout << ::std::string("    ") << test.name;
        if (test.result != PASSED) {
          ::std::cout << " -> line: ";
          for (auto line: test.failed_lines) 
            ::std::cout << line << " ,";
        }
        ::std::cout << ::std::endl;
      }
    }
  }

  ::std::vector <test_helper> list;

};





class sc_testbench: public ::sc_core::sc_module
{
public:
  sc_testbench(::sc_core::sc_module_name name):
    ::sc_core::sc_module(name),
    m_wait_expired_factor(5),
    m_elaborated(false)
  {
    SC_HAS_PROCESS(sc_testbench);
    SC_THREAD(run_tests); 
  }

  ~sc_testbench() {
    analysis();
  }

  void end_of_elaboration() {
    m_elaborated = true;
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
        ::std::cout << "  FAILED...(expired)" << ::std::endl;
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
    if (m_elaborated) {
      ::std::cout << ::std::endl;
      ::std::cout << ::std::endl;
      if (m_test_list.num_total() == m_test_list.num_with_result(PASSED)) {
        ::std::cout << " ------------------------------ " << ::std::endl;
        ::std::cout << "|       ALL TESTS PASSED!!     |" << ::std::endl;
        ::std::cout << " ------------------------------ " << ::std::endl;
      }
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
    } else {
      ::std::cout << "Elaboration failed.....\n";
    }
  }



protected:
  inline void set_fail(::std::string func_name, int failed_line) {
    m_test_list[func_name].result = FAILED; 
    m_test_list[func_name].failed_lines.push_back(failed_line);
  }

protected:
  test_list m_test_list;
  int m_wait_expired_factor;
  bool m_elaborated;
};

} /* sc_test */ 

#endif /* end of include guard: SC_TEST_H_TXT5SQCI */

