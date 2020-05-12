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

#include <QColorDialog>
#include <QDirIterator>
#include <QMessageBox>

#include "NeuroEditor.h"

NeuroEditor::NeuroEditor ( QWidget* parent )
  : QMainWindow ( parent )
  , _ui( new Ui::NeuroEditor )
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
  QObject::connect( _ui->actionExportMesh, SIGNAL( triggered( )),
                    this, SLOT( exportMesh( )));
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
}

NeuroEditor::~NeuroEditor ( )
{
  delete _viewer;
}

void NeuroEditor::init( void )
{
  _viewDock->init( _viewer );
  _selectDock->init( _viewer );
  _editDock->init( _viewer );
  _correctDock->init( _viewer );
}

void NeuroEditor::importMorphology ( )
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open Tracing" ), "./", tr ( ".swc(*.swc)" ));
  if ( !fileName.isNull ( ))
  {
    _viewer->loadMorphology( fileName );
  }
}

void NeuroEditor::exportMorphology ( )
{
  QString fileName= QFileDialog::getSaveFileName(
    this, tr( "Save Tracing" ), "./", tr ( ".swc(*.swc)" ) );
  if ( fileName.isEmpty( ))
    return;
  else
    _viewer->exportMorphology( fileName );
}

void NeuroEditor::exportMesh( )
{
  QString fileName = QFileDialog::getSaveFileName(
    this, tr( "Save Mesh" ), "./", tr ( ".obj(*.obj)" ) );
  if ( fileName.isEmpty( ))
    return;
  else
    _viewer->exportMesh( fileName );
}

void NeuroEditor::updateViewDock( void )
{
  if( _ui->actionView->isChecked( ))
    _viewDock->show( );
  else
    _viewDock->close( );
}

void NeuroEditor::updateSelectDock( void )
{
  if( _ui->actionSelect->isChecked( ))
    _selectDock->show( );
  else
    _selectDock->close( );
}

void NeuroEditor::updateEditDock( void )
{
  if( _ui->actionEdit->isChecked( ))
    _editDock->show( );
  else
    _editDock->close( );
}

void NeuroEditor::updateCorrectDock( void )
{
  if( _ui->actionCorrect->isChecked( ))
    _correctDock->show( );
  else
    _correctDock->close( );
}
