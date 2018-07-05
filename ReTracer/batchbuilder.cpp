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

#include <QFileDialog>

#include "batchbuilder.h"

BatchBuilder::BatchBuilder ( QWidget* parent )
  : QWidget ( parent )
{
  ui.setupUi ( this );

  QObject::connect ( ui.pushButton_inputDirectory, SIGNAL( clicked ( )),
                     this, SLOT( selectInputDirectory ( )));
  QObject::connect ( ui.pushButton_outputDirectory, SIGNAL( clicked ( )),
                     this, SLOT( selectOutputDirectory ( )));
  QObject::connect ( ui.okButton, SIGNAL( clicked ( )),
                     this, SLOT( generateNeurons ( )));
  QObject::connect ( ui.cancelButton, SIGNAL( clicked ( )),
                     this, SLOT( cancel ( )));
}

BatchBuilder::~BatchBuilder ( )
{

}

void BatchBuilder::selectInputDirectory ( )
{
  _inputDir = QFileDialog::getExistingDirectory ( this, tr ( "Open Directory" ),
                                                  "./",
                                                  QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks );

  ui.lineEdit_inputDirectory->setText ( _inputDir );
  ui.lineEdit_inputDirectory->setReadOnly ( true );
}

void BatchBuilder::selectOutputDirectory ( )
{
  _outputDir = QFileDialog::getExistingDirectory ( this, tr ( "Open Directory" ),
                                                  "./",
                                                  QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks );

  ui.lineEdit_outputDirectory->setText ( _outputDir );
  ui.lineEdit_outputDirectory->setReadOnly ( true );
}

void BatchBuilder::generateNeurons ( )
{
  if (( !_inputDir.isNull ( )) && ( !_outputDir.isNull ( )))
    emit directoriesReadies ( );
}

void BatchBuilder::cancel ( )
{
  this->hide ( );
}
