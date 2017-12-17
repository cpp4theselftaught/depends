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
/** \file Details/documentation.hpp Doxygen's main page.
 * You will normally never want to include this file: there's nothing here. */
/** \mainpage
 * The Depends library provides a full-fledged generic dependency tracker
 * written in C++. The cornerstone of this library is the Depends::DAG class, 
 * which is a generic implementation of a Directed Acyclic Graph.
 *
 * \section intro Introduction
 * As professional software developers we use programs that include dependency
 * trackers nearly every day: we basically can't do our work without them,
 * unless we start tracking dependencies by hand.
 *
 * The trackers we use on a daily basis are integrated into such fine tools
 * as GNU Make, Microsoft Visual Studio, etc.: dependency trackers are the
 * behind-the-scenes magic that make tools like these work.  They help us
 * track the dependencies between our source files to determine the order in
 * which they need to be compiled and which files need compiling. They make 
 * our jobs a whole lot easier, if not just plainly possible.
 *
 * Dependency trackers further help in such diverse applications as banking
 * (inside the calculation engine of one of France's most wide-spread fiscal
 * applications is a dependency tracker that tracks the dependencies of the
 * calculation engine's modules); OS kernels (using a dependency tracker to
 * know which modules to load and in what order); etc.
 *
 * Not many programmers seem to know how to write one. It's not generally a 
 * subject taught in a university's "standard" courses and most programmers 
 * simply take them for granted - and have the luxury to be able to do so. 
 * Writing one is not that hard, though: once you understand the basics of 
 * how they work, it becomes rather easy to roll your own...
 *
 * \section basic_principles The basic principles
 * Tracking the dependencies between a number of objects, events or whatever
 * basically amounts to constructing a Directed Acyclic Graph (DAG) with the
 * dependencies in question. A DAG carries its name very well: it means "day"
 * in dutch and most dependencies are tracked at daytime. Such feeble attempts
 * at humor aside, a DAG is, in deed, a collection of directed edges between
 * nodes (or vertices, if you prefer) which, by definition, is acyclic. This
 * means that if there exists a path (of edges) that goes from node A to node
 * B, there shall not be a path that goes from node B to node A.
 *
 * This makes validating a DAG a rather straight-forward affair: all you have
 * to do for each node in the DAG is determine that there is no path from that
 * node to itself, through whatever edges link it with the rest of the DAG,
 * directly or indirectly. We'll discuss an implementation of an algorithm
 * that does this a bit further on. It is also important to note that we can
 * do this verification before linking two nodes together by determining
 * whether a path from the target node to the source node already exists. If
 * that is the case, we can keep our DAG's invariant true by refusing to
 * create the new link. This is exactly what the Depends::DAG class does.
 *
 * Once we understand the basic meaning of DAG (Directed in that edges have a
 * source and a target and therefore go only one way; Acyclic in that no
 * circular paths from one node to itself shall exist; Graph because you can 
 * draw one on a piece of paper if you want to) building one becomes a piece
 * of cake: we simply define our DAG as a collection of nodes with edges
 * between them and before linking any edge A to edge B, we verify that no
 * path from edge B to edge A already exists.
 *
 * In C, a DAG can be represented as two simple structures: one for the nodes
 * and one for the edges, like this:
 * \code
 * struct dag_node;
 * struct dag_edge { struct dag_node * nodes[2]; };
 * struct dag_node { struct dag_edge ** edges[2]; };
 * \endcode
 * in which dag_edge::nodes contains two nodes linked by the edge (\c nodes[0]
 * being the source node and \c nodes[1] the target node) and
 * \c dag_node::edges contains two arrays of edges: one for the inbound
 * (\c edges[0]) and one for the outbound (\c edges[1]).
 *
 * We can simplify this a bit by taking into account that we're looking at a
 * directed graph, so we only need edges in one direction. Through the magic
 * of pointers, if we only need edges in one direction, we really don't need
 * edges at all: we can simply use pointers in stead. That means we can get
 * rid of struct \c dag_edge and make struct \c dag_node look like this:
 * \code
 * struct dag_node { struct dag_node ** targets; };
 * \endcode
 * Come to think of it, we said that we'd be doing this in C++, which makes
 * the node type look more like this:
 * \code
 * struct Node { std::vector< Node * > targets; };
 * \endcode
 * For sake of encapsulation, we'll wrap this up in a namespace we'll call
 * \c Details which will live in the same namespace as our DAG implementation.
 *
 * \section connecting_the_dots Connecting the dots
 * When linking two nodes in the DAG, we must verify that linking those nodes
 * does not break the DAG's basic invariant: that the graph shall be acyclic.
 * The logic for this will be tucked away in \link Depends::DAG::link DAG::link
 * \endlink which, before linking the two nodes passed to it, verifies that 
 * they are not already linked in the opposite direction. To do this, we flag 
 * the source node as "visited" and start visiting the DAG from the target node.
 * If, during the visit, we run into a node that has already been marked as 
 * visited, we have found a * circular path in the DAG . and therefore a circular 
 * dependency. Note, however, that there may be more than one path to a node 
 * (and we may want to repeat the visiting process) so once all targets of a node 
 * have been visited, we clear the flag that marks it as visited.
 *
 * To do all this, we'll need two new classes (initially):
 * \link Depends::Details::NodeVisitor Details::NodeVisitor \endlink and 
 * \link Depends::Details::ScopedFlag Details::ScopedFlag \endlink. The latter is
 * really very simple: it just sets a flag (which is an unsigned integer by
 * default) on construction and clears the flag on destruction:
 * \dontinclude Depends/Details/ScopedFlag.h
 * \skip template
 * \until };
 * Note that both the node type and the flag type are template parameters:
 * there's no reason to specify what the exact types will be as the same
 * principle could be useful elsewhere for some reason. I have a particular
 * dislike of writing the same code twice and generic programming helps avoid
 * it - so why not use it?
 *
 * The visitor class is a wee bit more complicated, but not much: it needs a
 * helper class to be generic enough to use for other things than just checking
 * the DAG's invariant - i.e. it needs some functor that tells it to do
 * something. By default, it will use the associated
 * \link Depends::Details::EmptyFunctor EmptyFunctor \endlink class.
 *
 * The visitor will call the functor passed to it for the node it is currently
 * visiting and subsequently recursively call itself on any node pointed to by
 * the node it is visiting. In this call, it will set a scoped "visited" flag
 * on the node it is currently in, which will be used for tracking circular
 * dependencies. If such a dependency is found, an exception is thrown.
 *
 * One interesting note is that the functor may not be called at all: if its
 * bool operator returns false (which may be the case if the functor is
 * actually a pointer and that pointer is NULL) it will not be called. Due to
 * this, the \c () operator of the class \link Depends::Details::EmptyFunctor 
 * EmptyFunctor \endlink is never really called.
 *
 * Once we've determined that no circular path will exist if we create the
 * link, we can create the link. This means we add the target node to the
 * \c targets vector in the source node, which is pretty straight-forward.
 *
 * We'll see below that there are two more steps after these to facilitate
 * traversing the DAG in a coherent manner, but the part of link that
 * interests us for now looks like this:
 * \code
 * void link(iterator source, iterator target)
 * {
 * 	{
 * 		Details::ScopedFlag<node_type> scoped_flag(source.node(),
 * 			node_type::VISITED);
 * 		Details::NodeVisitor<node_type>()(target.node());
 * 	}
 * 	source.node().targets_.push_back(target.node());
 * }
 * \endcode
 * 
 * Breaking the link between two nodes cannot create a circular path in the DAG
 * because it doesn't create a path. Therefore, we do not have to check for
 * this when we remove a link and can just remove the edge from the two nodes
 * it links together and destroy the edge when we're done. Note, however, that
 * it is impossible to destroy all paths from the source to the destination if
 * more than one exists - at least if we don't want to destroy paths that link
 * other, possibly "unrelated", nodes together as well.
 *
 * What we \b could (but won't) do, however, is report whether other paths
 * still exist. We can do that in the same way as we went looking for circular
 * paths, but this time we tag the target node as visited and start our visit
 * in the source node. If the visit succeeds, no more paths exist because the
 * target node wasn't visited (which would have resulted in an error). If it
 * fails, there's still a path that binds the two nodes . albeit not a direct
 * one - and we can report that to our caller. We won't do that, however,
 * because it implies an extra visit through the DAG whenever we remove a link,
 * which may become expensive. We'll add \link Depends::DAG::linked linked \endlink
 * method to the Depends::DAG class that uses this method to report whether two 
 * nodes are linked in stead, and just have unlink report whether a link was 
 * actually broken.
 *
 * We've now reached a point where we can construct a DAG in memory as a
 * collection of interconnected nodes. We can't do anything with it yet,
 * though, but we've taken the first step to a dependency tracker that we
 * could use for a bunch of purposes...
 *
 * \section decorating_the_dag Decorating the DAG
 * It's nice to be able to create a DAG in memory, but it would be even nicer
 * to be able to use it for something - such as a dependency tracker, for
 * example. For that, we need to be able to decorate the nodes of the DAG with
 * some type of information.
 *
 * As we're programming in C++, there's no reason to restrict the type of the
 * information we want to decorate the node with to something specific (and
 * even \c void \c * is too specific in this case). While we're at it, we'll
 * hide as much of the implementation details as we can from the user (at
 * least from the user that doesn't go snooping around in the header files)
 * and so we'll make the DAG a template class, which we'll call \link 
 * Depends::DAG DAG \endlink. The \link Depends::DAG DAG \endlink class is 
 * basically a wrapper around a vector of nodes (of type \link 
 * Depends::Details::Node Details::Node \endlink)..
 *
 * \subsection choice_of_type The choice of the type of decoration data
 * What type of data the DAG should be decorated with isn't, and should not be,
 * up to the implementor of a generic DAG class - and therefore isn't up to me.
 * C++ templates allow us to write code in such a way that the type of the
 * objects the code applies to need not be known until the compilation of the
 * code. We'll use this facility by defining the type of the values we store
 * to be of a copyable value type, which is the first template parameter of
 * our container. The only restriction we'll impose is the same as the
 * restrictions imposed by a vector: it must be copyable and
 * equality-comparable (i.e. the expression \c value1 \c == \c value2 must be
 * meaningful).
 *
 * With only the data we store, we can't really use our DAG for much more than
 * we could use a vector for: we need to be able to establish the order of
 * dependencies in some way. I.e., we need to be able to establist that, if we
 * want to traverse the DAG, the order in which we meet the nodes in the DAG
 * and the data they contain is somehow meaningful. Otherwise, all we can do
 * is "see whether we can construct a DAG with the given set of links" - which
 * can be useful, but hardly useful enough. Each node will therefore contain a
 * score - an unsigned integer value that indicates how many paths lead to it
 * in the DAG. When two nodes in the DAG are linked, their score (which starts
 * at 1) is propagated throughout all of the nodes that are now reachable from
 * the new source node. Meaning that if node A is linked to node B (node B is
 * a target of node A), the score of node A is 1 and the score of node B is 2.
 * If B is now linked to node C, node C will get score 3 (B's 2 + C's 1).
 * Because these scores are incremental and recursively propagated, the node
 * with the most paths leading to it will get the highest score. This means we
 * can sort the nodes in the DAG in the vector we store them in, according to
 * their score. The nodes ending up first in the DAG will be the ones that
 * have the least paths leading to them. We can easily use this to implement a
 * dependency tracker: all we have to do is establish the links between all
 * the nodes we want to track the dependencies for - the DAG will sort itself
 * automatically - and see how the dependencies are aligned. In a program
 * like Make, for example, all you'd have to do is establish a link between
 * the target file and its prerequisits and traverse the DAG from the end
 * (the prerequisits with the most dependencies leading to them) to the
 * beginning. Pseudo-code for such a dependency tracker would look a bit like
 * this:
 *
 * \code
 * int main(int argc, char ** argv)
 * {
 * 	Depends::DAG<Target> dag;
 *
 * 	parse_cmdline(argc, argv);
 *
 *	read_targets(makefile);
 *	// insert all the targets, without the dependencies, in the DAG
 * 	dag.insert(targets.begin(), targets.end())
 * 	for (	Targets::iterator iter = targets.begin();
 * 		iter != targets.end();
 * 		++iter)
 * 	{
 * 		Prerequisits preqs = iter->prerequisits();
 * 		for (	Prerequisits::iterator preq_iter = preqs.begin();
 * 			preq_iter != preqs.end();
 * 			++preq_iter)
 * 		{
 * 			dag.link(*preq_iter, *iter);
 * 		}
 * 	}
 * 	std::foreach(dag.begin(); dag.end(); Action());
 * }
 * \endcode
 * in which Targets and Prerequisits are user-defined STL-compatible containers
 * (or perhaps just vectors...).
 *
 * \section stl_compat Making the DAG an STL-Compatible Container
 * The DAG implementation presented here is an STL-compatible container: all of
 * the STL's standard algorithms - for as far as they are applicable to the
 * kind of container the DAG is, can be applied to this implementation. STL
 * containers adhere to a set of concepts, which define what we may expect of
 * them. In the same way, the DAG implementation is a \b Unique \b Associative
 * \b Container. It is also a \b Reversible \b Container in that the iterators it
 * provides are bidirectional and it has an \link Depends::DAG::rbegin rbegin \endlink
 * and a \link Depends::DAG::rend rend \endlink method.
 *
 * Like an \c std::set, the DAG implementation doesn't provide the possibility
 * to alter a value once it's stored in the DAG: as an associative container,
 * the keys (and therefore the values) stored in it are read-only.
 *
 * Aside from these standard accessors and mutators, the DAG also provides a
 * set of accessors and mutators not found on any other STL-compatible
 * container: it provides the possibility to \link Depends::DAG::link link \endlink
 * and \link Depends::DAG::unlink unlink \endlink the value
 * stored in it with eachother (and will raise an \c std::invalid_argument
 * exception if you try to link a value not in the container) and provides the
 * \link Depends::DAG::linked linked \endlink accessor which allows you to check 
 * whether two values in the container are already linked.
 *
 * \author Ronald Landheer-Cieslak \<blytkerchan at gmail dot com\>
 * */

/** The library's main namespace.
 * All of the library's code can be found in the Depends namespace. In versions 
 * prior to version 1.0.0, this namespace was called \c rlc, but that name has
 * been deprecated. In order to still use it, define \c DEPENDS_USE_DEPRECATED_NAMES
 * to something non-zero. */
namespace Depends
{
	/** This namespace contains implementation details user code should know about.
	 * In most cases, you won't need anything from this namespace yourself, but the
	 * implementation relies on the classes in this namespace being visible. Hence,
	 * the classes in this namespace are not fully documented. */
	namespace Details
	{ /* here for documentation only */ }
}
