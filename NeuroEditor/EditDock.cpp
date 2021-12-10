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

#include "EditDock.h"
#include "SimplificationHelpDialog.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include <iostream>

#define MAXDOUBLE std::numeric_limits<double>::max()

ModifierWidget::ModifierWidget(
  neuroeditor::TraceModifier::TModifierMethod modifierMethod_ )
  :modifierMethod( modifierMethod_ )
{
  modifierParams = neuroeditor::TraceModifier::defaultParams( modifierMethod );

  std::string description;
  description.append( neuroeditor::TraceModifier::description( modifierMethod ));
  description.append( ": " );

  QHBoxLayout* layout = new QHBoxLayout( );
  // layout->setAlignment( Qt::AlignLeft );
  setLayout( layout );
  layout->addWidget( new QLabel( QString( description.c_str( ))));

  QPushButton* applyButton = new QPushButton( QString( "apply" ));
  layout->addWidget( applyButton );
  applyButton->setMaximumSize( QSize( 50, 40 ));
  connect( applyButton, SIGNAL( pressed( )),
             this, SLOT( apply( )));
  QIcon removeIcon( QString::fromUtf8(":/icons/list-remove.png"));
  auto modifierRemove = new QToolButton( );
  modifierRemove->setIcon( removeIcon );
  layout->addWidget( modifierRemove );

  connect( modifierRemove, SIGNAL( pressed( void )),
           this, SLOT( sendRemoveSignal( void )));

  if ( modifierMethod == neuroeditor::TraceModifier::CUSTOM )
  {
    _scriptPathLine = new QLineEdit(  );
    layout->addWidget( _scriptPathLine );
    QPushButton* loadButton = new QPushButton( QString( "load" ));
    layout->addWidget( loadButton );
    loadButton->setMaximumSize( QSize( 50, 40 ));
    connect( loadButton, SIGNAL( pressed()),
             this, SLOT( loadPath( )));
  }
  else
  {
    auto validator = new QDoubleValidator( );
    for ( auto param: modifierParams )
    {
      auto vline = new QFrame( );
      vline->setFrameShape( QFrame::VLine );
      vline->setFrameShadow( QFrame::Sunken );
      layout->addWidget( vline );
      std::string paramName( param.first + ":" );
      layout->addWidget( new QLabel( QString( paramName.c_str( ))));
      auto paramLineEdit = new QLineEdit( QString::number( param.second ));
      paramLineEdit->setValidator( validator );
      // paramLineEdit->setFixedWidth( 50 );
      layout->addWidget( paramLineEdit );
      paramNames.push_back( param.first );
      paramLineEdits.push_back( paramLineEdit );
    }
  }
}

ModifierWidget::~ModifierWidget( void )
{
  modifierParams.clear( );
  paramNames.clear( );
  paramLineEdits.clear( );
}

void ModifierWidget::loadPath( void )
{
    QString path = QFileDialog::getOpenFileName(
            this, tr( "Open File" ), "./", tr ( ".py(*.py)" ));
    if ( !path.isNull( ))
    {
        scriptPath = path.toStdString();
        _scriptPathLine->setText(path);
    }
}

void ModifierWidget::apply( void )
{
  Q_EMIT applyModifier( this );
}

void ModifierWidget::sendRemoveSignal( void )
{
  Q_EMIT modifierToDelete( (QWidget*)this );
}


