/*
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

#ifndef __FIXER__
#define __FIXER__

#include "Utils.h"

namespace neuroeditor
{
  class Fixer
  {

  public:
    typedef enum
    {
      NODE_TEXT_OUT = 0,
      SECTION_TEXT_OUT,
      NEURITE_TEXT_OUT,
      DELETE_NODE,
      DISPLACE_TO_MIDDLE_EDGE,
      DISPLACE_TO_SOMA_SURFACE
    } TFixerMethod;

    typedef std::vector< int > (*TFixerFunc)( int, MorphologyStructure* ) ;

    static std::vector< std::vector < int >> fix( std::vector< int > indices_,
                     MorphologyStructure* morphoStructure_,
                     TFixerMethod fixerMethod_ );

    static int numFixers( void )
    {
      return 5;
    }

    static std::string description( TFixerMethod testerMethod_ );

  protected:

    static std::vector< int > _nodeTextOut(
      int index_, MorphologyStructure* morphoStructure_ );

    static std::vector< int > _sectionTextOut(
      int index_, MorphologyStructure* morphoStructure_ );

    static std::vector< int > _neuriteTextOut(
      int index_, MorphologyStructure* morphoStructure_ );

    static std::vector< int > _deleteNode(
      int index_, MorphologyStructure* morphoStructure_ );

    static std::vector< int > _displaceToMiddleSegment(
      int index_, MorphologyStructure* morphoStructure_ );

    static std::vector< int > _displaceToSomaSurface(
      int index_, MorphologyStructure* morphoStructure_ );

  };

}

#endif
