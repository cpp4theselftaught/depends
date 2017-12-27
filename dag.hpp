/* Depends: A generic dependency tracker in C++
 * Copyright (c) 2004-2017, Ronald Landheer-Cieslak
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
/** \file dag.hpp The implementation of the Directed Acyclic Graph (Depends::DAG) class.
 * If you want to build your own dependency tracker using this library, you will want
 * to include this file and build upon the Depends::DAG class contained in it. By 
 * including this file, you will have access to the complete Depends::DAG class and
 * all its dependencies, so you won't have to include anything else. */
#ifndef depends_dag_hpp
#define depends_dag_hpp

#include <vector>
#include <algorithm>
#include <boost/iterator/indirect_iterator.hpp>

#if DEPENDS_SUPPORT_SERIALIZATION
namespace boost { namespace serialization { class access; } }
#include "details/serialization.hpp"
#endif

#include "details/iterator.hpp"
#include "details/node.hpp"
#include "details/scopedflag.hpp"
#include "exceptions.hpp"

namespace Depends {
	/** A DAG is a collection of directed edges between nodes (or 
	 * vertices, if you prefer) which, by definition, is acyclic.
	 * This means that if there exists a path (of edges) that
	 * goes from node A to node B, there shall not be a path that goes
	 * from node B to node A.
	 *
	 * This \b particular DAG is a Reversible Container and a Unique
	 * Associative Container that just happens to sort its contents
	 * according to the links established between each of their values.
	 * The "normal" way to use this would therefore be to populate the
	 * DAG, and establish links between the values, using one of the
	 * link functions.
	 *
	 * As an example of use, there are two test cases that may be of
	 * interest:
	 * \dontinclude tests/dag.cpp
	 * First, a vector and a dag are both populated with integers ranging
	 * from 0 to 99, inclusive. 
	 * \skip test1
	 * \until }
	 * then, the vector is shuffled and links are created in the dag. As
	 * this carries on, more and more of those links will not be made as
	 * they would result in circular references.
	 * \until }
	 * \until }
	 * \until }
	 * \until }
	 * We then copy the contents of the DAG to std::cout, separating each
	 * entry with a space.
	 * \skipline std::copy
	 * The output is sorted in order of "score", putting the node with the
	 * \b least dependencies pointing \b to it first. This means that that
	 * particular node is the one on which nothing depends.
	 *
	 * The way this ordering is done is pretty simple: once a link is
	 * established in the DAG, a score (which starts at 1) is propagated
	 * through the DAG, adding the score of each source node to every one
	 * of its targets. As this is a recursive process, the final target
	 * node gets the highest score, which means it ends up at the end of
	 * the (serialized) DAG.
	 * 
	 * A second example would be a second test case in which we
	 * specifically check that a circular reference is detected. We first
	 * populate the DAG with ten values
	 * \skip test2
	 * \until dag.insert
	 * and then link the values 1 through 4 together, linking 1 to 2, 2
	 * to 3 and 3 to 4
	 * \until dag.link
	 * Finally, we try to link 4 back to 2, which should create a circular
	 * reference and therefore throw an exception
	 * \until }
	 * \until }
	 * The test-case should therefore, in test2, output "Circular reference
	 * detected"
	 *
	 * \param ValueType the type of whatever the DAG should be decorated
	 *        with
	 *
	 * \todo provide a way to specify the allocator to use
	 * \todo provide a way to provide a predicate to use to compare values
	 * */
	template <class ValueType>
	class DAG
	{
	public :
		// standard types for a container
		typedef ValueType value_type;
		typedef ValueType key_type;
		typedef ValueType & reference;
		typedef const ValueType & const_reference;
		typedef ValueType * pointer;
		typedef const ValueType * const_pointer;
		typedef unsigned long score_type;
		typedef Details::Node< ValueType, score_type > node_type;
		typedef std::vector< node_type* > nodes_type;
		typedef Details::Iterator< ValueType, const ValueType &, const ValueType *, score_type, typename nodes_type::iterator > iterator;
		typedef Details::Iterator< ValueType, const ValueType &, const ValueType *, score_type, typename nodes_type::iterator > const_iterator;
		typedef std::reverse_iterator< iterator > reverse_iterator;
		typedef std::reverse_iterator< const_iterator > const_reverse_iterator;
		typedef typename std::vector< node_type >::difference_type difference_type;
		typedef typename std::vector< node_type >::size_type size_type;

