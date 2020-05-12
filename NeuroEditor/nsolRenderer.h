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

#ifndef NSOLRENDERER_H_
#define NSOLRENDERER_H_

#include <GL/glut.h>
#include <nsol/nsol.h>

class nsolRenderer
{
    nsol::Vec3f iniPos, finPos, somaPos, mNodeColor, mLineColor;

    float mIncNode;

    nsol::NeuronMorphologyPtr _morphology;

    unsigned int resolution;

  public:

    nsolRenderer ( void );
    nsolRenderer ( const nsol::NeuronMorphologyPtr morphology );

    ~nsolRenderer ( void );

    void setMorphology ( const nsol::NeuronMorphologyPtr& morphology )
    {
      _morphology = morphology;
    };

    const nsol::NeuronMorphologyPtr& getMorphology ( ) { return _morphology; }
    void renderMorphology ( );
    void setNodeColor ( const nsol::Vec3f& color );
    void setLinkColor ( const nsol::Vec3f& color );
    void setIncrement ( float inc );
    void setResolution ( unsigned int resol );
};

#endif