EditDock::EditDock( void )
  : QDockWidget( )
  , _activeManipulation( false )
  , _nSelected( 0 )
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
  mainWidget->setMaximumHeight( 700 );
  QVBoxLayout* editDockLayout = new QVBoxLayout( );
  editDockLayout->setAlignment( Qt::AlignTop );
  mainWidget->setLayout( editDockLayout );
  
  _initSpinBoxes();

  QGroupBox* inspectorGroup = new QGroupBox( tr( "Translation && Rotation" ) );
  QVBoxLayout* inspectorLayout = new QVBoxLayout( );
  inspectorGroup->setLayout(inspectorLayout);
  editDockLayout->addWidget(inspectorGroup);

  QGroupBox* inspectorGroupOne = new QGroupBox( "Single Node" );
  QGridLayout* inspectorLayoutOne = new QGridLayout(  );
  inspectorGroupOne->setLayout( inspectorLayoutOne );
  inspectorLayout->addWidget(inspectorGroupOne);
  inspectorLayoutOne->addWidget( new QLabel( QString( "Position: " )),
                                 0,0,1,6 );
  inspectorLayoutOne->addWidget( new QLabel( QString( "x:" )),1,0,Qt::AlignRight );

  inspectorLayoutOne->addWidget( _xSpinBoxOnePosition,1,1 );
  inspectorLayoutOne->addWidget( new QLabel( QString( "y:" )),1,2,Qt::AlignRight );

  inspectorLayoutOne->addWidget( _ySpinBoxOnePosition,1,3 );
  inspectorLayoutOne->addWidget( new QLabel( QString( "z:" )),1,4,Qt::AlignRight );

  inspectorLayoutOne->addWidget( _zSpinBoxOnePosition,1,5 );
  _applyOnePosButton = new QPushButton( QString( "Apply"));
  _applyOnePosButton->setDisabled( true );
  inspectorLayoutOne->addWidget( _applyOnePosButton,1,6 );


  inspectorLayoutOne->addWidget( new QLabel( QString( "Radius: " )),
                                 2,1,1,2,Qt::AlignRight);

  inspectorLayoutOne->addWidget( _radiusOneSpinBox,2,3 );
  _applyOneRadButton = new class QPushButton( QString( "Apply"));
  _applyOneRadButton->setDisabled( true );
  inspectorLayoutOne->addWidget( _applyOneRadButton,2,6 );


  _inspectorGroupMulti = new QGroupBox( "Multiple Node ( 0 selected )" );
  QGridLayout* inspectorLayoutMulti = new QGridLayout( );
  _inspectorGroupMulti->setLayout( inspectorLayoutMulti );
  inspectorLayout->addWidget( _inspectorGroupMulti );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "Pivot Position: " )),
                                 0,0, 1, 5 );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "x:" )),1,0,Qt::AlignRight );
  inspectorLayoutMulti->addWidget( _xSpinBoxMultiPosition,1,1 );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "y:" )),1,2,Qt::AlignRight );
  inspectorLayoutMulti->addWidget( _ySpinBoxMultiPosition,1,3 );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "z:" )),1,4,Qt::AlignRight );
  inspectorLayoutMulti->addWidget(_zSpinBoxMultiPosition,1,5);

  _applyMultiPosButton = new QPushButton( "Apply" );
  _applyMultiPosButton->setDisabled( true );
  inspectorLayoutMulti->addWidget(_applyMultiPosButton,1,6);



  inspectorLayoutMulti->addWidget( new QLabel( QString( "Rotation (Degrees): " )),
                                 2,0,1,6 );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "x:" )),3,0,Qt::AlignRight );
  inspectorLayoutMulti->addWidget( _xSpinBoxMultiRotation,3,1 );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "y:" )),3,2,Qt::AlignRight );
  inspectorLayoutMulti->addWidget( _ySpinBoxMultiRotation,3,3 );

  inspectorLayoutMulti->addWidget( new QLabel( QString( "z:" )),3,4,Qt::AlignRight );
  inspectorLayoutMulti->addWidget( _zSpinBoxMultiRotation,3,5 );

  _applyMultiRotButton = new QPushButton( QString( "Apply"));
  _applyMultiRotButton->setDisabled( true );
  inspectorLayoutMulti->addWidget( _applyMultiRotButton,3,6 );



  inspectorLayoutMulti->addWidget( new QLabel( QString( "Average Radius: " )),
                                 4,1,1,2,Qt::AlignRight);
  inspectorLayoutMulti->addWidget( _radiusMultiSpinBox,4,3 );
  _applyMultiRadButton = new class QPushButton( QString( "Apply"));
  _applyMultiRadButton->setDisabled( true );
  inspectorLayoutMulti->addWidget( _applyMultiRadButton,4,6 );

  _checkBoxAutoApply = new QCheckBox("AutoApply");
  inspectorLayout->addWidget(_checkBoxAutoApply,0,Qt::AlignRight);




  QObject::connect( _viewer, SIGNAL( resetInspectorSignal( )),
                    this, SLOT( resetInspector( )));

  QObject::connect( _viewer,
                    SIGNAL( updateAveragePosSignal( Eigen::Vector3f& )),
                    this, SLOT( updatePosition( Eigen::Vector3f& )));


  QObject::connect( _viewer,
                    SIGNAL( updateRotationSignal( Eigen::Quaternionf& )),
                    this, SLOT( updateRotation( Eigen::Quaternionf& )));

  QObject::connect(viewer_,SIGNAL(updateSelectionSignal( int )),
                   this, SLOT(updateSelection( int )));


  QObject::connect( _viewer, SIGNAL( updateAverageRadiusSignal( float )),
                    this, SLOT( updateRadius( float )));

  QObject::connect( this, SIGNAL(visibilityChanged( bool )),
                    this, SLOT( onVisibilityChanged( bool )));

  _initSpinBoxesConnections();


  QIcon helpIcon( QString::fromUtf8(":/icons/help-browser.png"));
  auto multiSelectionHelp = new QToolButton( );
  multiSelectionHelp->setIcon( helpIcon );
  inspectorLayoutMulti->addWidget( multiSelectionHelp,0,6,Qt::AlignRight );

  auto message = QString(" <p> In this area, several nodes can be modified at the same time. Therefore, summary values of the selected nodes are displayed: </p>"
                         "<ul>"
                         " <li><strong>Pivot Position:</strong> displays the average position of all selected nodes (Coordinate axes appear at that point).</li>"
                         " <li><strong>Rotation:</strong> applies a rotation to the selected nodes using the pivot position as rotation point.</li>"
                         " <li><strong>Average Radius:</strong> Shows the average radius of all the selected nodes, modifying it will modify all the radii of the selected nodes by the same amount (for example increasing this value by 1 will increase by 1 all the radii of the selected nodes).</li>"
                         " </ul>");

  _multiSelectionHelpBox = new QMessageBox();
  _multiSelectionHelpBox->setIcon(QMessageBox::Information);
  _multiSelectionHelpBox->setInformativeText(message);
  QObject::connect( multiSelectionHelp, SIGNAL( pressed( )),
                   _multiSelectionHelpBox, SLOT( exec( )));

  QGroupBox* simplifyGroup = new QGroupBox( "Simplification && Refinement" );
  QVBoxLayout* simplifyGroupLayout = new QVBoxLayout( );
  simplifyGroup->setLayout( simplifyGroupLayout );
  editDockLayout->addWidget( simplifyGroup );

  QWidget* selectorWidget = new QWidget( );
  simplifyGroupLayout->addWidget( selectorWidget );
  auto selectorLayout = new QGridLayout( );
  selectorWidget->setLayout( selectorLayout );


  selectorLayout->addWidget( new QLabel( QString( "Select method: " )), 0, 0 );
  _methodSelector = new QComboBox( );
  selectorLayout->addWidget( new QLabel( QString( "Select method: " )), 0, 0 );
  selectorLayout->addWidget( _methodSelector, 0, 1 );
  QIcon addIcon( QString::fromUtf8( ":/icons/list-add.png" ));
  auto methodAdder = new QToolButton( );
  methodAdder->setIcon( addIcon );
  selectorLayout->addWidget( methodAdder, 0, 2 );
  auto simplifyMethodHelp = new QToolButton( );
  simplifyMethodHelp->setIcon( helpIcon );
  selectorLayout->addWidget( simplifyMethodHelp, 0, 3 );

  connect( methodAdder, SIGNAL( pressed( )),
           this, SLOT( addMethod( void )));

  auto dialog = new SimplificationHelpDialog( );
  QObject::connect( simplifyMethodHelp, SIGNAL( pressed( )),
                    dialog, SLOT( exec( )));

  _initMethodSelector( );

  QScrollArea* scrollArea = new QScrollArea( );
  scrollArea->setWidgetResizable( true );
  simplifyGroupLayout->addWidget( scrollArea );
  QWidget* methodWidget = new QWidget( );
  _methodsLayout = new QVBoxLayout( );
  _methodsLayout->setAlignment( Qt::AlignTop );
  methodWidget->setLayout( _methodsLayout );

  scrollArea->setWidget( methodWidget );
  scrollArea->setMaximumHeight( 300 );

  QWidget* buttonsWidget = new QWidget( );
  QHBoxLayout* buttonsLayout = new QHBoxLayout( );
  buttonsWidget->setLayout( buttonsLayout );
  simplifyGroupLayout->addWidget( buttonsWidget );

  QPushButton* clearButton = new QPushButton( QString( "Clear all" ));
  clearButton->setMaximumSize( QSize( 80, 40 ));
  buttonsLayout->addWidget( clearButton );
  QPushButton* applyAllButton = new QPushButton( QString( "Apply all" ));
  applyAllButton->setMaximumSize( QSize( 80, 40 ));
  buttonsLayout->addWidget( applyAllButton );

  connect( clearButton, SIGNAL( pressed( )),
           this, SLOT( clear( )));
  connect( applyAllButton, SIGNAL( pressed( )),
           this, SLOT( applyAll( )));
}

