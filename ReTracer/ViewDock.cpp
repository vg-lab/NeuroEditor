#include "ViewDock.h"

#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "ColorSelectionWidget.h"

ViewDock::ViewDock( void )
  : QDockWidget( )
{

}

void ViewDock::init( Viewer* viewer_ )
{
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
  QGridLayout* showGroupLayout = new QGridLayout( );
  showGroup->setLayout( showGroupLayout );
  viewDockLayout->addWidget( showGroup );

  showGroupLayout->addWidget( new QLabel( QString( "First view" )), 0, 0 );
  showGroupLayout->addWidget( new QLabel( QString( "Second view" )), 0, 1 );
  QCheckBox* sideCheckBox = new QCheckBox( );
  showGroupLayout->addWidget( sideCheckBox, 0, 2 );

  QCheckBox* msModifiedStructure =
    new QCheckBox( QString( "Modified tracing" ));
  showGroupLayout->addWidget( msModifiedStructure, 1, 0 );
  QCheckBox* ssModifiedStructure =
    new QCheckBox( QString( "Modified tracing" ));
  showGroupLayout->addWidget( ssModifiedStructure, 1, 1 );
  ssModifiedStructure->setEnabled( false );

  QCheckBox* msModifiedMesh = new QCheckBox( QString( "Modified 3D mesh" ));
  showGroupLayout->addWidget( msModifiedMesh, 2, 0 );
  QCheckBox* ssModifiedMesh = new QCheckBox( QString( "Modified 3D mesh" ));
  showGroupLayout->addWidget( ssModifiedMesh, 2, 1 );
  ssModifiedMesh->setEnabled( false );

  QCheckBox* msOriginalStructure =
    new QCheckBox( QString( "Original tracing" ));
  showGroupLayout->addWidget( msOriginalStructure, 3, 0 );
  QCheckBox* ssOriginalStructure =
    new QCheckBox( QString( "Original tracing" ));
  showGroupLayout->addWidget( ssOriginalStructure, 3, 1 );
  ssOriginalStructure->setEnabled( false );

  QCheckBox* msOriginalMesh = new QCheckBox( QString( "Original 3D mesh" ));
  showGroupLayout->addWidget( msOriginalMesh, 4, 0 );
  QCheckBox* ssOriginalMesh = new QCheckBox( QString( "Original 3D mesh" ));
  showGroupLayout->addWidget( ssOriginalMesh, 4, 1 );
  ssOriginalMesh->setEnabled( false );

  connect( sideCheckBox, SIGNAL( toggled( bool )),
           viewer_, SLOT( updateSideBySide( bool )));

  connect( sideCheckBox, SIGNAL( toggled( bool )),
           ssModifiedStructure, SLOT( setEnabled( bool )));
  connect( sideCheckBox, SIGNAL( toggled( bool )),
           ssModifiedMesh, SLOT( setEnabled( bool )));
  connect( sideCheckBox, SIGNAL( toggled( bool )),
           ssOriginalStructure, SLOT( setEnabled( bool )));
  connect( sideCheckBox, SIGNAL( toggled( bool )),
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

  renderOptionsLayout->addWidget( new QLabel( QString("Modified tracing color")),
                                  1, 0);
  ColorSelectionWidget* mtColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( mtColor, 1, 1 );
  renderOptionsLayout->addWidget( new QLabel( QString("Modified 3D mesh color")),
                                  1, 2);
  ColorSelectionWidget* mmColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( mmColor, 1, 3 );
  renderOptionsLayout->addWidget( new QLabel( QString("Original tracing color")),
                                  2, 0);
  ColorSelectionWidget* otColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( otColor, 2, 1 );
  renderOptionsLayout->addWidget( new QLabel( QString("Original 3D mesh color")),
                                  2, 2);
  ColorSelectionWidget* omColor = new ColorSelectionWidget( this );
  renderOptionsLayout->addWidget( omColor, 2, 3 );


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
