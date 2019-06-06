/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
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

#include <QColorDialog>
#include <QDirIterator>
#include <QMessageBox>

#include "ReTracer.h"

ReTracer::ReTracer ( QWidget* parent )
  : QMainWindow ( parent )
  , _ui( new Ui::ReTracer )
{

  _ui->setupUi( this );
  _viewer = new Viewer( );
  this->setCentralWidget( _viewer );
  _viewer->setupViewer ( );
  _viewer->show( );


  QObject::connect( _ui->actionQuit, SIGNAL( triggered( )),
                    QApplication::instance( ), SLOT( quit( )));
  QObject::connect( _ui->actionAbout, SIGNAL( triggered( )),
                    _viewer, SLOT( help( )));
  QObject::connect( _ui->actionOpenSWCFile, SIGNAL( triggered( )),
                    this, SLOT( importMorphology( )));
  QObject::connect( _ui->actionSaveSWCFile, SIGNAL( triggered( )),
                    this, SLOT( exportMorphology( )));
  // QObject::connect( _ui->exportMesh, SIGNAL( triggered( )),
  //                   this, SLOT( ));
  // QObject::connect( _ui->exportMorphometricMeasures, SIGNAL( triggered( )),
  //                   this, SLOT( ));
  QObject::connect( _ui->actionUndo, SIGNAL( triggered( )),
                    _viewer, SLOT( undoState( )));

  _viewDock = new ViewDock(  );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::LeftDockWidgetArea,
                       _viewDock, Qt::Vertical );
  _viewDock->close( );
  connect( _viewDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionView, SLOT( setChecked( bool )));
  connect( _ui->actionView, SIGNAL( triggered( )),
           this, SLOT( updateViewDock( )));

  _selectDock = new SelectDock(  );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::LeftDockWidgetArea,
                       _selectDock, Qt::Vertical );
  _selectDock->close( );
  connect( _selectDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionSelect, SLOT( setChecked( bool )));
  connect( _ui->actionSelect, SIGNAL( triggered( )),
           this, SLOT( updateSelectDock( )));

  _editDock = new EditDock(  );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                       _editDock, Qt::Vertical );
  _editDock->close( );
  connect( _editDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionEdit, SLOT( setChecked( bool )));
  connect( _ui->actionEdit, SIGNAL( triggered( )),
           this, SLOT( updateEditDock( )));

  _correctDock = new CorrectDock( );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                       _correctDock, Qt::Vertical );
  _correctDock->close( );
  connect( _correctDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionCorrect, SLOT( setChecked( bool )));
  connect( _ui->actionCorrect, SIGNAL( triggered( )),
           this, SLOT( updateCorrectDock( )));

  _simplifyDock = new SimplifyDock( );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                       _simplifyDock, Qt::Vertical );
  _simplifyDock->close( );
  connect( _simplifyDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionSimplify, SLOT( setChecked( bool )));
  connect( _ui->actionSimplify, SIGNAL( triggered( )),
           this, SLOT( updateSimplifyDock( )));
}

ReTracer::~ReTracer ( )
{
  delete _viewer;
}

void ReTracer::init( void )
{
  _viewDock->init( _viewer );
  _selectDock->init( _viewer );
  _editDock->init( _viewer );
  _correctDock->init( _viewer );
  _simplifyDock->init( _viewer );
}

void ReTracer::importMorphology ( )
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open File" ), "./", tr ( "NeuroMorpho(*.swc)" ));
  if ( !fileName.isNull ( ))
  {
    _viewer->loadMorphology( fileName );
  }
}

void ReTracer::exportMorphology ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( fileName.isEmpty ( ))
    return;
  else
    _viewer->exportMorphology ( fileName );
}

void ReTracer::updateViewDock( void )
{
  if( _ui->actionView->isChecked( ))
    _viewDock->show( );
  else
    _viewDock->close( );
}

void ReTracer::updateSelectDock( void )
{
  if( _ui->actionSelect->isChecked( ))
    _selectDock->show( );
  else
    _selectDock->close( );
}

void ReTracer::updateEditDock( void )
{
  if( _ui->actionEdit->isChecked( ))
    _editDock->show( );
  else
    _editDock->close( );
}

void ReTracer::updateCorrectDock( void )
{
  if( _ui->actionCorrect->isChecked( ))
    _correctDock->show( );
  else
    _correctDock->close( );
}

void ReTracer::updateSimplifyDock( void )
{
  if( _ui->actionSimplify->isChecked( ))
    _simplifyDock->show( );
  else
    _simplifyDock->close( );
}