		/** This exception is thrown in case a new link creates a
		 * circular reference */
		typedef CircularReference circular_reference_exception;

		//! DefaultConstructible
		DAG()
		{ /* no-op */ }
		//! CopyConstructible
		DAG(const DAG & d)
			: nodes_(d.nodes)
		{ /* no-op */ }
	
		//! Construct a directed acyclic graph from a range
		/** This constructor does not create any links and, for most
		 * intents and purposes, creates a simple vector-like
		 * container from the values in the given range. 
		 *
		 * \param first first iterator in the range to copy
		 * \param last one-past-the-end */
		template <typename InputIterator>
		DAG(InputIterator first, InputIterator last)
		{
			insert(first, last);
		}
	
		//! Assignable
		DAG & operator=(const DAG & d)
		{
			nodes_ = d.nodes_;
			return *this;
		}

		~DAG()
		{
			for (auto node : nodes_)
			{
				delete node;
			}
		}
	
		//! Get a bidirectional iterator to the beginning of the container
		iterator begin() { return iterator(nodes_.begin()); }
		//! Get a bidirectional const iterator to the beginning of the container
		const_iterator begin() const { return const_iterator(nodes_.begin()); }
		//! Get a bidirectional reverse iterator to the end of the container
		reverse_iterator rbegin() { return reverse_iterator(nodes_.rbegin()); }
		//! Get a bidirectional reverse const iterator to the end of the container
		const_reverse_iterator rbegin() const { return const_reverse_iterator(nodes_.rbegin()); }

		//! Get a bidirectional iterator to one-past-the-end of the container
		iterator end() { return iterator(nodes_.end()); }
		//! Get a bidirectional const iterator to one-past-the-end of the container
		const_iterator end() const { return const_iterator(nodes_.end()); }
		//! Get a bidirectional reverse iterator to beginning of the container
		reverse_iterator rend() { return reverse_iterator(nodes_.rend()); }
		//! Get a bidirectional reverse const iterator to beginning of the container
		const_reverse_iterator rend() const { return const_reverse_iterator(nodes_.rend()); }

		//! get the size of the container
		size_type size() const { return nodes_.size(); }
		//! get the maximal size of the container (here for compatbility purposes only)
		size_type max_size() const { return sizeof(score_type) > sizeof(size_type) ? (size_type)~0 : (score_type)~0; }

		//! check whether the container is empty
		bool empty() const { return nodes_.empty(); }
		//! swap the contents of this container with another one of the same type
		void swap(DAG & d) { nodes_.swap(d.nodes_); }

		//! Equality Comparable
		bool operator==(const DAG & d) const
		{
			return (nodes_.size() == d.nodes_.size()) && 
				std::equal(
					boost::indirect_iterator< typename nodes_type::const_iterator >(nodes_.begin()),
					boost::indirect_iterator< typename nodes_type::const_iterator >(nodes_.end()),
					boost::indirect_iterator< typename nodes_type::const_iterator >(d.nodes_.begin()));
			
		}
		//! Equality Comparable
		bool operator!=(const DAG & d) const
		{
			return !(*this == d);
		}

		//! LessThan Comparable
		bool operator<(const DAG & d) const
		{
			return std::lexicographical_compare(
				boost::indirect_iterator< typename nodes_type::const_iterator >(nodes_.begin()),
				boost::indirect_iterator< typename nodes_type::const_iterator >(nodes_.end()),
				boost::indirect_iterator< typename nodes_type::const_iterator >(d.nodes_.begin()),
				boost::indirect_iterator< typename nodes_type::const_iterator >(d.nodes_.end()),
				Details::CompareNodesByContents< node_type >()
				);
		}
		//! GreaterThan Comparable
		bool operator>(const DAG & d) const { return d < *this; }
		
