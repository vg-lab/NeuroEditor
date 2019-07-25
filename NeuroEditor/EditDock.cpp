#include "EditDock.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include <iostream>

ModifierWidget::ModifierWidget(
  neuroeditor::TraceModifier::TModifierMethod modifierMethod_ )
  : modifierMethod( modifierMethod_ )
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
  scriptPath = path.toStdString( );
  _scriptPathLine->setText( path );
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
  mainWidget->setMaximumHeight( 500 );
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

  auto message = QString( "In case a single node is selected, its 3D coordinates (x, y, z) are shown and can be edited. In case of multiple selection, the average position (x, y, z) of the selected nodes is shown. Editing these values implies modifying the selected nodes coordinates accordingly in order to obtain this new average position." );
  _positionHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( positionHelp, SIGNAL( pressed( )),
                    _positionHelpBox, SLOT( exec( )));

  message = QString( "In case multiple nodes are selected, the rotation is applied taking as reference a pivot computed as the average of the selected nodes positions." );
  _rotationHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( rotationHelp, SIGNAL( pressed( )),
                    _rotationHelpBox, SLOT( exec( )));

  message = QString( "In case a single node is selected, this box shows its radius and allows its edition. In case of multiple selection, it shows the average radius of the selected nodes; editing this value implies modifying the selected nodes radii accordingly in order to obtain the new average radius." );
  _radiusHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( radiusHelp, SIGNAL( pressed( )),
                    _radiusHelpBox, SLOT( exec( )));

  QGroupBox* simplifyGroup = new QGroupBox( );
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

  message = QString( "This menu provides several simplification and enhancement methods to apply over the neuron morphological tracing. Each method has its own parameters that are configurable by the user. In addition, there is a particular method named “Custom method” that allows to run user-written python code to simplify or enhance the tracing.\nIn case there are selected nodes, the simplification will run over the morphological sections that are completely or partially selected; in case no node is selected, the simplification will run over all the sections composing the tracing." );
  _simplifyMethodHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( simplifyMethodHelp, SIGNAL( pressed( )),
                    _simplifyMethodHelpBox, SLOT( exec( )));

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

  QPushButton* clearButton = new QPushButton( QString( "clear all" ));
  clearButton->setMaximumSize( QSize( 80, 40 ));
  buttonsLayout->addWidget( clearButton );
  QPushButton* applyAllButton = new QPushButton( QString( "apply all" ));
  applyAllButton->setMaximumSize( QSize( 80, 40 ));
  buttonsLayout->addWidget( applyAllButton );

  connect( clearButton, SIGNAL( pressed( )),
           this, SLOT( clear( )));
  connect( applyAllButton, SIGNAL( pressed( )),
           this, SLOT( applyAll( )));
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
