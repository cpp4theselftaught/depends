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
/** \file Details/Node.h Definition of the DAG's nodes.
 * You will normally never want to include this file directly, as it is included by DAG.h */
#ifndef depends_details_node_hpp
#define depends_details_node_hpp

#include "../exceptions.hpp"
#include "scopedflag.hpp"

namespace Depends
{
	namespace Details
	{
		template < typename NodeType >
		struct CompareNodesByContents
		{
			bool operator()(NodeType const &lhs, NodeType const &rhs)
			{
				// we consider lhs < rhs to be true if
				if (lhs < rhs)	// compare by scores
				{
					return true;
				}
				else if (lhs.value_ < rhs.value_) // compare by values
				{
					return true;
				}
				else
				{
					return std::lexicographical_compare(
						  boost::indirect_iterator< typename NodeType::targets_type::const_iterator >(lhs.targets_.begin())
						, boost::indirect_iterator< typename NodeType::targets_type::const_iterator >(lhs.targets_.end())
						, boost::indirect_iterator< typename NodeType::targets_type::const_iterator >(rhs.targets_.begin())
						, boost::indirect_iterator< typename NodeType::targets_type::const_iterator >(rhs.targets_.end())
						, *this);
				}
			}
		};

		//! A node, as stored in the DAG
		template <class ValueType, typename ScoreType>
		struct Node
		{
			typedef ValueType value_type;
			typedef ScoreType score_type;
			typedef std::vector< Node* > targets_type;
			
			enum Flag { VISITED = 1 };

			Node(ValueType const &v)
				: value_(v)
				, score_(1)
				, flags_(0)
			{
			}
			Node(Node const&) = default;
			Node(Node&&) = default;
			Node& operator=(Node const&) = default;
			Node& operator=(Node&&) = default;

			bool operator<(Node const &n) const
			{
				return score_ < n.score_;
			}

			bool operator==(Node const &rhs) const
			{
				return
					(value_ == rhs.value_) &&
					(score_ == rhs.score_) &&
					targets_.size() == rhs.targets_.size() &&
					std::equal(
						  boost::indirect_iterator< typename targets_type::const_iterator >(targets_.begin())
						, boost::indirect_iterator< typename targets_type::const_iterator >(targets_.end())
						, boost::indirect_iterator< typename targets_type::const_iterator >(rhs.targets_.begin())
					);
			}
	
			template < typename F, typename D >
			void visit(F f, D data)
			{
				if (flags_ & VISITED)
				{
					throw CircularReference("Circular reference detected");
				}
				else
				{ /* all is well so far */ }
				if (f)
				{
					f(this, data);
				}
				else
				{ /* no function */ }
				{
					ScopedFlag< Node > scoped_flag(this, VISITED);
					for (auto target : targets_)
					{
						target->visit(f, data);
					}
				}
			}
			void visit()
			{
				visit([](Node*, int){}, 0);
			}

			targets_type targets_;
			ValueType value_;
			ScoreType score_;
			unsigned int flags_;

		private :
			Node()
				: score_(0)
				, flags_(0)
			{ /* only here for serialization */ }

#if DEPENDS_SUPPORT_SERIALIZATION
			template < typename Archive >
			void serialize(Archive & ar, unsigned int const version)
			{
				ar & boost::serialization::make_nvp("targets_", targets_)
				   & boost::serialization::make_nvp("value_", value_)
				   & boost::serialization::make_nvp("score_", score_)
				   & boost::serialization::make_nvp("flags_", flags_)
				   ;
			}

			friend class boost::serialization::access;
#endif
		};
	}
}

#endif

