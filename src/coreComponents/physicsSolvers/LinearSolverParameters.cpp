/*
 * ------------------------------------------------------------------------------------------------------------
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * Copyright (c) 2018-2020 Lawrence Livermore National Security LLC
 * Copyright (c) 2018-2020 The Board of Trustees of the Leland Stanford Junior University
 * Copyright (c) 2018-2020 Total, S.A
 * Copyright (c) 2019-     GEOSX Contributors
 * All rights reserved
 *
 * See top level LICENSE, COPYRIGHT, CONTRIBUTORS, NOTICE, and ACKNOWLEDGEMENTS files for details.
 * ------------------------------------------------------------------------------------------------------------
 */

/**
 * @file LinearSolverParameters.cpp
 */

#include "LinearSolverParameters.hpp"

namespace geosx
{
using namespace dataRepository;

LinearSolverParametersInput::LinearSolverParametersInput( std::string const & name,
                                                          Group * const parent )
  :
  Group( name, parent )
{
  setInputFlags( InputFlags::OPTIONAL );
  enableLogLevelInput();

  registerWrapper( viewKeyStruct::solverTypeString, &m_parameters.solverType )->
    setApplyDefaultValue( m_parameters.solverType )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Linear solver type. Available options are:\n* " + EnumStrings< LinearSolverParameters::SolverType >::concat( "\n* " ) );

  registerWrapper( viewKeyStruct::preconditionerTypeString, &m_parameters.preconditionerType )->
    setApplyDefaultValue( m_parameters.preconditionerType )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Preconditioner type. Available options are:\n* " + EnumStrings< LinearSolverParameters::PreconditionerType >::concat( "\n* " ) );

  registerWrapper( viewKeyStruct::krylovMaxIterString, &m_parameters.krylov.maxIterations )->
    setApplyDefaultValue( m_parameters.krylov.maxIterations )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Maximum iterations allowed for an iterative solver" );

  registerWrapper( viewKeyStruct::krylovMaxRestartString, &m_parameters.krylov.maxRestart )->
    setApplyDefaultValue( m_parameters.krylov.maxRestart )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Maximum iterations before restart (GMRES only)" );

  registerWrapper( viewKeyStruct::krylovTolString, &m_parameters.krylov.relTolerance )->
    setApplyDefaultValue( m_parameters.krylov.relTolerance )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Relative convergence tolerance of the iterative method\n"
                    "If the method converges, the iterative solution :math:`\\mathsf{x}_k` is such that\n"
                    "the relative residual norm satisfies:\n"
                    ":math:`\\left\\lVert \\mathsf{b} - \\mathsf{A} \\mathsf{x}_k \\right\\rVert_2` < ``" +
                    std::string( viewKeyStruct::krylovTolString ) + "`` * :math:`\\left\\lVert\\mathsf{b}\\right\\rVert_2`" );

  registerWrapper( viewKeyStruct::krylovAdaptiveTolString, &m_parameters.krylov.useAdaptiveTol )->
    setApplyDefaultValue( m_parameters.krylov.useAdaptiveTol )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Use Eisenstat-Walker adaptive linear tolerance" );

  registerWrapper( viewKeyStruct::krylovWeakTolString, &m_parameters.krylov.weakestTol )->
    setApplyDefaultValue( m_parameters.krylov.weakestTol )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "Weakest-allowed tolerance for adaptive method" );

  registerWrapper( viewKeyStruct::amgNumSweepsString, &m_parameters.amg.numSweeps )->
    setApplyDefaultValue( m_parameters.amg.numSweeps )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "AMG smoother sweeps" );

  registerWrapper( viewKeyStruct::amgSmootherString, &m_parameters.amg.smootherType )->
    setApplyDefaultValue( m_parameters.amg.smootherType )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "AMG smoother type\n"
                    "Available options are: jacobi, blockJacobi, gaussSeidel, blockGaussSeidel, chebyshev, icc, ilu, ilut" );

  registerWrapper( viewKeyStruct::amgCoarseString, &m_parameters.amg.coarseType )->
    setApplyDefaultValue( m_parameters.amg.coarseType )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "AMG coarsest level solver/smoother type\n"
                    "Available options are: jacobi, gaussSeidel, blockGaussSeidel, chebyshev, direct" );

  registerWrapper( viewKeyStruct::amgThresholdString, &m_parameters.amg.threshold )->
    setApplyDefaultValue( m_parameters.amg.threshold )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "AMG strength-of-connection threshold" );

  registerWrapper( viewKeyStruct::iluFillString, &m_parameters.ilu.fill )->
    setApplyDefaultValue( m_parameters.ilu.fill )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "ILU(K) fill factor" );

  registerWrapper( viewKeyStruct::iluThresholdString, &m_parameters.ilu.threshold )->
    setApplyDefaultValue( m_parameters.ilu.threshold )->
    setInputFlag( InputFlags::OPTIONAL )->
    setDescription( "ILU(T) threshold factor" );
}

void LinearSolverParametersInput::PostProcessInput()
{
  m_parameters.logLevel = getLogLevel();

  GEOSX_ERROR_IF_LT_MSG( m_parameters.krylov.maxIterations, 0, "Invalid value of " << viewKeyStruct::krylovMaxIterString );
  GEOSX_ERROR_IF_LT_MSG( m_parameters.krylov.maxRestart, 0, "Invalid value of " << viewKeyStruct::krylovMaxRestartString );

  GEOSX_ERROR_IF_LT_MSG( m_parameters.krylov.relTolerance, 0.0, "Invalid value of " << viewKeyStruct::krylovTolString );
  GEOSX_ERROR_IF_GT_MSG( m_parameters.krylov.relTolerance, 1.0, "Invalid value of " << viewKeyStruct::krylovTolString );

  GEOSX_ERROR_IF_LT_MSG( m_parameters.ilu.fill, 0, "Invalid value of " << viewKeyStruct::iluFillString );
  GEOSX_ERROR_IF_LT_MSG( m_parameters.ilu.threshold, 0.0, "Invalid value of " << viewKeyStruct::iluThresholdString );

  GEOSX_ERROR_IF_LT_MSG( m_parameters.amg.numSweeps, 0, "Invalid value of " << viewKeyStruct::amgNumSweepsString );
  GEOSX_ERROR_IF_LT_MSG( m_parameters.amg.threshold, 0.0, "Invalid value of " << viewKeyStruct::amgThresholdString );
  GEOSX_ERROR_IF_GT_MSG( m_parameters.amg.threshold, 1.0, "Invalid value of " << viewKeyStruct::amgThresholdString );

  // TODO input validation for other AMG parameters ?
}

REGISTER_CATALOG_ENTRY( Group, LinearSolverParametersInput, std::string const &, Group * const )

} // namespace geosx
