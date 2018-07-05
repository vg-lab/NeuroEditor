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

#include "ReTracer.h"

#include <QColorDialog>
#include <QDirIterator>
#include <QMessageBox>

ReTracer::ReTracer ( QWidget* parent )
  : QMainWindow ( parent )
{
  ui.setupUi ( this );

  viewer = new Viewer ( this );

  //Add the viewer component to the interface
  ui.verticalLayout_Viewer->addWidget ( viewer );

  _simplificationMode = SIMP_METHOD::RADIAL;
  _enhanceMode = ENHANCE_METHOD::LINEAR;

  //Configure the compnent
  viewer->setupViewer ( );

  _batchBuilder = nullptr;

  //Actions
  QObject::connect(ui.actionOpen_morphological_tracing	,SIGNAL(triggered())	,this,	SLOT(importMorphology()));
  QObject::connect(ui.action_splitScreen	,SIGNAL(triggered())	,this,	SLOT(changeToDualView()));
  QObject::connect(ui.action_takeSnapshot	,SIGNAL(triggered())	,this,	SLOT( takeSnapshot()));
  QObject::connect(ui.action_help	,SIGNAL(triggered())	,this,	SLOT( help()));
  QObject::connect(ui.action_exit	,SIGNAL(triggered())	,this,	SLOT( quit()));

  //Main
  QObject::connect ( ui.pushButton_ImportMorphology, SIGNAL( clicked ( )),
                     this, SLOT( importMorphology ( )));
  QObject::connect ( ui.pushButton_ExportMorphology, SIGNAL( clicked ( )),
                     this, SLOT( exportMorphology ( )));
  QObject::connect ( ui.pushButton_save, SIGNAL( clicked ( )),
                     this, SLOT( saveState ( )));
  QObject::connect ( ui.checkBox_autoSave, SIGNAL( clicked ( )),
                     this, SLOT( setAutoSaveState ( )));
  QObject::connect ( ui.pushButton_undo, SIGNAL( clicked ( )),
                     this, SLOT( undoState ( )));

  //Simplify
  QObject::connect ( ui.radioButton_NthPoint, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_RadialDistance, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_PependicularDistance, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_ReumannWitkam, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_Opheim, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_Lang, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_DouglasPeucker, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.radioButton_DouglasPueckerModif, SIGNAL( clicked ( )),
                     this, SLOT( setSimplificationMethod ( )));
  QObject::connect ( ui.pushButton_ApplySimplification, SIGNAL( clicked ( )),
                     this, SLOT( simplify ( )));

  //Render
  QObject::connect ( ui.checkBox_ShowOriginalMorphologyText, SIGNAL( clicked ( )),
                     this, SLOT( setMorphologyToShowInfo ( )));
  QObject::connect ( ui.checkBox_ShowModifiedMorphologyText, SIGNAL( clicked ( )),
                     this, SLOT( setMorphologyToShowInfo ( )));
  QObject::connect ( ui.checkBox_renderOriginalMorphology, SIGNAL( clicked ( )),
                     this, SLOT( setRenderOriginalMorphology ( )));
  QObject::connect ( ui.checkBox_renderModifiedMorphology, SIGNAL( clicked ( )),
                     this, SLOT( setRenderModifiedMorphology ( )));
  QObject::connect ( ui.spinBox_resolution, SIGNAL( editingFinished ( )),
                     this, SLOT( setResolutionToNodes ( )));
  QObject::connect ( ui.doubleSpinBox_desp, SIGNAL( editingFinished ( )),
                     this, SLOT( setNodeIncToMorphology ( )));
  QObject::connect ( ui.pushButton_OriginalNodeColor, SIGNAL( clicked ( )),
                     this, SLOT( setNodeColorToOriginalMorphology ( )));
  QObject::connect ( ui.pushButton_OriginalLinkColor, SIGNAL( clicked ( )),
                     this, SLOT( setNodeColorToModifiedMorphology ( )));
  QObject::connect ( ui.pushButton_ModifiedNodeColor, SIGNAL( clicked ( )),
                     this, SLOT( setLinkColorToOriginalMorphology ( )));
  QObject::connect ( ui.pushButton_ModifiedLinkColor, SIGNAL( clicked ( )),
                     this, SLOT( setLinkColorToModifiedMorphology ( )));

  //Enhancement
  QObject::connect ( ui.radioButton_enhanceLinear, SIGNAL( clicked ( )),
                     this, SLOT( setEnhanceMethod ( )));
  QObject::connect ( ui.radioButton_enhanceSpline, SIGNAL( clicked ( )),
                     this, SLOT( setEnhanceMethod ( )));
  QObject::connect ( ui.radioButton_enhanceCustom, SIGNAL( clicked ( )),
                     this, SLOT( setEnhanceMethod ( )));
  QObject::connect ( ui.pushButton_applyEnhancement, SIGNAL( clicked ( )),
                     this, SLOT( enhance ( )));

  //Repair
  QObject::connect ( ui.pushButton_SetRadiusToSelectedNode,
                     SIGNAL( clicked ( )),
                     this,
                     SLOT( setRadiusToSelectedNode ( )));
  QObject::connect ( ui.pushButton_ApplyInterpolation, SIGNAL( clicked ( )),
                     this, SLOT( interpolateRadius ( )));
  QObject::connect ( ui.pushButton_CalcMidlePosition, SIGNAL( clicked ( )),
                     this, SLOT( middlePosition ( )));

  //Melt
  QObject::connect ( ui.pushButton_selectObject, SIGNAL( clicked ( )),
                     this, SLOT( selectNeuronObject ( )));
  QObject::connect ( ui.pushButton_simplifySelectedObject, SIGNAL( clicked ( )),
                     this, SLOT( simplifyNeuronObject ( )));
  QObject::connect ( ui.pushButton_enhanceSelectedObject, SIGNAL( clicked ( )),
                     this, SLOT( enhanceNeuronObject ( )));

  QObject::connect ( ui.pushButton_extractNeurite, SIGNAL( clicked ( )),
                     this, SLOT( extractNeurite ( )));
  QObject::connect ( ui.pushButton_removeNeurite, SIGNAL( clicked ( )),
                     this, SLOT( removeNeurite ( )));
  QObject::connect ( ui.pushButton_attachMorphology, SIGNAL( clicked ( )),
                     this, SLOT( attachMorphology ( )));

  QObject::connect ( ui.pushButton_brokeLink, SIGNAL( clicked ( )),
                     this, SLOT( brokeLink ( )));
  QObject::connect ( ui.pushButton_SetLink, SIGNAL( clicked ( )),
                     this, SLOT( setLink ( )));

  //Utils
  QObject::connect ( ui.pushButton_batchProcessing, SIGNAL( clicked ( )),
                     this, SLOT( showBatchBuilder ( )));

  //Others
  QObject::connect ( ui.pushButton_SetModifiedAsOriginal, SIGNAL( clicked ( )),
                     this, SLOT( setModifiedAsOriginal ( )));
  QObject::connect ( ui.PushButton_testPythonIntegration, SIGNAL( clicked ( )),
                     this, SLOT( testPythonIntegration ( )));

  //Default parameters
  setSimplificationMethod ( );
  setEnhanceMethod ( );
  colorToButtons ( );
}

