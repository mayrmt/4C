// This file is part of 4C multiphysics licensed under the
// GNU Lesser General Public License v3.0 or later.
//
// See the LICENSE.md file in the top-level for license information.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "4C_w1_poro_p1_scatra_eletypes.hpp"

#include "4C_io_linedefinition.hpp"
#include "4C_w1_poro_p1_scatra.hpp"

FOUR_C_NAMESPACE_OPEN

/*----------------------------------------------------------------------*
 |  QUAD 4 Element                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/

Discret::Elements::WallQuad4PoroP1ScatraType
    Discret::Elements::WallQuad4PoroP1ScatraType::instance_;

Discret::Elements::WallQuad4PoroP1ScatraType&
Discret::Elements::WallQuad4PoroP1ScatraType::instance()
{
  return instance_;
}

Core::Communication::ParObject* Discret::Elements::WallQuad4PoroP1ScatraType::create(
    Core::Communication::UnpackBuffer& buffer)
{
  Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad4>* object =
      new Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad4>(-1, -1);
  object->unpack(buffer);
  return object;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Discret::Elements::WallQuad4PoroP1ScatraType::create(
    const std::string eletype, const std::string eledistype, const int id, const int owner)
{
  if (eletype == "WALLQ4POROP1SCATRA")
  {
    Teuchos::RCP<Core::Elements::Element> ele =
        Teuchos::make_rcp<Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad4>>(
            id, owner);
    return ele;
  }
  return Teuchos::null;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Discret::Elements::WallQuad4PoroP1ScatraType::create(
    const int id, const int owner)
{
  Teuchos::RCP<Core::Elements::Element> ele =
      Teuchos::make_rcp<Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad4>>(id, owner);
  return ele;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
void Discret::Elements::WallQuad4PoroP1ScatraType::setup_element_definition(
    std::map<std::string, std::map<std::string, Input::LineDefinition>>& definitions)
{
  std::map<std::string, std::map<std::string, Input::LineDefinition>> definitions_wallporo;
  WallQuad4PoroP1Type::setup_element_definition(definitions_wallporo);

  std::map<std::string, Input::LineDefinition>& defs_wallporo =
      definitions_wallporo["WALLQ4POROP1"];

  std::map<std::string, Input::LineDefinition>& defs = definitions["WALLQ4POROP1SCATRA"];

  defs["QUAD4"] =
      Input::LineDefinition::Builder(defs_wallporo["QUAD4"]).add_named_string("TYPE").build();
}

/*----------------------------------------------------------------------*
 |  QUAD 9 Element                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/

Discret::Elements::WallQuad9PoroP1ScatraType
    Discret::Elements::WallQuad9PoroP1ScatraType::instance_;

Discret::Elements::WallQuad9PoroP1ScatraType&
Discret::Elements::WallQuad9PoroP1ScatraType::instance()
{
  return instance_;
}

Core::Communication::ParObject* Discret::Elements::WallQuad9PoroP1ScatraType::create(
    Core::Communication::UnpackBuffer& buffer)
{
  Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad9>* object =
      new Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad9>(-1, -1);
  object->unpack(buffer);
  return object;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Discret::Elements::WallQuad9PoroP1ScatraType::create(
    const std::string eletype, const std::string eledistype, const int id, const int owner)
{
  if (eletype == "WALLQ9POROP1SCATRA")
  {
    Teuchos::RCP<Core::Elements::Element> ele =
        Teuchos::make_rcp<Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad9>>(
            id, owner);
    return ele;
  }
  return Teuchos::null;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Discret::Elements::WallQuad9PoroP1ScatraType::create(
    const int id, const int owner)
{
  Teuchos::RCP<Core::Elements::Element> ele =
      Teuchos::make_rcp<Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::quad9>>(id, owner);
  return ele;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
void Discret::Elements::WallQuad9PoroP1ScatraType::setup_element_definition(
    std::map<std::string, std::map<std::string, Input::LineDefinition>>& definitions)
{
  std::map<std::string, std::map<std::string, Input::LineDefinition>> definitions_wallporo;
  WallQuad9PoroP1Type::setup_element_definition(definitions_wallporo);

  std::map<std::string, Input::LineDefinition>& defs_wallporo =
      definitions_wallporo["WALLQ9POROP1"];

  std::map<std::string, Input::LineDefinition>& defs = definitions["WALLQ9POROP1SCATRA"];

  defs["QUAD9"] =
      Input::LineDefinition::Builder(defs_wallporo["QUAD9"]).add_named_string("TYPE").build();
}

/*----------------------------------------------------------------------*
 |  TRI 3 Element                                         schmidt 09/17 |
 *----------------------------------------------------------------------*/

Discret::Elements::WallTri3PoroP1ScatraType Discret::Elements::WallTri3PoroP1ScatraType::instance_;

Discret::Elements::WallTri3PoroP1ScatraType& Discret::Elements::WallTri3PoroP1ScatraType::instance()
{
  return instance_;
}

Core::Communication::ParObject* Discret::Elements::WallTri3PoroP1ScatraType::create(
    Core::Communication::UnpackBuffer& buffer)
{
  Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::tri3>* object =
      new Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::tri3>(-1, -1);
  object->unpack(buffer);
  return object;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Discret::Elements::WallTri3PoroP1ScatraType::create(
    const std::string eletype, const std::string eledistype, const int id, const int owner)
{
  if (eletype == "WALLT3POROP1SCATRA")
  {
    Teuchos::RCP<Core::Elements::Element> ele =
        Teuchos::make_rcp<Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::tri3>>(
            id, owner);
    return ele;
  }
  return Teuchos::null;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
Teuchos::RCP<Core::Elements::Element> Discret::Elements::WallTri3PoroP1ScatraType::create(
    const int id, const int owner)
{
  Teuchos::RCP<Core::Elements::Element> ele =
      Teuchos::make_rcp<Discret::Elements::Wall1PoroP1Scatra<Core::FE::CellType::tri3>>(id, owner);
  return ele;
}

/*----------------------------------------------------------------------*
 |                                                        schmidt 09/17 |
 *----------------------------------------------------------------------*/
void Discret::Elements::WallTri3PoroP1ScatraType::setup_element_definition(
    std::map<std::string, std::map<std::string, Input::LineDefinition>>& definitions)
{
  std::map<std::string, std::map<std::string, Input::LineDefinition>> definitions_wallporo;
  WallTri3PoroP1Type::setup_element_definition(definitions_wallporo);

  std::map<std::string, Input::LineDefinition>& defs_wallporo =
      definitions_wallporo["WALLT3POROP1"];

  std::map<std::string, Input::LineDefinition>& defs = definitions["WALLT3POROP1SCATRA"];

  defs["TRI3"] =
      Input::LineDefinition::Builder(defs_wallporo["TRI3"]).add_named_string("TYPE").build();
}

FOUR_C_NAMESPACE_CLOSE
