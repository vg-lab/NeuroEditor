#include "CorrectDock.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>


TestWidget::TestWidget( retracer::Tester::TTesterMethod testMethod_ )
  : testMethod( testMethod_ )
{

  std::string testerDescription;
  testerDescription.append( retracer::Tester::description( testMethod ));
  testerDescription.append( ": " );

  QHBoxLayout* layout = new QHBoxLayout( );
  setLayout( layout );
  layout->addWidget( new QLabel( QString( testerDescription.c_str( ))));
  _fixerCombo = new QComboBox( );
  layout->addWidget( _fixerCombo );

  connect( _fixerCombo, SIGNAL( currentIndexChanged( int )),
           this, SLOT( changeFixerMethod( int )));

  QIcon removeIcon( QString::fromUtf8(":/icons/close.png"));
  auto testRemove = new QToolButton( );
  testRemove->setIcon( removeIcon );
  layout->addWidget( testRemove );

  connect( testRemove, SIGNAL( pressed( void )),
           this, SLOT( sendRemoveSignal( void )));

  auto fixers = retracer::Tester::associatedFixers( testMethod );
  if ( fixers.size( ) > 0 )
  {
    fixerMethod = fixers[0];
    for ( auto fixer: fixers )
    {
      int intFixer = static_cast< int >( fixer );
      std::string description = retracer::Fixer::description( fixer );
      _fixerCombo->addItem( QString( description.c_str( )),
                            QVariant( intFixer ));
    }
  }
}

TestWidget::~TestWidget( void )
{

}

void TestWidget::sendRemoveSignal( void )
{
  Q_EMIT testToDelete( (QWidget*)this );
}

void TestWidget::changeFixerMethod( int /*index_*/ )
{
  auto fixer = static_cast< retracer::Fixer::TFixerMethod >(
    _fixerCombo->currentData( ).toInt( ));
  fixerMethod = fixer;
}

CorrectDock::CorrectDock( void )
  : QDockWidget( )
{

}

CorrectDock::~CorrectDock( void )
{

}

void CorrectDock::init( Viewer* viewer_ )
{
  _viewer = viewer_;

  setSizePolicy( QSizePolicy::Expanding,
                 QSizePolicy::MinimumExpanding );
  setFeatures( QDockWidget::DockWidgetClosable |
               QDockWidget::DockWidgetMovable |
               QDockWidget::DockWidgetFloatable );
  setWindowTitle( QString( "Correct" ));
  setMinimumSize( 500, 200 );

  QWidget* mainWidget = new QWidget( );
  setWidget( mainWidget );
  mainWidget->setMaximumHeight( 600 );
  QVBoxLayout* correctDockLayout = new QVBoxLayout( );
  mainWidget->setLayout( correctDockLayout );
  correctDockLayout->setAlignment( Qt::AlignTop );

  QGroupBox* selectionGroup = new QGroupBox( );
  QVBoxLayout* selectionGroupLayout = new QVBoxLayout( );
  selectionGroup->setLayout( selectionGroupLayout );
  correctDockLayout->addWidget( selectionGroup );


  QWidget* selectorWidget = new QWidget( );
  selectionGroupLayout->addWidget( selectorWidget );
  auto selectorLayout = new QGridLayout( );
  selectorWidget->setLayout( selectorLayout );

  _testSelector = new QComboBox( );
  selectorLayout->addWidget( _testSelector, 0, 0 );
  QIcon addIcon( QString::fromUtf8(":/icons/add.png"));
  _testAdder = new QToolButton( );
  _testAdder->setIcon( addIcon );
  selectorLayout->addWidget( _testAdder, 0, 1 );
  _testAddAll = new QToolButton( );
  _testAddAll->setIcon( addIcon );
  selectorLayout->addWidget( _testAddAll, 0, 2 );

  connect( _testAdder, SIGNAL( pressed( )),
           this, SLOT( addTest( void )));
  connect( _testAddAll, SIGNAL( pressed( )),
           this, SLOT( addAllTests( void )));

  _initTestSelector( );

  QScrollArea* scrollArea = new QScrollArea( );
  scrollArea->setWidgetResizable(true);
  selectionGroupLayout->addWidget( scrollArea );
  QWidget* testWidget = new QWidget( );
  _testsLayout = new QVBoxLayout( );
  _testsLayout->setAlignment( Qt::AlignTop );
  testWidget->setLayout( _testsLayout );
  scrollArea->setWidget( testWidget );
  scrollArea->setMaximumHeight( 400 );

  QWidget* buttonsWidget = new QWidget( );
  QHBoxLayout* buttonsLayout = new QHBoxLayout( );
  buttonsWidget->setLayout( buttonsLayout );
  selectionGroupLayout->addWidget( buttonsWidget );

  QPushButton* clearButton = new QPushButton( QString( "clear" ));
  buttonsLayout->addWidget( clearButton );
  QPushButton* runButton = new QPushButton( QString( "correct" ));
  buttonsLayout->addWidget( runButton );

  connect( clearButton, SIGNAL( pressed( )),
           this, SLOT( clearTests( )));
  connect( runButton, SIGNAL( pressed( )),
           this, SLOT( correct( )));


  QGroupBox* outputGroup = new QGroupBox( QString( "Output"));
  QVBoxLayout* outputGroupLayout = new QVBoxLayout( );
  outputGroup->setLayout( outputGroupLayout );
  correctDockLayout->addWidget( outputGroup );

  scrollArea = new QScrollArea( );
  scrollArea->setWidgetResizable(true);
  outputGroupLayout->addWidget( scrollArea );

  QWidget* outputWidget = new QWidget( );
  _outputLayout = new QVBoxLayout( );
  _outputLayout->setAlignment( Qt::AlignTop );
  outputWidget->setLayout( _outputLayout );
  scrollArea->setWidget( outputWidget );
  scrollArea->setMaximumHeight( 400 );


  connect( _viewer, SIGNAL( morphologyChanged( )),
           this, SLOT( clearOutput( )));
}

