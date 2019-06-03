#include "SelectDock.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>

#include <iostream>

SelectDock::SelectDock( void )
  : QDockWidget( )
{

}

void SelectDock::init( Viewer* viewer_ )
{
  _viewer = viewer_;

  setSizePolicy( QSizePolicy::Expanding,
                 QSizePolicy::MinimumExpanding );
  setFeatures( QDockWidget::DockWidgetClosable |
               QDockWidget::DockWidgetMovable |
               QDockWidget::DockWidgetFloatable );
  setWindowTitle( QString( "Selection" ));
  setMinimumSize( 200, 200 );

  QWidget* mainWidget = new QWidget( );
  setWidget( mainWidget );
  mainWidget->setMaximumHeight( 500 );
  QVBoxLayout* selectDockLayout = new QVBoxLayout( );
  selectDockLayout->setAlignment( Qt::AlignTop );
  mainWidget->setLayout( selectDockLayout );

  QGroupBox* selectionTreeGroup = new QGroupBox( QString( "Selection tree" ));
  QGridLayout* selectionTreeLayout = new QGridLayout( );
  selectionTreeGroup->setLayout( selectionTreeLayout );
  selectDockLayout->addWidget( selectionTreeGroup );
  _morphologyTree = new QTreeView( );
  selectionTreeLayout->addWidget( _morphologyTree, 0, 0, 1, 2 );
  _morphologyTree->setModel( viewer_->getTreeModel( ));
  _morphologyTree->setSelectionMode( QTreeView::NoSelection );
  _morphologyTree->setSelectionBehavior( QTreeView::SelectRows );
  _morphologyTree->setExpandsOnDoubleClick( false );
  auto selectAllButton = new QPushButton( QString( "Select All" ));
  selectAllButton->setFixedWidth( 105 );
  selectionTreeLayout->addWidget( selectAllButton, 1, 0, 1, 1 );
  auto clearSelectionButton = new QPushButton( QString( "Clear Selection" ));
  clearSelectionButton->setFixedWidth( 105 );
  selectionTreeLayout->addWidget( clearSelectionButton, 1, 1, 1, 1 );

  QObject::connect( _morphologyTree, SIGNAL( clicked( const QModelIndex )),
                    this, SLOT( clicked( const QModelIndex ) ));
  QObject::connect( _viewer, SIGNAL( morphologyChanged( )),
                    this, SLOT( updateMorphology( )));
  QObject::connect(
    this, SIGNAL( updateSelectionSignal( std::unordered_set< int > )),
    viewer_, SLOT( updateSelection( std::unordered_set< int > )));
  QObject::connect(
    viewer_, SIGNAL( updateSelectionSignal( std::unordered_set< int > )),
    this, SLOT( updateSelection( std::unordered_set< int > )));
  QObject::connect( selectAllButton, SIGNAL( clicked( )),
                    this, SLOT( selectAll( )));
  QObject::connect( clearSelectionButton, SIGNAL( clicked( )),
                    this, SLOT( clearSelection( )));

  auto hline = new QFrame( );
  hline->setFrameShape( QFrame::HLine );
  hline->setFrameShadow( QFrame::Sunken );
  selectDockLayout->addWidget( hline );

  QGroupBox* selectionTypeGroup =
    new QGroupBox( QString( "Click selection type" ));
  QGridLayout* selectionTypeLayout = new QGridLayout( );
  selectionTypeGroup->setLayout( selectionTypeLayout );
  selectDockLayout->addWidget( selectionTypeGroup );

  auto nodeRadioButton = new QRadioButton( QString( "node" ));
  selectionTypeLayout->addWidget( nodeRadioButton, 0, 0, 1, 1 );
  auto sectionRadioButton = new QRadioButton( QString( "section" ));
  selectionTypeLayout->addWidget( sectionRadioButton, 0, 1, 1, 1 );
  auto neuriteRadioButton = new QRadioButton( QString( "neurite" ));
  selectionTypeLayout->addWidget( neuriteRadioButton, 0, 2, 1, 1 );
  nodeRadioButton->setChecked(true);
  connect( nodeRadioButton, SIGNAL( clicked( )),
           this, SLOT( nodeButtonClicked( )));
  connect( sectionRadioButton, SIGNAL( clicked( )),
           this, SLOT( sectionButtonClicked( )));
  connect( neuriteRadioButton, SIGNAL( clicked( )),
           this, SLOT( neuriteButtonClicked( )));

  QGroupBox* selectionModeGroup =
    new QGroupBox( QString( "Click selection mode" ));
  QGridLayout* selectionModeLayout = new QGridLayout( );
  selectionModeGroup->setLayout( selectionModeLayout );
  selectDockLayout->addWidget( selectionModeGroup );

  auto inclusiveRadioButton = new QRadioButton( QString( "inclusive" ));
  selectionModeLayout->addWidget( inclusiveRadioButton, 0, 0, 1, 1 );
  auto exclusiveRadioButton = new QRadioButton( QString( "exclusive" ));
  selectionModeLayout->addWidget( exclusiveRadioButton, 0, 1, 1, 1 );
  inclusiveRadioButton->setChecked(true);
  connect( inclusiveRadioButton, SIGNAL( clicked( )),
           this, SLOT( inclusiveButtonClicked( )));
  connect( exclusiveRadioButton, SIGNAL( clicked( )),
           this, SLOT( exclusiveButtonClicked( )));

}