ReTracer::~ReTracer ( )
{
  delete viewer;
  if ( _batchBuilder != nullptr )
    delete _batchBuilder;
}

void ReTracer::quit()
{
  QString text = QObject::tr("Are you sure you want to quit?");
  QMessageBox msgBox(QObject::tr("Quit"), text, QMessageBox::Warning, QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape, QMessageBox::NoButton);
  if (msgBox.exec() == QMessageBox::Yes)  close();
}

void ReTracer::takeSnapshot()
{
  QString fileName = QFileDialog::getSaveFileName ( this, tr ( "Save snapshot" ), "./" );
  viewer->saveSnapshot ( fileName + ".png" );
}

void ReTracer::help()
{
  viewer->help();
}

//Main
void ReTracer::importMorphology ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this,
                                                    tr ( "Open File" ),
                                                    "./",
                                                    tr ( "NeuroMorpho(*.swc)" ));

  if ( !fileName.isNull ( ))
  {
    viewer->loadMorphology ( fileName );
    setTreeModel ( );
  }
}

void ReTracer::exportMorphology ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( fileName.isEmpty ( ))
    return;
  else
    viewer->exportMorphology ( fileName );
}

void ReTracer::setModificationInterval ( )
{
//	viewer->setSimplificationInterval(ui.spinBoxManualIniParam->value()
//                                    ,ui.spinBoxManualFinParam->value());
}

