/* 
 * File:   WidthMinimizingTreeDecompositionAlgorithm.cpp
 * 
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2017, Michael Abseher
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

#ifndef HTD_HTD_WIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_CPP
#define HTD_HTD_WIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_CPP

#include <htd/WidthMinimizingTreeDecompositionAlgorithm.hpp>
#include <htd/BucketEliminationTreeDecompositionAlgorithm.hpp>

#include <cstdarg>
#include <algorithm>

/**
 *  Private implementation details of class htd::WidthMinimizingTreeDecompositionAlgorithm.
 */
struct htd::WidthMinimizingTreeDecompositionAlgorithm::Implementation
{
    /**
     *  Constructor for the implementation details structure.
     *
     *  @param[in] manager  The management instance to which the current object instance belongs.
     */
    Implementation(const htd::LibraryInstance * const manager)
        : managementInstance_(manager), algorithm_(new htd::BucketEliminationTreeDecompositionAlgorithm(manager)), iterationCount_(1), nonImprovementLimit_(-1)
    {

    }

    /**
     *  Constructor for the implementation details structure.
     *
     *  @param[in] manager                  The management instance to which the current object instance belongs.
     *  @param[in] manipulationOperations   The manipulation operations which shall be applied globally to each decomposition generated by the algorithm.
     */
    Implementation(const htd::LibraryInstance * const manager, const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations)
        : managementInstance_(manager), algorithm_(new htd::BucketEliminationTreeDecompositionAlgorithm(manager, manipulationOperations)), iterationCount_(1), nonImprovementLimit_(-1)
    {

    }

    /**
     *  Copy constructor for the implementation details structure.
     *
     *  @param[in] original The original implementation details structure.
     */
    Implementation(const Implementation & original) : managementInstance_(original.managementInstance_), algorithm_(original.algorithm_->clone()), iterationCount_(original.iterationCount_), nonImprovementLimit_(original.nonImprovementLimit_)
    {

    }

    virtual ~Implementation()
    {
        delete algorithm_;
    }

    /**
     *  The management instance to which the current object instance belongs.
     */
    const htd::LibraryInstance * managementInstance_;

    /**
     *  The decomposition algorithm which will be called repeatedly.
     */
    htd::BucketEliminationTreeDecompositionAlgorithm * algorithm_;

    /**
     *  The number of iterations which shall be performed.
     */
    std::size_t iterationCount_;

    /**
     *  The maximum number of iterations without improvement after which the algorithm shall terminate.
     */
    std::size_t nonImprovementLimit_;
};

htd::WidthMinimizingTreeDecompositionAlgorithm::WidthMinimizingTreeDecompositionAlgorithm(const htd::LibraryInstance * const manager) : implementation_(new Implementation(manager))
{

}

htd::WidthMinimizingTreeDecompositionAlgorithm::WidthMinimizingTreeDecompositionAlgorithm(const htd::LibraryInstance * const manager, const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) : implementation_(new Implementation(manager, manipulationOperations))
{

}

htd::WidthMinimizingTreeDecompositionAlgorithm::WidthMinimizingTreeDecompositionAlgorithm(const htd::WidthMinimizingTreeDecompositionAlgorithm & original) : implementation_(new Implementation(*(original.implementation_)))
{

}

htd::WidthMinimizingTreeDecompositionAlgorithm::~WidthMinimizingTreeDecompositionAlgorithm()
{

}

htd::ITreeDecomposition * htd::WidthMinimizingTreeDecompositionAlgorithm::computeDecomposition(const htd::IMultiHypergraph & graph) const
{
    return computeDecomposition(graph, std::vector<htd::IDecompositionManipulationOperation *>(), [](const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, std::size_t){});
}

htd::ITreeDecomposition * htd::WidthMinimizingTreeDecompositionAlgorithm::computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                                               const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, std::size_t)> & progressCallback) const
{
    return computeDecomposition(graph, std::vector<htd::IDecompositionManipulationOperation *>(), progressCallback);
}

htd::ITreeDecomposition * htd::WidthMinimizingTreeDecompositionAlgorithm::computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                                               const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations) const
{
    return computeDecomposition(graph, manipulationOperations, [](const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, std::size_t){});
}

