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
/** \file Details/Iterator.h Definition of the DAG's iterators.
 * You will normally never want to include this file directly, as it is included by DAG.h */
#ifndef _depends_details_iterator_h
#define _depends_details_iterator_h

#include <iterator>
#include "node.hpp"

namespace Depends
{
	namespace Details
	{
		//! It's a tag, folks!
		struct reverse_tag {};
		
		//! Base of the bidirectional iterators for the DAG
		template <class ValueType, typename ReferenceType, typename PointerType, typename ScoreType, typename IteratorType>
		struct IteratorBase : public std::iterator<std::forward_iterator_tag, ValueType, ptrdiff_t, PointerType, ReferenceType>
		{
			typedef IteratorBase<ValueType, ValueType&, ValueType*, ScoreType, IteratorType> iterator;
			typedef IteratorBase<ValueType, const ValueType&, const ValueType*, ScoreType, IteratorType> const_iterator;
			typedef Node<ValueType, ScoreType> node_type;
			
			IteratorBase(const IteratorType & i) : iter_(i) {}
			IteratorBase(const iterator & i) : iter_(i.iter_) {}

			ReferenceType operator*() const { return (*iter_)->value_; }
			PointerType operator->() const { return &((*iter_)->value_); }

			bool operator==(const IteratorBase & i) const { return iter_ == i.iter_; }
			bool operator!=(const IteratorBase & i) const { return iter_ != i.iter_; }

			const node_type * node() const { return *iter_; }
			node_type * node() { return *iter_; }

			IteratorType iter_;
		};

		//! A forward-moving bidirectional iterator for the DAG
		template <class ValueType, typename ReferenceType, typename PointerType, typename ScoreType, typename IteratorType>
		struct Iterator : public IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>
		{
			typedef Iterator<ValueType, ValueType&, ValueType*, ScoreType, IteratorType> iterator;
			typedef Iterator<ValueType, const ValueType&, const ValueType*, ScoreType, IteratorType> const_iterator;
			typedef IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType> super;
			
			Iterator(const IteratorType & i) : IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}
			Iterator(const iterator & i) : IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}

			Iterator & operator++() { ++(super::iter_); return *this; };
			Iterator operator++(int) { Iterator tmp = *this; ++(super::iter_); return tmp; };

			Iterator & operator--() { --(super::iter_); return *this; };
			Iterator operator--(int) { Iterator tmp = *this; --(super::iter_); return tmp; };
		};

		//! A backward-moving bidirectional iterator for the DAG
		template <class ValueType, typename ReferenceType, typename PointerType, typename ScoreType, typename IteratorType>
		struct ReverseIterator : public IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>
		{
			typedef ReverseIterator<ValueType, ValueType&, ValueType*, ScoreType, IteratorType> iterator;
			typedef ReverseIterator<ValueType, const ValueType&, const ValueType*, ScoreType, IteratorType> const_iterator;
			typedef IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType> super;
			
			ReverseIterator(const IteratorType & i) : IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}
			ReverseIterator(const iterator & i) : IteratorBase<ValueType, ReferenceType, PointerType, ScoreType, IteratorType>(i) {}

			ReverseIterator & operator++() { ++(super::iter_); return *this; };
			ReverseIterator operator++(int) { ReverseIterator tmp = *this; ++(super::iter_); return tmp; };

			ReverseIterator & operator--() { --(super::iter_); return *this; };
			ReverseIterator operator--(int) { ReverseIterator tmp = *this; --(super::iter_); return tmp; };
		};
	}
}

#endif

