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

#ifndef __NEUROEDITOR_H__
#define __NEUROEDITOR_H__

#include <QMainWindow>
#include "ui_NeuroEditor.h"
#include "ViewDock.h"
#include "SelectDock.h"
#include "EditDock.h"
#include "CorrectDock.h"

namespace Ui
{
  class NeuroEditor;
}

class NeuroEditor
  : public QMainWindow
{
  Q_OBJECT

public:

  explicit NeuroEditor( QWidget* parent = 0 );

  ~NeuroEditor( );

  Viewer* viewer( void )
  {
    return _viewer;
  }

  void init( void );

private Q_SLOTS:

  void importMorphology( void );
  void exportMorphology( void );
  void exportMesh( void );

  void updateViewDock( void );
  void updateSelectDock( void );
  void updateEditDock( void );
  void updateCorrectDock( void );

private:

  Ui::NeuroEditor* _ui;
  Viewer* _viewer;


  ViewDock* _viewDock;
  SelectDock* _selectDock;
  EditDock* _editDock;
  CorrectDock* _correctDock;

};

#endif // __NEUROEDITOR_H__
