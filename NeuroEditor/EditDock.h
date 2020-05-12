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

#ifndef __EDIT_DOCK__
#define __EDIT_DOCK__

#include <QDockWidget>
#include <QObject>
#include <QTreeView>
#include <QGridLayout>
#include <QComboBox>
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include "viewer.h"
#include "TraceModifier.h"

class ModifierWidget: public QWidget
{
  Q_OBJECT

public:

  ModifierWidget( neuroeditor::TraceModifier::TModifierMethod modifierMethod_ );

  ~ModifierWidget( void );

  neuroeditor::TraceModifier::TModifierMethod modifierMethod;
  neuroeditor::TraceModifier::TModifierParams modifierParams;
  std::vector< std::string > paramNames;
  std::vector< QLineEdit*> paramLineEdits;
  std::string scriptPath;

public Q_SLOTS:

  void loadPath( void );

  void apply( void );

  void sendRemoveSignal( void );

Q_SIGNALS:
  void modifierToDelete( QWidget* widget_ );

  void applyModifier( ModifierWidget* modifierWidget_ );

protected:

  QLineEdit* _scriptPathLine;

};

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

  void addMethod( void );
  void removeMethod( QWidget* modifierWidget_ );
  void apply( ModifierWidget* modifierWidget_ );
  void applyAll( void );
  void clear( void );

protected:

  Eigen::Quaternionf _rotToQuat( Eigen::Vector3f& rot_ );

  Eigen::Vector3f _quatToRot( Eigen::Quaternionf& q_ );

  void _initMethodSelector( void );

  bool _apply( ModifierWidget* modifierWidget_,
               std::unordered_set< nsol::Section* >& sections_ );

  std::unordered_set< nsol::Section* > _uniqueSections( void );

  Viewer* _viewer;

  QLineEdit* _xTextBoxPosition;
  QLineEdit* _yTextBoxPosition;
  QLineEdit* _zTextBoxPosition;

  QLineEdit* _xTextBoxRotation;
  QLineEdit* _yTextBoxRotation;
  QLineEdit* _zTextBoxRotation;

  QLineEdit* _radiusTextBox;

  bool _activeManipulation;

  QMessageBox* _positionHelpBox;
  QMessageBox* _rotationHelpBox;
  QMessageBox* _radiusHelpBox;

  QComboBox* _methodSelector;
  QVBoxLayout* _methodsLayout;
  QMessageBox* _simplifyMethodHelpBox;


};

#endif
