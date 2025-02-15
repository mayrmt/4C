// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "4C_fbi_immersedcoupler_factory.hpp"

#include "4C_fbi_immersed_geometry_coupler.hpp"
#include "4C_fbi_immersed_geometry_coupler_binning.hpp"
#include "4C_inpar_fbi.hpp"

#include <Teuchos_StandardParameterEntryValidators.hpp>

FOUR_C_NAMESPACE_OPEN
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
Teuchos::RCP<FBI::FBIGeometryCoupler> FBI::GeometryCouplerFactory::create_geometry_coupler(
    const Teuchos::ParameterList& fbidyn)
{
  Inpar::FBI::BeamToFluidPreSortStrategy presort_strategy =
      Teuchos::getIntegralValue<Inpar::FBI::BeamToFluidPreSortStrategy>(fbidyn, "PRESORT_STRATEGY");

  Teuchos::RCP<FBI::FBIGeometryCoupler> coupler;

  if (presort_strategy == Inpar::FBI::BeamToFluidPreSortStrategy::bruteforce)
  {
    coupler = Teuchos::RCP(new FBI::FBIGeometryCoupler());
  }
  else if (presort_strategy == Inpar::FBI::BeamToFluidPreSortStrategy::binning)
  {
    coupler = Teuchos::RCP(new FBI::FBIBinningGeometryCoupler());
  }
  else
    FOUR_C_THROW("Unknown Beam to Fluid PreSort Strategy");

  return coupler;
}

FOUR_C_NAMESPACE_CLOSE
