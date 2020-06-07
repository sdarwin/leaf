// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/handle_exception.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

struct error1: std::exception { };
struct error2: std::exception { };
struct error3: std::exception { };

template <class R,class Ex>
R f( Ex && ex )
{
	throw leaf::exception(std::move(ex),info<1>{1},info<2>{2},info<3>{3});
}

template <class R>
R f()
{
	return R(42);
}

int main()
{
	// void (success)
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				c = f<int>();
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			} );
		BOOST_TEST_EQ(c, 42);
	}

	// void (failure), one exception type by const &
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( error2 const & )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]( error1 const &, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 3;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 4;
			} );
		BOOST_TEST_EQ(c, 3);
	}

	// void (failure), multiple exception types by const &
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( error2 const & )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]( error1 const & e1, std::exception const & ex, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(&e1, &ex);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 3;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 4;
			} );
		BOOST_TEST_EQ(c, 3);
	}

	// void (failure), catch_<>, one exception type
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( error2 const & )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 3;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 4;
			} );
		BOOST_TEST_EQ(c, 3);
	}

	// void (failure), catch_<>, multiple exception types
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
	}

	//////////////////////////////////////

	// void (failure, initially not matched), catch<>, single exception type
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				leaf::try_catch(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error2> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(false);
			},
			[&c]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
	}

	// void (failure, initially not matched), catch<>, multiple exception types
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				leaf::try_catch(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error2> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(false);
			},
			[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
	}

	// void (failure, initially matched), catch_<>, single exception type
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				leaf::try_catch(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
	}

	// void (failure, initially matched), catch_<>, multiple exception types
	{
		int c=0;
		leaf::try_catch(
			[&c]
			{
				leaf::try_catch(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
					{
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
	}

	//////////////////////////////////////

	// int, handle_all (success)
	{
		int r = leaf::try_catch(
			[]
			{
				return f<int>();
			},
			[]
			{
				return 1;
			} );
		BOOST_TEST_EQ(r, 42);
	}

	// int, handle_all (failure), match_enum (single enum value)
	{
		int r = leaf::try_catch(
			[]
			{
				return f<int>(error1());
			},
			[]( leaf::catch_<error2> )
			{
				return 1;
			},
			[]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_all (failure), match_enum (multiple enum values)
	{
		int r = leaf::try_catch(
			[]
			{
				return f<int>(error1());
			},
			[]( leaf::catch_<error2> )
			{
				return 1;
			},
			[]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	//////////////////////////////////////

	// int, handle_some (failure, matched), match_enum (single enum value)
	{
		int r = leaf::try_catch(
			[]
			{
				return f<int>(error1());
			},
			[]( leaf::catch_<error2> )
			{
				return 1;
			},
			[]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, matched), match_enum (multiple enum values)
	{
		int r = leaf::try_catch(
			[]
			{
				return f<int>(error1());
			},
			[]( leaf::catch_<error2> )
			{
				return 1;
			},
			[]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match_enum (single enum value)
	{
		int r = leaf::try_catch(
			[]
			{
				int r = leaf::try_catch(
					[]
					{
						return f<int>(error1());
					},
					[]( leaf::catch_<error2> )
					{
						return 1;
					} );
				BOOST_TEST(false);
				return r;
			},
			[]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match_enum (multiple enum values)
	{
		int r = leaf::try_catch(
			[]
			{
				int r = leaf::try_catch(
					[]
					{
						return f<int>(error1());
					},
					[]( leaf::catch_<error2> )
					{
						return 1;
					} );
				BOOST_TEST(false);
				return r;
			},
			[]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially matched), match_enum (single enum value)
	{
		int r = leaf::try_catch(
			[]
			{
				int r = leaf::try_catch(
					[]
					{
						return f<int>(error1());
					},
					[]( leaf::catch_<error1>, info<1> const & x, info<2> y )
					{
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						return 1;
					} );
				BOOST_TEST_EQ(r, 1);
				return r;
			},
			[]( leaf::catch_<error1> )
			{
				return 2;
			},
			[]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	// int, handle_some (failure, initially matched), match_enum (multiple enum values)
	{
		int r = leaf::try_catch(
			[]
			{
				int r = leaf::try_catch(
					[]
					{
						return f<int>(error1());
					},
					[]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
					{
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						return 1;
					} );
				BOOST_TEST_EQ(r, 1);
				return r;
			},
			[]( leaf::catch_<error1> )
			{
				return 2;
			},
			[]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	//////////////////////////////////////

	return boost::report_errors();
}

#endif
