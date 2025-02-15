// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "4C_comm_utils_factory.hpp"

#include "4C_comm_parobjectfactory.hpp"

FOUR_C_NAMESPACE_OPEN

/*----------------------------------------------------------------------*
 |  allocate an instance of a specific impl. of ParObject (public) mwgee 12/06|
 *----------------------------------------------------------------------*/
Core::Communication::ParObject* Core::Communication::factory(
    Core::Communication::UnpackBuffer& buffer)
{
  return ParObjectFactory::instance().create(buffer);
}

/*----------------------------------------------------------------------*
 |  allocate an element of a specific type (public)          mwgee 03|07|
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Core::Communication::factory(
    const std::string eletype, const std::string eledistype, const int id, const int owner)
{
  return ParObjectFactory::instance().create(eletype, eledistype, id, owner);
}

FOUR_C_NAMESPACE_CLOSE
