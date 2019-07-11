#include "EditDock.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include <iostream>

EditDock::EditDock( void )
  : QDockWidget( )
  , _activeManipulation( false )
{

}

void EditDock::init( Viewer* viewer_ )
{
  _viewer = viewer_;

  setSizePolicy( QSizePolicy::Expanding,
                 QSizePolicy::MinimumExpanding );
  setFeatures( QDockWidget::DockWidgetClosable |
               QDockWidget::DockWidgetMovable |
               QDockWidget::DockWidgetFloatable );
  setWindowTitle( QString( "Edit" ));
  setMinimumSize( 200, 200 );

  QWidget* mainWidget = new QWidget( );
  setWidget( mainWidget );
  mainWidget->setMaximumHeight( 300 );
  QVBoxLayout* editDockLayout = new QVBoxLayout( );
  editDockLayout->setAlignment( Qt::AlignTop );
  mainWidget->setLayout( editDockLayout );

  QGroupBox* inspectorGroup = new QGroupBox( );
  QGridLayout* inspectorLayout = new QGridLayout( );
  auto validator = new QDoubleValidator( );
  inspectorGroup->setLayout( inspectorLayout );
  editDockLayout->addWidget( inspectorGroup );
  inspectorLayout->addWidget( new QLabel( QString( "Position: " )),
                              0, 0, 1, 6 );
  inspectorLayout->addWidget( new QLabel( QString( "x:" )), 1, 0 );
  _xTextBoxPosition = new QLineEdit( );
  _xTextBoxPosition->setValidator( validator );
  _xTextBoxPosition->setFixedWidth( 75 );
  _xTextBoxPosition->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _xTextBoxPosition, 1, 1 );
  inspectorLayout->addWidget( new QLabel( QString( "y:" )), 1, 2 );
  _yTextBoxPosition = new QLineEdit( );
  _yTextBoxPosition->setValidator( validator );
  _yTextBoxPosition->setFixedWidth( 75 );
  _yTextBoxPosition->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _yTextBoxPosition, 1, 3 );
  inspectorLayout->addWidget( new QLabel( QString( "z:" )), 1, 4 );
  _zTextBoxPosition = new QLineEdit( );
  _zTextBoxPosition->setValidator( validator );
  _zTextBoxPosition->setFixedWidth( 75 );
  _zTextBoxPosition->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _zTextBoxPosition, 1, 5 );
  auto applyPosButton = new QPushButton( QString( "apply"));
  inspectorLayout->addWidget( applyPosButton, 1, 6 );

  inspectorLayout->addWidget( new QLabel( QString( "Rotation (radians): " )),
                              2, 0, 1, 6 );
  inspectorLayout->addWidget( new QLabel( QString( "x:" )), 3, 0 );
  _xTextBoxRotation = new QLineEdit( );
  _xTextBoxRotation->setValidator( validator );
  _xTextBoxRotation->setFixedWidth( 75 );
  _xTextBoxRotation->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _xTextBoxRotation, 3, 1 );
  inspectorLayout->addWidget( new QLabel( QString( "y:" )), 3, 2 );
  _yTextBoxRotation = new QLineEdit( );
  _yTextBoxRotation->setValidator( validator );
  _yTextBoxRotation->setFixedWidth( 75 );
  _yTextBoxRotation->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _yTextBoxRotation, 3, 3 );
  inspectorLayout->addWidget( new QLabel( QString( "z:" )), 3, 4 );
  _zTextBoxRotation = new QLineEdit( );
  _zTextBoxRotation->setValidator( validator );
  _zTextBoxRotation->setFixedWidth( 75 );
  _zTextBoxRotation->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _zTextBoxRotation, 3, 5 );
  auto applyRotButton = new QPushButton( QString( "apply"));
  inspectorLayout->addWidget( applyRotButton, 3, 6 );

  inspectorLayout->addWidget( new QLabel( QString( "Radius: " )),
                              4, 0, 1, 2 );
  _radiusTextBox = new QLineEdit( );
  _radiusTextBox->setValidator( validator );
  _radiusTextBox->setFixedWidth( 75 );
  _radiusTextBox->setPlaceholderText( QString( "---" ));
  inspectorLayout->addWidget( _radiusTextBox, 4, 3 );
  auto applyRadButton = new QPushButton( QString( "apply"));
  inspectorLayout->addWidget( applyRadButton, 4, 6 );

  

  QObject::connect( _viewer, SIGNAL( resetInspectorSignal( )),
                    this, SLOT( resetInspector( )));

  QObject::connect( _viewer,
                    SIGNAL( updateAveragePosSignal( Eigen::Vector3f& )),
                    this, SLOT( updatePosition( Eigen::Vector3f& )));
  QObject::connect( applyPosButton, SIGNAL( clicked( )),
                    this, SLOT( applyPosition( )));

  QObject::connect( _viewer,
                    SIGNAL( updateRotationSignal( Eigen::Quaternionf& )),
                    this, SLOT( updateRotation( Eigen::Quaternionf& )));
  QObject::connect( applyRotButton, SIGNAL( clicked( )),
                    this, SLOT( applyRotation( )));

  QObject::connect( _viewer, SIGNAL( updateAverageRadiusSignal( float )),
                    this, SLOT( updateRadius( float )));
  QObject::connect( applyRadButton, SIGNAL( clicked( )),
                    this, SLOT( applyRadius( )));

  QIcon helpIcon( QString::fromUtf8(":/icons/help-browser.png"));
  auto positionHelp = new QToolButton( );
  positionHelp->setIcon( helpIcon );
  inspectorLayout->addWidget( positionHelp, 1, 7 );
  auto rotationHelp = new QToolButton( );
  rotationHelp->setIcon( helpIcon );
  inspectorLayout->addWidget( rotationHelp, 3, 7 );
  auto radiusHelp = new QToolButton( );
  radiusHelp->setIcon( helpIcon );
  inspectorLayout->addWidget( radiusHelp, 4, 7 );

  auto message = QString( "If only one node is selected this menu shows the position of the node and allows its edition, if several nodes are selected it shows the average of their positions and allows to edit the node positions taking this average position as reference." );
  _positionHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( positionHelp, SIGNAL( pressed( )),
                    _positionHelpBox, SLOT( exec( )));

  message = QString( "When several nodes are selected this menu allows to rotate each node taking as reference a pivot computed as the average of the node positions." );
  _rotationHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( rotationHelp, SIGNAL( pressed( )),
                    _rotationHelpBox, SLOT( exec( )));

  message = QString( "If only one node is selected this menu shows the radius of the node and allows its edition, if several nodes are selected it shows the average of their radios and allows to edit the average radius, modifying the selected nodes radios keeping the ratio with the average radius." );
  _radiusHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( radiusHelp, SIGNAL( pressed( )),
                    _radiusHelpBox, SLOT( exec( )));
}