void ReTracer::setModifiedAsOriginal ( )
{
  viewer->setModifiedAsOriginal ( );
}

void ReTracer::saveState ( )
{
  viewer->saveState ( );
}

void ReTracer::setAutoSaveState ( )
{
  viewer->setAutoSaveState ( ui.checkBox_autoSave->isChecked ( ));
  ui.pushButton_save->setEnabled ( !ui.checkBox_autoSave->isChecked ( ));
}

void ReTracer::undoState ( )
{
  viewer->undoState ( );
}

//Simplify
void ReTracer::setSimplificationMethod ( )
{
  if ( ui.radioButton_NthPoint->isChecked ( ))
    _simplificationMode = SIMP_METHOD::NTHPOINT;
  else if ( ui.radioButton_RadialDistance->isChecked ( ))
    _simplificationMode = SIMP_METHOD::RADIAL;
  else if ( ui.radioButton_PependicularDistance->isChecked ( ))
    _simplificationMode = SIMP_METHOD::PERPDIST;
  else if ( ui.radioButton_ReumannWitkam->isChecked ( ))
    _simplificationMode = SIMP_METHOD::REUMANNWITKAM;
  else if ( ui.radioButton_Opheim->isChecked ( ))
    _simplificationMode = SIMP_METHOD::OPHEIN;
  else if ( ui.radioButton_Lang->isChecked ( ))
    _simplificationMode = SIMP_METHOD::LANG;
  else if ( ui.radioButton_DouglasPeucker->isChecked ( ))
    _simplificationMode = SIMP_METHOD::DOUGLASPEUCKER;
  else if ( ui.radioButton_DouglasPueckerModif->isChecked ( ))
    _simplificationMode = SIMP_METHOD::DOUGLASPEUCKERMOD;
  else if ( ui.radioButton_pythonSimplification->isChecked ( ))
    _simplificationMode = SIMP_METHOD::CUSTOM_SIMP;
}

void ReTracer::simplify ( int objectId_, OBJECT_TYPE objectType)
{
  _params.clear ( );
  switch (( int ) _simplificationMode )
  {
    case SIMP_METHOD::NTHPOINT:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     SIMP_METHOD::NTHPOINT ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.spinBox_NthPoints
                                                            ->value ( )));
      break;

    case SIMP_METHOD::RADIAL:
      _params.insert ( std::pair < std::string, float > (
        "Method",
        SIMP_METHOD::RADIAL ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.doubleSpinBox_RadialDistance
                                                            ->value ( )));
      break;

    case SIMP_METHOD::PERPDIST:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     SIMP_METHOD::PERPDIST ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.doubleSpinBox_PerpendicularDistance
                                                            ->value ( )));
      break;

    case SIMP_METHOD::REUMANNWITKAM:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     SIMP_METHOD::REUMANNWITKAM ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.doubleSpinBox_ReumannWitkam
                                                            ->value ( )));
      break;

    case SIMP_METHOD::OPHEIN:
      _params.insert ( std::pair < std::string, float > (
        "Method",
        SIMP_METHOD::OPHEIN ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.doubleSpinBox_OpheinMinThreshold
                                                            ->value ( )));
      _params.insert ( std::pair < std::string, float > ( "Value2",
                                                          ui.doubleSpinBox__OpheinMaxThreshold
                                                            ->value ( )));
      break;

    case SIMP_METHOD::LANG:
      _params.insert ( std::pair < std::string, float > (
        "Method",
        SIMP_METHOD::LANG ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.doubleSpinBox_Lang_Threshold
                                                            ->value ( )));
      _params.insert ( std::pair < std::string, float > ( "Value2",
                                                          ui.doubleSpinBox_Lang_Size
                                                            ->value ( )));
      break;

    case SIMP_METHOD::DOUGLASPEUCKER:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     SIMP_METHOD::DOUGLASPEUCKER ));
      _params
        .insert ( std::pair < std::string, float > ( "Value",
                                                     ui.doubleSpinBox_DouglasPeuckerThreshold
                                                       ->value ( )));
      break;

    case SIMP_METHOD::DOUGLASPEUCKERMOD:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     SIMP_METHOD::DOUGLASPEUCKERMOD ));
      _params.insert ( std::pair < std::string, float > ( "Value",
                                                          ui.spinBox_DouglasPeucker_NumPoints
                                                            ->value ( )));
      break;

    case SIMP_METHOD::CUSTOM_SIMP:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     SIMP_METHOD::CUSTOM_SIMP ));
      break;
    default:std::cout << "Unhandeled simplification method!!!" << std::endl;
  }
  viewer->simplify ( _params, objectId_ , objectType );
}

