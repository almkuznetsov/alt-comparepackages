#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <boost/asio/io_service.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/json/src.hpp>
#include <boost/json.hpp>
#include <boost/json/value.hpp>

boost::json::value getPackages(const std::string& branch, const std::string& arch);
boost::json::object comparePackages(const boost::json::value& branch1, const boost::json::value& branch2);