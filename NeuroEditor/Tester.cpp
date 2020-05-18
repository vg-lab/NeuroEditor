/**
 * Copyright (c) 2020 CCS/GMRV/URJC/UPM.
 *
 * Authors: Juan P. Brito <juanpedro.brito@upm.es>
 *          Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
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

namespace neuroeditor
{

  #define LIMIT 0.00001f

  std::vector< int > Tester::test( MorphologyStructure* morphoStructure_,
                                   TTesterMethod testerMethod_ )
  {
    TTesterFunc testerFunc = nullptr;

    switch( testerMethod_ )
    {
    case DUPLICATE_NODES:
      testerFunc = _duplicateNodes;
      break;
    case OVERSAMPLING:
      testerFunc = _oversampling;
      break;
    case NODES_IN_SOMA:
      testerFunc = _nodesInSoma;
      break;
    case NODES_FAR_SOMA:
      testerFunc = _nodesFarSoma;
      break;
    case LESS_90_DEGREES:
      testerFunc = _less90Degrees;
      break;
    case GREATER_RADIUS:
      testerFunc = _greaterRadius;
      break;
    case NEURITE_SAME_RADIUS:
      testerFunc = _neuriteSameRadius;
      break;
    case BIFURCATION_COLLISION:
      testerFunc = _bifurcationCollision;
      break;
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
    case OVERSAMPLING:
      description = std::string( "Oversampling" );
      break;
    case NODES_IN_SOMA:
      description = std::string( "Nodes inside soma" );
      break;
    case NODES_FAR_SOMA:
      description = std::string( "Nodes far from soma" );
      break;
    case LESS_90_DEGREES:
      description = std::string( "Angle smaller than 90º" );
      break;
    case GREATER_RADIUS:
      description = std::string( "Greater radius child branch" );
      break;
    case NEURITE_SAME_RADIUS:
      description = std::string( "Neurite same radius" );
      break;
    case BIFURCATION_COLLISION:
      description = std::string( "Bifurcation collision" );
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
      fixers.push_back( Fixer::DELETE_NODE );
      fixers.push_back( Fixer::DISPLACE_TO_MIDDLE_EDGE );
      break;
    case OVERSAMPLING:
      fixers.push_back( Fixer::NODE_TEXT_OUT );
      fixers.push_back( Fixer::DELETE_NODE );
      fixers.push_back( Fixer::DISPLACE_TO_MIDDLE_EDGE );
      break;
    case NODES_IN_SOMA:
      fixers.push_back( Fixer::NODE_TEXT_OUT );
      fixers.push_back( Fixer::DELETE_NODE );
      fixers.push_back( Fixer::DISPLACE_TO_SOMA_SURFACE );
      break;
    case NODES_FAR_SOMA:
      fixers.push_back( Fixer::NODE_TEXT_OUT );
      fixers.push_back( Fixer::DISPLACE_TO_SOMA_SURFACE );
      break;
    case LESS_90_DEGREES:
      fixers.push_back( Fixer::NODE_TEXT_OUT );
      fixers.push_back( Fixer::DELETE_NODE );
      fixers.push_back( Fixer::DISPLACE_TO_MIDDLE_EDGE );
      break;
    case GREATER_RADIUS:
      fixers.push_back( Fixer::SECTION_TEXT_OUT );
      break;
    case NEURITE_SAME_RADIUS:
      fixers.push_back( Fixer::NEURITE_TEXT_OUT );
      break;
    case BIFURCATION_COLLISION:
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


  std::vector< int > Tester::_oversampling(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

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
            if (( node->point( ) - previousNode->point( )).norm( ) <=
                std::max( previousNode->radius( ), node->radius( )))
              result.push_back( node->id( ));
            else
            {
              previousNode = node;
              if ( i == sectionNodes.size( ) -2 )
              {
                auto postNode = sectionNodes[i+1];
                if (( node->point( ) - postNode->point( )).norm( ) <=
                    std::max( postNode->radius( ), node->radius( )))
                  result.push_back( node->id( ));
              }
            }
          }
        }
      }
    }
    return result;
  }

  std::vector< int > Tester::_nodesInSoma(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

    Eigen::Vector3f somaCenter = morpho->soma( )->center( );
    float somaRadius = morpho->soma( )->meanRadius( );

    for ( auto neurite: morpho->neurites( ))
    {
      auto node = neurite->firstSection( )->firstNode( );
      if (( node->point( ) - somaCenter ).norm( ) <= somaRadius )
        result.push_back( node->id( ));
    }
    return result;
  }

  std::vector< int > Tester::_nodesFarSoma(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

    Eigen::Vector3f somaCenter = morpho->soma( )->center( );
    float somaRadius = morpho->soma( )->meanRadius( );

    for ( auto neurite: morpho->neurites( ))
    {
      auto node = neurite->firstSection( )->firstNode( );
      if (( node->point( ) - somaCenter ).norm( ) > 4.0f * somaRadius )
        result.push_back( node->id( ));
    }
    return result;
  }

  std::vector< int > Tester::_less90Degrees(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

    for ( auto neurite: morpho->neurites( ))
    {
      for ( auto section: neurite->sections( ))
      {
        auto sectionNodes = section->nodes( );
        for (  unsigned int i = 1; i < sectionNodes.size( ) - 1; i++ )
        {
            auto node = sectionNodes[i];
            auto preNode = sectionNodes[i-1];
            auto postNode = sectionNodes[i+1];
            Eigen::Vector3f axis0 =
              ( preNode->point( ) - node->point( )).normalized( );
            Eigen::Vector3f axis1 =
              ( postNode->point( ) - node->point( )).normalized( );

            if ( axis0.dot( axis1 ) > 0.0f )
              result.push_back( node->id( ));
        }
      }
    }
    return result;
  }

  std::vector< int > Tester::_greaterRadius(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

    for ( auto neurite: morpho->neurites( ))
    {
      for ( auto section: neurite->sections( ))
      {
        auto sectionNodes = section->nodes( );
        if ( sectionNodes[1]->radius( ) > sectionNodes[0]->radius( ))
        {
          result.push_back( sectionNodes[1]->id( ));
        }
      }
    }
    return result;
  }

  std::vector< int > Tester::_neuriteSameRadius(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

    for ( auto neurite: morpho->neurites( ))
    {
      auto fNode = neurite->firstSection( )->firstNode( );
      float radius = fNode->radius( );
      bool same = true;
      for ( auto section: neurite->sections( ))
      {
        for ( auto node: section->nodes( ))
        {
          if ( node->radius( ) != radius )
          {
            same = false;
            break;
          }
        }
        if ( !same )
          break;
      }
      if ( same )
        result.push_back( fNode->id( ));
    }
    return result;
  }

  std::vector< int > Tester::_bifurcationCollision(
    MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;
    auto morpho = morphoStructure_->morphology;

    for ( auto neurite: morpho->neurites( ))
      for ( auto section: neurite->sections( ))
      {
        auto morphoSection =
          dynamic_cast< nsol::NeuronMorphologySection* >( section );
        auto children = morphoSection->children( );
        if ( children.size( ) > 1 )
        {
          auto child0Node = children[0]->nodes( )[1];
          auto child1Node = children[1]->nodes( )[1];
          if (( child0Node->point( ) - child1Node->point( )).norm( ) <=
              ( child0Node->radius( ) + child1Node->radius( )))
          {
            result.push_back(child0Node->id( ));
            result.push_back(child1Node->id( ));
          }
        }
      }

    return result;
  }
}
