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

#ifndef __CORRECT_DOCK__
#define __CORRECT_DOCK__

#include <QDockWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QToolButton>
#include <QPushButton>
#include "viewer.h"
#include "Tester.h"

class TestWidget: public QWidget
{
  Q_OBJECT

public:

  TestWidget( retracer::Tester::TTesterMethod testMethod_ );

  ~TestWidget( void );

  retracer::Tester::TTesterMethod testMethod;
  retracer::Fixer::TFixerMethod fixerMethod;

public Q_SLOTS:

  void sendRemoveSignal( void );

  void changeFixerMethod( int index_ );

Q_SIGNALS:
  void testToDelete( QWidget* widget_ );

protected:

  QComboBox* _fixerCombo;

};

class ResultButton: public QPushButton
{
  Q_OBJECT

public:

  ResultButton( std::vector< int > indices_, Viewer* viewer_ )
    : QPushButton( )
    , _viewer( viewer_ )
  {
    std::string buttonName( std::to_string( indices_[0]));
    if ( indices_.size( ) > 1 )
      buttonName.append( ",..." );
    this->setText( QString( buttonName.c_str( )));
    for( auto index: indices_ )
      _selection.insert( index );
    this->setMaximumSize( QSize( 40, 40 ));

    connect( this, SIGNAL( pressed( )),
             this, SLOT( sendSelection( )));
  }

  ~ResultButton( void )
  {
    _selection.clear( );
  }

public Q_SLOTS:

  void sendSelection( void )
  {
    _viewer->updateSelection( _selection );
  }

protected:

  Viewer* _viewer;
  std::unordered_set< int > _selection;

};

class CorrectDock: public QDockWidget
{
  Q_OBJECT

public:

  CorrectDock( void );

  ~CorrectDock( void );

  void init( Viewer* viewer_ );

public Q_SLOTS:

  void addTest( void );

  void addAllTests( void );

  void removeTest( QWidget* testWidget_ );

  void clearTests( void );

  void correct( void );

  void clearOutput( void );

protected:

  void _initTestSelector( void );

  void _addTest( retracer::Tester::TTesterMethod testerMethod_ );

  Viewer* _viewer;

  QComboBox* _testSelector;
  QToolButton* _testAdder;
  QToolButton* _testAddAll;
  QVBoxLayout* _testsLayout;
  QVBoxLayout* _outputLayout;
};

#endif