//Enhance
void ReTracer::setEnhanceMethod ( )
{
  if ( ui.radioButton_enhanceLinear->isChecked ( ))
    _enhanceMode = ENHANCE_METHOD::LINEAR;
  else if ( ui.radioButton_enhanceSpline->isChecked ( ))
    _enhanceMode = ENHANCE_METHOD::SPLINE;
  else if ( ui.radioButton_enhanceCustom->isChecked ( ))
    _enhanceMode = ENHANCE_METHOD::CUSTOM_ENHANCE;
}

void ReTracer::enhance ( int objectId_, OBJECT_TYPE objectType )
{
  _params.clear ( );
  switch (( int ) _enhanceMode )
  {
    case ENHANCE_METHOD::LINEAR:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     ENHANCE_METHOD::LINEAR ));
      break;

    case ENHANCE_METHOD::SPLINE:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     ENHANCE_METHOD::SPLINE ));
      break;

    case ENHANCE_METHOD::CUSTOM_ENHANCE:
      _params
        .insert ( std::pair < std::string, float > ( "Method",
                                                     ENHANCE_METHOD::CUSTOM_ENHANCE ));
      break;
    default:std::cout << "Unhandeled enhanced method!!!" << std::endl;
  }
  _params.insert ( std::pair < std::string, float > ( "Value",
                                                      ui.doubleSpinBox_separationBetwenNodes
                                                        ->value ( )));
  viewer->enhance ( _params, objectId_ , objectType );
}

//Repair
void ReTracer::insertNode ( )
{

}

void ReTracer::deleteNode ( )
{
  //###viewer->getNeuronGraphOptimizer()->directDeleteVertexs( ui.spinBox_ManualStep->value() );
}

void ReTracer::insertEdge ( )
{
  viewer->setLink ( );
}

void ReTracer::deleteEdge ( )
{
  viewer->brokeLink ( );
}

void ReTracer::interpolateRadius ( )
{
  viewer
    ->interpolateRadius ( ui.doubleSpinBox_InitialDendirteThickness->value ( ),
                          ui.doubleSpinBox_FinalDendriteThickness->value ( ),
                          ui.doubleSpinBox_InitialApicalThickness->value ( ),
                          ui.doubleSpinBox_FinalApicalThickness->value ( )
    );
}

void ReTracer::middlePosition ( )
{
  viewer->middlePosition ( );
}

void ReTracer::setRadiusToSelectedNode ( )
{
  viewer->setRadiusToSelectedNode ( ui.doubleSpinBox_SetNodeRadius->value ( ));
}

void ReTracer::brokeLink ( )
{
  viewer->brokeLink ( );
}

void ReTracer::setLink ( )
{
  viewer->setLink ( );
}

//Render
void ReTracer::setMorphologyToShowInfo ( )
{
  //That controls must be checkboxes!!!
  if ( ui.checkBox_ShowOriginalMorphologyText->isChecked ( ))
    viewer->setMorphologyInfoToShow ( 1 );
  else if ( ui.checkBox_ShowModifiedMorphologyText->isChecked ( ))
    viewer->setMorphologyInfoToShow ( 2 );
  else if (( ui.checkBox_ShowOriginalMorphologyText->isChecked ( ))
    && ( ui.checkBox_ShowModifiedMorphologyText->isChecked ( )))
    viewer->setMorphologyInfoToShow ( 3 );
  else
    viewer->setMorphologyInfoToShow ( 0 );
}

void ReTracer::showTextValues ( )
{
  viewer->setShowTextValues ( ui.spinBox_TextIniValue->value ( ),
                              ui.spinBox_TextFinValue->value ( ));
}

void ReTracer::setRenderOriginalMorphology ( )
{
  std::cout << "Modified morphology render:"
            << ui.checkBox_renderOriginalMorphology->isChecked ( ) << std::endl;
  viewer->setRenderModifiedMorphology ( ui.checkBox_renderOriginalMorphology
                                          ->isChecked ( ));
}

