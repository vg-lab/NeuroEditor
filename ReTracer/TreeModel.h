//
// Created by juanpebm on 29/06/18.
//

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