htd::ITreeDecomposition * htd::WidthMinimizingTreeDecompositionAlgorithm::computeDecomposition(const htd::IMultiHypergraph & graph,
                                                                                               const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations,
                                                                                               const std::function<void(const htd::IMultiHypergraph &, const htd::ITreeDecomposition &, std::size_t)> & progressCallback) const
{
    std::size_t bestMaxBagSize = (std::size_t)-1;

    const htd::LibraryInstance & managementInstance = *(implementation_->managementInstance_);

    htd::ITreeDecomposition * ret = nullptr;

    htd::index_t iteration = 0;

    while ((iteration == 0 || implementation_->iterationCount_ == 0 || iteration < implementation_->iterationCount_) && !managementInstance.isTerminated())
    {
        std::vector<htd::IDecompositionManipulationOperation *> clonedManipulationOperations;

        for (const htd::IDecompositionManipulationOperation * operation : manipulationOperations)
        {
#ifndef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
            clonedManipulationOperations.push_back(operation->clone());
#else
            clonedManipulationOperations.push_back(operation->cloneDecompositionManipulationOperation());
#endif
        }

        std::size_t remainingIterations = implementation_->iterationCount_ - iteration;

        if (implementation_->nonImprovementLimit_ < (std::size_t)-1)
        {
            remainingIterations = std::min(remainingIterations, implementation_->nonImprovementLimit_);
        }

        std::pair<htd::ITreeDecomposition *, std::size_t> decompositionResult = implementation_->algorithm_->computeDecomposition(graph, clonedManipulationOperations, bestMaxBagSize - 1, remainingIterations);

        htd::ITreeDecomposition * currentDecomposition = decompositionResult.first;

        iteration += decompositionResult.second;

        if (decompositionResult.second < implementation_->nonImprovementLimit_)
        {
            iteration += decompositionResult.second;
        }
        else
        {
            iteration = (std::size_t)-1;
        }

        if (currentDecomposition != nullptr)
        {
            if (!managementInstance.isTerminated())
            {
                std::size_t currentMaxBagSize = currentDecomposition->maximumBagSize();

                if (!managementInstance.isTerminated())
                {
                    progressCallback(graph, *currentDecomposition, currentMaxBagSize);

                    if (iteration == 0 || currentMaxBagSize < bestMaxBagSize)
                    {
                        if (iteration > 0)
                        {
                            delete ret;
                        }

                        ret = currentDecomposition;

                        bestMaxBagSize = currentMaxBagSize;
                    }
                    else
                    {
                        delete currentDecomposition;
                    }
                }
                else
                {
                    delete currentDecomposition;
                }
            }
            else
            {
                delete currentDecomposition;
            }
        }
    }

    for (htd::IDecompositionManipulationOperation * operation : manipulationOperations)
    {
        delete operation;
    }

    return ret;
}

htd::ITreeDecomposition * htd::WidthMinimizingTreeDecompositionAlgorithm::computeDecomposition(const htd::IMultiHypergraph & graph, int manipulationOperationCount, ...) const
{
    va_list arguments;

    va_start(arguments, manipulationOperationCount);

    std::vector<htd::IDecompositionManipulationOperation *> manipulationOperations;

    for (int manipulationOperationIndex = 0; manipulationOperationIndex < manipulationOperationCount; manipulationOperationIndex++)
    {
        manipulationOperations.push_back(va_arg(arguments, htd::IDecompositionManipulationOperation *));
    }

    va_end(arguments);

    return computeDecomposition(graph, manipulationOperations);
}


void htd::WidthMinimizingTreeDecompositionAlgorithm::setManipulationOperations(const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations)
{
    implementation_->algorithm_->setManipulationOperations(manipulationOperations);
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::addManipulationOperation(htd::IDecompositionManipulationOperation * manipulationOperation)
{
    implementation_->algorithm_->addManipulationOperation(manipulationOperation);
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::addManipulationOperations(const std::vector<htd::IDecompositionManipulationOperation *> & manipulationOperations)
{
    implementation_->algorithm_->addManipulationOperations(manipulationOperations);
}

bool htd::WidthMinimizingTreeDecompositionAlgorithm::isSafelyInterruptible(void) const
{
    return true;
}

std::size_t htd::WidthMinimizingTreeDecompositionAlgorithm::iterationCount(void) const
{
    return implementation_->iterationCount_;
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::setIterationCount(std::size_t iterationCount)
{
    implementation_->iterationCount_ = iterationCount;
}

std::size_t htd::WidthMinimizingTreeDecompositionAlgorithm::nonImprovementLimit(void) const
{
    return implementation_->nonImprovementLimit_;
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::setNonImprovementLimit(std::size_t nonImprovementLimit)
{
    implementation_->nonImprovementLimit_ = nonImprovementLimit;
}

bool htd::WidthMinimizingTreeDecompositionAlgorithm::isCompressionEnabled(void) const
{
    return implementation_->algorithm_->isCompressionEnabled();
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::setCompressionEnabled(bool compressionEnabled)
{
    implementation_->algorithm_->setCompressionEnabled(compressionEnabled);
}

bool htd::WidthMinimizingTreeDecompositionAlgorithm::isComputeInducedEdgesEnabled(void) const
{
    return implementation_->algorithm_->isComputeInducedEdgesEnabled();
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::setComputeInducedEdgesEnabled(bool computeInducedEdgesEnabled)
{
    implementation_->algorithm_->setComputeInducedEdgesEnabled(computeInducedEdgesEnabled);
}

const htd::LibraryInstance * htd::WidthMinimizingTreeDecompositionAlgorithm::managementInstance(void) const HTD_NOEXCEPT
{
    return implementation_->managementInstance_;
}

void htd::WidthMinimizingTreeDecompositionAlgorithm::setManagementInstance(const htd::LibraryInstance * const manager)
{
    HTD_ASSERT(manager != nullptr)

    implementation_->managementInstance_ = manager;
}

htd::WidthMinimizingTreeDecompositionAlgorithm * htd::WidthMinimizingTreeDecompositionAlgorithm::clone(void) const
{
    return new htd::WidthMinimizingTreeDecompositionAlgorithm(*this);
}

#endif /* HTD_HTD_WIDTHMINIMIZINGTREEDECOMPOSITIONALGORITHM_CPP */
