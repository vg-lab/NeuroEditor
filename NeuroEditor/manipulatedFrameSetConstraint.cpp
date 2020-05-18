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

#include "manipulatedFrameSetConstraint.h"
#include <QGLViewer/frame.h>

using namespace qglviewer;

void ManipulatedFrameSetConstraint::clearSet ( )
{
  objects_.clear ( );
}

void ManipulatedFrameSetConstraint::addObjectToSet ( Object* o )
{
  objects_.append ( o );
}

void ManipulatedFrameSetConstraint::constrainTranslation ( qglviewer::Vec& translation,
                                                           Frame* const )
{
  for (
    QList < Object* >::iterator it = objects_.begin ( ), end = objects_.end ( );
    it != end; ++it )
    ( *it )->frame.translate ( translation );
}

void ManipulatedFrameSetConstraint::constrainRotation ( qglviewer::Quaternion& rotation,
                                                        Frame* const frame )
{
  // A little bit of math. Easy to understand, hard to guess (tm).
  // rotation is expressed in the frame local coordinates system. Convert it back to world coordinates.
  const Vec worldAxis = frame->inverseTransformOf ( rotation.axis ( ));
  const Vec pos = frame->position ( );
  const float angle = rotation.angle ( );

  for (
    QList < Object* >::iterator it = objects_.begin ( ), end = objects_.end ( );
    it != end; ++it )
  {
    // Rotation has to be expressed in the object local coordinates system.
    Quaternion qObject (( *it )->frame.transformOf ( worldAxis ), angle );
    ( *it )->frame.rotate ( qObject );

    // Comment these lines only rotate the objects
    Quaternion qWorld ( worldAxis, angle );
    // Rotation around frame world position (pos)
    ( *it )->frame.setPosition (
      pos + qWorld.rotate (( *it )->frame.position ( ) - pos ));
  }
}
