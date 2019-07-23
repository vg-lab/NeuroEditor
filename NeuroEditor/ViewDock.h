/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero  <juanjose.garcia@urjc.es>
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

#ifndef __VIEW_DOCK__
#define __VIEW_DOCK__


#include <QDockWidget>
#include <QObject>
#include "viewer.h"

class ViewDock: public QDockWidget
{
  Q_OBJECT

public:

  ViewDock( );

  ~ViewDock( void ){ };

  void init( Viewer* viewer_ );

public Q_SLOTS:

  void originalAlphaChanged( int value );
  void modifiedAlphaChanged( int value );

protected:
  Viewer* _viewer;

};

#endif
