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
/** \file depends.hpp A simple, generic dependency tracker. */
#ifndef depends_depends_hpp
#define depends_depends_hpp

#include "dag.hpp"
#include <algorithm>
#include <cassert>
#include <set>

namespace Depends
{
	/** A simple, generic dependency tracker.
	 * The requirements this class has to meet are:
	 * \li be able to add dependencies to the tracker
	 * \li be able to remove dependencies from the tracker 
	 * \li be able to find all values in the tracker that depend on a given
	 *     value
	 * \li be able to find all values in the tracker that depend on any value
	 *     that corresponds to a given predicate
	 *
	 * This dependency tracker, unlike the DAG class, is neither CopyConstructible 
	 * nor Assignable and does, therefore, \b not completely emulate an STL container. 
	 * 
	 * The dependency tracker contains an internal storage to which you can add 
	 * elements, which (even) remain mutable. Its purpose is to track the dependencies 
	 * between the items in that internal storage, for which it uses a DAG. The existance
	 * of that DAG, however, is hidden from you for most intents and purposes.
	 *
	 * In order to track the dependencies between items in the tracker, you must 
	 * first \link Depends::select select \endlink the item in the tracker who's 
	 * dependencies you want to track. You can then add either \b prerequisites
	 * or \b dependants
	 *
	 * \section prerequisites_and_dependants Prerequisites and Dependants
	 * A \b prerequisite is something \b on \b which the currently selected item
	 * depends while a \b dependant is something that \b depends \b on the currently
	 * selected item. The following code creates a dependency tracker containing three 
	 * integer values and defines '0' as a prerequisite of '1' (and thus defines '1'
	 * as a dependant of '0')
	 * \dontinclude tests/depends.cpp
	 * \skip test8
	 * \until }
	 * \until }
	 * and the following does exactly the same thing:
	 * \skip test9
	 * \until }
	 * \until }
	 * 
	 * Similarly, it is possible to use the \link Depends::getDependants \endlink 
	 * function to retrieve either all \b direct dependants of a given value:
	 * \skip test11
	 * \until }
	 * \until }
	 * or \b all dependants of a given value:
	 * \skip test12
	 * \until }
	 * \until }
	 * The same is true for the \link Depends::getPrerequisites getPrerequisites \enlink
	 * method.
	 * 
	 * \internal To do all this, the dependency tracker has to do quite a bit of 
	 * house-keeping: it keeps a set of whatever is stored in the tracker, of 
	 * which it takes pointers that it puts in the DAG. Clause 8 of section 23.1.2
	 * of the standard allows us to safely do this as it guarantees that "references"
	 * (and therefore also pointers) into the container remain valid in the face of
	 * insertions and erasures. */
	template < typename ValueType >
	class Depends
	{
		// here only so we can use it below
		typedef std::set< ValueType > Storage;
	public :
		/** A random-access iterator into our storage. */
		typedef typename Storage::iterator iterator;
		/** A random-access const_iterator into our storage. */
		typedef typename Storage::const_iterator const_iterator;
		/** A random-access iterator into our storage. */
		typedef typename Storage::reverse_iterator reverse_iterator;
		/** A random-access const_iterator into our storage. */
		typedef typename Storage::const_reverse_iterator const_reverse_iterator;

		//! The value-type as exposed
		typedef typename std::iterator_traits< iterator >::value_type value_type;
		//! The reference-type as exposed
		typedef typename std::iterator_traits< iterator >::reference reference;
		//! The const reference-type as exposed
		typedef typename std::iterator_traits< const_iterator >::reference const_reference;
		//! The pointer-type as exposed
		typedef typename std::iterator_traits< iterator >::pointer pointer;
		//! The difference type as exposed
		typedef typename std::iterator_traits< iterator >::difference_type difference_type;
		//! The size-type as exposed
		typedef typename Storage::size_type size_type;

		//! Default-construct and empty tracker
		Depends()
			: selected_(0)
		{ /* no-op */ }
		//! Construct a tracker from a range of things convertible to ValueType
		template < typename InputIterator >
		Depends(InputIterator begin, InputIterator end)
			: selected_(0)
		{ insert(begin, end); }

