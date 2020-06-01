// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/common.hpp>
#include <stdio.h>
#include <iostream>

namespace leaf = boost::leaf;

struct file_open_error: std::exception
{
	static void format_message( leaf::catch_<file_open_error>, leaf::e_file_name fn )
	{
		std::cerr << "Error opening file " << fn.value;
	}
};

struct file_read_error: std::exception
{
	static void format_message( leaf::catch_<file_read_error>, leaf::e_file_name fn )
	{
		std::cerr << "Error reading file " << fn.value;
	}
};

std::shared_ptr<FILE> open_file( char const * fn )
{
	if( FILE * f = fopen(fn,"rb") )
		return std::shared_ptr<FILE>(f, &fclose);
	else
		LEAF_THROW(file_open_error());
}

void read_file( FILE * f, char buf[], size_t buf_size )
{
	if( fread(buf, 1, buf_size, f) != buf_size || ferror(f) )
		LEAF_THROW(file_read_error());
}

int main()
{
	char const * fn = "test.txt";
	size_t const buf_size = 1024;
	leaf::try_catch(
		[&]
		{
			auto propagate = leaf::preload( leaf::e_file_name{fn} );
			auto f = open_file(fn);
			char buf[buf_size];
			read_file(f.get(), buf, buf_size);
		},
		&file_open_error::format_message,
		&file_read_error::format_message);
	return 0;
}
