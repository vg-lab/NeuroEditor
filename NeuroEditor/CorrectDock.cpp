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

#include "CorrectDock.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>


TestWidget::TestWidget( neuroeditor::Tester::TTesterMethod testMethod_ )
  : testMethod( testMethod_ )
{

  std::string testerDescription;
  testerDescription.append( neuroeditor::Tester::description( testMethod ));

  QHBoxLayout* layout = new QHBoxLayout( );
  layout->setAlignment( Qt::AlignLeft );
  setLayout( layout );
  layout->addWidget( new QLabel( QString( testerDescription.c_str( ))));
  _fixerCombo = new QComboBox( );
  auto vline = new QFrame( );
  vline->setFrameShape( QFrame::VLine );
  vline->setFrameShadow( QFrame::Sunken );
  layout->addWidget( vline );
  layout->addWidget( new QLabel( QString( "Action to apply: " )));
  layout->addWidget( _fixerCombo );

  connect( _fixerCombo, SIGNAL( currentIndexChanged( int )),
           this, SLOT( changeFixerMethod( int )));

  QIcon removeIcon( QString::fromUtf8(":/icons/list-remove.png"));
  auto testRemove = new QToolButton( );
  testRemove->setIcon( removeIcon );
  layout->addWidget( testRemove );

  connect( testRemove, SIGNAL( pressed( void )),
           this, SLOT( sendRemoveSignal( void )));

  auto fixers = neuroeditor::Tester::associatedFixers( testMethod );
  if ( fixers.size( ) > 0 )
  {
    fixerMethod = fixers[0];
    for ( auto fixer: fixers )
    {
      int intFixer = static_cast< int >( fixer );
      std::string description = neuroeditor::Fixer::description( fixer );
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
  auto fixer = static_cast< neuroeditor::Fixer::TFixerMethod >(
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
  auto selectorLayout = new QHBoxLayout( );
  selectorWidget->setLayout( selectorLayout );
  selectorLayout->setAlignment( Qt::AlignLeft );

  selectorLayout->addWidget( new QLabel( QString( "Test method:" )) );
  _testSelector = new QComboBox( );
  selectorLayout->addWidget( _testSelector );
  QIcon addIcon( QString::fromUtf8(":/icons/list-add.png"));
  _testAdder = new QToolButton( );
  _testAdder->setIcon( addIcon );
  selectorLayout->addWidget( _testAdder );
  QIcon addAllIcon( QString::fromUtf8(":/icons/list-add-all.png"));
  _testAddAll = new QToolButton( );
  _testAddAll->setIcon( addAllIcon );
  selectorLayout->addWidget( _testAddAll );

  QIcon helpIcon( QString::fromUtf8(":/icons/help-browser.png"));
  auto testMethodHelp = new QToolButton( );
  testMethodHelp->setIcon( helpIcon );
  selectorLayout->addWidget( testMethodHelp );

  connect( _testAdder, SIGNAL( pressed( )),
           this, SLOT( addTest( void )));
  connect( _testAddAll, SIGNAL( pressed( )),
           this, SLOT( addAllTests( void )));

  auto message = QString( "This menu allows the selection of several tests that fix common errors present in the neuron descriptions. Each test has several associated fixing actions that can be selected. Once the tests and their associated actions are defined, they will be applied over the neuron morphology, fixing the errors or giving a text message in the Output panel below." );
  _testMethodHelpBox = new QMessageBox(
    QMessageBox::Information, QString( "Help" ), message );
  QObject::connect( testMethodHelp, SIGNAL( pressed( )),
                    _testMethodHelpBox, SLOT( exec( )));

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

  QPushButton* clearButton = new QPushButton( QString( "clear all" ));
  clearButton->setMaximumSize( QSize( 80, 40 ));
  buttonsLayout->addWidget( clearButton );
  QPushButton* runButton = new QPushButton( QString( "apply all" ));
  runButton->setMaximumSize( QSize( 80, 40 ));
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

  // connect( _viewer, SIGNAL( morphologyChanged( )),
  //          this, SLOT( clearOutput( )));
}

void CorrectDock::addTest( void )
{
  auto tester = static_cast< neuroeditor::Tester::TTesterMethod >(
    _testSelector->currentData( ).toInt( ));

  _addTest( tester );
}

void CorrectDock::addAllTests( void )
{
  for ( int i = 0; i < _testSelector->count( ); i++ )
  {
    auto tester = static_cast< neuroeditor::Tester::TTesterMethod >(
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
  _viewer->saveState( );

  bool modified = false;
  for( int i = 0; i < _testsLayout->count( ); i++ )
  {
    TestWidget* test =
      static_cast< TestWidget* >( _testsLayout->itemAt( i )->widget( ));
    std::string testOut( "  -Test  ");
    testOut.append( neuroeditor::Tester::description( test->testMethod  ));
    testOut.append( ": ");
    _outputLayout->addWidget( new QLabel( QString( testOut.c_str( ))));

    auto testResult = neuroeditor::Tester::test(
      _viewer->morphologyStructure( ), test->testMethod );

    if ( testResult.size( ) == 0 )
    {
      testOut = std::string( "\tNo errors detected" );
      _outputLayout->addWidget( new QLabel( QString( testOut.c_str( ))));
    }
    else
    {
      auto fixResult =
      neuroeditor::Fixer::fix( testResult, _viewer->morphologyStructure( ),
                               test->fixerMethod );

      if ( fixResult.size( ) == 0 )
      {
        modified = true;
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

  auto neurites = _viewer->morphologyStructure( )->morphology->neurites( );

  unsigned int numNeurites = neurites.size( );
  unsigned int numSections = 0;
  unsigned int numTracingPoints = 0;

  for ( auto neurite: neurites )
  {
    numSections += neurite->sections( ).size( );
    numTracingPoints ++;
    for ( auto section: neurite->sections( ))
    {
      numTracingPoints += section->nodes( ).size( ) - 1;
    }
  }

  auto hline = new QFrame( );
  hline->setFrameShape( QFrame::HLine );
  hline->setFrameShadow( QFrame::Sunken );
  _outputLayout->addWidget( hline );
  std::string stats( "number neurites: " + std::to_string( numNeurites ) +
                     "\nnumber sections: " + std::to_string( numSections ) +
                     "\nnumber tracing points: " +
                     std::to_string( numTracingPoints ));
  _outputLayout->addWidget(
    new QLabel( QString( stats.c_str( ))));

  if ( modified )
    _viewer->updateMorphology( );
  else
    _viewer->undoState( );


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
  for ( int i = 0; i < neuroeditor::Tester::numTesters( ); i++ )
  {
    neuroeditor::Tester::TTesterMethod testerMethod =
      static_cast< neuroeditor::Tester::TTesterMethod >( i );
    std::string description = neuroeditor::Tester::description( testerMethod );
    _testSelector->addItem( QString( description.c_str( )), QVariant( i ));
  }
}

void CorrectDock::_addTest( neuroeditor::Tester::TTesterMethod testerMethod_ )
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