void EditDock::resetInspector( void )
{
  _disableOneLocation();
  _disableMultiLocation();
  _activeManipulation = false;
}


void EditDock::updatePosition( Eigen::Vector3f& pos_ )
{
  auto oldSignals = _setBlockedSpinBoxSignals( true );
  if (_nSelected == 1)
  {
    _xSpinBoxOnePosition->setValue( pos_.x( ) );
    _ySpinBoxOnePosition->setValue( pos_.y( ) );
    _zSpinBoxOnePosition->setValue( pos_.z( ) );

    _xSpinBoxOnePosition->setEnabled( true );
    _ySpinBoxOnePosition->setEnabled( true );
    _zSpinBoxOnePosition->setEnabled( true );

    _applyOnePosButton->setDisabled( _checkBoxAutoApply->isChecked( ) );
  } else
  {
    _xSpinBoxMultiPosition->setValue( pos_.x( ) );
    _ySpinBoxMultiPosition->setValue( pos_.y( ) );
    _zSpinBoxMultiPosition->setValue( pos_.z( ) );

    _xSpinBoxMultiPosition->setEnabled( true );
    _ySpinBoxMultiPosition->setEnabled( true );
    _zSpinBoxMultiPosition->setEnabled( true );

    _applyMultiPosButton->setDisabled( _checkBoxAutoApply->isChecked( ) );

  }
  _setBlockedSpinBoxSignals( oldSignals );


  _activeManipulation = true;
}

