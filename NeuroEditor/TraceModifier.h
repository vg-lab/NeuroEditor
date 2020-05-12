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

#ifndef __TRACE_MODIFIER__
#define __TRACE_MODIFIER__

#include "Utils.h"
#include <unordered_set>

namespace neuroeditor
{

  class TraceModifier
  {

  public:

    typedef enum
    {
      NTHPOINT = 0,
      RADIAL,
      PERPDIST,
      REUMANNWITKAM,
      OPHEIN,
      LANG,
      DOUGLASPEUCKER,
      DOUGLASPEUCKERMOD,
      LINEAR_ENHANCE,
      SPLINE_ENHANCE,
      CUSTOM
    } TModifierMethod;

    typedef std::unordered_map< std::string, float > TModifierParams;
    typedef bool (*TSimplifierFunc)( nsol::Section*, TModifierParams );
    typedef bool (*TEnhacerFunc)( nsol::Section*, TModifierParams, int& );

    static bool modify( std::unordered_set< nsol::Section* >& sections_,
                        TModifierMethod modifierMethod_,
                        TModifierParams modifierParams_ );

    static bool customModify( std::unordered_set< nsol::Section* >& sections_,
                              const std::string& scriptPath_ );

    static int numModifiers( void )
    {
      return 11;
    }

    static std::string description( TModifierMethod modifierMethod_ );

    static TModifierParams defaultParams( TModifierMethod modifierMethod_ );

  protected:

    static bool _nthpoint( nsol::Section* section_,
                           TModifierParams modifierParams_ );

    static bool _radial( nsol::Section* section_,
                         TModifierParams modifierParams_ );

    static bool _perpdist( nsol::Section* section_,
                           TModifierParams modifierParams_ );
    static bool _reumannWitkam( nsol::Section* section_,
                                TModifierParams modifierParams_ );

    static bool _ophein( nsol::Section* section_,
                         TModifierParams modifierParams_ );

    static bool _lang( nsol::Section* section_,
                       TModifierParams modifierParams_ );

    static bool _douglasPeucker( nsol::Section* section_,
                                 TModifierParams modifierParams_ );

    static bool _douglasPeuckerMod( nsol::Section* section_,
                                    TModifierParams modifierParams_ );

    static bool _linearEnhance( nsol::Section* section_,
                                TModifierParams modifierParams_,
                                int& currentId_ );

    static bool _splineEnhance( nsol::Section* section_,
                                TModifierParams modifierParams_,
                                int& currentId_ );

    static std::vector< float > _vectorizeSection( nsol::Section* section_ );

    static bool _fillSection( nsol::Section* section_,
                              std::vector< float >& simplVecNodes_ );

    static int _maximumId(  std::unordered_set< nsol::Section* >& sections_ );

  };

}

#endif
