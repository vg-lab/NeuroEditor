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

#include "ViewDock.h"

#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include "ColorSelectionWidget.h"

ViewDock::ViewDock( void )
  : QDockWidget( )
{

}

void ViewDock::init( Viewer* viewer_ )
{
  _viewer = viewer_;
  QFrame* hline;

  setSizePolicy( QSizePolicy::MinimumExpanding,
                 QSizePolicy::MinimumExpanding );
  setFeatures( QDockWidget::DockWidgetClosable |
               QDockWidget::DockWidgetMovable |
               QDockWidget::DockWidgetFloatable );
  setWindowTitle( QString( "View" ));
  setMinimumSize( 200, 200 );

  QWidget* mainWidget = new QWidget( );
  setWidget( mainWidget );
  mainWidget->setMaximumHeight( 600 );
  QVBoxLayout* viewDockLayout = new QVBoxLayout( );
  viewDockLayout->setAlignment( Qt::AlignTop );
  mainWidget->setLayout( viewDockLayout );

  //Show
  QGroupBox* showGroup = new QGroupBox( QString( "Show" ));
  auto showGroupLayout = new QHBoxLayout( );
  showGroup->setLayout( showGroupLayout );
  viewDockLayout->addWidget( showGroup );

  auto vline = new QFrame( );
  vline->setFrameShape( QFrame::VLine );
  vline->setFrameShadow( QFrame::Sunken );

  auto firstViewLayout = new QVBoxLayout( );
  auto secondViewLayout = new QVBoxLayout( );
  showGroupLayout->addLayout( firstViewLayout );
  showGroupLayout->addWidget( vline, Qt::AlignLeft );
  showGroupLayout->addLayout( secondViewLayout );

  firstViewLayout->addWidget( new QLabel(" First View") );
  auto secondViewCheckBox = new QCheckBox( "Second View" );
  secondViewLayout->addWidget( secondViewCheckBox );

  auto firstCheckBoxLayout = new QVBoxLayout( );
  auto secondCheckBoxLayout = new QVBoxLayout( );
  firstCheckBoxLayout->setContentsMargins( 20, 1, 1, 1);
  secondCheckBoxLayout->setContentsMargins( 20, 1, 1, 1);
  firstViewLayout->addLayout( firstCheckBoxLayout );
  secondViewLayout->addLayout( secondCheckBoxLayout );

  QCheckBox* msModifiedStructure =
    new QCheckBox( QString( "Modified tracing" ));
  firstCheckBoxLayout->addWidget( msModifiedStructure );
  QCheckBox* ssModifiedStructure =
    new QCheckBox( QString( "Modified tracing" ));
  secondCheckBoxLayout->addWidget( ssModifiedStructure );
  ssModifiedStructure->setEnabled( false );

  QCheckBox* msModifiedMesh = new QCheckBox( QString( "Modified 3D mesh" ));
  firstCheckBoxLayout->addWidget( msModifiedMesh );
  QCheckBox* ssModifiedMesh = new QCheckBox( QString( "Modified 3D mesh" ));
  secondCheckBoxLayout->addWidget( ssModifiedMesh );
  ssModifiedMesh->setEnabled( false );

  QCheckBox* msOriginalStructure =
    new QCheckBox( QString( "Original tracing" ));
  firstCheckBoxLayout->addWidget( msOriginalStructure, 3, 0 );
  QCheckBox* ssOriginalStructure =
    new QCheckBox( QString( "Original tracing" ));
  secondCheckBoxLayout->addWidget( ssOriginalStructure );
  ssOriginalStructure->setEnabled( false );

  QCheckBox* msOriginalMesh = new QCheckBox( QString( "Original 3D mesh" ));
  firstCheckBoxLayout->addWidget( msOriginalMesh, 4, 0 );
  QCheckBox* ssOriginalMesh = new QCheckBox( QString( "Original 3D mesh" ));
  secondCheckBoxLayout->addWidget( ssOriginalMesh );
  ssOriginalMesh->setEnabled( false );

  connect( secondViewCheckBox, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateSideBySide( bool )));

  connect( secondViewCheckBox, SIGNAL( toggled( bool )),
           ssModifiedStructure, SLOT( setEnabled( bool )));
  connect( secondViewCheckBox, SIGNAL( toggled( bool )),
           ssModifiedMesh, SLOT( setEnabled( bool )));
  connect( secondViewCheckBox, SIGNAL( toggled( bool )),
           ssOriginalStructure, SLOT( setEnabled( bool )));
  connect( secondViewCheckBox, SIGNAL( toggled( bool )),
           ssOriginalMesh, SLOT( setEnabled( bool )));

  msModifiedStructure->setCheckState( Qt::Checked );
  connect( msModifiedStructure, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateFLRModifiedStructure( bool )));
  connect( ssModifiedStructure, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateSLRModifiedStructure( bool )));
  msModifiedMesh->setCheckState( Qt::Checked );
  connect( msModifiedMesh, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateFLRModifiedMesh( bool )));
  connect( ssModifiedMesh, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateSLRModifiedMesh( bool )));
  connect( msOriginalStructure, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateFLROriginalStructure( bool )));
  ssOriginalStructure->setCheckState( Qt::Checked );
  connect( ssOriginalStructure, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateSLROriginalStructure( bool )));
  connect( msOriginalMesh, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateFLROriginalMesh( bool )));
  ssOriginalMesh->setCheckState( Qt::Checked );
  connect( ssOriginalMesh, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateSLROriginalMesh( bool )));

  hline = new QFrame( );
  hline->setFrameShape( QFrame::HLine );
  hline->setFrameShadow( QFrame::Sunken );
  viewDockLayout->addWidget( hline );

  QGroupBox* renderOptionsGroup = new QGroupBox( QString( "Render options" ));
  QGridLayout* renderOptionsLayout = new QGridLayout( );
  renderOptionsGroup->setLayout( renderOptionsLayout );
  viewDockLayout->addWidget( renderOptionsGroup );
  renderOptionsLayout->addWidget( new QLabel( QString("Background color")),
                                  0, 0);
  ColorSelectionWidget* backgroundColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( backgroundColor, 0, 1 );

  renderOptionsLayout->addWidget(
    new QLabel( QString("Modified tracing color")), 1, 0);
  ColorSelectionWidget* mtColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( mtColor, 1, 1 );
  renderOptionsLayout->addWidget(
    new QLabel( QString("Modified 3D mesh color")), 1, 2);
  ColorSelectionWidget* mmColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( mmColor, 1, 3 );
  renderOptionsLayout->addWidget(
    new QLabel( QString("Modified 3D mesh transparency" )), 2, 0 );
  auto mAlphaSlider = new QSlider( Qt::Horizontal );
  mAlphaSlider->setRange( 0, 100 );
  mAlphaSlider->setValue( 50 );
  renderOptionsLayout->addWidget( mAlphaSlider, 2, 2 );
  renderOptionsLayout->addWidget(
    new QLabel( QString("Original tracing color")), 3, 0);
  ColorSelectionWidget* otColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( otColor, 3, 1 );
  renderOptionsLayout->addWidget(
    new QLabel( QString("Original 3D mesh color")), 3, 2);
  ColorSelectionWidget* omColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( omColor, 3, 3 );
  renderOptionsLayout->addWidget(
    new QLabel( QString("Original 3D mesh transparency" )), 4, 0 );
  auto oAlphaSlider = new QSlider( Qt::Horizontal );
  oAlphaSlider->setRange( 0, 100 );
  oAlphaSlider->setValue( 50 );
  renderOptionsLayout->addWidget( oAlphaSlider, 4, 2 );

  connect( mAlphaSlider, SIGNAL( valueChanged( int )),
           this, SLOT( modifiedAlphaChanged( int )));
  connect( oAlphaSlider, SIGNAL( valueChanged( int )),
           this, SLOT( originalAlphaChanged( int )));
  connect( backgroundColor, SIGNAL( colorChanged( QColor )),
           viewer_, SLOT( changeBackgroundColor( QColor )));
  backgroundColor->color( QColor( 255, 255, 255 ));
  connect( mtColor, SIGNAL( colorChanged( QColor )),
           viewer_, SLOT( changeModifiedStructureColor( QColor )));
  mtColor->color( QColor( 63, 84, 125 ));
  connect( mmColor, SIGNAL( colorChanged( QColor )),
           viewer_, SLOT( changeModifiedMeshColor( QColor )));
  mmColor->color( QColor( 63, 84, 125 ));
  connect( otColor, SIGNAL( colorChanged( QColor )),
           viewer_, SLOT( changeOriginalStructureColor( QColor )));
  otColor->color( QColor( 124, 125, 63 ));
  connect( omColor, SIGNAL( colorChanged( QColor )),
           viewer_, SLOT( changeOriginalMeshColor( QColor )));
  omColor->color( QColor( 124, 125, 63 ));

  hline = new QFrame( );
  hline->setFrameShape( QFrame::HLine );
  hline->setFrameShadow( QFrame::Sunken );
  viewDockLayout->addWidget( hline );

  QGroupBox* cameraGroup = new QGroupBox( QString( "Camera" ));
  QGridLayout* cameraLayout = new QGridLayout( );
  cameraGroup->setLayout( cameraLayout );
  viewDockLayout->addWidget( cameraGroup );
  QPushButton* cameraTopButton = new QPushButton( QString( "Top" ));
  cameraTopButton->setMaximumSize( QSize( 65, 25 ));
  cameraLayout->addWidget( cameraTopButton, 0, 1 );
  QPushButton* cameraFrontButton = new QPushButton( QString( "Front" ));
  cameraFrontButton->setMaximumSize( QSize( 65, 25 ));
  cameraLayout->addWidget( cameraFrontButton, 1, 1 );
  QPushButton* cameraBottomButton = new QPushButton( QString( "Bottom" ));
  cameraBottomButton->setMaximumSize( QSize( 65, 25 ));
  cameraLayout->addWidget( cameraBottomButton, 2, 1 );
  QPushButton* cameraLeftButton = new QPushButton( QString( "Left" ));
  cameraLeftButton->setMaximumSize( QSize( 65, 25 ));
  cameraLayout->addWidget( cameraLeftButton, 1, 0 );
  QPushButton* cameraRightButton = new QPushButton( QString( "Right" ));
  cameraRightButton->setMaximumSize( QSize( 65, 25 ));
  cameraLayout->addWidget( cameraRightButton, 1, 2 );

  viewDockLayout->addStretch( 1 );

  connect( cameraFrontButton, SIGNAL( clicked( )),
           viewer_, SLOT( setCameraViewFront( )));
  connect( cameraTopButton, SIGNAL( clicked( )),
           viewer_, SLOT( setCameraViewTop( )));
  connect( cameraBottomButton, SIGNAL( clicked( )),
           viewer_, SLOT( setCameraViewBottom( )));
  connect( cameraLeftButton, SIGNAL( clicked( )),
           viewer_, SLOT( setCameraViewLeft( )));
  connect( cameraRightButton, SIGNAL( clicked( )),
           viewer_, SLOT( setCameraViewRight( )));

}


void ViewDock::originalAlphaChanged( int value )
{
  float alpha = value * 0.01;
  _viewer->changeOriginalMeshAlpha( alpha );
}

void ViewDock::modifiedAlphaChanged( int value )
{
  float alpha = value * 0.01;
  _viewer->changeModifiedMeshAlpha( alpha );
}
