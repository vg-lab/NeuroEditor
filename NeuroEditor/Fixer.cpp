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

#include "Fixer.h"

namespace neuroeditor
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
    case NEURITE_TEXT_OUT:
      fixerFunc = Fixer::_neuriteTextOut;
      break;
    case DELETE_NODE:
      fixerFunc = Fixer::_deleteNode;
      break;
    case DISPLACE_TO_MIDDLE_EDGE:
      fixerFunc = Fixer::_displaceToMiddleSegment;
      break;
    case DISPLACE_TO_SOMA_SURFACE:
      fixerFunc = Fixer::_displaceToSomaSurface;
      break;
    }

    std::vector< std::vector< int >> results;

    for ( auto index: indices_ )
    {
      auto result = fixerFunc( index, morphoStructure_ );
      if ( !result.empty( ))
        results.push_back( result );
    }
    return results;
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
    case NEURITE_TEXT_OUT:
      description = std::string( "Condole output" );
      break;
    case DELETE_NODE:
      description = std::string( "Delete node" );
      break;
    case DISPLACE_TO_MIDDLE_EDGE:
      description = std::string( "Move to middle edge" );
      break;
    case DISPLACE_TO_SOMA_SURFACE:
      description = std::string( "Move to soma surface" );
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

  std::vector< int > Fixer::_neuriteTextOut(
    int index_, MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;

    auto currentSection = dynamic_cast< nsol::NeuronMorphologySection* >(
      morphoStructure_->nodeToSection[morphoStructure_->idToNode[index_]]);
    for ( auto section: currentSection->neurite( )->sections( ))
      for ( auto node: section->nodes( ))
        result.push_back( node->id( ));
    return result;
  }

  std::vector< int > Fixer::_deleteNode(
      int index_, MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;

    auto cNode = morphoStructure_->idToNode[index_];
    nsol::Nodes& cNodes = morphoStructure_->nodeToSection[cNode]->nodes( );
    for ( auto it = cNodes.begin( ); it != cNodes.end( ); it++ )
    {
      if ( (*it)->id( ) == cNode->id( ))
      {
        cNodes.erase( it );
        break;
      }
    }

    return result;
  }

  std::vector< int > Fixer::_displaceToMiddleSegment(
      int index_, MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;

    auto cNode = morphoStructure_->idToNode[index_];
    auto cNodes = morphoStructure_->nodeToSection[cNode]->nodes( );
    for ( unsigned int i = 1; i < cNodes.size( )-1; i++ )
    {
      auto node = cNodes[i];
      if ( node->id( ) == cNode->id( ))
      {
        Eigen::Vector3f pos =
          ( cNodes[i-1]->point( ) + cNodes[i+1]->point( )) * 0.5f;
        cNode->point( pos );
      }
    }

    return result;
  }

  std::vector< int > Fixer::_displaceToSomaSurface(
      int index_, MorphologyStructure* morphoStructure_ )
  {
    std::vector< int > result;

    auto cNode = morphoStructure_->idToNode[index_];
    auto soma = morphoStructure_->morphology->soma( );
    Eigen::Vector3f somaCenter = soma->center( );
    float somaRadius = soma->meanRadius( ) * 1.01f;

    Eigen::Vector3f axis = ( cNode->point( ) - somaCenter ).normalized( );
    cNode->point( somaCenter + axis * somaRadius );

    return result;
  }

}