void EditDock::updateRotation( Eigen::Quaternionf& q_ )
{
  if (_nSelected > 1)
  {
    Eigen::Vector3f rot = _quatToRot( q_ );
    auto oldSignals = _setBlockedSpinBoxSignals( true );

    _xSpinBoxMultiRotation->setValue( rot.x( ) );
    _ySpinBoxMultiRotation->setValue( rot.y( ) );
    _zSpinBoxMultiRotation->setValue( rot.z( ) );
    _setBlockedSpinBoxSignals( oldSignals );

    _xSpinBoxMultiRotation->setEnabled( true );
    _ySpinBoxMultiRotation->setEnabled( true );
    _zSpinBoxMultiRotation->setEnabled( true );

    _applyMultiRotButton->setDisabled( _checkBoxAutoApply->isChecked( ) );
    _activeManipulation = true;
  }
}

void EditDock::updateRadius( float radius_ )
{
  auto oldSignals = _radiusOneSpinBox->blockSignals( true );
  if( _nSelected == 1 )
  {
    _radiusOneSpinBox->setValue( radius_ );
    _radiusOneSpinBox->setEnabled( true );
    _applyOneRadButton->setDisabled( _checkBoxAutoApply->isChecked( ) );
  }
  else // selected > 1
  {
    _radiusMultiSpinBox->setValue( radius_ );
    _radiusMultiSpinBox->setEnabled( true );
    _applyMultiRadButton->setDisabled( _checkBoxAutoApply->isChecked());
  }
  _radiusOneSpinBox->blockSignals( oldSignals );

  _activeManipulation = true;
}

void EditDock::applyPosition( void )
{
  if ( _activeManipulation )
  {
    if (_nSelected == 1)
    {
      float x,y,z;
      x = ( float ) _xSpinBoxOnePosition->value( );
      y = ( float ) _ySpinBoxOnePosition->value( );
      z = ( float ) _zSpinBoxOnePosition->value( );
      Eigen::Vector3f averagePosition( x,y,z );
      _viewer->changeAveragePos( averagePosition );
    } else {
      float x,y,z;
      x = ( float ) _xSpinBoxMultiPosition->value( );
      y = ( float ) _ySpinBoxMultiPosition->value( );
      z = ( float ) _zSpinBoxMultiPosition->value( );
      Eigen::Vector3f averagePosition( x,y,z );
      _viewer->changeAveragePos( averagePosition );
    }
  }
  else
    resetInspector( );
}

void EditDock::applyRotation( void )
{
  if ( _activeManipulation )
  {
    if( _nSelected > 1 )
    {
      float x,y,z;
      x = ( float ) _xSpinBoxMultiRotation->value( );
      y = ( float ) _ySpinBoxMultiRotation->value( );
      z = ( float ) _zSpinBoxMultiRotation->value( );
      Eigen::Vector3f rotation = Eigen::Vector3f( x,y,z );
      rotation *= M_PI / 180; //Deg to Rad
      Eigen::Quaternionf q = _rotToQuat( rotation );
      rotation = _quatToRot( q );
      _viewer->changeRotation( q );
    }
  }
  else
    resetInspector( );
}