		//! LessThanOrEqual Comparable
		bool operator<=(const DAG & d) const { return !(d < *this); }
		//! GreaterThanOrEqual Comparable
		bool operator>=(const DAG & d) const { return !(*this < d); }

		/** Insert a value in the container.
		 * This does not create any links, but simply prepares a value to be linked
		 * with another one by inserting it in the container.
		 *
		 * \return a pair containing an iterator where the value was inserted and a
		 *         boolean indicating that it was inserted. These two are set to end
		 *         and false, resp. if the value was not inserted (which should only
		 *         happen if the value is already in the container). */
		std::pair<iterator, bool> insert(const value_type & val)
		{
			if (std::find_if(nodes_.begin(), nodes_.end(), [&val](auto node){ return *node == val; }) == nodes_.end())
			{
				nodes_.insert(nodes_.begin(), new node_type(val));
				return std::make_pair(begin(), true);
			}
			else
			{
				return std::make_pair(end(), false);
			}
		}

		/** Insert a range of values into the container, skipping anything that would be a duplicate.
		 * \param first the first iterator in the range
		 * \param last the last iterator in the range, which is expected to point one-past-the-end */
		template <typename InputIterator>
		void insert(InputIterator first, InputIterator last)
		{
			for ( ; first != last; ++first)
			{
				insert(*first);
			}
		}
	
		/** Link two values (nodes) at the give locations
		 * \pre neither source nor target must be the end iterator
		 * \pre both source and target must be valid iterators of this container
		 * \param source the source node to link
		 * \param target the node to link to
		 * \throws circular_reference_exception if the link would create a circular reference */
		void link(iterator source, iterator target)
		{
			{
				Details::ScopedFlag<node_type> scoped_flag(source.node(), node_type::VISITED);
				target.node()->visit();
			}
			source.node()->targets_.push_back(target.node());

			target.node()->visit([](node_type *node, score_type score){ node->score_ += score; }, source.node()->score_);

			std::sort(nodes_.begin(), nodes_.end(), [](auto lhs, auto rhs){ return lhs->score_ < rhs->score_; });
		}

		/** Link a node at a given location with a given value
		 * \pre the value must already be in the container
		 * \pre target must not be the end iterator and must be a valid iterator of this container
		 * \param source the iterator at which the source value (node) can be found
		 * \param target the value to link to 
		 * \throws circular_reference_exception if the link would create a circular reference */
		void link(iterator source, value_type target)
		{
			iterator target_iter = std::find(begin(), end(), target);

			if (target_iter == end())
				throw std::invalid_argument("value not found");
			link(source, target_iter);
		}

		/** Link a node at with a given value to a node at a given location
		 * \pre the value must already be in the container
		 * \pre source must not be the end iterator and must be a valid iterator of this container
		 * \param source the value to link from
		 * \param target the iterator at the location to link to
		 * \throws circular_reference_exception if the link would create a circular reference */
		void link(value_type source, iterator target)
		{
			iterator source_iter = std::find(begin(), end(), source);

			if (source_iter == end())
				throw std::invalid_argument("value not found");
			link(source_iter, target);
		}

		/** link two values together
		 * \pre both values must already be in the container
		 * \param source the value of the node to link from
		 * \param target the value of the node to link to
		 * \throws circular_reference_exception of the link would create a circular reference */
		void link(value_type source, value_type target)
		{
			iterator source_iter = std::find(begin(), end(), source);
			iterator target_iter = std::find(begin(), end(), target);

			if (source_iter == end() || target_iter == end())
				throw std::invalid_argument("value not found");
			link(source_iter, target_iter);
		}

		//! check whether the source and target nodes are linked
		bool linked(iterator source, iterator target) const
		{
			try
			{
				Details::ScopedFlag<node_type> scoped_flag(target.node(), node_type::VISITED);
				source.node()->visit();
			}
			catch (circular_reference_exception &)
			{
				return true;
			}

			return false;
		}

		//! check whether the source and target nodes are linked
		bool linked(iterator source, value_type target) const
		{
			iterator target_iter = std::find(begin(), end(), target);

			if (target_iter == end())
				return false;
			return linked(source, target_iter);
		}