void ReTracer::setRenderModifiedMorphology ( )
{
  std::cout << "Original morphology render:"
            << ui.checkBox_renderModifiedMorphology->isChecked ( ) << std::endl;
  viewer->setRenderOriginalMorphology ( ui.checkBox_renderModifiedMorphology
                                          ->isChecked ( ));
}

void ReTracer::changeToDualView ( )
{
  viewer->changeToDualView ( );
}

void ReTracer::setResolutionToNodes ( )
{
  viewer->getOriginalMorphologyRenderer ( )
        ->setResolution ( ui.spinBox_resolution->value ( ));
  viewer->getModifiedMorphologyRenderer ( )
        ->setResolution ( ui.spinBox_resolution->value ( ));
}

void ReTracer::setNodeIncToMorphology ( )
{
  viewer->getModifiedMorphologyRenderer ( )
        ->setIncrement ( ui.doubleSpinBox_desp->value ( ));
}

void ReTracer::setNodeColorToOriginalMorphology ( )
{
  QColor color = QColorDialog::getColor ( Qt::black, this );
  if ( color.isValid ( ))
  {
    viewer->getOriginalMorphologyRenderer ( )
          ->setNodeColor ( nsol::Vec3f ( color.redF ( ),
                                         color.greenF ( ),
                                         color.blueF ( )));

    QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
    ui.pushButton_OriginalNodeColor->setStyleSheet ( qss );
  }
}

void ReTracer::setNodeColorToModifiedMorphology ( )
{
  QColor color = QColorDialog::getColor ( Qt::black, this );
  if ( color.isValid ( ))
  {
    viewer->getOriginalMorphologyRenderer ( )
          ->setLinkColor ( nsol::Vec3f ( color.redF ( ),
                                         color.greenF ( ),
                                         color.blueF ( )));
    QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
    ui.pushButton_OriginalLinkColor->setStyleSheet ( qss );
  }

}

void ReTracer::setLinkColorToOriginalMorphology ( )
{
  QColor color = QColorDialog::getColor ( Qt::black, this );
  if ( color.isValid ( ))
  {
    viewer->getModifiedMorphologyRenderer ( )
          ->setNodeColor ( nsol::Vec3f ( color.redF ( ),
                                         color.greenF ( ),
                                         color.blueF ( )));
    QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
    ui.pushButton_ModifiedNodeColor->setStyleSheet ( qss );
  }
}

void ReTracer::setLinkColorToModifiedMorphology ( )
{
  QColor color = QColorDialog::getColor ( Qt::black, this );
  if ( color.isValid ( ))
  {
    viewer->getModifiedMorphologyRenderer ( )
          ->setLinkColor ( nsol::Vec3f ( color.redF ( ),
                                         color.greenF ( ),
                                         color.blueF ( )));
    QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
    ui.pushButton_ModifiedLinkColor->setStyleSheet ( qss );
  }
}

void ReTracer::colorToButtons ( )
{
  QColor color = QColor ( Qt::red );
  QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
  ui.pushButton_OriginalNodeColor->setStyleSheet ( qss );

  color = QColor ( 192, 192, 192 );
  qss = QString ( "background-color: %1" ).arg ( color.name ( ));
  ui.pushButton_OriginalLinkColor->setStyleSheet ( qss );

  color = QColor ( Qt::blue );
  qss = QString ( "background-color: %1" ).arg ( color.name ( ));
  ui.pushButton_ModifiedNodeColor->setStyleSheet ( qss );

  color = QColor ( Qt::green );
  qss = QString ( "background-color: %1" ).arg ( color.name ( ));
  ui.pushButton_ModifiedLinkColor->setStyleSheet ( qss );

}

//Utils
void ReTracer::showBatchBuilder ( )
{
  if ( _batchBuilder == nullptr )
  {
    _batchBuilder = new BatchBuilder ( );
    QObject::connect ( _batchBuilder,
                       SIGNAL( directoriesReadies ( )),
                       this,
                       SLOT( genetareNeuronsInBatch ( )));
  }

  _batchBuilder->show ( );
  //QObject::connect(mBatchBuilder, SIGNAL(fileReady(QString pFile)), this, SLOT(mSomaCreatorWidget->loadSWCFile(pFile)));
}