void EditDock::applyRadius( void )
{
  if ( _activeManipulation )
  {
    if (_nSelected == 1)
    {
      float radius = ( float ) _radiusOneSpinBox->value( );
      _viewer->changeAverageRadius( radius );
    }
    else
    {
      float radius = ( float ) _radiusMultiSpinBox->value( );
      _viewer->changeAverageRadius( radius );
    }
  }
  else
    resetInspector( );
}


void EditDock::addMethod( void )
{
  auto modifier = static_cast< neuroeditor::TraceModifier::TModifierMethod >(
    _methodSelector->currentData( ).toInt( ));

  bool included = false;
  if ( modifier != neuroeditor::TraceModifier::CUSTOM )
  {
    for ( int i = 0; i < _methodsLayout->count( ); i++ )
    {
      ModifierWidget* modifierWidget =
        static_cast< ModifierWidget* >( _methodsLayout->itemAt( i )->widget( ));
      if ( modifierWidget->modifierMethod == modifier )
      {
        included = true;
        break;
      }
    }
  }

  if ( included )
    return;

  auto modifierWidget = new ModifierWidget( modifier );
  _methodsLayout->addWidget( modifierWidget );
  connect( modifierWidget, SIGNAL( modifierToDelete( QWidget* )),
           this, SLOT( removeMethod( QWidget* )));
  connect( modifierWidget, SIGNAL( applyModifier( ModifierWidget* )),
           this, SLOT( apply( ModifierWidget* )));

}

void EditDock::removeMethod( QWidget* modifierWidget_ )
{
  _methodsLayout->removeWidget( modifierWidget_ );
  delete modifierWidget_;
}

void EditDock::apply( ModifierWidget* modifierWidget_ )
{
  _viewer->saveState( );
  auto sections = _uniqueSections( );
  if ( _apply( modifierWidget_, sections ))
    _viewer->updateMorphology( );
  else
    _viewer->undoState( );
}

void EditDock::applyAll( void )
{
  if ( _methodsLayout->count( ) == 0 )
    return;
  _viewer->saveState( );
  auto sections = _uniqueSections( );
  bool modified = false;
  for( int i = 0; i < _methodsLayout->count( ); i++ )
  {
    ModifierWidget* modifierWidget =
      static_cast< ModifierWidget* >( _methodsLayout->itemAt( i )->widget( ));
    modified = modified | _apply( modifierWidget, sections );
  }

  if ( modified )
    _viewer->updateMorphology( );
  else
    _viewer->undoState( );
}

