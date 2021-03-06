#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Configuration.hh"
#include "AdaptersFactory.hh"

class ConfigurationTest : public CppUnit::TestFixture {
public:
  char *av[4];
  Configuration *conf;

  void setUp()
  {
    char *args[4] = { strdup("tests"), strdup("--help"), strdup("-o"), strdup("file") };
    memcpy(av, args, sizeof(av));
    conf = new Configuration(4, av, nullptr);
  }

  void tearDown()
  {
    for (unsigned long i = 0; i < (sizeof(av) / sizeof(char*)); i++)
    {
      free(av[i]);
    }
    delete conf;
  }

  void testArgumentAvailability()
  {
    CPPUNIT_ASSERT(conf->count("help") == 1);
    CPPUNIT_ASSERT((*conf)["log"].as<std::string>() == av[3]);
  }

  static CppUnit::Test *suite()
  {
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "ConfigurationTest" );
    suiteOfTests->addTest( new CppUnit::TestCaller<ConfigurationTest>(
                                   "Test if arguments are available",
                                   &ConfigurationTest::testArgumentAvailability ) );
    return suiteOfTests;
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ConfigurationTest);
