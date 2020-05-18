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

#include <QGLViewer/constraint.h>
#include "object.h"

class ManipulatedFrameSetConstraint: public qglviewer::Constraint
{
  public:
    void clearSet ( );
    void addObjectToSet ( Object* o );

    virtual void constrainTranslation ( qglviewer::Vec& translation,
                                        qglviewer::Frame* const frame );
    virtual void constrainRotation ( qglviewer::Quaternion& rotation,
                                     qglviewer::Frame* const frame );

  private :
    QList < Object* > objects_;
};