		//! Check whether the tracker is empty.
		bool empty() const throw()
		{ return storage_.empty(); }
		size_type size() const throw()
		{ return storage_.size(); }

		//! Get a random-access iterator to the start of our storage
		iterator begin()
		{ return iterator(storage_.begin()); }
		//! Get a random-access iterator to the start of our storage
		const_iterator begin() const
		{ return const_iterator(storage_.begin()); }
		//! Get a random-access iterator one past the end of our storage
		iterator end()
		{ return iterator(storage_.end()); }
		//! Get a random-access iterator one past the end of our storage
		const_iterator end() const
		{ return const_iterator(storage_.end()); }
		//! Get a random-access reverse iterator to the end of our storage
		reverse_iterator rbegin()
		{ return reverse_iterator(storage_.rbegin()); }
		//! Get a random-access reverse iterator to the end of our storage
		const_reverse_iterator rbegin() const
		{ return const_reverse_iterator(storage_.rbegin()); }
		//! Get a random-access reverse iterator to the start of our storage
		reverse_iterator rend()
		{ return reverse_iterator(storage_.rend()); }
		//! Get a random-access reverse iterator to the start of our storage
		const_reverse_iterator rend() const
		{ return const_reverse_iterator(storage_.rend()); }

		//! find a value equivalent to v
		template < typename V >
		const_iterator find(const V & v) const throw()
		{ return storage_.find(v); }
		template < typename V >
		iterator find(const V & v) throw()
		{ return storage_.find(v); }

		//! Insert a range of elements into our storage
		template < typename InputIterator >
		void insert(InputIterator begin, InputIterator end)
		{
			for (InputIterator where(begin); where != end; ++where)
				insert(*where);
		}
		//! Insert an element into our storage and return an iterator and a bool indicating whether there was an insertion
		std::pair< iterator, bool > insert( const value_type & v )
		{
			std::pair< iterator, bool > retval(storage_.insert(v));
			if (retval.second)
			{
				prerequisites_.insert(getPointer(retval.first));
				dependants_.insert(getPointer(retval.first));
			}
			else
			{ /* nothing really inserted */ }

			return retval;
		}
		//! Insert an element with a hint as to the location where that should be done. Note we don't use the hint.
		iterator insert( const iterator & where, const value_type & what )
		{ return insert(what).first; }

		//! Erase any value equivalent to v, return the amount of elements erased
		template < typename V >
		size_type erase(const V & v)
		{
			size_type erased(0);
			iterator where;
			while ((where = find(v)) != end())
			{
				erase(where);
				++erased;
			}

			return erased;
		}
		// erase the value at the indicated position
		void erase(iterator where)
		{
			if (selected_)
			{
				if (where == *selected_)
					clearSelection();
				else
				{ /* not erasing the selection */ }
			}
			else
			{ /* no selection - nothing to clear */ }
			pointer p(getPointer(where));
			bool found_in_blockers(false);
			typename DAG< pointer >::iterator whence(std::find(dependants_.begin(), dependants_.end(), p));
			if (whence != dependants_.end())
			{
				dependants_.erase(whence);
				found_in_blockers = true;
			}
			else
			{ /* value not in the blockers DAG */ }
			whence = std::find(prerequisites_.begin(), prerequisites_.end(), p);
			if (whence != prerequisites_.end())
			{
				prerequisites_.erase(whence);
				assert(found_in_blockers);
			}
			else
			{	// if it's no in the prerequisites_ DAG, it shouldn't have been in the dependants_ DAG either
				assert(!found_in_blockers);
			}
			storage_.erase(where);
		}
		// erase all the values in the given sequence
		void erase(const iterator & begin, const iterator & end)
		{
			for (iterator where(begin); where != end; erase(where++))
				;
		}
		// clear the container
		void clear()
		{
			clearSelection();
			dependants_.clear();
			prerequisites_.clear();
			storage_.clear();
		}

