#include <boost/assign.hpp>
#include <boost/log/support/date_time.hpp>
#include "AdaptersFactory.hh"
#include "Configuration.hh"

namespace bl = boost::log;

/*
 * Include Adapters
 */
#include "adapters/android/AndroidAdapter.hh"
#include "adapters/ios/IOSAdapter.hh"

/*
 * AdaptersFactory
 */

std::unique_ptr<AdaptersFactory>
AdaptersFactory::instance_ = nullptr;

AdaptersFactory::AdaptersFactory()
{
  add<Adapters::AndroidAdapter>();
  add<Adapters::IOSAdapter>();
  // Add your adapters here
}

const AdaptersFactory::AdaptersContainer &
AdaptersFactory::getAdapters() const
{
  return adapters_;
}

std::shared_ptr<Adapters::BaseAdapter> &
AdaptersFactory::operator[](const std::string & key)
{
  return adapters_.at(key);
}

void
AdaptersFactory::initLogging(const Configuration & conf)
{
  std::cout << "Initializing logger..." << std::endl;

  bl::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
  bl::formatter formatter = bl::expressions::stream << bl::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "> " << bl::expressions::smessage;
  bl::add_file_log
  (
    bl::keywords::file_name = conf["log"].as<std::string>(),
    bl::keywords::rotation_size = 10 * 1024 * 1024,
    bl::keywords::time_based_rotation = bl::sinks::file::rotation_at_time_point(0, 0, 0),
    bl::keywords::auto_flush = true
  )->set_formatter(formatter);
  BOOST_LOG(logger()) << "Start logging instance";
}

bl::sources::logger_mt &
AdaptersFactory::logger()
{
  static bl::sources::logger_mt lg;

  return lg;
}

void
AdaptersFactory::initAdapters(const Configuration & conf)
{
  this->initLogging(conf);
  auto it = adapters_.begin();
  while (it != adapters_.end())
  {
    std::cout << "Initializing adapter " << it->first << "...";
    std::cout.flush();
    if (it->second->init(conf) == false)
    {
      std::cerr << " Error!" << std::endl;
      it = adapters_.erase(it);
    }
    else
    {
      std::cout << " Done!" << std::endl;
      ++it;
    }
  }
}

/**
 * ITERATORS
 */

AdaptersFactory::iterator
AdaptersFactory::begin()
{
  return adapters_.begin();
}

AdaptersFactory::const_iterator
AdaptersFactory::begin() const
{
  return adapters_.begin();
}

AdaptersFactory::iterator
AdaptersFactory::end()
{
  return adapters_.end();
}

AdaptersFactory::const_iterator
AdaptersFactory::end() const
{
  return adapters_.end();
}

template<typename A>
void
AdaptersFactory::add()
{
  std::shared_ptr<Adapters::BaseAdapter> adapter = std::make_shared<A>();
  adapters_[adapter->getName()] = adapter;
}

std::unique_ptr<AdaptersFactory>&
AdaptersFactory::getInstance()
{
  if (!instance_)
    AdaptersFactory::instance_ = std::unique_ptr<AdaptersFactory>(new AdaptersFactory());
  return AdaptersFactory::instance_;
}
