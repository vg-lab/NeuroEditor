//
// Created by juanpebm on 29/06/18.
//

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
