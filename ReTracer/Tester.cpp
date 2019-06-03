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

#include "Tester.h"

namespace retracer
{

  #define LIMIT 0.00001f

  std::vector< int > Tester::test( MorphologyStructure* morphoStructure_,
                                   TTesterMethod testerMethod_ )
  {
    TTesterFunc testerFunc = nullptr;

    switch( testerMethod_ )
    {
    case TTesterMethod::DUPLICATE_NODES:
      testerFunc = Tester::_duplicateNodes;
      break;
    default:
      testerFunc = Tester::_duplicateNodes;
    }

    std::vector< int > result;
    if ( testerFunc )
      result = testerFunc( morphoStructure_ );
    return result;
  }

  std::string Tester::description( TTesterMethod testerMethod_ )
  {
    std::string description;
    switch( testerMethod_ )
    {
    case DUPLICATE_NODES:
      description = std::string( "Duplicate nodes" );
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

  std::vector< Fixer::TFixerMethod > Tester::associatedFixers(
    TTesterMethod testerMethod_ )
  {
    std::vector< Fixer::TFixerMethod > fixers;
    switch( testerMethod_ )
    {
    case DUPLICATE_NODES:
      fixers.push_back( Fixer::NODE_TEXT_OUT );
      break;
    case A:
      fixers.push_back( Fixer::SECTION_TEXT_OUT );
      fixers.push_back( Fixer::A );
      break;
    case B:
      fixers.push_back( Fixer::A );
      fixers.push_back( Fixer::B );
      break;
    case C:
      fixers.push_back( Fixer::A );
      fixers.push_back( Fixer::B );
      fixers.push_back( Fixer::C );
      fixers.push_back( Fixer::NODE_TEXT_OUT );
      break;
    }
    return fixers;
  }

  std::vector< int > Tester::_duplicateNodes(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;

    auto morpho = morphoStructure_->morphology;
    auto somaNodes = morphoStructure_->somaSection->nodes( );

    if ( somaNodes.size( ) > 1 )
    {
      auto previousNode = somaNodes[0];
      for (  unsigned int i = 1; i < somaNodes.size( ) - 1; i++ )
      {
        auto node = somaNodes[i];
        auto nextNode = somaNodes[i+1];
        if ((( node->point( ) - previousNode->point( )).norm( ) < LIMIT &&
             node->radius( ) - previousNode->radius( ) < LIMIT ) ||
            (( node->point( ) - nextNode->point( )).norm( ) < LIMIT &&
             node->radius( ) - nextNode->radius( ) < LIMIT ))
        {
          result.push_back( node->id( ));
          i++;
          previousNode = nextNode;
        }
        else
          previousNode = node;
      }
    }

    for ( auto neurite: morpho->neurites( ))
    {
      for ( auto section: neurite->sections( ))
      {
        auto sectionNodes = section->nodes( );
        if ( sectionNodes.size( ) > 1 )
        {
          auto previousNode = sectionNodes[0];
          for (  unsigned int i = 1; i < sectionNodes.size( ) - 1; i++ )
          {
            auto node = sectionNodes[i];
            auto nextNode = sectionNodes[i+1];
            if ((( node->point( ) - previousNode->point( )).norm( ) < LIMIT &&
                 node->radius( ) - previousNode->radius( ) < LIMIT ) ||
                (( node->point( ) - nextNode->point( )).norm( ) < LIMIT &&
                 node->radius( ) - nextNode->radius( ) < LIMIT ))
            {
              result.push_back( node->id( ));
              i++;
              previousNode = nextNode;
            }
            else
              previousNode = node;
          }
        }
      }
    }

    return result;
  }
}
