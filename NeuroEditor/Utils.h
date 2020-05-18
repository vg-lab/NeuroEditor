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

#ifndef __NEUROEDITOR_UTILS__
#define __NEUROEDITOR_UTILS__

#include <nsol/nsol.h>
#include "glmSpline.hpp"
#include "object.h"

enum SIMP_METHOD
{
  NTHPOINT,
  RADIAL,
  PERPDIST,
  PTOPDIST,
  REUMANNWITKAM,
  OPHEIN,
  LANG,
  DOUGLASPEUCKER,
  DOUGLASPEUCKERMOD,
  CUSTOM_SIMP
};

enum ENHANCE_METHOD { LINEAR, SPLINE, CUSTOM_ENHANCE };
enum OBJECT_TYPE { NEURITE, SECTION, NODE, SOMA };

typedef std::unordered_map< int, nsol::NodePtr > IdToNode;
typedef std::unordered_map< nsol::NodePtr, nsol::SectionPtr > NodeToSection;

namespace neuroeditor //Probably this code must be in nsol in the future
{

  class MorphologyStructure
  {

  public:

    typedef QList< Object* > Objects;
    typedef std::unordered_map< int, Object* > IdToObject;
    typedef std::unordered_map< int, nsol::NodePtr > IdToNode;
    typedef std::unordered_map< nsol::NodePtr, nsol::SectionPtr > NodeToSection;

    MorphologyStructure( nsol::NeuronMorphologyPtr morphology_ );

    ~MorphologyStructure( void );

    void clear( void );
    void update( void );
    void changeMorphology( nsol::NeuronMorphologyPtr morphology_ );

    nsol::NeuronMorphologyPtr morphology;
    nsol::NeuritePtr somaNeurite;
    nsol::NeuronMorphologySectionPtr somaSection;
    Objects objects;
    IdToObject idToObject;
    IdToNode idToNode;
    NodeToSection nodeToSection;
  };
}
#endif // __NEUROEDITOR_UTILS__
