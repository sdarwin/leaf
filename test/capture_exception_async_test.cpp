// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#if defined(LEAF_NO_EXCEPTIONS) || defined(LEAF_NO_THREADS)

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/preload.hpp>
#include "lightweight_test.hpp"
#include <vector>
#include <future>
#include <iterator>
#include <algorithm>

namespace leaf = boost::leaf;

template <int> struct info { int value; };

struct fut_info
{
	int a;
	int b;
	int result;
	std::future<int> fut;
};

template <class H, class F>
std::vector<fut_info> launch_tasks( int task_count, F f )
{
	assert(task_count>0);
	std::vector<fut_info> fut;
	std::generate_n( std::inserter(fut,fut.end()), task_count,
		[=]
		{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
			int const a = rand();
			int const b = rand();
			int const res = (rand()%10) - 5;
std::cout << __FILE__ << ':' << __LINE__ << '\n';
			return fut_info { a, b, res, std::async( std::launch::async,
				[=]
				{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
					return leaf::capture(leaf::make_shared_context<H>(), f, a, b, res);
				} ) };
		} );
	return fut;
}

int main()
{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
	auto error_handler = []( leaf::error_info const & err, int a, int b )
	{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
		return leaf::remote_handle_exception( err,
			[&]( info<1> const & x1, info<2> const & x2,info<4> const & x4 )
			{
				BOOST_TEST_EQ(x1.value, a);
				BOOST_TEST_EQ(x2.value, b);
std::cout << __FILE__ << ':' << __LINE__ << '\n';
				return -1;
			},
			[]
			{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
				return -2;
			} );
	};

	std::vector<fut_info> fut = launch_tasks<decltype(error_handler)>(
		2,
		[]( int a, int b, int res )
		{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
			if( res>=0 )
				return res;
			else
				throw leaf::exception( std::exception(), info<1>{a}, info<2>{b}, info<3>{} );
		} );

	for( auto & f : fut )
	{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
		f.fut.wait();
std::cout << __FILE__ << ':' << __LINE__ << '\n';
		int r = leaf::remote_try_catch(
			[&]
			{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
				auto propagate = leaf::preload( info<4>{} );
std::cout << __FILE__ << ':' << __LINE__ << '\n';
				return leaf::future_get(f.fut);
			},
			[&]( leaf::error_info const & err )
			{
std::cout << __FILE__ << ':' << __LINE__ << '\n';
				return error_handler(err, f.a, f.b);
			} );
		if( f.result>=0 )
			BOOST_TEST_EQ(r, f.result);
		else
			BOOST_TEST_EQ(r, -1);
	}

	return boost::report_errors();
}

#endif
