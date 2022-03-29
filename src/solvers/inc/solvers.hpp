#pragma once

#ifndef SOLVERS_HEADER
#define SOLVERS_HEADER

#include <randomx.h>

#include "../../utilities/inc/utilities.hpp"
#include "../../hashes/inc/hashes.hpp"

/**
 * @brief Solvers for algorithm
 * 
 * @author GerrFrog
 */
namespace Solvers
{
    /**
     * @brief Abstract class for solver object
     * 
     * @author GerrFrog
     */
    class Solver
    {
        // TODO: Implement solver for algorithm
        private:

        protected:

        public:
            /**
             * @brief Construct a new Solver object
             * 
             * @author GerrFrog
             */
            Solver() = default;

            /**
             * @brief Destroy the Solver object
             * 
             * @author GerrFrog
             */
            ~Solver() = default;
    };
}























#endif