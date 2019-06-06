/*
 * Copyright (c) 2019 CCS/UPM - GMRV/URJC.
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

#ifndef __SIMPLIFY_DOCK__
#define __SIMPLIFY_DOCK__

#include <QDockWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QToolButton>
#include <QPushButton>
#include "viewer.h"
#include "TraceModifier.h"

class ModifierWidget: public QWidget
{
  Q_OBJECT

public:

  ModifierWidget( retracer::TraceModifier::TModifierMethod modifierMethod_ );

  ~ModifierWidget( void );

  retracer::TraceModifier::TModifierMethod modifierMethod;
  retracer::TraceModifier::TModifierParams modifierParams;
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

class SimplifyDock: public QDockWidget
{
  Q_OBJECT

public:

  SimplifyDock( void );

  ~SimplifyDock( void );

  void init( Viewer* viewer_ );

public Q_SLOTS:

  void addMethod( void );

  void removeMethod( QWidget* modifierWidget_ );

  void apply( ModifierWidget* modifierWidget_ );

  void applyAll( void );

  void clear( void );

protected:

  void _initMethodSelector( void );

  bool _apply( ModifierWidget* modifierWidget_,
               std::unordered_set< nsol::Section* >& sections_ );

  std::unordered_set< nsol::Section* > _uniqueSections( void );

  Viewer* _viewer;

  QComboBox* _methodSelector;
  QVBoxLayout* _methodsLayout;
};

#endif
