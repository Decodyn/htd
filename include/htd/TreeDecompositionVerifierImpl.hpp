/* 
 * File:   TreeDecompositionVerifierImpl.hpp
 *
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015, Michael Abseher
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

#ifndef HTD_HTD_TREEDECOMPOSITIONVERIFIERIMPL_HPP
#define	HTD_HTD_TREEDECOMPOSITIONVERIFIERIMPL_HPP

#include <htd/Globals.hpp>
#include <htd/IHypergraph.hpp>
#include <htd/ILabeledTree.hpp>
#include <htd/ITreeDecompositionVerifier.hpp>

namespace htd
{
    class TreeDecompositionVerifierImpl : public htd::ITreeDecompositionVerifier
    {
        public:
            TreeDecompositionVerifierImpl(void);
                        
            ~TreeDecompositionVerifierImpl();
                        
            bool verify(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition) const HTD_OVERRIDE;
            
            bool verifyVertexExistence(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition) const HTD_OVERRIDE;
            
            bool verifyHyperEdgeCoverage(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition) const HTD_OVERRIDE;
            
            bool verifyConnectednessCriterion(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition) const HTD_OVERRIDE;
            
            void getViolationsVertexExistence(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition, htd::vertex_container & output) const HTD_OVERRIDE;
            
            void getViolationsHyperEdgeCoverage(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition, htd::hyperedge_container & output) const HTD_OVERRIDE;
            
            void getViolationsConnectednessCriterion(const htd::IHypergraph & graph, const htd::ILabeledTree & decomposition, htd::vertex_container & output) const HTD_OVERRIDE;

        protected:
            TreeDecompositionVerifierImpl & operator=(const TreeDecompositionVerifierImpl &) { return *this; }

        private:
            void getReachableVertices(htd::vertex_t start, const htd::ILabeledTree & decomposition, const htd::vertex_container & filter, htd::vertex_container & output) const;
    };
}

#endif /* HTD_HTD_TREEDECOMPOSITIONVERIFIERIMPL_HPP */
