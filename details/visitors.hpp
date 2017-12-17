/* Depends: A generic dependency tracker in C++
 * Copyright (c) 2004-2007, Ronald Landheer-Cieslak
 * All rights reserved
 * 
 * This is free software. You may distribute it and/or modify it and
 * distribute modified forms provided that the following terms are met:
 *
 * * Redistributions of the source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer;
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the distribution;
 * * None of the names of the authors of this software may be used to endorse
 *   or promote this software, derived software or any distribution of this 
 *   software or any distribution of which this software is part, without 
 *   prior written permission from the authors involved;
 * * Unless you have received a written statement from Ronald Landheer-Cieslak
 *   that says otherwise, the terms of the GNU General Public License, as 
 *   published by the Free Software Foundation, version 2 or (at your option)
 *   any later version, also apply.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/** \file Details/Visitors.h Definition of the DAG's visitors and helpers.
 * You will normally never want to include this file directly, as it is included by DAG.h */
#ifndef _depends_details_visitors_h
#define _depends_details_visitors_h

#include <iterator>
#include "scopedflag.hpp"
#include "circularreferenceexception.hpp"

namespace Depends
{
	namespace Details
	{
		//! Default, no-op, functor
		struct EmptyFunctor
		{
			template <class NodeType, class DataType>
			EmptyFunctor & operator()(NodeType * node, DataType & data)
			{
				return *this;
			}

			operator bool()
			{
				return false;
			}
		};
	
		//! Functor to increment a node's score with a visitor
		struct IncScoreFunctor
		{
			template <class NodeType, class DataType>
			IncScoreFunctor & operator()(NodeType * node, DataType & data)
			{
				node->score_ += data;

				return *this;
			}

			operator bool()
			{
				return true;
			}
		};
	
		//! Functor to decrement a node's score with a visitor
		struct DecScoreFunctor
		{
			template <class NodeType, class DataType>
			DecScoreFunctor & operator()(NodeType * node, DataType & data)
			{
				node->score_ -= data;

				return *this;
			}

			operator bool()
			{
				return true;
			}
		};

		//! Functor to retrieve all the targets of all the nodes encountered during the visit
		struct RetrieveTargets
		{
			template < class NodeType, class DataType >
			RetrieveTargets & operator()(NodeType * node, DataType & data)
			{
				data->insert(node->value_);

				return *this;
			}

			operator bool()
			{
				return true;
			}
		};
		
		//! Visitor that will automagically visit an entire DAG
		template <class NodeType, class BinaryFunction = EmptyFunctor, class DataType = void*>
		struct NodeVisitor
		{
			NodeVisitor()
			{
			}

			NodeVisitor(const BinaryFunction & binary_function, const DataType data)
				: binary_function_(binary_function),
				  data_(data)
			{
			}
			
			NodeVisitor & operator()(NodeType * node)
			{
				if (node->flags_ & NodeType::VISITED)
					throw CircularReferenceException();
				if (binary_function_)
					binary_function_(node, data_);
				{
					ScopedFlag<NodeType> scoped_flag(node, NodeType::VISITED);
					std::for_each(
						node->targets_.begin(),
						node->targets_.end(),
						*this);
				}
				return *this;
			}

			BinaryFunction binary_function_;
			DataType data_;
		};
	}
}

#endif

