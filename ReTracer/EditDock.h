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

#ifndef __EDIT_DOCK__
#define __EDIT_DOCK__

#include <QDockWidget>
#include <QObject>
#include <QTreeView>
#include "viewer.h"

class EditDock: public QDockWidget
{
  Q_OBJECT

public:

  EditDock( void );

  ~EditDock( void ){ };

  void init( Viewer* viewer_ );

public Q_SLOTS:

  void resetInspector( void );

  void updatePosition( Eigen::Vector3f& pos_ );
  void updateRotation( Eigen::Quaternionf& q_ );
  void updateRadius( float radius_ );

  void applyPosition( void );
  void applyRotation( void );
  void applyRadius( void );

protected:

  Eigen::Quaternionf _rotToQuat( Eigen::Vector3f& rot_ );

  Eigen::Vector3f _quatToRot( Eigen::Quaternionf& q_ );

  Viewer* _viewer;
  QTreeView* _morphologyTree;

  QLineEdit* _xTextBoxPosition;
  QLineEdit* _yTextBoxPosition;
  QLineEdit* _zTextBoxPosition;

  QLineEdit* _xTextBoxRotation;
  QLineEdit* _yTextBoxRotation;
  QLineEdit* _zTextBoxRotation;

  QLineEdit* _radiusTextBox;

  bool _activeManipulation;

};

#endif
