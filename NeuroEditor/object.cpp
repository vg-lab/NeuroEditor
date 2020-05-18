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

#include <qgl.h>
#include "object.h"

using namespace qglviewer;

void Object::draw ( ) const
{
  static GLUquadric* quad = gluNewQuadric ( );

  glPushMatrix ( );
  glMultMatrixd ( frame.matrix ( ));
  glColor3f ( 1, 1, 0 );
  gluSphere ( quad, radius, 10, 6 );
  gluCylinder ( quad, 0.03, 0.0, 0.09, 10, 1 );
  glPopMatrix ( );
}
