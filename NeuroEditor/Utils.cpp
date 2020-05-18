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

#include "Utils.h"
#include "imppsimpl.h"


namespace neuroeditor //Probably this code must be in nsol in the future
{

  MorphologyStructure::MorphologyStructure(
    nsol::NeuronMorphologyPtr morphology_ )
    : morphology( morphology_ )
  {
    somaNeurite = new nsol::Neurite( );
    somaSection = new nsol::NeuronMorphologySection( );
    somaNeurite->firstSection( somaSection );
    somaSection->neurite( somaNeurite );
    update( );
  }

  MorphologyStructure::~MorphologyStructure( void )
  {
    clear( );
    delete somaNeurite;
    delete somaSection;
  }

  void MorphologyStructure::clear( void )
  {
    objects.clear( );
    idToNode.clear( );
    nodeToSection.clear( );
    somaSection->nodes( ).clear( );
  }

  void MorphologyStructure::update( void )
  {
    clear( );

    for ( auto node: morphology->soma( )->nodes( ))
    {
      auto object = new Object( );
      object->frame.setPosition( node->point( ).x( ),
                                 node->point( ).y( ),
                                 node->point( ).z( ));
      object->id = node->id( );
      object->radius = node->radius( );
      objects.append( object );
      idToNode[node->id( )] = node;
      idToObject[node->id( )] = object;
      somaSection->addNode( node );
      nodeToSection[node] = somaSection;
    }

    for ( auto neurite: morphology->neurites( ))
      for ( auto section: neurite->sections( ))
        for ( auto node: section->nodes( ))
        {
          auto object = new Object ( );
          object->frame.setPosition( node->point( ).x( ),
                                     node->point( ).y( ),
                                     node->point( ).z( ));
          object->id = node->id ( );
          object->radius = node->radius( );
          objects.append ( object );
          idToNode[node->id( )] = node;
          idToObject[node->id( )] = object;
          nodeToSection[node] = section;
        }
  }

  void MorphologyStructure::changeMorphology(
    nsol::NeuronMorphologyPtr morphology_ )
  {
    morphology = morphology_;
    update( );
  }
}
