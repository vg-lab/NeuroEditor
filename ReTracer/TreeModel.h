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

#ifndef RETRACER_TREEMODEL_H
#define RETRACER_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include <nsol/nsol.h>
#include "TreeItem.h"

class TreeModel: public QAbstractItemModel
{
  Q_OBJECT

  public:

    explicit TreeModel ( const nsol::NeuronMorphologyPtr& morphology_,
                         QObject* parent = 0 );

    ~TreeModel ( );

    QVariant data ( const QModelIndex& index, int role ) const override;

    Qt::ItemFlags flags ( const QModelIndex& index ) const override;

    QVariant headerData ( int section, Qt::Orientation orientation,
                          int role = Qt::DisplayRole ) const override;

    QModelIndex index ( int row, int column,
                        const QModelIndex& parent = QModelIndex ( )) const override;

    QModelIndex parent ( const QModelIndex& index ) const override;

    int rowCount ( const QModelIndex& parent = QModelIndex ( )) const override;

    int columnCount ( const QModelIndex& parent = QModelIndex ( )) const override;

  private:
    void setupModelData ( const nsol::NeuronMorphologyPtr& morphology_,
                          TreeItem* parent );

    TreeItem* _rootItem;
};

#endif //RETRACER_TREEMODEL_H