void CorrectDock::addTest( void )
{
  auto tester = static_cast< retracer::Tester::TTesterMethod >(
    _testSelector->currentData( ).toInt( ));

  _addTest( tester );
}

void CorrectDock::addAllTests( void )
{
  for ( int i = 0; i < _testSelector->count( ); i++ )
  {
    auto tester = static_cast< retracer::Tester::TTesterMethod >(
      _testSelector->itemData( i ).toInt( ));
    _addTest( tester );
  }
}

void CorrectDock::removeTest( QWidget* testWidget_ )
{
  _testsLayout->removeWidget( testWidget_ );
  delete testWidget_;
}

void CorrectDock::clearTests( void )
{
  while( _testsLayout->count( ) > 0 )
  {
    delete _testsLayout->takeAt( 0 )->widget( );
  }
}

void CorrectDock::correct( void )
{
  clearOutput( );

  _outputLayout->addWidget(
    new QLabel( QString( "Cabecera con informacion morfologica" )));
  auto hline = new QFrame( );
  hline->setFrameShape( QFrame::HLine );
  hline->setFrameShadow( QFrame::Sunken );
  _outputLayout->addWidget( hline );


  for( int i = 0; i < _testsLayout->count( ); i++ )
  {
    TestWidget* test =
      static_cast< TestWidget* >( _testsLayout->itemAt( i )->widget( ));
    std::string testOut( "  -Test  ");
    testOut.append( retracer::Tester::description( test->testMethod  ));
    testOut.append( ": ");
    _outputLayout->addWidget( new QLabel( QString( testOut.c_str( ))));

    auto testResult = retracer::Tester::test( _viewer->morphologyStructure( ),
                                              test->testMethod );

    if ( testResult.size( ) == 0 )
    {
      testOut = std::string( "\tNo errors detected" );
      _outputLayout->addWidget( new QLabel( QString( testOut.c_str( ))));
    }
    else
    {
      auto fixResult =
      retracer::Fixer::fix( testResult, _viewer->morphologyStructure( ),
                            test->fixerMethod );

      if ( fixResult.size( ) == 0 )
      {
        testOut = std::string( "\tCorrected: " +
                               std::to_string( testResult.size( )) +
                               " errors" );
        _outputLayout->addWidget( new QLabel( QString( testOut.c_str( ))));
      }
      else
      {
        QWidget* testResultWidget = new QWidget(  );
        QHBoxLayout* testResultLayout = new QHBoxLayout( );
        testResultLayout->setAlignment( Qt::AlignLeft );
        testResultWidget->setLayout( testResultLayout );
        _outputLayout->addWidget( testResultWidget );
        testOut = std::string( "\tManual correction: ");
        testResultLayout->addWidget( new QLabel( QString( testOut.c_str( ))));
        for( auto indices: fixResult )
        {
          auto rButton = new ResultButton( indices, _viewer );
          testResultLayout->addWidget( rButton );
        }
      }
    }
  }


}

void CorrectDock::clearOutput( void )
{
  while( _outputLayout->count( ) > 0 )
  {
    delete _outputLayout->takeAt( 0 )->widget( );
  }
}

void CorrectDock::_initTestSelector( void )
{
  for ( int i = 0; i < retracer::Tester::numTesters( ); i++ )
  {
    retracer::Tester::TTesterMethod testerMethod =
      static_cast< retracer::Tester::TTesterMethod >( i );
    std::string description = retracer::Tester::description( testerMethod );
    _testSelector->addItem( QString( description.c_str( )), QVariant( i ));
  }
}

void CorrectDock::_addTest( retracer::Tester::TTesterMethod testerMethod_ )
{
  bool included = false;
  for ( int i = 0; i < _testsLayout->count( ); i++ )
  {
    TestWidget* testWidget =
      static_cast< TestWidget* >( _testsLayout->itemAt( i )->widget( ));
    if ( testWidget->testMethod  == testerMethod_ )
    {
      included = true;
      break;
    }
  }

  if ( included )
    return;

  auto testWidget = new TestWidget( testerMethod_ );
  _testsLayout->addWidget( testWidget );
  connect( testWidget, SIGNAL( testToDelete( QWidget* )),
           this, SLOT( removeTest( QWidget* )));
}