		/** Select something in the tracker to track the dependencies for.
		 * This method performs the selection by iterator. */
		void select(const_iterator what)
		{
			clearSelection();
			if (what == end())
				throw std::invalid_argument("Cannot select end");
			else
			{ /* OK */ }
			selected_ = new const_iterator(what);
		}
		/** Select something in the tracker to track the dependencies for.
		 * This method tries to find the given value in the tracker and will
		 * insert it if it doesn't exist yet. */
		void select(const value_type & v)
		{
			select(insert(v).first);
		}

		/** Link the pointed-to value to the currently selected value as a prerequisite. */
		void addPrerequisite(const_iterator whence)
		{
			assert(selected_);
			prerequisites_.link(getPointer(*selected_), getPointer(whence));
			dependants_.link(getPointer(whence), getPointer(*selected_));
		}
		/** Link the value to the currently selected value as a prerequisite - the value is added to the tracker if need be. */
		void addPrerequisite(const value_type & v)
		{
			addPrerequisite(insert(v).first);
		}
		/** Remove the link between the pointed-to value and the currently selected one.
		 * \warning Such a link can only be broken if it is a direct one! */
		void removePrerequisite(const_iterator whence)
		{
			assert(selected_);
			if (whence == end())
				return;
			else
			{ /* dependency could exist */ }
			bool was_prereq(prerequisites_.unlink(getPointer(*selected_), getPointer(whence)));
			bool was_dep(dependants_.unlink(getPointer(whence), getPointer(*selected_)));
			assert((was_dep && was_prereq) || (!was_dep && !was_prereq));
		}
		/** Remove the link between the give value and the currently selected one.
		 * \warning Such a link can only be broken if it is a direct one! */
		void removePrerequisite(const value_type & value)
		{
			removePrerequisite(find(value));
		}
		/** Get the prerequisites of the currently selected value.
		 * \param all set to true if you want \b all prerequisites, including those that 
		 *        are not direct prerequisites. If false (the default) only direct 
		 *        prerequisites will be returned.  */
		std::set< value_type > getPrerequisites(bool all = false) const
		{
			typedef typename DAG< pointer >::node_type node_type;

			std::set< value_type > retval;
			typename DAG< pointer >::const_iterator selected(std::find(prerequisites_.begin(), prerequisites_.end(), getPointer(*selected_)));
			node_type const *selected_node(selected.node());
			if (all)
			{
				auto retrieveTargets = [](node_type *node, std::set< pointer > *data){
					data->insert(node->value_);
				};

				std::set< pointer > temp;
				for (auto target : selected_node->targets_)
				{
					target->visit(retrieveTargets, &temp);
				}

				std::copy(
					  boost::indirect_iterator< typename std::set< pointer >::const_iterator >(temp.begin())
					, boost::indirect_iterator< typename std::set< pointer >::const_iterator >(temp.end())
					, std::inserter(retval, retval.begin())
					);
			}
			else
			{
				typename DAG< pointer >::node_type::targets_type::const_iterator end(selected_node->targets_.end());
				typename DAG< pointer >::node_type::targets_type::const_iterator where(selected_node->targets_.begin());
				while (where != end)
				{
					retval.insert(*((*where)->value_));
					++where;
				}
			}

			return retval;
		}