void SelectDock::updateMorphology( void )
{
  _morphologyTree->setModel( _viewer->getTreeModel( ));
}

void SelectDock::clicked( const QModelIndex& index_ )
{
  auto index =  index_.sibling( index_.row( ), 0 );

  if ( _morphologyTree->selectionModel( )->isSelected( index ))
  {
    _recursiveDownSelectDeselectItem( index, QItemSelectionModel::Deselect );
    _recursiveUpSelectDeselectItem( index.parent( ),
                                    QItemSelectionModel::Deselect );
  }
  else
  {
    _recursiveDownSelectDeselectItem( index, QItemSelectionModel::Select );
    _recursiveUpSelectDeselectItem( index.parent( ),
                                    QItemSelectionModel::Select );
  }

  auto selectedIndices = _fromTreeToSelection( );
  Q_EMIT updateSelectionSignal( selectedIndices );
}

void SelectDock::updateSelection( std::unordered_set< int > selection_ )
{
  _fromSelectionToTree( selection_ );
}

void SelectDock::selectAll( void )
{
  std::unordered_set< int > selection;
  for( int i = 0; i < _morphologyTree->model( )->rowCount( ); ++i )
  {
    auto child = _morphologyTree->model( )->index( i, 0 );
    _recursiveSelectAll( child, selection );
  }
  Q_EMIT updateSelectionSignal( selection );
  _morphologyTree->setFocus( );
}

void SelectDock::clearSelection( void )
{
  std::unordered_set< int > selection;
  _fromSelectionToTree( selection );
  Q_EMIT updateSelectionSignal( selection );
  _morphologyTree->setFocus( );
}

void SelectDock::nodeButtonClicked( void )
{
  _viewer->updateSelectionType( Viewer::NODE );
}

void SelectDock::sectionButtonClicked( void )
{
  _viewer->updateSelectionType( Viewer::SECTION );
}

void SelectDock::neuriteButtonClicked( void )
{
  _viewer->updateSelectionType( Viewer::NEURITE );
}

void SelectDock::inclusiveButtonClicked( void )
{
  _viewer->updateSelectionInclusionMode( Viewer::INCLUSIVE );
}

void SelectDock::exclusiveButtonClicked( void )
{
  _viewer->updateSelectionInclusionMode( Viewer::EXCLUSIVE );
}

std::unordered_set< int > SelectDock::_fromTreeToSelection( void )
{
  std::unordered_set< int > selection;
  auto indicesList = _morphologyTree->selectionModel( )->selectedRows( );

  for ( auto index: indicesList )
    if ( index.model( )->rowCount( index ) == 0 )
      selection.insert( index.sibling( index.row( ), 1 ).data( ).toInt( ));
  return selection;
}

