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
#ifndef RETRACER_TREEITEM_H
#define RETRACER_TREEITEM_H

#include <QList>
#include <QVariant>

class TreeItem
{
  public:
    explicit TreeItem ( const QList < QVariant >& data,
                        TreeItem* parentItem = 0 );
    ~TreeItem ( );

    void appendChild ( TreeItem* child );

    TreeItem* child ( int row );
    int childCount ( ) const;
    int columnCount ( ) const;
    QVariant data ( int column ) const;
    int row ( ) const;
    TreeItem* parentItem ( );

  private:
    QList < TreeItem* > _childItems;
    QList < QVariant > _itemData;
    TreeItem* _parentItem;
};

#endif //RETRACER_TREEITEM_H
