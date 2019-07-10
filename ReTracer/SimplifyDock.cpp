#include "SimplifyDock.h"

#include <QGroupBox>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>

ModifierWidget::ModifierWidget(
  retracer::TraceModifier::TModifierMethod modifierMethod_ )
  : modifierMethod( modifierMethod_ )
{
  modifierParams = retracer::TraceModifier::defaultParams( modifierMethod );

  std::string description;
  description.append( retracer::TraceModifier::description( modifierMethod ));
  description.append( ": " );

  QHBoxLayout* layout = new QHBoxLayout( );
  setLayout( layout );
  layout->setAlignment( Qt::AlignLeft );
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

  if ( modifierMethod == retracer::TraceModifier::CUSTOM )
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
      paramLineEdit->setFixedWidth( 50 );
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

SimplifyDock::SimplifyDock( void )
  : QDockWidget( )
{

}

SimplifyDock::~SimplifyDock( void )
{

}

void SimplifyDock::init( Viewer* viewer_ )
{
  _viewer = viewer_;

  setSizePolicy( QSizePolicy::Expanding,
                 QSizePolicy::MinimumExpanding );
  setFeatures( QDockWidget::DockWidgetClosable |
               QDockWidget::DockWidgetMovable |
               QDockWidget::DockWidgetFloatable );
  setWindowTitle( QString( "Simplifycation" ));
  setMinimumSize( 450, 200 );

  QWidget* mainWidget = new QWidget( );
  setWidget( mainWidget );
  mainWidget->setMaximumHeight( 350 );
  QVBoxLayout* simplifyDockLayout = new QVBoxLayout( );
  mainWidget->setLayout( simplifyDockLayout );
  simplifyDockLayout->setAlignment( Qt::AlignTop );

  QGroupBox* simplifyGroup = new QGroupBox( );
  QVBoxLayout* simplifyGroupLayout = new QVBoxLayout( );
  simplifyGroup->setLayout( simplifyGroupLayout );
  simplifyDockLayout->addWidget( simplifyGroup );

  QWidget* selectorWidget = new QWidget( );
  simplifyGroupLayout->addWidget( selectorWidget );
  auto selectorLayout = new QGridLayout( );
  selectorWidget->setLayout( selectorLayout );

  _methodSelector = new QComboBox( );
  selectorLayout->addWidget( _methodSelector, 0, 0 );
  QIcon addIcon( QString::fromUtf8( ":/icons/list-add.png" ));
  auto methodAdder = new QToolButton( );
  methodAdder->setIcon( addIcon );
  selectorLayout->addWidget( methodAdder, 0, 1 );

  connect( methodAdder, SIGNAL( pressed( )),
           this, SLOT( addMethod( void )));

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

  QPushButton* clearButton = new QPushButton( QString( "clear" ));
  buttonsLayout->addWidget( clearButton );
  QPushButton* applyAllButton = new QPushButton( QString( "apply all" ));
  buttonsLayout->addWidget( applyAllButton );

  connect( clearButton, SIGNAL( pressed( )),
           this, SLOT( clear( )));
  connect( applyAllButton, SIGNAL( pressed( )),
           this, SLOT( applyAll( )));

}


void SimplifyDock::addMethod( void )
{
  auto modifier = static_cast< retracer::TraceModifier::TModifierMethod >(
    _methodSelector->currentData( ).toInt( ));

  bool included = false;
  if ( modifier != retracer::TraceModifier::CUSTOM )
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

void SimplifyDock::removeMethod( QWidget* modifierWidget_ )
{
  _methodsLayout->removeWidget( modifierWidget_ );
  delete modifierWidget_;
}

void SimplifyDock::apply( ModifierWidget* modifierWidget_ )
{
  _viewer->saveState( );
  auto sections = _uniqueSections( );
  if ( _apply( modifierWidget_, sections ))
    _viewer->updateMorphology( );
  else
    _viewer->undoState( );
}

void SimplifyDock::applyAll( void )
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

void SimplifyDock::clear( void )
{
  while( _methodsLayout->count( ) > 0 )
  {
    delete _methodsLayout->takeAt( 0 )->widget( );
  }
}

void SimplifyDock::_initMethodSelector( void )
{
  for ( int i = 0; i < retracer::TraceModifier::numModifiers( ); i++ )
  {
    retracer::TraceModifier::TModifierMethod modifierMethod =
      static_cast< retracer::TraceModifier::TModifierMethod >( i );
    std::string description =
      retracer::TraceModifier::description( modifierMethod );
    _methodSelector->addItem( QString( description.c_str( )), QVariant( i ));
  }
}

bool SimplifyDock::_apply( ModifierWidget* mWidget_,
                           std::unordered_set< nsol::Section* >& sections_ )
{
  for ( unsigned int i = 0; i < mWidget_->modifierParams.size( ); i++ )
  {
    std::string paramName = mWidget_->paramNames[i];
    float paramValue = mWidget_->paramLineEdits[i]->text( ).toFloat( );
    mWidget_->modifierParams[paramName] = paramValue;
  }

  if ( mWidget_->modifierMethod != retracer::TraceModifier::CUSTOM )
    return retracer::TraceModifier::modify( sections_, mWidget_->modifierMethod,
                                            mWidget_->modifierParams );
  else if ( !mWidget_->scriptPath.empty( ))
    return retracer::TraceModifier::customModify(
      sections_, mWidget_->scriptPath );
  else
    std::cout << "empty path" << std::endl;
  return false;
}

std::unordered_set< nsol::Section* > SimplifyDock::_uniqueSections( void )
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