void SelectDock::_fromSelectionToTree( std::unordered_set< int > selection_ )
{
  _morphologyTree->selectionModel( )->clear( );

  for( int i = 0; i < _morphologyTree->model( )->rowCount( ); ++i )
  {
    auto child = _morphologyTree->model( )->index( i, 0 );
    _recursiveFromSelectionToTree( child, selection_ );
  }
}

bool SelectDock::_recursiveFromSelectionToTree(
  QModelIndex index_, std::unordered_set< int >& selection_ )
{
  if ( !index_.isValid( ))
    return true;
  if ( _morphologyTree->model( )->rowCount( index_ ) == 0 )
  {
    int nodeId = index_.sibling( index_.row( ), 1 ).data( ).toInt( );
    // std::cout << "Node id " << nodeId << std::endl;
    if ( selection_.find( nodeId ) != selection_.end( ))
    {
      _morphologyTree->selectionModel( )->select(
        index_, QItemSelectionModel::Select );
      _morphologyTree->selectionModel( )->select(
        index_.sibling( index_.row( ), 1 ), QItemSelectionModel::Select );
      return true;
    }
    return false;
  }
  else
  {
    bool allSelected = true;
    for ( int i = 0; i < _morphologyTree->model( )->rowCount( index_ ); i++ )
    {
      auto childIndex = _morphologyTree->model( )->index( i, 0, index_ );
      allSelected = allSelected &
        _recursiveFromSelectionToTree( childIndex, selection_ );
    }
    if ( allSelected )
    {
      _morphologyTree->selectionModel( )->select(
        index_, QItemSelectionModel::Select );
      _morphologyTree->selectionModel( )->select(
        index_.sibling( index_.row( ), 1 ), QItemSelectionModel::Select );
    }
    return allSelected;
  }
}

void SelectDock::_recursiveDownSelectDeselectItem(
  const QModelIndex& index_,
  const QItemSelectionModel::SelectionFlag state_ )
{
  if ( !index_.isValid( ))
    return;
  auto model = _morphologyTree->model( );
  auto selectionModel = _morphologyTree->selectionModel( );
  selectionModel->select( index_, state_ );
  selectionModel->select( index_.sibling( index_.row( ), 1 ), state_ );

  for ( int i = 0; i < model->rowCount( index_ ); i++ )
  {
    auto childIndex = model->index( i, 0, index_ );
    _recursiveDownSelectDeselectItem( childIndex, state_ );
  }
}

void SelectDock::_recursiveUpSelectDeselectItem(
  const QModelIndex& index_,
  const QItemSelectionModel::SelectionFlag state_ )
{
  if ( !index_.isValid( ))
    return;
  auto model = _morphologyTree->model( );
  auto selectionModel = _morphologyTree->selectionModel( );

  bool changeState = true;

  if ( state_ == QItemSelectionModel::Select )
  {
    for ( int i = 0; i < model->rowCount( index_ ); i++ )
    {
      auto childIndex = model->index( i, 0, index_ );
      changeState = changeState && selectionModel->isSelected( childIndex );
      if ( !changeState )
        break;
    }
  }
  if ( changeState )
  {
    selectionModel->select( index_, state_ );
    selectionModel->select( index_.sibling( index_.row( ), 1 ), state_ );
    _recursiveUpSelectDeselectItem( index_.parent( ), state_ );
  }
}

void SelectDock::_recursiveSelectAll( QModelIndex index_,
                                    std::unordered_set< int >& selection_ )
{
  if (!index_.isValid( ))
    return;
  for ( int i = 0; i < _morphologyTree->model( )->rowCount( index_ ); i++ )
  {
    auto childIndex = _morphologyTree->model( )->index( i, 0, index_ );
    _recursiveSelectAll( childIndex, selection_ );
  }
  _morphologyTree->selectionModel( )->select(
    index_, QItemSelectionModel::Select );
  _morphologyTree->selectionModel( )->select(
    index_.sibling( index_.row( ), 1 ), QItemSelectionModel::Select );
  if ( _morphologyTree->model( )->rowCount( index_)  == 0 )
  {
    int nodeId = index_.sibling( index_.row( ), 1 ).data( ).toInt( );
    selection_.insert( nodeId );
  }
}
