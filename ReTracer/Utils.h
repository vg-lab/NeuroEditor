/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
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

#ifndef __RETRACER_UTILS__
#define __RETRACER_UTILS__

#include <PyGEmS/StrategyParams.h>
#include <PyGEmSManager/PyGEmSManager.h>

#include <nsol/nsol.h>
#include <glm/glm.hpp>
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

namespace retracer //Probably this code must be in nsol in the future
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


  class Utils
  {
      static Utils * instance;

      static NSPGManager::PyGEmSManager *  myPyGEmSManager;

    Utils() {};

    void customSimplification(const nsol::SectionPtr & section,
                              std::vector < float > & simplifiedCoords);

    void customEnhance(const nsol::SectionPtr & section,
                       std::vector < NSPyGEmS::StrategyParams > & simplifiedCoords);

    public:
      static Utils * getInstance();

      void reloadInstance();

      void Simplify ( nsol::NeuronMorphologyPtr &morphology_,
                      std::map < std::string, float > params_,
                      int objectId_ = -1,
                      OBJECT_TYPE objectType = OBJECT_TYPE::NEURITE );

      void Enhance ( nsol::NeuronMorphologyPtr &morphology_, std::map < std::string, float > params_,
                             int objectId_, OBJECT_TYPE objectType = OBJECT_TYPE::NEURITE );

      void Autofix ( nsol::NeuronMorphologyPtr &morphology_, std::map < std::string, float > params_ );

  };
}
#endif // __RETRACER_UTILS__