		/** Link the pointed-to value to the currently selected value as a dependant. */
		void addDependant(const_iterator whence)
		{
			assert(selected_);
			dependants_.link(getPointer(*selected_), getPointer(whence));
			prerequisites_.link(getPointer(whence), getPointer(*selected_));
		}
		/** Link the value to the currently selected value as a dependant - the value is added to the tracker if need be. */
		void addDependant(const value_type & v)
		{
			addDependant(insert(v).first);
		}
		/** Remove the link between the pointed-to value and the currently selected one.
		 * \warning Such a link can only be broken if it is a direct one! */
		void removeDependant(const_iterator whence)
		{
			assert(selected_);
			if (whence == end())
				return;
			else
			{ /* dependency could exist */ }
			dependants_.unlink(getPointer(*selected_), getPointer(whence));
			prerequisites_.unlink(getPointer(whence), getPointer(*selected_));
		}
		/** Remove the link between the pointed-to value and the currently selected one.
		 * \warning Such a link can only be broken if it is a direct one! */
		void removeDependant(const value_type & value)
		{
			removeDependant(find(value));
		}
		/** Get the dependants of the currently selected value.
		* \param all set to true if you want \b all dependants, including those that 
		*        are not direct dependants. If false (the default) only direct 
		*        dependants will be returned.  */
		std::set< value_type > getDependants(bool all = false) const
		{
			typedef typename DAG< pointer >::node_type node_type;

			std::set< value_type > retval;
			typename DAG< pointer >::const_iterator selected(std::find(dependants_.begin(), dependants_.end(), getPointer(*selected_)));
			node_type const *selected_node(selected.node());
			if (all)
			{
				auto retrieveTargets = [](node_type *node, std::set< pointer > *data){
					data->insert(node->value_);
				};

				std::set< pointer > temp;
				for (auto target : selected_node->targets_)
				{
					target->visit(retrieveTargets, &temp);
				}

				std::copy(
					  boost::indirect_iterator< typename std::set< pointer >::const_iterator >(temp.begin())
					, boost::indirect_iterator< typename std::set< pointer >::const_iterator >(temp.end())
					, std::inserter(retval, retval.begin())
					);
			}
			else
			{
				typename DAG< pointer >::node_type::targets_type::const_iterator end(selected_node->targets_.end());
				typename DAG< pointer >::node_type::targets_type::const_iterator where(selected_node->targets_.begin());
				while (where != end)
				{
					retval.insert(*((*where)->value_));
					++where;
				}
			}

			return retval;
		}


		//! check whether target depends on source
		bool depends(const_iterator target, const_iterator source) const
		{
			if (target == end() || source == end())
				return false;
			else
			{ /* such a dependency could exist */ }

			// target depends on source if target is a dependant of source
			// in which case source should also be a prerequisite of target
#ifndef NDEBUG
			assert(
				(dependants_.linked(getPointer(source), getPointer(target)) && prerequisites_.linked(getPointer(target), getPointer(source))) ||
				((!dependants_.linked(getPointer(source), getPointer(target))) && (!prerequisites_.linked(getPointer(target), getPointer(source)))));
#endif
			return dependants_.linked(getPointer(source), getPointer(target));
		}
		//! check whether target depends on source
		bool depends(const_iterator target, const value_type & source) const
		{
			return depends(target, find(source));
		}
		//! check whether target depends on source
		bool depends(const value_type & target, const_iterator source) const
		{
			return depends(find(target), source);
		}
		//! check whether target depends on source
		bool depends(const value_type & target, const value_type & source) const
		{
			return depends(find(target), find(source));
		}

	private :
		// Neither CopyConstructible nor Assignable
		Depends(const Depends &);
		Depends & operator=(const Depends &);

		/** \internal Clear the current selection by deleting the iterator and
		 * setting the pointer to 0. */
		void clearSelection()
		{
			delete selected_;
			selected_ = 0;
		}

		static pointer getPointer(const_iterator i)
		{
			return const_cast< pointer >(&(*i));
		}

#if DEPENDS_SUPPORT_SERIALIZATION
		template < typename Archive >
		void serialize( Archive & ar, const unsigned int version )
		{
			ar & boost::serialization::make_nvp("storage_", storage_)
			   & boost::serialization::make_nvp("dependants_", dependants_)
			   & boost::serialization::make_nvp("prerequisites_", prerequisites_)
			   ;
		}
#endif

		Storage storage_;
		DAG< pointer > dependants_;
		DAG< pointer > prerequisites_;
		/** \internal A pointer to an iterator containing the current selection. 
		 * This pointer is NULL if there is no current selection, and owned by 
		 * the tracker if there is. The selection is cleared when the selected 
		 * object is removed from the container. */
		const_iterator * selected_;

#if DEPENDS_SUPPORT_SERIALIZATION
		friend class boost::serialization::access;
#endif
	};
}

#endif
