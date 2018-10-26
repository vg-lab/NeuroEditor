/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 * Based on Qt TreeItem example from Qt SDK.
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