void EditDock::clear( void )
{
  while( _methodsLayout->count( ) > 0 )
  {
    delete _methodsLayout->takeAt( 0 )->widget( );
  }
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


void EditDock::_initMethodSelector( void )
{
  for ( int i = 0; i < neuroeditor::TraceModifier::numModifiers( ); i++ )
  {
    neuroeditor::TraceModifier::TModifierMethod modifierMethod =
      static_cast< neuroeditor::TraceModifier::TModifierMethod >( i );
    std::string description =
      neuroeditor::TraceModifier::description( modifierMethod );
    _methodSelector->addItem( QString( description.c_str( )), QVariant( i ));
  }
}

void EditDock::_initSpinBoxes( )
{
  _xSpinBoxOnePosition = new QDoubleSpinBox( );
  _xSpinBoxOnePosition->setSingleStep( 0.1 );
  _xSpinBoxOnePosition->setMaximum( MAXDOUBLE );
  _xSpinBoxOnePosition->setMinimum( -MAXDOUBLE );
  _xSpinBoxOnePosition->setValue( _xSpinBoxOnePosition->minimum() );
  _xSpinBoxOnePosition->setDisabled( true );
  _xSpinBoxOnePosition->setFixedWidth( 75 );
  _xSpinBoxOnePosition->setSpecialValueText( "---" );

  _ySpinBoxOnePosition = new QDoubleSpinBox( );
  _ySpinBoxOnePosition->setSingleStep( 0.1 );
  _ySpinBoxOnePosition->setMaximum( MAXDOUBLE );
  _ySpinBoxOnePosition->setMinimum( -MAXDOUBLE );
  _ySpinBoxOnePosition->setValue( _ySpinBoxOnePosition->minimum() );
  _ySpinBoxOnePosition->setDisabled( true );
  _ySpinBoxOnePosition->setFixedWidth( 75 );
  _ySpinBoxOnePosition->setSpecialValueText( "---" );

  _zSpinBoxOnePosition = new QDoubleSpinBox( );
  _zSpinBoxOnePosition->setSingleStep( 0.1 );
  _zSpinBoxOnePosition->setMaximum( MAXDOUBLE );
  _zSpinBoxOnePosition->setMinimum( -MAXDOUBLE );
  _zSpinBoxOnePosition->setValue( _zSpinBoxOnePosition->minimum() );
  _zSpinBoxOnePosition->setDisabled( true );
  _zSpinBoxOnePosition->setFixedWidth( 75 );
  _zSpinBoxOnePosition->setSpecialValueText( "---" );

  _radiusOneSpinBox = new QDoubleSpinBox( );
  _radiusOneSpinBox->setSingleStep( 0.05 );
  _radiusOneSpinBox->setMaximum( MAXDOUBLE );
  _radiusOneSpinBox->setValue( _radiusOneSpinBox->minimum() );
  _radiusOneSpinBox->setDisabled( true );
  _radiusOneSpinBox->setFixedWidth( 75 );
  _radiusOneSpinBox->setSpecialValueText( "---" );

  _xSpinBoxMultiPosition = new QDoubleSpinBox( );
  _xSpinBoxMultiPosition->setSingleStep( 0.1 );
  _xSpinBoxMultiPosition->setMaximum( MAXDOUBLE );
  _xSpinBoxMultiPosition->setMinimum( -MAXDOUBLE );
  _xSpinBoxMultiPosition->setValue( _xSpinBoxMultiPosition->minimum() );
  _xSpinBoxMultiPosition->setDisabled( true );
  _xSpinBoxMultiPosition->setFixedWidth( 75 );
  _xSpinBoxMultiPosition->setSpecialValueText( "---" );

  _ySpinBoxMultiPosition = new QDoubleSpinBox( );
  _ySpinBoxMultiPosition->setSingleStep( 0.1 );
  _ySpinBoxMultiPosition->setMaximum( MAXDOUBLE );
  _ySpinBoxMultiPosition->setMinimum( -MAXDOUBLE );
  _ySpinBoxMultiPosition->setValue( _ySpinBoxMultiPosition->minimum() );
  _ySpinBoxMultiPosition->setDisabled( true );
  _ySpinBoxMultiPosition->setFixedWidth( 75 );
  _ySpinBoxMultiPosition->setSpecialValueText( "---" );

  _zSpinBoxMultiPosition = new QDoubleSpinBox( );
  _zSpinBoxMultiPosition->setSingleStep( 0.1 );
  _zSpinBoxMultiPosition->setMaximum( MAXDOUBLE );
  _zSpinBoxMultiPosition->setMinimum( -MAXDOUBLE );
  _zSpinBoxMultiPosition->setValue( _zSpinBoxMultiPosition->minimum() );
  _zSpinBoxMultiPosition->setDisabled( true );
  _zSpinBoxMultiPosition->setFixedWidth( 75 );
  _zSpinBoxMultiPosition->setSpecialValueText( "---" );
  
  _xSpinBoxMultiRotation = new QDoubleSpinBox( );
  _xSpinBoxMultiRotation->setSingleStep( 1.0 );
  _xSpinBoxMultiRotation->setMaximum( MAXDOUBLE );
  _xSpinBoxMultiRotation->setMinimum( -MAXDOUBLE );
  _xSpinBoxMultiRotation->setValue( _xSpinBoxMultiRotation->minimum() );
  _xSpinBoxMultiRotation->setDisabled( true );
  _xSpinBoxMultiRotation->setFixedWidth( 75 );
  _xSpinBoxMultiRotation->setSpecialValueText( "---" );

  _ySpinBoxMultiRotation = new QDoubleSpinBox( );
  _ySpinBoxMultiRotation->setSingleStep( 1.0 );
  _ySpinBoxMultiRotation->setMaximum( MAXDOUBLE );
  _ySpinBoxMultiRotation->setMinimum( -MAXDOUBLE );
  _ySpinBoxMultiRotation->setValue( _ySpinBoxMultiRotation->minimum() );
  _ySpinBoxMultiRotation->setDisabled( true );
  _ySpinBoxMultiRotation->setFixedWidth( 75 );
  _ySpinBoxMultiRotation->setSpecialValueText( "---" );

  _zSpinBoxMultiRotation = new QDoubleSpinBox( );
  _zSpinBoxMultiRotation->setSingleStep( 1.0 );
  _zSpinBoxMultiRotation->setMaximum( MAXDOUBLE );
  _zSpinBoxMultiRotation->setMinimum( -MAXDOUBLE );
  _zSpinBoxMultiRotation->setValue( _zSpinBoxMultiRotation->minimum() );
  _zSpinBoxMultiRotation->setDisabled( true );
  _zSpinBoxMultiRotation->setFixedWidth( 75 );
  _zSpinBoxMultiRotation->setSpecialValueText( "---" );

  _radiusMultiSpinBox = new QDoubleSpinBox( );
  _radiusMultiSpinBox->setSingleStep( 0.05 );
  _radiusMultiSpinBox->setMaximum( MAXDOUBLE );
  _radiusMultiSpinBox->setValue( _radiusMultiSpinBox->minimum() );
  _radiusMultiSpinBox->setDisabled( true );
  _radiusMultiSpinBox->setFixedWidth( 75 );
  _radiusMultiSpinBox->setSpecialValueText( "---" );

  //Avoid autoapply
  _setBlockedSpinBoxSignals( true );

}

bool EditDock::_apply( ModifierWidget* mWidget_,
                           std::unordered_set< nsol::Section* >& sections_ )
{
  for ( unsigned int i = 0; i < mWidget_->modifierParams.size( ); i++ )
  {
    std::string paramName = mWidget_->paramNames[i];
    float paramValue = mWidget_->paramLineEdits[i]->text( ).toFloat( );
    mWidget_->modifierParams[paramName] = paramValue;
  }

  if ( mWidget_->modifierMethod != neuroeditor::TraceModifier::CUSTOM )
    return neuroeditor::TraceModifier::modify( sections_, mWidget_->modifierMethod,
                                            mWidget_->modifierParams );
  else if ( !mWidget_->scriptPath.empty( ))
    return neuroeditor::TraceModifier::customModify(
      sections_, mWidget_->scriptPath );
  return false;
}

bool EditDock::_setBlockedSpinBoxSignals( bool isBlocked )
{
  auto oldSignal = _xSpinBoxOnePosition->blockSignals( isBlocked );
  _ySpinBoxOnePosition->blockSignals( isBlocked );
  _zSpinBoxOnePosition->blockSignals( isBlocked );

  _radiusOneSpinBox->blockSignals( isBlocked );

  _xSpinBoxMultiPosition->blockSignals( isBlocked );
  _ySpinBoxMultiPosition->blockSignals( isBlocked );
  _zSpinBoxMultiPosition->blockSignals( isBlocked );

  _xSpinBoxMultiRotation->blockSignals( isBlocked );
  _ySpinBoxMultiRotation->blockSignals( isBlocked );
  _zSpinBoxMultiRotation->blockSignals( isBlocked );

  _radiusMultiSpinBox->blockSignals( isBlocked );

  return oldSignal;
}

void EditDock::_disableMultiLocation( )
{
  auto oldSignals = _setBlockedSpinBoxSignals( true );
  _xSpinBoxMultiPosition->setValue( _xSpinBoxMultiPosition->minimum() );
  _ySpinBoxMultiPosition->setValue( _ySpinBoxMultiPosition->minimum() );
  _zSpinBoxMultiPosition->setValue( _zSpinBoxMultiPosition->minimum() );

  _xSpinBoxMultiRotation->setValue( _xSpinBoxMultiRotation->minimum() );
  _ySpinBoxMultiRotation->setValue( _ySpinBoxMultiRotation->minimum() );
  _zSpinBoxMultiRotation->setValue( _zSpinBoxMultiRotation->minimum() );

  _radiusMultiSpinBox->setValue( _radiusMultiSpinBox->minimum() );
  _setBlockedSpinBoxSignals( oldSignals );

  _xSpinBoxMultiPosition->setDisabled( true );
  _ySpinBoxMultiPosition->setDisabled( true );
  _zSpinBoxMultiPosition->setDisabled( true );

  _xSpinBoxMultiRotation->setDisabled( true );
  _ySpinBoxMultiRotation->setDisabled( true );
  _zSpinBoxMultiRotation->setDisabled( true );

  _radiusMultiSpinBox->setDisabled( true );

  _applyMultiPosButton->setDisabled( true );
  _applyMultiRotButton->setDisabled( true );
  _applyMultiRadButton->setDisabled( true );
}

void EditDock::_disableOneLocation( )
{
  auto oldSignals = _setBlockedSpinBoxSignals( true );
  // Set Spinbox to minimun shows the special text, in this case "---"
  _xSpinBoxOnePosition->setValue( _xSpinBoxOnePosition->minimum() );
  _ySpinBoxOnePosition->setValue( _ySpinBoxOnePosition->minimum() );
  _zSpinBoxOnePosition->setValue( _zSpinBoxOnePosition->minimum() );

  _radiusOneSpinBox->setValue( _radiusOneSpinBox->minimum() );

  _setBlockedSpinBoxSignals( oldSignals );


  _xSpinBoxOnePosition->setDisabled( true );
  _ySpinBoxOnePosition->setDisabled( true );
  _zSpinBoxOnePosition->setDisabled( true );

  _radiusOneSpinBox->setDisabled( true );

  _applyOnePosButton->setDisabled( true );
  _applyOneRadButton->setDisabled( true );
}

void EditDock::_initSpinBoxesConnections( void )
{
  // Spinboxes
  QObject::connect( _xSpinBoxOnePosition,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyPosition()));
  QObject::connect( _ySpinBoxOnePosition,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyPosition()));
  QObject::connect( _zSpinBoxOnePosition,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyPosition()));

  QObject::connect( _radiusOneSpinBox,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyRadius()));


  QObject::connect( _xSpinBoxMultiPosition,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyPosition()));
  QObject::connect( _ySpinBoxMultiPosition,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyPosition()));
  QObject::connect( _zSpinBoxMultiPosition,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyPosition()));

  QObject::connect( _xSpinBoxMultiRotation,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyRotation()));
  QObject::connect( _ySpinBoxMultiRotation,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyRotation()));
  QObject::connect( _zSpinBoxMultiRotation,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyRotation()));

  QObject::connect( _radiusMultiSpinBox,SIGNAL ( valueChanged( double) ),
                    this, SLOT( applyRadius()));

  // Buttons
  QObject::connect( _applyOnePosButton,SIGNAL( clicked( )),
                    this, SLOT( applyPosition( )));

  QObject::connect( _applyOneRadButton,SIGNAL( clicked( )),
                    this, SLOT( applyRadius( )));


  QObject::connect( _applyMultiPosButton, SIGNAL(clicked( )),
                    this, SLOT( applyPosition( )));

  QObject::connect( _applyMultiRotButton,SIGNAL( clicked( )),
                    this, SLOT( applyRotation( )));

  QObject::connect( _applyMultiRadButton, SIGNAL( clicked( )),
                    this, SLOT( applyRadius( )));

  QObject::connect( _checkBoxAutoApply, SIGNAL(stateChanged(int)),
                    this, SLOT(autoApplyChanged(int)));



}