void EditDock::resetInspector( void )
{
  _xTextBoxPosition->clear( );
  _yTextBoxPosition->clear( );
  _zTextBoxPosition->clear( );

  _xTextBoxRotation->clear( );
  _yTextBoxRotation->clear( );
  _zTextBoxRotation->clear( );

  _radiusTextBox->clear( );

  _activeManipulation = false;
}

void EditDock::updatePosition( Eigen::Vector3f& pos_ )
{
  _xTextBoxPosition->setText( QString::number( pos_.x( )));
  _yTextBoxPosition->setText( QString::number( pos_.y( )));
  _zTextBoxPosition->setText( QString::number( pos_.z( )));

  _activeManipulation = true;
}

void EditDock::updateRotation( Eigen::Quaternionf& q_ )
{
  Eigen::Vector3f rot = _quatToRot( q_ );
  _xTextBoxRotation->setText( QString::number( rot.x( )));
  _yTextBoxRotation->setText( QString::number( rot.y( )));
  _zTextBoxRotation->setText( QString::number( rot.z( )));
  _activeManipulation = true;
}

void EditDock::updateRadius( float radius_ )
{
  _radiusTextBox->setText( QString::number( radius_ ));
  _activeManipulation = true;
}

void EditDock::applyPosition( void )
{
  if ( _activeManipulation )
  {
    float x,y,z;
    x = _xTextBoxPosition->text().toFloat( );
    y = _yTextBoxPosition->text().toFloat( );
    z = _zTextBoxPosition->text().toFloat( );
    Eigen::Vector3f averagePosition( x, y, z );
    _viewer->changeAveragePos( averagePosition );
  }
  else
    resetInspector( );
}

void EditDock::applyRotation( void )
{
  if ( _activeManipulation )
  {
    float x,y,z;
    x = _xTextBoxRotation->text().toFloat( );
    y = _yTextBoxRotation->text().toFloat( );
    z = _zTextBoxRotation->text().toFloat( );
    Eigen::Vector3f rotation = Eigen::Vector3f( x, y, z );
    Eigen::Quaternionf q = _rotToQuat( rotation );
    rotation = _quatToRot( q );

    _viewer->changeRotation( q );
  }
  else
    resetInspector( );
}

void EditDock::applyRadius( void )
{
  if ( _activeManipulation )
  {
    float radius = _radiusTextBox->text( ).toFloat( );
    _viewer->changeAverageRadius( radius );
  }
  else
    resetInspector( );
}

Eigen::Quaternionf EditDock::_rotToQuat( Eigen::Vector3f& rot_ )
{
  Eigen::Quaternionf q;
  q = Eigen::AngleAxisf( rot_.x( ), Eigen::Vector3f::UnitX( ))
    * Eigen::AngleAxisf( rot_.y( ), Eigen::Vector3f::UnitY( ))
    * Eigen::AngleAxisf( rot_.z( ), Eigen::Vector3f::UnitZ( ))
    ;
  return q;
}

Eigen::Vector3f EditDock::_quatToRot( Eigen::Quaternionf& q_ )
{
  return q_.toRotationMatrix( ).eulerAngles( 0, 1, 2 );
}
