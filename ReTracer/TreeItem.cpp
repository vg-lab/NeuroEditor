//
// Created by juanpebm on 29/06/18.
//

#include "TreeItem.h"

TreeItem::TreeItem ( const QList < QVariant >& data, TreeItem* parent )
{
  _parentItem = parent;
  _itemData = data;
}

TreeItem::~TreeItem ( )
{
  qDeleteAll ( _childItems );
}

void TreeItem::appendChild ( TreeItem* item )
{
  _childItems.append ( item );
}

TreeItem* TreeItem::child ( int row )
{
  return _childItems.value ( row );
}

int TreeItem::childCount ( ) const
{
  return _childItems.count ( );
}

int TreeItem::columnCount ( ) const
{
  return _itemData.count ( );
}

QVariant TreeItem::data ( int column ) const
{
  return _itemData.value ( column );
}

TreeItem* TreeItem::parentItem ( )
{
  return _parentItem;
}

int TreeItem::row ( ) const
{
  if ( _parentItem )
    return _parentItem->_childItems.indexOf ( const_cast<TreeItem*>(this));

  return 0;
}