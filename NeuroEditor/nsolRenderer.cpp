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

#include "nsolRenderer.h"

nsolRenderer::nsolRenderer ( void )
{
  _morphology = nullptr;
  mNodeColor = nsol::Vec3f ( 1, 0, 0 );
  mLineColor = nsol::Vec3f ( 0, 0, 0 );
  mIncNode = 0.0;
  resolution = 4;
}

nsolRenderer::nsolRenderer ( const nsol::NeuronMorphologyPtr morphology )
  : _morphology ( morphology )
{
  mNodeColor = nsol::Vec3f ( 1, 0, 0 );
  mLineColor = nsol::Vec3f ( 0, 0, 0 );
  mIncNode = 0.0;
  resolution = 4;
}

nsolRenderer::~nsolRenderer ( void )
{
}

void nsolRenderer::setNodeColor ( const nsol::Vec3f& color )
{
  mNodeColor = color;
}

void nsolRenderer::setLinkColor ( const nsol::Vec3f& color )
{
  mLineColor = color;
}

void nsolRenderer::setIncrement ( float inc )
{
  mIncNode = inc;
}

void nsolRenderer::setResolution ( unsigned int resol )
{
  resolution = resol;
};

void nsolRenderer::renderMorphology ( )
{
  if ( _morphology != nullptr )
  {
    float lRadius;

    //###Change this simple render to use shaders!!!
    glColor3f ( mNodeColor[0], mNodeColor[1], mNodeColor[2] );

    //Soma
    glPushMatrix ( );
    somaPos = _morphology->soma ( )->center ( );
    glTranslatef ( somaPos.x ( ), somaPos.y ( ), somaPos.z ( ));
    lRadius = _morphology->soma ( )->minRadius ( );
    glutSolidSphere ( lRadius + mIncNode, resolution, resolution );
    glPopMatrix ( );

    for ( unsigned int j = 0; j < _morphology->neurites ( ).size ( ); ++j )
    {
      nsol::NeuritePtr neurite = _morphology->neurites ( )[j];
      iniPos = neurite->firstSection ( )->firstNode ( )->point ( );
      glDisable ( GL_LIGHTING );
      glBegin ( GL_LINES );
      glVertex3f ( iniPos[0], iniPos[1], iniPos[2] );
      glVertex3f ( somaPos[0], somaPos[1], somaPos[2] );
      glEnd ( );
      glEnable ( GL_LIGHTING );
    }

    int i;
    nsol::Nodes* nodes = nullptr;
    for ( auto neurite: _morphology->neurites ( ))
    {
      for ( auto section: neurite->sections ( ))
      {
        i = 0;
        for ( auto node: section->nodes ( ))
        {
          glColor3f ( mNodeColor[0], mNodeColor[1], mNodeColor[2] );
          glPushMatrix ( );
          iniPos = node->point ( );
          glTranslatef ( iniPos.x ( ), iniPos.y ( ), iniPos.z ( ));
          lRadius = node->radius ( );
          glutSolidSphere ( lRadius + ( lRadius*mIncNode ),
                            resolution,
                            resolution );
          glPopMatrix ( );

          nodes = &section->nodes ( );
          finPos = ( *nodes )[( i > 0 ? i - 1 : i )]->point ( );

          glColor3f ( mLineColor[0], mLineColor[1], mLineColor[2] );
          glDisable ( GL_LIGHTING );
          glBegin ( GL_LINES );
          glVertex3f ( iniPos[0], iniPos[1], iniPos[2] );
          glVertex3f ( finPos[0], finPos[1], finPos[2] );
          glEnd ( );
          glEnable ( GL_LIGHTING );
          ++i;
        }
      }
    }
  }
}
