#pragma once
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <iterator>
#include <boost/json/src.hpp>
#include <boost/json.hpp>
#include <boost/json/value.hpp>
#include <curl/curl.h>

boost::json::value getPackages(const std::string& branch);
boost::json::object comparePackages(const boost::json::value& branch1, const boost::json::value& branch2);