std::unordered_set< nsol::Section* > EditDock::_uniqueSections( void )
{
  auto morphoSt = _viewer->morphologyStructure( );
  auto selection = _viewer->selection( );
  std::unordered_set< nsol::Section* > uSections;
  for ( auto id: selection )
  {
    auto section = morphoSt->nodeToSection[morphoSt->idToNode[id]];
    if ( section != morphoSt->somaSection )
      uSections.insert( section );
  }
  if ( uSections.size( ) == 0 )
  {
    for ( auto neurite: morphoSt->morphology->neurites( ))
      for( auto section: neurite->sections( ))
        uSections.insert( section );
  }
  return uSections;
}

void EditDock::autoApplyChanged( int state )
{
  bool b = state == Qt::Checked;

  _applyOnePosButton->setEnabled( !b && _nSelected == 1 );
  _applyOneRadButton->setEnabled( !b && _nSelected == 1 );

  _applyMultiPosButton->setEnabled( !b && _nSelected > 1 );
  _applyMultiRotButton->setEnabled( !b && _nSelected > 1 );
  _applyMultiRadButton->setEnabled( !b && _nSelected > 1 );

  // Blocked signal to prevent autoapply
  _setBlockedSpinBoxSignals( !b );
}

void EditDock::updateSelection( int nSelected )
{
  _nSelected = nSelected;
  _inspectorGroupMulti->setTitle(QString("Multiple Node (%1 selected)").arg(nSelected));
  if (_nSelected == 0 ) {
    resetInspector();
    _viewer->showAxis(false);
  } else if (_nSelected == 1) {
    _disableMultiLocation( );
    _viewer->showAxis(false);
  } else {
    _disableOneLocation( );
    _viewer->showAxis( this->isVisible( ));
  }

}

void EditDock::onVisibilityChanged( bool isVisible )
{
  _viewer->showAxis(isVisible && _nSelected > 1);
}