void ReTracer::genetareNeuronsInBatch ( )
{
  _inputFilePath = _batchBuilder->getInputDir ( );
  _ouputFilePath = _batchBuilder->getOutputDir ( );

  /*
  setOptimizeMethod();
  viewer->setOptimizeParams(
                ui.spinBox_NthPoints->value()
                ,ui.doubleSpinBox_RadialDistance->value()
                ,ui.doubleSpinBox_PerpendicularDistance->value()
                ,ui.doubleSpinBox_ReumannWitkam->value()
                ,ui.doubleSpinBox_OpheinMinThreshold->value()
                ,ui.doubleSpinBox__OpheinMaxThreshold->value()
                ,ui.doubleSpinBox_Lang_Threshold->value()
                ,ui.doubleSpinBox_Lang_Size->value()
                ,ui.doubleSpinBox_DouglasPeuckerThreshold->value()
                ,ui.spinBox_DouglasPeucker_NumPoints->value()
              );
   */

  this->copyRecursively ( _inputFilePath,
                          _ouputFilePath );
}

bool ReTracer::copyRecursively ( const QString& sourceFolder,
                                 const QString& destFolder )
{
  bool success = false;
  QDir sourceDir ( sourceFolder );

  if ( !sourceDir.exists ( ))
    return false;

  QDir destDir ( destFolder );
  if ( !destDir.exists ( ))
    destDir.mkdir ( destFolder );

  QStringList filters;
  filters << "*.swc" << "*.SWC";

  QStringList files = sourceDir.entryList ( filters, QDir::Files );
  for ( int i = 0; i < files.count ( ); i++ )
  {
    QString path = sourceFolder + QDir::separator ( ) + files[i];
    viewer->loadMorphology ( path );

    //NO funciona si se saca fuera del bucle ...
    setSimplificationMethod ( );
//      viewer->setOptimizeParams(
//                    ui.spinBox_NthPoints->value()
//                    ,ui.doubleSpinBox_RadialDistance->value()
//                    ,ui.doubleSpinBox_PerpendicularDistance->value()
//                    ,ui.doubleSpinBox_ReumannWitkam->value()
//                    ,ui.doubleSpinBox_OpheinMinThreshold->value()
//                    ,ui.doubleSpinBox__OpheinMaxThreshold->value()
//                    ,ui.doubleSpinBox_Lang_Threshold->value()
//                    ,ui.doubleSpinBox_Lang_Size->value()
//                    ,ui.doubleSpinBox_DouglasPeuckerThreshold->value()
//                    ,ui.spinBox_DouglasPeucker_NumPoints->value()
//
//                  );
    simplify ( );
    path = destFolder + QDir::separator ( ) + _batchBuilder->getBaseName ( )
      + files[i];
    viewer->exportMorphology ( path );

    //QString srcName = sourceFolder + QDir::separator() + files[i];
    //QString destName = destFolder + QDir::separator() + files[i];
    //success = QFile::copy(srcName, destName);
    //if(!success)
    //    return false;
  }

  files.clear ( );
  files = sourceDir.entryList ( QDir::AllDirs | QDir::NoDotAndDotDot );
  for ( int i = 0; i < files.count ( ); i++ )
  {
    QString srcName = sourceFolder + QDir::separator ( ) + files[i];
    QString destName = destFolder + QDir::separator ( ) + files[i];
    success = copyRecursively ( srcName, destName );
    if ( !success )
      return false;
  }

  return true;
}


//Melt

void ReTracer::setTreeModel ( )
{
  ui.treeView_morphologyThree->setModel ( viewer->getTreeModel ( ));
}

void ReTracer::attachMorphology ( )
{
  QString fileName = QFileDialog::getOpenFileName ( this,
                                                    tr ( "Open File" ),
                                                    "./",
                                                    tr ( "NeuroMorpho(*.swc)" ));

  if ( !fileName.isNull ( ))
  {
    //###NSSWCImporter::SWCImporter * lSWCImporter = new NSSWCImporter::SWCImporter(fileName.toStdString());
    //###viewer->attachMorphology(*lSWCImporter);
    //###delete lSWCImporter;
  }

}

void ReTracer::extractNeurite ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    viewer->ExtractNeurite ( fileName );
  }
}

void ReTracer::removeNeurite ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( fileName.isEmpty ( ))
  {
    return;
  }
  else
  {
    viewer->ExtractNeurite ( fileName );
  }
}

