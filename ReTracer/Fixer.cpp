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

#include "Fixer.h"

namespace retracer
{


  std::vector< std::vector< int >> Fixer::fix( std::vector< int > indices_,
                   MorphologyStructure* morphoStructure_,
                   TFixerMethod fixerMethod_ )
  {
    TFixerFunc fixerFunc = nullptr;

    switch( fixerMethod_ )
    {
    case NODE_TEXT_OUT:
      fixerFunc = Fixer::_nodeTextOut;
      break;
    case SECTION_TEXT_OUT:
      fixerFunc = Fixer::_sectionTextOut;
      break;
    default:
      fixerFunc = Fixer::_nodeTextOut;
    }

    std::vector< std::vector< int >> result;

    for ( auto index: indices_ )
    {
      result.push_back( fixerFunc( index, morphoStructure_ ));
    }
    return result;
  }

  std::string Fixer::description( TFixerMethod fixerMethod_ )
  {
    std::string description;
    switch( fixerMethod_ )
    {
    case NODE_TEXT_OUT:
      description = std::string( "Console output" );
      break;
    case SECTION_TEXT_OUT:
      description = std::string( "Console output" );
      break;
    case A:
      description = std::string( "Method a" );
      break;
    case B:
      description = std::string( "Method b" );
      break;
    case C:
      description = std::string( "Method c" );
      break;
    }
    return description;
  }

  std::vector< int > Fixer::_nodeTextOut(
    int index_, MorphologyStructure* /*morphoStructure_*/ )
  {
    std::vector< int > result;
      result.push_back( index_ );
    return result;
  }

  std::vector< int > Fixer::_sectionTextOut(
    int index_, MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;

    auto section =
      morphoStructure_->nodeToSection[morphoStructure_->idToNode[index_]];
    for ( auto node: section->nodes( ))
      result.push_back( node->id( ));
    return result;
  }
}
