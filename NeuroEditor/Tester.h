/*
 * Copyright (c) 2014-2019 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of nsol <https://github.com/gmrvvis/nsol>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef __TESTER__
#define __TESTER__

#include "Utils.h"
#include "Fixer.h"

namespace neuroeditor
{
  class Tester
  {

  public:
    typedef enum
    {
      DUPLICATE_NODES = 0,
      OVERSAMPLING,
      NODES_IN_SOMA,
      NODES_FAR_SOMA,
      LESS_90_DEGREES,
      GREATER_RADIUS,
      NEURITE_SAME_RADIUS,
      BIFURCATION_COLLISION
    } TTesterMethod;

    typedef std::vector< int > (*TTesterFunc)( MorphologyStructure* ) ;

    static std::vector< int > test( MorphologyStructure* morphoStructure_,
                                    TTesterMethod testerMethod_ );

    static int numTesters( void )
    {
      return 8;
    }

    static std::string description( TTesterMethod testerMethod_ );

    static std::vector< Fixer::TFixerMethod > associatedFixers(
      TTesterMethod testerMethod_ );

  protected:

    static std::vector< int > _duplicateNodes(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _oversampling(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _nodesInSoma(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _nodesFarSoma(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _less90Degrees(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _greaterRadius(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _neuriteSameRadius(
      MorphologyStructure* morphoStructure_ );

    static std::vector< int > _bifurcationCollision(
      MorphologyStructure* morphoStructure_ );

  };

}

#endif
