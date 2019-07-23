/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero  <juanjose.garcia@urjc.es>
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

#ifndef __SELECT_DOCK__
#define __SELECT_DOCK__

#include <QDockWidget>
#include <QObject>
#include <QTreeView>
#include <QGridLayout>
#include <QComboBox>
#include <QToolButton>
#include <QPushButton>
#include <QMessageBox>
#include "viewer.h"

class SelectDock: public QDockWidget
{
  Q_OBJECT

public:

  SelectDock( void );

  ~SelectDock( void ){ };

  void init( Viewer* viewer_ );

public Q_SLOTS:

  void updateMorphology( void );

  void clicked( const QModelIndex& index_ );

  void updateSelection( std::unordered_set< int > selection_ );

  void selectAll( void );

  void clearSelection( void );

  void nodeButtonClicked( void );

  void sectionButtonClicked( void );

  void neuriteButtonClicked( void );

  void inclusiveButtonClicked( void );

  void exclusiveButtonClicked( void );

Q_SIGNALS:

 void updateSelectionSignal( std::unordered_set< int > selection_ );

protected:

  std::unordered_set< int > _fromTreeToSelection( void );

  void _fromSelectionToTree( std::unordered_set< int > selection_ );

  bool _recursiveFromSelectionToTree( QModelIndex index_,
                                      std::unordered_set< int >& selection_ );

  void _recursiveDownSelectDeselectItem(
    const QModelIndex& index_,
    const QItemSelectionModel::SelectionFlag state_ );

  void _recursiveUpSelectDeselectItem(
    const QModelIndex& index_,
    const QItemSelectionModel::SelectionFlag state_ );

  void _recursiveSelectAll( QModelIndex index_,
                            std::unordered_set< int >& selection_ );

  Viewer* _viewer;
  QTreeView* _morphologyTree;

};

#endif
