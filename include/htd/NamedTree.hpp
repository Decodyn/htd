/*
 * File:   NamedTree.hpp
 *
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2016, Michael Abseher
 *    E-Mail: <abseher@dbai.tuwien.ac.at>
 * 
 * This file is part of htd.
 * 
 * htd is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation, either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * htd is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with htd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTD_HTD_NAMEDTREE_HPP
#define	HTD_HTD_NAMEDTREE_HPP

#include <htd/Globals.hpp>
#include <htd/Helpers.hpp>
#include <htd/LabeledTreeFactory.hpp>
#include <htd/BidirectionalGraphNaming.hpp>
#include <htd/Label.hpp>
#include <htd/VectorAdapter.hpp>
#include <htd/NamedVertexHyperedge.hpp>
#include <htd/PostOrderTreeTraversal.hpp>

#include <vector>
#include <stdexcept>

namespace htd
{
    template<typename VertexNameType, typename EdgeNameType>
    class NamedTree
    {
        public:
            NamedTree(void) : base_(htd::LabeledTreeFactory::instance().getLabeledTree()), names_()
            {

            }

            NamedTree(const NamedTree<VertexNameType, EdgeNameType> & original) : base_(original.base_->clone()), names_(original.names_)
            {

            }

            ~NamedTree()
            {
                if (base_ != nullptr)
                {
                    delete base_;

                    base_ = nullptr;
                }
            }

            /**
             *  Getter for the number of vertices in the tree.
             *
             *  @return The number of vertices in the tree.
             */
            std::size_t vertexCount(void) const
            {
                return base_->vertexCount();
            }

            /**
             *  Getter for the number of edges in the tree.
             *
             *  @return The number of edges in the tree.
             */
            std::size_t edgeCount(void) const
            {
                return base_->edgeCount();
            }

            /**
             *  Check whether a given vertex name is indeed the name of a vertex of the tree.
             *
             *  @param[in] vertexName   The vertex name to test.
             *
             *  @return True if the given vertex name is indeed the name of a vertex of the tree, false otherwise.
             */
            bool isVertexName(const VertexNameType & vertexName) const
            {
                return names_.isVertexName(vertexName);
            }

            void setVertexName(htd::vertex_t vertex, const VertexNameType & vertexName)
            {
                if (!base_->isVertex(vertex))
                {
                    throw std::logic_error("void htd::NamedTree<VertexNameType, EdgeNameType>::setVertexName(htd::vertex_t, const VertexNameType &)");
                }

                names_.setVertexName(vertex, vertexName);
            }

            /**
             *  Check whether a given edge name is indeed the name of an edge of the tree.
             *
             *  @param[in] edgeName The edge name to test.
             *
             *  @return True if the given edge name is indeed the name of an edge of the tree, false otherwise.
             */
            bool isEdgeName(const EdgeNameType & edgeName) const
            {
                return names_.isEdgeName(edgeName);
            }

            void setEdgeName(htd::id_t edgeId, const EdgeNameType & edgeName)
            {
                if (!base_->isEdge(edgeId))
                {
                    throw std::logic_error("void htd::NamedTree<VertexNameType, EdgeNameType>::setEdgeName(htd::id_t, const EdgeNameType &)");
                }

                names_.setEdgeName(edgeId, edgeName);
            }

            const VertexNameType & vertexName(htd::vertex_t vertex) const
            {
                if (!names_.isNamedVertex(vertex))
                {
                    throw std::logic_error("const VertexNameType & htd::NamedTree<VertexNameType, EdgeNameType>::vertexName(htd::vertex_t) const");
                }

                return names_.vertexName(vertex);
            }

            const EdgeNameType & edgeName(htd::id_t edgeId) const
            {
                if (!names_.isNamedEdge(edgeId))
                {
                    throw std::logic_error("const EdgeNameType & htd::NamedTree<VertexNameType, EdgeNameType>::edgeName(htd::id_t) const");
                }

                return names_.edgeName(edgeId);
            }

            htd::vertex_t lookupVertex(const VertexNameType & vertexName) const
            {
                if (!names_.isVertexName(vertexName))
                {
                    throw std::logic_error("htd::vertex_t htd::NamedTree<VertexNameType, EdgeNameType>::lookupVertex(const VertexNameType &) const");
                }

                return names_.lookupVertex(vertexName);
            }

            htd::NamedVertexHyperedge<VertexNameType> lookupHyperedge(const EdgeNameType & edgeName) const
            {
                if (!names_.isEdgeName(edgeName))
                {
                    throw std::logic_error("htd::NamedVertexHyperedge<VertexNameType> htd::NamedTree<VertexNameType, EdgeNameType>::lookupHyperedge(const EdgeNameType &) const");
                }

                htd::id_t edgeId = names_.lookupEdge(edgeName);

                NamedVertexHyperedge<VertexNameType> ret(edgeId);

                for (htd::vertex_t vertex : base_->hyperedge(edgeId))
                {
                    ret.push_back(vertexName(vertex));
                }

                return ret;
            }

            /**
             *  Getter for the number of edges in the tree containing a specific vertex.
             *
             *  @param[in] vertexName   The name of the vertex which shall be contained in the edges.
             *
             *  @return The number of edges in the tree containing the specific vertex.
             */
            std::size_t edgeCount(const VertexNameType & vertexName) const
            {
                return base_->edgeCount(lookupVertex(vertexName));
            }

            /**
             *  Check whether a given edge is part of the tree.
             *
             *  @param[in] elements The endpoints of the hyperedge.
             *
             *  @return True if the edge is part of the tree, false otherwise.
             */
            bool isEdge(const std::vector<VertexNameType> & elements)
            {
                return isEdge(htd::ConstCollection<VertexNameType>::getInstance(elements));
            }

            /**
             *  Check whether a given edge is part of the tree.
             *
             *  @param[in] elements The endpoints of the hyperedge.
             *
             *  @return True if the edge is part of the tree, false otherwise.
             */
            bool isEdge(const htd::ConstCollection<VertexNameType> & elements)
            {
                bool ok = true;

                for (auto it = elements.begin(); ok && it != elements.end(); it++)
                {
                    if (!isVertex(*it))
                    {
                        ok = false;
                    }
                }

                if (ok)
                {
                    ok = base_->isEdge(elements);
                }

                return ok;
            }

            /**
             *  Get the associated edge ID.
             *
             *  @param[in] edgeName The name of the edge.
             *
             *  @return The ID of the edge with the given name.
             */
            htd::id_t associatedEdgeId(const EdgeNameType & edgeName) const
            {
                if (!names_.isEdgeName(edgeName))
                {
                    throw std::logic_error("htd::id_t htd::NamedTree<VertexNameType, EdgeNameType>::associatedEdgeId(const EdgeNameType &) const");
                }

                return names_.lookupEdge(edgeName);
            }

            /**
             *  Get all associated edge IDs.
             *
             *  @param[in] vertexName1  The first endpoint of the edge.
             *  @param[in] vertexName2  The second endpoint of the edge.
             *
             *  @return The collection of all edges containing exactly the provided endpoints in the given order.
             */
            htd::ConstCollection<htd::id_t> associatedEdgeIds(const VertexNameType & vertexName1, const VertexNameType & vertexName2) const
            {
                if (isVertexName(vertexName1) && isVertexName(vertexName2))
                {
                    return base_->associatedEdgeIds(lookupVertex(vertexName1), lookupVertex(vertexName2));
                }

                return htd::ConstCollection<htd::id_t>::getInstance(htd::VectorAdapter<htd::id_t>());
            }

            /**
             *  Access the collection of all vertices in the tree.
             *
             *  @return The collection of all vertices in the tree sorted in ascending order.
             */
            htd::ConstCollection<VertexNameType> vertices(void) const
            {
                htd::VectorAdapter<VertexNameType> ret;

                std::vector<VertexNameType> & container = ret.container();

                for (htd::vertex_t vertex : base_->vertices())
                {
                    container.push_back(vertexName(vertex));
                }

                return htd::ConstCollection<VertexNameType>::getInstance(ret);
            }

            /**
             *  Access the vertex at the specific position.
             *
             *  @param[in] index    The position of the vertex.
             *
             *  @return The vertex at the specific position.
             */
            const VertexNameType & vertexAtPosition(htd::index_t index) const
            {
                return vertexName(base_->vertexAtPosition(index));
            }

            /**
             *  Check whether the tree is connected.
             *
             *  @note This function always returns true as a tree is connected by definition.
             *
             *  @return True if the tree is connected, false otherwise.
             */
            bool isConnected(void) const
            {
                return base_->isConnected();
            }

            /**
             *  Check whether two vertices are contained in the same connected component.
             *
             *  @param[in] vertexName1  The first vertex.
             *  @param[in] vertexName2  The second vertex.
             *
             *  @return True if the two vertices are contained in the same connected component, false otherwise.
             */
            bool isConnected(const VertexNameType & vertexName1, const VertexNameType & vertexName2) const
            {
                return base_->isConnected(lookupVertex(vertexName1), lookupVertex(vertexName2));
            }

            /**
             *  Check whether two vertices are neighbors.
             *
             *  @param[in] vertexName1  The first vertex.
             *  @param[in] vertexName2  The second vertex.
             *
             *  @return True if the two vertices are neighbors, false otherwise.
             */
            bool isNeighbor(const VertexNameType & vertexName1, const VertexNameType & vertexName2) const
            {
                return base_->isNeighbor(lookupVertex(vertexName1), lookupVertex(vertexName2));
            }

            /**
             *  Getter for the number of neighbors of a given vertex in the tree.
             *
             *  @param[in] vertexName   The name of the vertex for which the number of neighbors shall be computed.
             *
             *  @return The number of neighbors of the given vertex in the tree.
             */
            std::size_t neighborCount(const VertexNameType & vertexName) const
            {
                return base_->neighborCount(lookupVertex(vertexName));
            }

            /**
             *  Access the collection of all neighbors of a given vertex in the tree.
             *
             *  @param[in] vertexName   The name of the vertex for which the collection of neighbors shall be returned.
             *
             *  @return The collection of all neighbors of the given vertex in the tree in ascending order.
             */
            htd::ConstCollection<VertexNameType> neighbors(const VertexNameType & vertexName) const
            {
                htd::VectorAdapter<VertexNameType> ret;

                std::vector<VertexNameType> & container = ret.container();

                for (htd::vertex_t neighbor : base_->neighbors(lookupVertex(vertexName)))
                {
                    container.push_back(this->vertexName(neighbor));
                }

                return htd::ConstCollection<VertexNameType>::getInstance(ret);
            }

            /**
             *  Access the neighbor at the specific position.
             *
             *  @param[in] vertexName   The name of the vertex for which the neighbor shall be returned.
             *  @param[in] index        The position of the neighbor.
             *
             *  @return The neighbor of the given vertex at the specific position.
             */
            const VertexNameType & neighborAtPosition(const VertexNameType & vertexName, htd::index_t index) const
            {
                return vertexName(base_->neighborAtPosition(lookupVertex(vertexName), index));
            }

            /**
             *  Getter for the number of isolated vertices in the tree.
             *
             *  @return The number of isolated vertices in the tree.
             */
            std::size_t isolatedVertexCount(void) const
            {
                return base_->isolatedVertexCount();
            }

            /**
             *  Access the collection of all isolated vertices in the tree.
             *
             *  @return The collection of all isolated vertices in the tree sorted in ascending order.
             */
            htd::ConstCollection<VertexNameType> isolatedVertices(void) const
            {
                htd::VectorAdapter<VertexNameType> ret;

                std::vector<VertexNameType> & container = ret.container();

                for (htd::vertex_t isolatedVertex : base_->isolatedVertices())
                {
                    container.push_back(vertexName(isolatedVertex));
                }

                return htd::ConstCollection<VertexNameType>::getInstance(ret);
            }

            /**
             *  Access the isolated vertex at the specific position.
             *
             *  @param[in] index    The position of the isolated vertex.
             *
             *  @return The isolated vertex at the specific position.
             */
            const VertexNameType & isolatedVertexAtPosition(htd::index_t index) const
            {
                return vertexName(base_->isolatedVertexAtPosition(index));
            }

            /**
             *  Check whether a given vertex is isolated.
             *
             *  @param[in] vertexName   The name of the vertex to test for isolation.
             *
             *  @return True if the vertex has no neighbors in the tree, false otherwise.
             */
            bool isIsolatedVertex(const VertexNameType & vertexName) const
            {
                return base_->isIsolatedVertex(lookupVertex(vertexName));
            }

            /**
             *  Access the collection of all hyperedges in the tree.
             *
             *  @return The collection of all hyperedges in the tree sorted by ID in ascending order.
             */
            htd::ConstCollection<NamedVertexHyperedge<VertexNameType>> hyperedges(void) const
            {
                htd::VectorAdapter<NamedVertexHyperedge<VertexNameType>> ret;

                std::vector<NamedVertexHyperedge<VertexNameType>> & container = ret.container();

                for (const htd::Hyperedge & hyperedge : base_->hyperedges())
                {
                    container.push_back(NamedVertexHyperedge<VertexNameType>(hyperedge.id()));

                    NamedVertexHyperedge<VertexNameType> & newHyperedge = container[container.size() - 1];

                    for (htd::vertex_t vertex : hyperedge)
                    {
                        newHyperedge.push_back(vertexName(vertex));
                    }
                }

                return htd::ConstCollection<NamedVertexHyperedge<VertexNameType>>::getInstance(ret);
            }

            /**
             *  Access the collection of all hyperedges containing a specific vertex.
             *
             *  @param[in] vertexName   The name of the vertex which shall be contained in the edges.
             *
             *  @return The collection of all hyperedges containing the specific vertex sorted by ID in ascending order.
             */
            htd::ConstCollection<NamedVertexHyperedge<VertexNameType>> hyperedges(const VertexNameType & vertexName) const
            {
                htd::VectorAdapter<NamedVertexHyperedge<VertexNameType>> ret;

                std::vector<NamedVertexHyperedge<VertexNameType>> & container = ret.container();

                for (const htd::Hyperedge & hyperedge : base_->hyperedges(lookupVertex(vertexName)))
                {
                    container.push_back(NamedVertexHyperedge<VertexNameType>(hyperedge.id()));

                    NamedVertexHyperedge<VertexNameType> & newHyperedge = container[container.size() - 1];

                    for (htd::vertex_t vertex : hyperedge)
                    {
                        newHyperedge.push_back(vertexName(vertex));
                    }
                }

                return htd::ConstCollection<NamedVertexHyperedge<VertexNameType>>::getInstance(ret);
            }

            /**
             *  Access the hyperedge with a specific ID.
             *
             *  @param[in] edgeId   The ID of the hyperedge.
             *
             *  @return The hyperedge with the specific ID.
             */
            NamedVertexHyperedge<VertexNameType> hyperedge(htd::id_t edgeId) const
            {
                NamedVertexHyperedge<VertexNameType> ret(edgeId);

                for (htd::vertex_t vertex : base_->hyperedge(edgeId))
                {
                    ret.push_back(vertexName(vertex));
                }

                return ret;
            }

            /**
             *  Access the hyperedge at the specific position. Edges not containing the given vertex are ignored.
             *
             *  @param[in] index        The position of the hyperedge.
             *  @param[in] vertexName   The name of the vertex which shall be contained in the edge.
             *
             *  @return The hyperedge at the specific position. Edges not containing the given vertex are ignored.
             */
            NamedVertexHyperedge<VertexNameType> hyperedgeAtPosition(htd::index_t index, const VertexNameType & vertexName) const
            {
                const htd::Hyperedge & hyperedge = base_->hyperedgeAtPosition(index, lookupVertex(vertexName));

                NamedVertexHyperedge<VertexNameType> ret(hyperedge.id());

                for (htd::vertex_t vertex : hyperedge)
                {
                    ret.push_back(vertexName(vertex));
                }

                return ret;
            }

            /**
             *  Remove a vertex from the tree.
             *
             *  @note This operation retains the tree structure by connecting the neighbors of the removed vertex in a valid way.
             *
             *  @param[in] vertexName   The name of the vertex which shall be removed.
             */
            void removeVertex(const VertexNameType & vertexName)
            {
                if (isVertexName(vertexName))
                {
                    htd::vertex_t locatedVertex = lookupVertex(vertexName);

                    names_.removeVertexName(locatedVertex);

                    base_->removeVertex(locatedVertex);
                }
            }

            void removeSubtree(const VertexNameType & vertexName)
            {
                if (isVertexName(vertexName))
                {
                    htd::vertex_t locatedVertex = lookupVertex(vertexName);

                    htd::PostOrderTreeTraversal treeTraversal;

                    treeTraversal.traverse(*this, [&](htd::vertex_t vertex, htd::vertex_t parent, std::size_t distanceToSubtreeRoot)
                    {
                        HTD_UNUSED(parent)
                        HTD_UNUSED(distanceToSubtreeRoot)

                        names_.removeVertexName(vertex);
                    }, locatedVertex);

                    base_->removeSubtree(locatedVertex);
                }
            }

            htd::vertex_t insertRoot(const VertexNameType & vertexName)
            {
                if (base_->vertexCount() > 0)
                {
                    names_.clear();

                    base_->removeSubtree(base_->root());
                }

                htd::vertex_t ret = base_->insertRoot();

                names_.setVertexName(ret, vertexName);

                return ret;
            }

            void removeRoot(void)
            {
                if (base_->vertexCount() > 0)
                {
                    names_.clear();

                    base_->removeRoot();
                }
            }

            htd::vertex_t addChild(const VertexNameType & vertexName, const VertexNameType & childName)
            {
                if (!isVertexName(vertexName) || isVertexName(childName))
                {
                    throw std::logic_error("htd::vertex_t addChild(const VertexNameType &, const VertexNameType &)");
                }

                htd::vertex_t ret = base_->addChild(lookupVertex(vertexName));

                names_.setVertexName(ret, childName);

                return ret;
            }

            void removeChild(const VertexNameType & vertexName, const VertexNameType & childName)
            {
                if (!isVertexName(vertexName) || !isVertexName(childName))
                {
                    throw std::logic_error("void removeChild(const VertexNameType &, const VertexNameType &)");
                }

                htd::vertex_t child = lookupVertex(childName);

                base_->removeChild(lookupVertex(vertexName), child);

                names_.removeVertexName(child);
            }

            htd::vertex_t addParent(const VertexNameType & vertexName, const VertexNameType & parentName)
            {
                if (!isVertexName(vertexName) || isVertexName(parentName))
                {
                    throw std::logic_error("htd::vertex_t addParent(const VertexNameType &, const VertexNameType &)");
                }

                htd::vertex_t ret = base_->addParent(lookupVertex(vertexName));

                names_.setVertexName(ret, parentName);

                return ret;
            }

            void setParent(const VertexNameType & vertexName, const VertexNameType & newParentName)
            {
                base_->setParent(lookupVertex(vertexName), lookupVertex(newParentName));
            }

            /**
             *  Getter for the number of different label names in the graph.
             *
             *  @return The number of different label names in the graph.
             */
            std::size_t labelCount(void) const
            {
                return base_->labelCount();
            }

            /**
             *  Access the collection of all label names used in the tree.
             *
             *  @return The collection of all label names used in the tree sorted in ascending order.
             */
            htd::ConstCollection<std::string> labelNames(void) const
            {
                return base_->labelNames();
            }

            /**
             *  Access the label name at the specific position.
             *
             *  @param[in] index    The position of the label name.
             *
             *  @return The label name at the specific position.
             */
            const std::string & labelNameAtPosition(htd::index_t index) const
            {
                return base_->labelNameAtPosition(index);
            }

            /**
             *  Access the label associated with the given vertex.
             *
             *  @param[in] labelName    The name of the label.
             *  @param[in] vertexName   The name of the vertex.
             *
             *  @return The label associated with the given vertex.
             */
            const htd::ILabel & vertexLabel(const std::string & labelName, const VertexNameType & vertexName) const
            {
                return base_->vertexLabel(labelName, lookupVertex(vertexName));
            }

            /**
             *  Access the label associated with the given edge.
             *
             *  @param[in] labelName    The name of the label.
             *  @param[in] edgeId       The ID of the edge.
             *
             *  @return The label associated with the given edge.
             */
            const htd::ILabel & edgeLabel(const std::string & labelName, htd::id_t edgeId) const
            {
                return base_->edgeLabel(labelName, edgeId);
            }

            /**
             *  Access the label associated with the given edge.
             *
             *  @param[in] labelName    The name of the label.
             *  @param[in] edgeName     The name of the edge.
             *
             *  @return The label associated with the given edge.
             */
            const htd::ILabel & edgeLabel(const std::string & labelName, const EdgeNameType & edgeName) const
            {
                return base_->edgeLabel(labelName, names_.lookupEdge(edgeName));
            }

            /**
             *  Set the label associated with the given vertex.
             *
             *  If the vertex is already labeled, the existing label will
             *  be replaced and the memory of the previous label is freed.
             *
             *  @note When calling this method the control over the memory region of the new label is transferred to the
             *  tree. Deleting the label outside the tree or assigning the same label object to multiple vertices or
             *  edges will lead to undefined behavior.
             *
             *  @param[in] labelName    The name of the new label.
             *  @param[in] vertexName   The name of the vertex to be labeled.
             *  @param[in] label        The new label.
             */
            void setVertexLabel(const std::string & labelName, const VertexNameType & vertexName, htd::ILabel * label)
            {
                base_->setVertexLabel(labelName, lookupVertex(vertexName), label);
            }

            /**
             *  Set the label associated with the given edge.
             *
             *  If the edge is already labeled, the existing label will
             *  be replaced and the memory of the previous label is freed.
             *
             *  @note When calling this method the control over the memory region of the new label is transferred to the
             *  tree. Deleting the label outside the tree or assigning the same label object to multiple vertices or
             *  edges will lead to undefined behavior.
             *
             *  @param[in] labelName    The name of the new label.
             *  @param[in] edgeId       The ID of the edge to be labeled.
             *  @param[in] label        The new label.
             */
            void setEdgeLabel(const std::string & labelName, htd::id_t edgeId, htd::ILabel * label)
            {
                base_->setEdgeLabel(labelName, edgeId, label);
            }

            /**
             *  Set the label associated with the given edge.
             *
             *  If the edge is already labeled, the existing label will
             *  be replaced and the memory of the previous label is freed.
             *
             *  @note When calling this method the control over the memory region of the new label is transferred to the
             *  tree. Deleting the label outside the tree or assigning the same label object to multiple vertices or
             *  edges will lead to undefined behavior.
             *
             *  @param[in] labelName    The name of the new label.
             *  @param[in] edgeName     The name of the edge to be labeled.
             *  @param[in] label        The new label.
             */
            void setEdgeLabel(const std::string & labelName, const EdgeNameType & edgeName, htd::ILabel * label)
            {
                base_->setEdgeLabel(labelName, names_.lookupEdge(edgeName), label);
            }

            /**
             *  Remove the label associated with the given vertex.
             *
             *  @param[in] labelName    The name of the label which shall be removed.
             *  @param[in] vertexName   The name of the vertex.
             */
            void removeVertexLabel(const std::string & labelName, const VertexNameType & vertexName)
            {
                base_->removeVertexLabel(labelName, lookupVertex(vertexName));
            }

            /**
             *  Remove the label associated with the given edge.
             *
             *  @param[in] labelName    The name of the label which shall be removed.
             *  @param[in] edgeId       The ID of the edge.
             */
            void removeEdgeLabel(const std::string & labelName, htd::id_t edgeId)
            {
                base_->removeEdgeLabel(labelName, edgeId);
            }

            /**
             *  Remove the label associated with the given edge.
             *
             *  @param[in] labelName    The name of the label which shall be removed.
             *  @param[in] edgeName     The name of the edge.
             */
            void removeEdgeLabel(const std::string & labelName, const EdgeNameType & edgeName)
            {
                base_->removeEdgeLabel(labelName, names_.lookupEdge(edgeName));
            }

            /**
             *  Swap the labels of two vertices.
             *
             *  @param[in] vertexName1  The name of the first vertex.
             *  @param[in] vertexName2  The name of the second vertex.
             */
            void swapVertexLabels(const VertexNameType & vertexName1, const VertexNameType & vertexName2)
            {
                base_->swapVertexLabels(lookupVertex(vertexName1), lookupVertex(vertexName2));
            }

            /**
             *  Swap the labels of two edges.
             *
             *  @param[in] edgeId1  The ID of the first edge.
             *  @param[in] edgeId2  The ID of the second edge.
             */
            void swapEdgeLabels(htd::id_t edgeId1, htd::id_t edgeId2)
            {
                base_->swapEdgeLabels(edgeId1, edgeId2);
            }

            /**
             *  Swap the labels of two edges.
             *
             *  @param[in] edgeName1    The name of the first edge.
             *  @param[in] edgeName2    The name of the second edge.
             */
            void swapEdgeLabels(const EdgeNameType & edgeName1, const EdgeNameType & edgeName2)
            {
                base_->swapEdgeLabels(names_.lookupEdge(edgeName1), names_.lookupEdge(edgeName2));
            }

            /**
             *  Swap the labels of two vertices.
             *
             *  @param[in] labelName    The name of the label which shall be swapped.
             *  @param[in] vertexName1  The name of the first vertex.
             *  @param[in] vertexName2  The name of the second vertex.
             */
            void swapVertexLabel(const std::string & labelName, const VertexNameType & vertexName1, const VertexNameType & vertexName2)
            {
                base_->swapVertexLabel(labelName, lookupVertex(vertexName1), lookupVertex(vertexName2));
            }

            /**
             *  Swap the labels of two edges.
             *
             *  @param[in] labelName    The name of the label which shall be swapped.
             *  @param[in] edgeId1      The ID of the first edge.
             *  @param[in] edgeId2      The ID of the second edge.
             */
            void swapEdgeLabel(const std::string & labelName, htd::id_t edgeId1, htd::id_t edgeId2)
            {
                base_->swapEdgeLabel(labelName, edgeId1, edgeId2);
            }

            /**
             *  Swap the labels of two edges.
             *
             *  @param[in] labelName    The name of the label which shall be swapped.
             *  @param[in] edgeName1    The name of the first edge.
             *  @param[in] edgeName2    The name of the second edge.
             */
            void swapEdgeLabel(const std::string & labelName, const EdgeNameType & edgeName1, const EdgeNameType & edgeName2)
            {
                base_->swapEdgeLabel(labelName, names_.lookupEdge(edgeName1), names_.lookupEdge(edgeName2));
            }

            /**
             *  Create a deep copy the current tree.
             *
             *  @return A new NamedTree object identical to the current path.
             */
            NamedTree<VertexNameType, EdgeNameType> * clone(void) const
            {
                return new NamedTree<VertexNameType, EdgeNameType>(*this);
            }

            const htd::ILabeledTree & internalGraph(void) const
            {
                return *base_;
            }

        private:
            htd::IMutableLabeledTree * base_;

            htd::BidirectionalGraphNaming<VertexNameType, EdgeNameType> names_;
    };
}

#endif /* HTD_HTD_NAMEDTREE_HPP */
