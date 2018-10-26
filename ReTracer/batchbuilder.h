/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
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

#ifndef BATCHBUILDER_H
#define BATCHBUILDER_H

#include <QWidget>
#include "ui_batchbuilder.h"

class BatchBuilder: public QWidget
{
  Q_OBJECT

    QString _inputDir;
    QString _outputDir;

  public:
    BatchBuilder ( QWidget* parent = 0 );
    ~BatchBuilder ( );

    QString getInputDir ( ) { return _inputDir; }
    QString getOutputDir ( ) { return _outputDir; }

    QString getBaseName ( ) { return ui.lineEdit_baseName->text ( ); }

    bool applySimplify ( ) { return ui.radioButton_applySimplification->isChecked(); }
    bool applyEnhance ( ) { return ui.radioButton_applyEnhance->isChecked(); }
    bool applyFix ( ) { return ui.radioButton_applyFix->isChecked(); }

  Q_SIGNALS:

    void directoriesReadies ( );

  private:

    Ui::BatchBuilder ui;

  public Q_SLOTS://slots:

    void selectInputDirectory ( );
    void selectOutputDirectory ( );

    void generateNeurons ( );
    void cancel ( );
};

#endif // BATCHBUILDER_H
