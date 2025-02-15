// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "4C_fluid_ele_xwall.hpp"

#include "4C_comm_utils_factory.hpp"
#include "4C_fem_discretization.hpp"
#include "4C_fluid_ele_nullspace.hpp"
#include "4C_global_data.hpp"
#include "4C_io_linedefinition.hpp"

FOUR_C_NAMESPACE_OPEN

Discret::Elements::FluidXWallType Discret::Elements::FluidXWallType::instance_;

Discret::Elements::FluidXWallType& Discret::Elements::FluidXWallType::instance()
{
  return instance_;
}

Core::Communication::ParObject* Discret::Elements::FluidXWallType::create(
    Core::Communication::UnpackBuffer& buffer)
{
  Discret::Elements::FluidXWall* object = new Discret::Elements::FluidXWall(-1, -1);
  object->unpack(buffer);
  return object;
}

Teuchos::RCP<Core::Elements::Element> Discret::Elements::FluidXWallType::create(
    const std::string eletype, const std::string eledistype, const int id, const int owner)
{
  if (eletype == "FLUIDXW")
  {
    return Teuchos::make_rcp<Discret::Elements::FluidXWall>(id, owner);
  }
  return Teuchos::null;
}

Teuchos::RCP<Core::Elements::Element> Discret::Elements::FluidXWallType::create(
    const int id, const int owner)
{
  return Teuchos::make_rcp<Discret::Elements::FluidXWall>(id, owner);
}

void Discret::Elements::FluidXWallType::nodal_block_information(
    Core::Elements::Element* dwele, int& numdf, int& dimns, int& nv, int& np)
{
  // this is necessary here! Otherwise it would not be consistent with the non-enriched nodes
  // since we are assuming that all elements are equal during nullspace computation
  numdf = 4;
  dimns = numdf;
  nv = numdf - 1;
  np = 1;
}

Core::LinAlg::SerialDenseMatrix Discret::Elements::FluidXWallType::compute_null_space(
    Core::Nodes::Node& node, const double* x0, const int numdof, const int dimnsp)
{
  return FLD::compute_fluid_null_space(node, numdof, dimnsp);
}

void Discret::Elements::FluidXWallType::setup_element_definition(
    std::map<std::string, std::map<std::string, Input::LineDefinition>>& definitions)
{
  std::map<std::string, Input::LineDefinition>& defsxwall = definitions["FLUIDXW"];

  defsxwall["HEX8"] = Input::LineDefinition::Builder()
                          .add_int_vector("HEX8", 8)
                          .add_named_int("MAT")
                          .add_named_string("NA")
                          .build();
  defsxwall["TET4"] = Input::LineDefinition::Builder()
                          .add_int_vector("TET4", 4)
                          .add_named_int("MAT")
                          .add_named_string("NA")
                          .build();
}


/*----------------------------------------------------------------------*
 |  ctor (public)                                            gammi 02/08|
 |  id             (in)  this element's global id                       |
 *----------------------------------------------------------------------*/
Discret::Elements::FluidXWall::FluidXWall(int id, int owner) : Fluid(id, owner) { return; }

/*----------------------------------------------------------------------*
 |  copy-ctor (public)                                       gammi 02/08|
 *----------------------------------------------------------------------*/
Discret::Elements::FluidXWall::FluidXWall(const Discret::Elements::FluidXWall& old) : Fluid(old)
{
  return;
}

/*----------------------------------------------------------------------*
 |  Deep copy this instance of Fluid and return pointer to it (public) |
 |                                                          gammi 02/08 |
 *----------------------------------------------------------------------*/
Core::Elements::Element* Discret::Elements::FluidXWall::clone() const
{
  Discret::Elements::FluidXWall* newelement = new Discret::Elements::FluidXWall(*this);
  return newelement;
}



/*----------------------------------------------------------------------*
 |  get vector of lines              (public)                           |
 *----------------------------------------------------------------------*/
std::vector<Teuchos::RCP<Core::Elements::Element>> Discret::Elements::FluidXWall::lines()
{
  return Core::Communication::get_element_lines<FluidXWallBoundary, FluidXWall>(*this);
}


/*----------------------------------------------------------------------*
 |  get vector of surfaces (public)                                     |
 *----------------------------------------------------------------------*/
std::vector<Teuchos::RCP<Core::Elements::Element>> Discret::Elements::FluidXWall::surfaces()
{
  return Core::Communication::get_element_surfaces<FluidXWallBoundary, FluidXWall>(*this);
}

/*----------------------------------------------------------------------*
 |  print this element (public)                                         |
 *----------------------------------------------------------------------*/
void Discret::Elements::FluidXWall::print(std::ostream& os) const
{
  os << "FluidXWall ";
  Element::print(os);
  return;
}

FOUR_C_NAMESPACE_CLOSE