		//! check whether the source and target nodes are linked
		bool linked(value_type source, iterator target) const
		{
			iterator source_iter = std::find(begin(), end(), source);

			if (source_iter == end())
				return false;
			return linked(source_iter, target);
		}

		//! check whether the source and target nodes are linked
		bool linked(value_type source, value_type target) const
		{
			iterator source_iter = std::find(begin(), end(), source);
			iterator target_iter = std::find(begin(), end(), target);

			if (source_iter == end() || target_iter == end())
				return false;
			return linked(source_iter, target_iter);
		}

		//! unlink source from target if they are linked
		bool unlink(iterator source, iterator target)
		{
			bool rv(true);
			typename node_type::targets_type::iterator where(std::find(source.node()->targets_.begin(), source.node()->targets_.end(), target.node()));
			if (where != source.node()->targets_.end())
			{
				source.node()->targets_.erase(where);
			}
			else
			{
				rv = false;
			}

			target.node()->visit([](node_type *node, score_type score){ node->score_ -= score; }, source.node()->score_);

			std::sort(nodes_.begin(), nodes_.end());

			return rv;
		}

		//! unlink source from target if they are linked
		bool unlink(iterator source, value_type target)
		{
			iterator target_iter = std::find(begin(), end(), target);

			if (target_iter == end())
				throw std::invalid_argument("value not found");
			return unlink(source, target_iter);
		}

		//! unlink source from target if they are linked
		bool unlink(value_type source, iterator target)
		{
			iterator source_iter = std::find(begin(), end(), source);

			if (source_iter == end())
				throw std::invalid_argument("value not found");
			return unlink(source_iter, target);
		}

		//! unlink source from target if they are linked
		bool unlink(value_type source, value_type target)
		{
			iterator source_iter = std::find(begin(), end(), source);
			iterator target_iter = std::find(begin(), end(), target);

			if (source_iter == end() || target_iter == end())
				throw std::invalid_argument("value not found");
			return unlink(source_iter, target_iter);
		}

		/** erase the node at the given iterator, unlinking it from the DAG
		 * \pre the iterator must be a valid iterator within this container and must not be end
		 * \param where the iterator indicating the value to delete from the container.*/
		iterator erase(iterator where)
		{
			while (!where.node()->targets_.empty())
			{
				unlink(where, (where.node()->targets_[0])->value_);
			}
			auto target(where.node());
			std::for_each(
				  nodes_.begin()
				, nodes_.end()
				, [target](auto *node) {
						typedef typename std::remove_reference< decltype(*node) >::type NodeType;
						typedef typename NodeType::targets_type Targets;
						typename Targets::iterator end(node->targets_.end());
						for (typename Targets::iterator which(node->targets_.begin()); which != end; )
						{
							if (*which == target)
							{
								which = node->targets_.erase(which);
							}
							else
							{
								++which;
							}
						}
					}
				);

			delete where.node();
			typename nodes_type::iterator whence(nodes_.erase(where.iter_));

			return iterator(whence);
		}
	
		/** erase the values in the given range.
		 * \pre both begin and end must be valid iterators in this container
		 * \pre end must be reachable by incrementing from begin
		 * \param begin iterator pointing to the first value to delete from the container
		 * \param end iterator pointing one-past-the-end of the range to delete */
		iterator erase(iterator begin, iterator end)
		{
			for (iterator where(begin); where != end; ++where)
			{
				delete where.node();
			}
			return iterator(nodes_.erase(begin.iter_, end.iter_));
		}

		/** Clear the DAG of all its contents.
		 * \internal Note that this implementation doesn't meet the standard's 
		 *           performance requirements. */
		void clear()
		{
			erase(begin(), end());
		}
		
	private :
#if DEPENDS_SUPPORT_SERIALIZATION
		template < typename Archive >
		void serialize( Archive & ar, const unsigned int version )
		{
			ar & boost::serialization::make_nvp("nodes_", nodes_);
		}
#endif

		mutable nodes_type nodes_;

#if DEPENDS_SUPPORT_SERIALIZATION
		friend class boost::serialization::access;
#endif
	};
}

#endif

