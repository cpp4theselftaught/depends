#ifndef depends_exceptions_hpp
#define depends_exceptions_hpp

#include "exceptions/exception.hpp"

namespace Depends {
	enum struct Errors {
		  circular_reference__
		};

	typedef Vlinder::Exceptions::Exception< std::runtime_error, Errors, Errors::circular_reference__ > CircularReference;
}

#endif