void ReTracer::selectNeuronObject ( )
{
  QModelIndex index = ui.treeView_morphologyThree->currentIndex ( );
  QVariant
    extractedData = ui.treeView_morphologyThree->model ( )->data ( index );
  unsigned int deph = 0;

  while (index.isValid ())
  {
    ++deph;
    index = index.parent();
  }

  unsigned int value = extractedData.toInt ( );
  viewer->selectDendrite ( value );
}

void ReTracer::simplifyNeuronObject ( )
{
  QModelIndex index = ui.treeView_morphologyThree->currentIndex ( );
  QVariant
    extractedData = ui.treeView_morphologyThree->model ( )->data ( index );
  unsigned int value = extractedData.toInt ( );

  unsigned int deph = 0;

  while (index.isValid ())
  {
    ++deph;
    index = index.parent();
  }
  --deph;

  simplify ( value, ( OBJECT_TYPE )deph );
}

void ReTracer::enhanceNeuronObject ( )
{
  QModelIndex index = ui.treeView_morphologyThree->currentIndex ( );
  QVariant
    extractedData = ui.treeView_morphologyThree->model ( )->data ( index );
  unsigned int value = extractedData.toInt ( );
  unsigned int deph = 0;
  while (index.isValid ())
  {
    ++deph;
    index = index.parent();
  }
  --deph;

  enhance ( value, ( OBJECT_TYPE )deph );
}

//Other
void ReTracer::testPythonIntegration ( )
{
//  using namespace NSPyGEmS;
//  const unsigned int vecSize            = 10;
//  unsigned int numResizedNodesInPython  = 15;
//  
//  try
//  {
//	  //BPCode
//    PyGEmSManager myPyGEmSManager("StrFramework", &initStrFramework, "Strategies", "Strategies.py");    
//    bp::object Strategy   = myPyGEmSManager.getModuleAttrib( "Strategy" );    
//
//    Container       _Container;    
//    StrategyParams  lStrategyParams;    
//    for (unsigned int i=0;i<vecSize;++i) 
//    {
//      lStrategyParams.stringParam = "MyTest_" + std::to_string(lStrategyParams.intParam) + "_";      
//      lStrategyParams.intParam++;
//      _Container.addElement ( lStrategyParams );      
//    }
//    bp::object _strategy  = Strategy( _Container );    
//    
//    std::cout<<"Initial vector (Showing only string (from base class) parameter)."<<std::endl;
//    for (unsigned int i=0;i<vecSize;++i) std::cout<<_Container.getContainer()[i].stringParam <<std::endl;
//    
//    std::string injectedVarName   = "vecIn";
//    std::string injectedVarName3  = "listOut";
//    
//    //BPCode
//    myPyGEmSManager.getModule().attr(injectedVarName.c_str())   = _Container.getContainer();
//    myPyGEmSManager.getModule().attr(injectedVarName3.c_str())  = 0;
//
//    std::cout <<"Initial vector dimensions. Value:" <<_Container.getContainer().size() <<std::endl;
//    
//    std::cout <<"Set new dimensions of the vector to python. Value:" <<numResizedNodesInPython <<std::endl;    
//    _strategy.attr("setContainerResizeDimension")(numResizedNodesInPython);
//
//    std::cout <<"Calling python methods from CPP."<<std::endl;
//    
//    _strategy.attr("simplify")();
//    _strategy.attr("fix")();
//    _strategy.attr("enhance")();
//
//    std::cout <<"Recovering new container from Python."<<std::endl;            
//    bp::list result = myPyGEmSManager.extractListFromModule(injectedVarName3.c_str());    
//
//    //int n = bp::extract<int>(result.attr("__len__")());    
//    int n = bp::len(result);
//    std::cout <<"Container recovered dimensions Value:" <<n <<std::endl;
//
//    std::cout <<"Showing received string value (modified on python):"<<std::endl;    
//    for (int  i=0;i<n;++i) 
//    {
//        StrategyParams val = bp::extract<StrategyParams>(result[i]);
//        //StrategyParams val = myPyGEmSManager.extractObjectFromList(mStrategyParams,result,i);
//        std::cout <<val.stringParam <<std::endl;;
//    }
//
//    //Ping pong test
//    std::cout <<"Starting ping-pong test:"<<std::endl;
//    _strategy.attr("eval")();
//
//    std::cout <<std::endl;
//  }
//  catch ( const bp::error_already_set & )
//  {
//    std::cerr << ">>> Error! Uncaught exception:\n";
//    PyErr_Print();
//  }
}

