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
/** \file Details/Unlinker.h Definition of the Unlinker helper functor used by \link Depends::DAG::erase erase \endlink.
 * You will normally never want to include this file directly, as it is included by DAG.h */
#ifndef _depends_details_unlinker_h
#define _depends_details_unlinker_h

#include <stdexcept>

namespace Depends
{
	namespace Details
	{
		//! Unlinks all nodes from a given target
		template < typename NodeType >
		struct Unlinker
		{
			Unlinker(NodeType * target)
				: target_(target)
			{ /* no-op */ }

			Unlinker & operator()(NodeType * node)
			{
				typedef typename NodeType::targets_type Targets;
				typename Targets::iterator end(node->targets_.end());
				for (typename Targets::iterator where(node->targets_.begin()); where != end; )
				{
					if (*where == target_)
						where = node->targets_.erase(where);
					else
						++where;
				}

				return *this;
			}

			NodeType * target_;
		};
	}
}

#endif
