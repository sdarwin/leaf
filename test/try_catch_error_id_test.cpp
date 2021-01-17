// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/config.hpp>
#ifdef BOOST_LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/exception.hpp>
#   include <boost/leaf/pred.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info1 { int value; };
struct info2 { int value; };

struct my_error: std::exception { };

int main()
{
    {
        int r = leaf::try_catch(
            []() -> int
            {
                throw leaf::exception( my_error(), info1{42} );
            },
            []( my_error const & x, leaf::catch_<leaf::error_id> id )
            {
                BOOST_TEST(dynamic_cast<leaf::error_id const *>(&id.matched)!=0 && dynamic_cast<leaf::error_id const *>(&id.matched)->value()==1);
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r, 1);
    }

    {
        leaf::try_catch(
            []
            {
                leaf::try_catch(
                    []
                    {
                        try
                        {
                            throw leaf::exception( info1{1} );
                        }
                        catch( std::exception & )
                        {
                        }
                        throw leaf::exception( info2{2} );
                    },
                    []( info1 const * )
                    {
                        throw;
                    } );
            },
            []( leaf::verbose_diagnostic_info const & di )
            {
                std::cout << di;
                std::stringstream ss; ss << di;
                std::string s = ss.str();
                BOOST_TEST(s.find("info1") == s.npos);
                BOOST_TEST(s.find("info2") != s.npos);
            } );
    }

    {
        leaf::try_catch(
            []
            {
                leaf::try_catch(
                    []
                    {
                        try
                        {
                            throw leaf::exception( info1{1} );
                        }
                        catch( std::exception & )
                        {
                        }
                        throw leaf::exception( info2{2} );
                    },
                    []( info1 const * )
                    {
                        throw;
                    } );
            },
            []( leaf::verbose_diagnostic_info const & di, info1 const * x )
            {
                BOOST_TEST(x==0);
                std::cout << di;
                std::stringstream ss; ss << di;
                std::string s = ss.str();
                BOOST_TEST(s.find("info1") == s.npos);
                BOOST_TEST(s.find("info2") != s.npos);
            } );
    }

    return boost::report_errors();
}

#endif
