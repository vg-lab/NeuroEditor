//
// Created by juanpebm on 29/06/18.
//

#include "TreeModel.h"

#include <QStringList>

TreeModel::TreeModel ( const nsol::NeuronMorphologyPtr& morphology_,
                       QObject* parent )
  : QAbstractItemModel ( parent )
{
  QList < QVariant > rootData;
  rootData << "Title" << "Description";
  _rootItem = new TreeItem ( rootData );
  setupModelData ( morphology_, _rootItem );
}

TreeModel::~TreeModel ( )
{
  delete _rootItem;
}

int TreeModel::columnCount ( const QModelIndex& parent ) const
{
  if ( parent.isValid ( ))
    return static_cast<TreeItem*>(parent.internalPointer ( ))->columnCount ( );
  else
    return _rootItem->columnCount ( );
}

QVariant TreeModel::data ( const QModelIndex& index, int role ) const
{
  if ( !index.isValid ( ))
    return QVariant ( );

  if ( role != Qt::DisplayRole )
    return QVariant ( );

  TreeItem* item = static_cast<TreeItem*>(index.internalPointer ( ));

  return item->data ( index.column ( ));
}

Qt::ItemFlags TreeModel::flags ( const QModelIndex& index ) const
{
  if ( !index.isValid ( ))
    return 0;

  return QAbstractItemModel::flags ( index );
}

QVariant TreeModel::headerData ( int section, Qt::Orientation orientation,
                                 int role ) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return _rootItem->data ( section );

  return QVariant ( );
}

QModelIndex TreeModel::index ( int row, int column, const QModelIndex& parent )
const
{
  if ( !hasIndex ( row, column, parent ))
    return QModelIndex ( );

  TreeItem* parentItem;

  if ( !parent.isValid ( ))
    parentItem = _rootItem;
  else
    parentItem = static_cast<TreeItem*>(parent.internalPointer ( ));

  TreeItem* childItem = parentItem->child ( row );
  if ( childItem )
    return createIndex ( row, column, childItem );
  else
    return QModelIndex ( );
}

QModelIndex TreeModel::parent ( const QModelIndex& index ) const
{
  if ( !index.isValid ( ))
    return QModelIndex ( );

  TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer ( ));
  TreeItem* parentItem = childItem->parentItem ( );

  if ( parentItem == _rootItem )
    return QModelIndex ( );

  return createIndex ( parentItem->row ( ), 0, parentItem );
}

int TreeModel::rowCount ( const QModelIndex& parent ) const
{
  TreeItem* parentItem;
  if ( parent.column ( ) > 0 )
    return 0;

  if ( !parent.isValid ( ))
    parentItem = _rootItem;
  else
    parentItem = static_cast<TreeItem*>(parent.internalPointer ( ));

  return parentItem->childCount ( );
}

void TreeModel::setupModelData ( const nsol::NeuronMorphologyPtr& morphology_,
                                 TreeItem* parent )
{
  QList < TreeItem* > parents;
  QList < int > indentations;
  parents << parent;
  unsigned int neuriteId_ = 0;
  unsigned int sectionId_ = 0;

  for ( auto neurite: morphology_->neurites ( ))
  {
    QList < QVariant > neuriteList;
    neuriteList << neuriteId_;
    ++neuriteId_;

    //parents << parents.last()->child(parents.last()->childCount()-1);
    parents.last ( )
           ->appendChild ( new TreeItem ( neuriteList, parents.last ( )));
    for ( auto section: neurite->sections ( ))
    {
      QList < QVariant > sectionList;
      sectionList << sectionId_;
      ++sectionId_;
      parents<< parents.last ( )->child ( parents.last ( )->childCount ( ) - 1 );
      parents.last ( )
             ->appendChild ( new TreeItem ( sectionList, parents.last ( )));
      for ( auto node: section->nodes ( ))
      {
        QList < QVariant > nodeList;
        nodeList << node->id ( );
        parents
          << parents.last ( )->child ( parents.last ( )->childCount ( ) - 1 );
        parents.last ( )
               ->appendChild ( new TreeItem ( nodeList, parents.last ( )));
        parents.pop_back ( );
      }
      parents.pop_back ( );
    }
  }
}
