#ifndef SC_TEST_H_TXT5SQCI
#define SC_TEST_H_TXT5SQCI

#include <map>
#include <string>
#include <systemc>

namespace sc_test
{

#define PASS true
#define FAIL false

#define SC_TEST(func) \
  m_tests.push_back(std::pair<std::string, std::function<void()> > \
      ( #func, [=]{ return this->func(); } ) );

#define ASSERT_EQ(val1, val2) if((val1) != (val2)) m_test_result = FAIL;

#define EXPECT_DELAY(operation, time, unit) \
  sc_core::sc_time __tmp = sc_core::sc_time_stamp();\
  (operation);\
  if (sc_core::sc_time_stamp() - __tmp != sc_core::sc_time((time), (unit))) \
    m_test_result = FAIL;

#define EXPECT_EXCEPTION(operation, exception_type) \
  try { \
    (operation); \
  } catch (exception_type) {\
    ;\
  }





class sc_testbench: sc_core::sc_module
{
public:
  sc_testbench(sc_core::sc_module_name name):
    sc_core::sc_module(name)
  {
    SC_HAS_PROCESS(sc_testbench);
    SC_THREAD(run_tests); 
  }

  void run_tests() {
    for (auto test: m_tests) {
      std::cout << "Testing: " << test.first << "...";

      m_test_result = PASS;
      test.second();

      if (m_test_result == PASS) {
        std::cout << "  PASSED!" << std::endl;
        m_passed_tests.push_back(test.first);
      }
      else {
        std::cout << "  FAILED..." << std::endl;
        m_failed_tests.push_back(test.first);
      }
    }
    analysis();
  }

  void analysis() {
    std::cout << "Total number of tests: " << m_tests.size() << std::endl;
    std::cout << "Passed tests (" << m_passed_tests.size() << "): " << std::endl;
    for (auto passed_test_name: m_passed_tests) {
      std::cout << std::string("    ") << passed_test_name << std::endl;
    }

    std::cout << "Failed tests (" << m_failed_tests.size() << "): " << std::endl;
    for (auto failed_test_name: m_failed_tests) {
      std::cout << std::string("    ") << failed_test_name << std::endl;
    }
  }

protected:
  std::vector< std::pair< std::string, std::function<void ()> > > m_tests;
  std::vector <std::string> m_passed_tests, m_failed_tests;
  bool m_test_result;
};

} /* sc_test */ 

#endif /* end of include guard: SC_TEST_H_TXT5SQCI */

