/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 * Based on Qt TreeModel example from Qt SDK.
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
 *
 */

#include "TreeModel.h"

#include <QStringList>

TreeModel::TreeModel (  nsol::NeuronMorphologyPtr morphology_,
                        QObject* parent )
  : QAbstractItemModel ( parent )
{
  QList < QVariant > rootData;
  rootData << "Type of element" << "Id."; // << "Description";
  _rootItem = new TreeItem ( rootData );
  _setupModelData( morphology_ );
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

void TreeModel::_setupModelData ( nsol::NeuronMorphologyPtr morphology_ )
{
  if( !morphology_ )
    return;

  QList < TreeItem* > parents;
  QList < int > indentations;
  parents << _rootItem;
  unsigned int neuriteId_ = 0;
  unsigned int sectionId_ = 0;

  QList< QVariant > somaList;
  somaList << "Soma" << 0;
  parents.last( )->appendChild( new TreeItem( somaList, parents.last( )));
  for ( auto node: morphology_->soma( )->nodes( ))
  {
    QList< QVariant > nodeList;
    nodeList << "Node" << node->id( );
    parents << parents.last( )->child( parents.last( )->childCount( ) - 1 );
    parents.last( )->appendChild( new TreeItem( nodeList, parents.last( )));
    parents.pop_back( );
  }
  for ( auto neurite: morphology_->neurites ( ))
  {
    QList < QVariant > neuriteList;
    neuriteList << "Neurite" << neuriteId_;
    ++neuriteId_;

    parents.last ( )->appendChild ( new TreeItem ( neuriteList, parents.last ( )));
    for ( auto section: neurite->sections ( ))
    {
      QList < QVariant > sectionList;
      sectionList << "Section" << sectionId_;
      ++sectionId_;
      parents<< parents.last ( )->child ( parents.last ( )->childCount ( ) - 1 );
      parents.last ( )
             ->appendChild ( new TreeItem ( sectionList, parents.last ( )));
      for ( auto node: section->nodes ( ))
      {
        auto neuroMorphoSection =
          dynamic_cast< nsol::NeuronMorphologySectionPtr >( section );
        if ( node == neuroMorphoSection->firstNode( ) &&
             neuroMorphoSection != neurite->firstSection( ))
          continue;
        QList < QVariant > nodeList;
        nodeList << "Node" << node->id ( );
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
