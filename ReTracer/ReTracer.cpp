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

#include <QColorDialog>
#include <QDirIterator>
#include <QMessageBox>

#include "ReTracer.h"

ReTracer::ReTracer ( QWidget* parent )
  : QMainWindow ( parent )
  , _ui( new Ui::ReTracer )
{

  _ui->setupUi( this );
  _viewer = new Viewer( );
  this->setCentralWidget( _viewer );
  _viewer->setupViewer ( );
  _viewer->show( );




  // _simplificationMode = SIMP_METHOD::RADIAL;
  // _enhanceMode = ENHANCE_METHOD::LINEAR;

  // _batchBuilder = nullptr;

  QObject::connect( _ui->actionQuit, SIGNAL( triggered( )),
                    QApplication::instance( ), SLOT( quit( )));
  QObject::connect( _ui->actionAbout, SIGNAL( triggered( )),
                    _viewer, SLOT( help( )));
  QObject::connect( _ui->actionOpenSWCFile, SIGNAL( triggered( )),
                    this, SLOT( importMorphology( )));
  QObject::connect( _ui->actionSaveSWCFile, SIGNAL( triggered( )),
                    this, SLOT( exportMorphology( )));
  // QObject::connect( _ui->exportMesh, SIGNAL( triggered( )),
  //                   this, SLOT( ));
  // QObject::connect( _ui->exportMorphometricMeasures, SIGNAL( triggered( )),
  //                   this, SLOT( ));
  QObject::connect( _ui->actionUndo, SIGNAL( triggered( )),
                    _viewer, SLOT( undoState( )));

  _viewDock = new ViewDock(  );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::LeftDockWidgetArea,
                       _viewDock, Qt::Vertical );
  _viewDock->close( );
  connect( _viewDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionView, SLOT( setChecked( bool )));
  connect( _ui->actionView, SIGNAL( triggered( )),
           this, SLOT( updateViewDock( )));

  _selectDock = new SelectDock(  );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::LeftDockWidgetArea,
                       _selectDock, Qt::Vertical );
  _selectDock->close( );
  connect( _selectDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionSelect, SLOT( setChecked( bool )));
  connect( _ui->actionSelect, SIGNAL( triggered( )),
           this, SLOT( updateSelectDock( )));

  _editDock = new EditDock(  );
  this->addDockWidget( Qt::DockWidgetAreas::enum_type::RightDockWidgetArea,
                       _editDock, Qt::Vertical );
  _editDock->close( );
  connect( _editDock->toggleViewAction( ), SIGNAL( toggled( bool )),
           _ui->actionEditAndCorrect, SLOT( setChecked( bool )));
  connect( _ui->actionEditAndCorrect, SIGNAL( triggered( )),
           this, SLOT( updateEditDock( )));

// QObject::connect(ui.actionImport_Morphology	,SIGNAL(triggered())	,this,	SLOT(importMorphology()));
  // QObject::connect(ui.actionExport_Morphology	,SIGNAL(triggered())	,this,	SLOT(exportMorphology()));
  // QObject::connect(ui.actionDual_view	,SIGNAL(triggered())	,this,	SLOT(changeToDualView()));
  // QObject::connect(ui.actionTake_snapshot	,SIGNAL(triggered())	,this,	SLOT( takeSnapshot()));
  // QObject::connect(ui.actionHelp	,SIGNAL(triggered())	,this,	SLOT( help()));
  // QObject::connect(ui.actionExit	,SIGNAL(triggered())	,this,	SLOT( quit()));

  // QObject::connect(ui.actionOpen_morphological_tracing	,SIGNAL(triggered())	,this,	SLOT(importMorphology()));
  // QObject::connect(ui.action_splitScreen	,SIGNAL(triggered())	,this,	SLOT(changeToDualView()));
  // QObject::connect(ui.action_takeSnapshot	,SIGNAL(triggered())	,this,	SLOT( takeSnapshot()));
  // QObject::connect(ui.action_help	,SIGNAL(triggered())	,this,	SLOT( help()));
  // QObject::connect(ui.action_exit	,SIGNAL(triggered())	,this,	SLOT( quit()));

  // //Main
  // QObject::connect ( ui.pushButton_ImportMorphology, SIGNAL( clicked ( )),
  //                    this, SLOT( importMorphology ( )));
  // QObject::connect ( ui.pushButton_ExportMorphology, SIGNAL( clicked ( )),
  //                    this, SLOT( exportMorphology ( )));
  // QObject::connect ( ui.pushButton_save, SIGNAL( clicked ( )),
  //                    this, SLOT( saveState ( )));
  // QObject::connect ( ui.checkBox_autoSave, SIGNAL( clicked ( )),
  //                    this, SLOT( setAutoSaveState ( )));
  // QObject::connect ( ui.pushButton_undo, SIGNAL( clicked ( )),
  //                    this, SLOT( undoState ( )));

  // //Simplify
  // QObject::connect ( ui.radioButton_NthPoint, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_RadialDistance, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_PependicularDistance, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_ReumannWitkam, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_Opheim, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_Lang, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_DouglasPeucker, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_DouglasPueckerModif, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.radioButton_pythonSimplification, SIGNAL( clicked ( )),
  //                    this, SLOT( setSimplificationMethod ( )));
  // QObject::connect ( ui.pushButton_ApplySimplification, SIGNAL( clicked ( )),
  //                    this, SLOT( simplify ( )));

  // //Render
  // QObject::connect ( ui.checkBox_ShowOriginalMorphologyText, SIGNAL( clicked ( )),
  //                    this, SLOT( setMorphologyToShowInfo ( )));
  // QObject::connect ( ui.checkBox_ShowModifiedMorphologyText, SIGNAL( clicked ( )),
  //                    this, SLOT( setMorphologyToShowInfo ( )));
  // QObject::connect ( ui.checkBox_renderOriginalMorphology, SIGNAL( clicked ( )),
  //                    this, SLOT( setRenderOriginalMorphology ( )));
  // QObject::connect ( ui.checkBox_renderModifiedMorphology, SIGNAL( clicked ( )),
  //                    this, SLOT( setRenderModifiedMorphology ( )));
  // QObject::connect ( ui.spinBox_resolution, SIGNAL( editingFinished ( )),
  //                    this, SLOT( setResolutionToNodes ( )));
  // QObject::connect ( ui.doubleSpinBox_desp, SIGNAL( editingFinished ( )),
  //                    this, SLOT( setNodeIncToMorphology ( )));
  // QObject::connect ( ui.pushButton_OriginalNodeColor, SIGNAL( clicked ( )),
  //                    this, SLOT( setNodeColorToOriginalMorphology ( )));
  // QObject::connect ( ui.pushButton_OriginalLinkColor, SIGNAL( clicked ( )),
  //                    this, SLOT( setNodeColorToModifiedMorphology ( )));
  // QObject::connect ( ui.pushButton_ModifiedNodeColor, SIGNAL( clicked ( )),
  //                    this, SLOT( setLinkColorToOriginalMorphology ( )));
  // QObject::connect ( ui.pushButton_ModifiedLinkColor, SIGNAL( clicked ( )),
  //                    this, SLOT( setLinkColorToModifiedMorphology ( )));

  // //Enhancement
  // QObject::connect ( ui.radioButton_enhanceLinear, SIGNAL( clicked ( )),
  //                    this, SLOT( setEnhanceMethod ( )));
  // QObject::connect ( ui.radioButton_enhanceSpline, SIGNAL( clicked ( )),
  //                    this, SLOT( setEnhanceMethod ( )));
  // QObject::connect ( ui.radioButton_enhanceCustom, SIGNAL( clicked ( )),
  //                    this, SLOT( setEnhanceMethod ( )));
  // QObject::connect ( ui.pushButton_applyEnhancement, SIGNAL( clicked ( )),
  //                    this, SLOT( enhance ( )));

  // //Repair
  // QObject::connect ( ui.pushButton_SetRadiusToSelectedNode,
  //                    SIGNAL( clicked ( )),
  //                    this,
  //                    SLOT( setRadiusToSelectedNode ( )));
  // QObject::connect ( ui.pushButton_ApplyInterpolation, SIGNAL( clicked ( )),
  //                    this, SLOT( interpolateRadius ( )));
  // QObject::connect ( ui.pushButton_CalcMidlePosition, SIGNAL( clicked ( )),
  //                    this, SLOT( middlePosition ( )));

  // //Melt
  // QObject::connect ( ui.pushButton_selectObject, SIGNAL( clicked ( )),
  //                    this, SLOT( selectNeuronObject ( )));
  // QObject::connect ( ui.pushButton_simplifySelectedObject, SIGNAL( clicked ( )),
  //                    this, SLOT( simplifyNeuronObject ( )));
  // QObject::connect ( ui.pushButton_enhanceSelectedObject, SIGNAL( clicked ( )),
  //                    this, SLOT( enhanceNeuronObject ( )));

  // QObject::connect ( ui.pushButton_extractNeurite, SIGNAL( clicked ( )),
  //                    this, SLOT( extractNeurite ( )));
  // QObject::connect ( ui.pushButton_removeNeurite, SIGNAL( clicked ( )),
  //                    this, SLOT( removeNeurite ( )));
  // QObject::connect ( ui.pushButton_attachMorphology, SIGNAL( clicked ( )),
  //                    this, SLOT( attachMorphology ( )));

  // QObject::connect ( ui.pushButton_brokeLink, SIGNAL( clicked ( )),
  //                    this, SLOT( brokeLink ( )));
  // QObject::connect ( ui.pushButton_SetLink, SIGNAL( clicked ( )),
  //                    this, SLOT( setLink ( )));

  // //Utils
  // QObject::connect ( ui.pushButton_batchProcessing, SIGNAL( clicked ( )),
  //                    this, SLOT( showBatchBuilder ( )));

  // //Others
  // QObject::connect ( ui.pushButton_SetModifiedAsOriginal, SIGNAL( clicked ( )),
  //                    this, SLOT( setModifiedAsOriginal ( )));
  // QObject::connect ( ui.PushButton_reloadPythonModule, SIGNAL( clicked ( )),
  //                    this, SLOT( testPythonIntegration ( )));

  // //Default parameters
  // setSimplificationMethod ( );
  // setEnhanceMethod ( );
  // colorToButtons ( );
}

ReTracer::~ReTracer ( )
{
  delete _viewer;
  // if ( _batchBuilder != nullptr )
  //   delete _batchBuilder;
}

void ReTracer::init( void )
{
  _viewDock->init( _viewer );
  _selectDock->init( _viewer );
  _editDock->init( _viewer );
}

// void ReTracer::quit()
// {
//   QString text = QObject::tr("Are you sure you want to quit?");
//   QMessageBox msgBox(QObject::tr("Quit"), text, QMessageBox::Warning, QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape, QMessageBox::NoButton);
//   if (msgBox.exec() == QMessageBox::Yes)  close();
// }

// void ReTracer::takeSnapshot()
// {
//   QString fileName = QFileDialog::getSaveFileName ( this, tr ( "Save snapshot" ), "./" );
//   viewer->saveSnapshot ( fileName + ".png" );
// }


void ReTracer::importMorphology ( )
{
  QString fileName = QFileDialog::getOpenFileName(
    this, tr( "Open File" ), "./", tr ( "NeuroMorpho(*.swc)" ));
  if ( !fileName.isNull ( ))
  {
    _viewer->loadMorphology( fileName );
    // setTreeModel( );
  }
}

void ReTracer::exportMorphology ( )
{
  QString fileName = QFileDialog::getSaveFileName ( this );
  if ( fileName.isEmpty ( ))
    return;
  else
    _viewer->exportMorphology ( fileName );
}

void ReTracer::updateViewDock( void )
{
  if( _ui->actionView->isChecked( ))
    _viewDock->show( );
  else
    _viewDock->close( );
}

void ReTracer::updateSelectDock( void )
{
  if( _ui->actionSelect->isChecked( ))
    _selectDock->show( );
  else
    _selectDock->close( );
}

void ReTracer::updateEditDock( void )
{
  if( _ui->actionEditAndCorrect->isChecked( ))
    _editDock->show( );
  else
    _editDock->close( );
}
// void ReTracer::setModificationInterval ( )
// {
// }

// void ReTracer::setModifiedAsOriginal ( )
// {
//   viewer->setModifiedAsOriginal ( );
// }

// void ReTracer::saveState ( )
// {
//   viewer->saveState ( );
// }

// void ReTracer::setAutoSaveState ( )
// {
//   viewer->setAutoSaveState ( ui.checkBox_autoSave->isChecked ( ));
//   ui.pushButton_save->setEnabled ( !ui.checkBox_autoSave->isChecked ( ));
// }

// void ReTracer::undoState ( )
// {
//   viewer->undoState ( );
// }

// //Simplify
// void ReTracer::setSimplificationMethod ( )
// {
//   if ( ui.radioButton_NthPoint->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::NTHPOINT;
//   else if ( ui.radioButton_RadialDistance->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::RADIAL;
//   else if ( ui.radioButton_PependicularDistance->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::PERPDIST;
//   else if ( ui.radioButton_ReumannWitkam->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::REUMANNWITKAM;
//   else if ( ui.radioButton_Opheim->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::OPHEIN;
//   else if ( ui.radioButton_Lang->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::LANG;
//   else if ( ui.radioButton_DouglasPeucker->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::DOUGLASPEUCKER;
//   else if ( ui.radioButton_DouglasPueckerModif->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::DOUGLASPEUCKERMOD;
//   else if ( ui.radioButton_pythonSimplification->isChecked ( ))
//     _simplificationMode = SIMP_METHOD::CUSTOM_SIMP;
// }

// void ReTracer::simplify ( int objectId_, OBJECT_TYPE objectType)
// {
//   _params.clear ( );
//   switch (( int ) _simplificationMode )
//   {
//     case SIMP_METHOD::NTHPOINT:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      SIMP_METHOD::NTHPOINT ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.spinBox_NthPoints
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::RADIAL:
//       _params.insert ( std::pair < std::string, float > (
//         "Method",
//         SIMP_METHOD::RADIAL ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.doubleSpinBox_RadialDistance
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::PERPDIST:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      SIMP_METHOD::PERPDIST ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.doubleSpinBox_PerpendicularDistance
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::REUMANNWITKAM:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      SIMP_METHOD::REUMANNWITKAM ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.doubleSpinBox_ReumannWitkam
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::OPHEIN:
//       _params.insert ( std::pair < std::string, float > (
//         "Method",
//         SIMP_METHOD::OPHEIN ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.doubleSpinBox_OpheinMinThreshold
//                                                             ->value ( )));
//       _params.insert ( std::pair < std::string, float > ( "Value2",
//                                                           ui.doubleSpinBox__OpheinMaxThreshold
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::LANG:
//       _params.insert ( std::pair < std::string, float > (
//         "Method",
//         SIMP_METHOD::LANG ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.doubleSpinBox_Lang_Threshold
//                                                             ->value ( )));
//       _params.insert ( std::pair < std::string, float > ( "Value2",
//                                                           ui.doubleSpinBox_Lang_Size
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::DOUGLASPEUCKER:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      SIMP_METHOD::DOUGLASPEUCKER ));
//       _params
//         .insert ( std::pair < std::string, float > ( "Value",
//                                                      ui.doubleSpinBox_DouglasPeuckerThreshold
//                                                        ->value ( )));
//       break;

//     case SIMP_METHOD::DOUGLASPEUCKERMOD:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      SIMP_METHOD::DOUGLASPEUCKERMOD ));
//       _params.insert ( std::pair < std::string, float > ( "Value",
//                                                           ui.spinBox_DouglasPeucker_NumPoints
//                                                             ->value ( )));
//       break;

//     case SIMP_METHOD::CUSTOM_SIMP:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      SIMP_METHOD::CUSTOM_SIMP ));
//       break;
//     default:std::cout << "Unhandeled simplification method!!!" << std::endl;
//   }
//   viewer->simplify ( _params, objectId_ , objectType );
// }

// //Enhance
// void ReTracer::setEnhanceMethod ( )
// {
//   if ( ui.radioButton_enhanceLinear->isChecked ( ))
//     _enhanceMode = ENHANCE_METHOD::LINEAR;
//   else if ( ui.radioButton_enhanceSpline->isChecked ( ))
//     _enhanceMode = ENHANCE_METHOD::SPLINE;
//   else if ( ui.radioButton_enhanceCustom->isChecked ( ))
//     _enhanceMode = ENHANCE_METHOD::CUSTOM_ENHANCE;
// }

// void ReTracer::enhance ( int objectId_, OBJECT_TYPE objectType )
// {
//   _params.clear ( );
//   switch (( int ) _enhanceMode )
//   {
//     case ENHANCE_METHOD::LINEAR:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      ENHANCE_METHOD::LINEAR ));
//       break;

//     case ENHANCE_METHOD::SPLINE:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      ENHANCE_METHOD::SPLINE ));
//       break;

//     case ENHANCE_METHOD::CUSTOM_ENHANCE:
//       _params
//         .insert ( std::pair < std::string, float > ( "Method",
//                                                      ENHANCE_METHOD::CUSTOM_ENHANCE ));
//       break;
//     default:std::cout << "Unhandeled enhanced method!!!" << std::endl;
//   }
//   _params.insert ( std::pair < std::string, float > ( "Value",
//                                                       ui.doubleSpinBox_separationBetwenNodes
//                                                         ->value ( )));
//   viewer->enhance ( _params, objectId_ , objectType );
// }

// //Repair
// void ReTracer::insertNode ( )
// {

// }

// void ReTracer::deleteNode ( )
// {
// }

// void ReTracer::insertEdge ( )
// {
//   viewer->setLink ( );
// }

// void ReTracer::deleteEdge ( )
// {
//   viewer->brokeLink ( );
// }

// void ReTracer::interpolateRadius ( )
// {
//   viewer
//     ->interpolateRadius ( ui.doubleSpinBox_InitialDendirteThickness->value ( ),
//                           ui.doubleSpinBox_FinalDendriteThickness->value ( ),
//                           ui.doubleSpinBox_InitialApicalThickness->value ( ),
//                           ui.doubleSpinBox_FinalApicalThickness->value ( )
//     );
// }

// void ReTracer::middlePosition ( )
// {
//   viewer->middlePosition ( );
// }

// void ReTracer::setRadiusToSelectedNode ( )
// {
//   viewer->setRadiusToSelectedNode ( ui.doubleSpinBox_SetNodeRadius->value ( ));
// }

// void ReTracer::brokeLink ( )
// {
//   viewer->brokeLink ( );
// }

// void ReTracer::setLink ( )
// {
//   viewer->setLink ( );
// }

// //Render
// void ReTracer::setMorphologyToShowInfo ( )
// {
//   //That controls must be checkboxes!!!
//   if ( ui.checkBox_ShowOriginalMorphologyText->isChecked ( ))
//     viewer->setMorphologyInfoToShow ( 1 );
//   else if ( ui.checkBox_ShowModifiedMorphologyText->isChecked ( ))
//     viewer->setMorphologyInfoToShow ( 2 );
//   else if (( ui.checkBox_ShowOriginalMorphologyText->isChecked ( ))
//     && ( ui.checkBox_ShowModifiedMorphologyText->isChecked ( )))
//     viewer->setMorphologyInfoToShow ( 3 );
//   else
//     viewer->setMorphologyInfoToShow ( 0 );
// }

// void ReTracer::showTextValues ( )
// {
//   viewer->setShowTextValues ( ui.spinBox_TextIniValue->value ( ),
//                               ui.spinBox_TextFinValue->value ( ));
// }

// void ReTracer::setRenderOriginalMorphology ( )
// {
//   std::cout << "Modified morphology render:"
//             << ui.checkBox_renderOriginalMorphology->isChecked ( ) << std::endl;
//   viewer->setRenderModifiedMorphology ( ui.checkBox_renderOriginalMorphology
//                                           ->isChecked ( ));
// }

// void ReTracer::setRenderModifiedMorphology ( )
// {
//   std::cout << "Original morphology render:"
//             << ui.checkBox_renderModifiedMorphology->isChecked ( ) << std::endl;
//   viewer->setRenderOriginalMorphology ( ui.checkBox_renderModifiedMorphology
//                                           ->isChecked ( ));
// }

// void ReTracer::changeToDualView ( )
// {
//   viewer->changeToDualView ( );
// }

// void ReTracer::setResolutionToNodes ( )
// {
//   // viewer->getOriginalMorphologyRenderer ( )
//   //       ->setResolution ( ui.spinBox_resolution->value ( ));
//   // viewer->getModifiedMorphologyRenderer ( )
//   //       ->setResolution ( ui.spinBox_resolution->value ( ));
// }

// void ReTracer::setNodeIncToMorphology ( )
// {
//   // viewer->getModifiedMorphologyRenderer ( )
//   //       ->setIncrement ( ui.doubleSpinBox_desp->value ( ));
// }

// void ReTracer::setNodeColorToOriginalMorphology ( )
// {
//   // QColor color = QColorDialog::getColor ( Qt::black, this );
//   // if ( color.isValid ( ))
//   // {
//   //   viewer->getOriginalMorphologyRenderer ( )
//   //         ->setNodeColor ( nsol::Vec3f ( color.redF ( ),
//   //                                        color.greenF ( ),
//   //                                        color.blueF ( )));

//   //   QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   //   ui.pushButton_OriginalNodeColor->setStyleSheet ( qss );
//   // }
// }

// void ReTracer::setNodeColorToModifiedMorphology ( )
// {
//   // QColor color = QColorDialog::getColor ( Qt::black, this );
//   // if ( color.isValid ( ))
//   // {
//   //   viewer->getOriginalMorphologyRenderer ( )
//   //         ->setLinkColor ( nsol::Vec3f ( color.redF ( ),
//   //                                        color.greenF ( ),
//   //                                        color.blueF ( )));
//   //   QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   //   ui.pushButton_OriginalLinkColor->setStyleSheet ( qss );
//   // }
// }

// void ReTracer::setLinkColorToOriginalMorphology ( )
// {
//   // QColor color = QColorDialog::getColor ( Qt::black, this );
//   // if ( color.isValid ( ))
//   // {
//   //   viewer->getModifiedMorphologyRenderer ( )
//   //         ->setNodeColor ( nsol::Vec3f ( color.redF ( ),
//   //                                        color.greenF ( ),
//   //                                        color.blueF ( )));
//   //   QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   //   ui.pushButton_ModifiedNodeColor->setStyleSheet ( qss );
//   // }
// }

// void ReTracer::setLinkColorToModifiedMorphology ( )
// {
//   // QColor color = QColorDialog::getColor ( Qt::black, this );
//   // if ( color.isValid ( ))
//   // {
//   //   viewer->getModifiedMorphologyRenderer ( )
//   //         ->setLinkColor ( nsol::Vec3f ( color.redF ( ),
//   //                                        color.greenF ( ),
//   //                                        color.blueF ( )));
//   //   QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   //   ui.pushButton_ModifiedLinkColor->setStyleSheet ( qss );
//   // }
// }

// void ReTracer::colorToButtons ( )
// {
//   QColor color = QColor ( Qt::red );
//   QString qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   ui.pushButton_OriginalNodeColor->setStyleSheet ( qss );

//   color = QColor ( 192, 192, 192 );
//   qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   ui.pushButton_OriginalLinkColor->setStyleSheet ( qss );

//   color = QColor ( Qt::blue );
//   qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   ui.pushButton_ModifiedNodeColor->setStyleSheet ( qss );

//   color = QColor ( Qt::green );
//   qss = QString ( "background-color: %1" ).arg ( color.name ( ));
//   ui.pushButton_ModifiedLinkColor->setStyleSheet ( qss );

// }

// //Utils
// void ReTracer::showBatchBuilder ( )
// {
//   if ( _batchBuilder == nullptr )
//   {
//     _batchBuilder = new BatchBuilder ( );
//     QObject::connect ( _batchBuilder,
//                        SIGNAL( directoriesReadies ( )),
//                        this,
//                        SLOT( genetareNeuronsInBatch ( )));
//   }

//   _batchBuilder->show ( );
// }

// void ReTracer::genetareNeuronsInBatch ( )
// {
//   _inputFilePath = _batchBuilder->getInputDir ( );
//   _ouputFilePath = _batchBuilder->getOutputDir ( );
//   this->copyRecursively ( _inputFilePath,
//                           _ouputFilePath );
// }

// bool ReTracer::copyRecursively ( const QString& sourceFolder,
//                                  const QString& destFolder )
// {
//   bool success = false;
//   QDir sourceDir ( sourceFolder );

//   if ( !sourceDir.exists ( ))
//     return false;

//   QDir destDir ( destFolder );
//   if ( !destDir.exists ( ))
//     destDir.mkdir ( destFolder );

//   QStringList filters;
//   filters << "*.swc" << "*.SWC";

//   QStringList files = sourceDir.entryList ( filters, QDir::Files );
//   for ( int i = 0; i < files.count ( ); i++ )
//   {
//     QString path = sourceFolder + QDir::separator ( ) + files[i];
//     viewer->loadMorphology ( path );

//     //###Does not work out of the loop    
//     if (_batchBuilder->applySimplify ( ))
//     {
//       setSimplificationMethod ( );
//       simplify ( );    
//     } else if (_batchBuilder->applyEnhance ( ) )
//     {
//       setEnhanceMethod ( );
//       enhance ( );
//     } 
//     else
//     {
//       //###Fix methods needs to be developed
//     }
    
//     path = destFolder + QDir::separator ( ) + _batchBuilder->getBaseName ( ) + files[i];
//     viewer->exportMorphology ( path );
//   }

//   files.clear ( );
//   files = sourceDir.entryList ( QDir::AllDirs | QDir::NoDotAndDotDot );
//   for ( int i = 0; i < files.count ( ); i++ )
//   {
//     QString srcName = sourceFolder + QDir::separator ( ) + files[i];
//     QString destName = destFolder + QDir::separator ( ) + files[i];
//     success = copyRecursively ( srcName, destName );
//     if ( !success )
//       return false;
//   }

//   return true;
// }


// //Fine detail selecction
// void ReTracer::setTreeModel ( )
// {
//   ui.treeView_morphologyThree->setModel ( viewer->getTreeModel ( ));
// }

// void ReTracer::attachMorphology ( )
// {
//   QString fileName = QFileDialog::getOpenFileName ( this,
//                                                     tr ( "Open File" ),
//                                                     "./",
//                                                     tr ( "NeuroMorpho(*.swc)" ));
//   if ( !fileName.isNull ( ))
//   {
//   }
// }

// void ReTracer::extractNeurite ( )
// {
//   QString fileName = QFileDialog::getSaveFileName ( this );
//   if ( fileName.isEmpty ( ))
//   {
//     return;
//   }
//   else
//   {
//     viewer->ExtractNeurite ( fileName );
//   }
// }

// void ReTracer::removeNeurite ( )
// {
//   QString fileName = QFileDialog::getSaveFileName ( this );
//   if ( fileName.isEmpty ( ))
//   {
//     return;
//   }
//   else
//   {
//     viewer->ExtractNeurite ( fileName );
//   }
// }

// void ReTracer::selectNeuronObject ( )
// {
//   QModelIndex index = ui.treeView_morphologyThree->currentIndex ( );
//   QVariant extractedData = ui.treeView_morphologyThree->model ( )->data ( index );
//   unsigned int value = extractedData.toInt ( );
//   unsigned int deph = 0;

//   while (index.isValid ())
//   {
//     ++deph;
//     index = index.parent();
//   }
//   --deph;

//   switch ( deph )
//   {
//     case OBJECT_TYPE::NEURITE:
//       viewer->selectDendrite ( value );
//       break;

//     case OBJECT_TYPE::NODE:
//       viewer->selectNode ( value );
//       break;
//     case OBJECT_TYPE::SECTION:
//       viewer->selectSection ( value );
//       break;
//   }
// }

// void ReTracer::simplifyNeuronObject ( )
// {
//   QModelIndex index = ui.treeView_morphologyThree->currentIndex ( );
//   QVariant extractedData = ui.treeView_morphologyThree->model ( )->data ( index );
//   unsigned int value = extractedData.toInt ( );
//   unsigned int deph = 0;

//   while (index.isValid ())
//   {
//     ++deph;
//     index = index.parent();
//   }
//   --deph;

//   simplify ( value, ( OBJECT_TYPE )deph );
// }

// void ReTracer::enhanceNeuronObject ( )
// {
//   QModelIndex index = ui.treeView_morphologyThree->currentIndex ( );
//   QVariant
//     extractedData = ui.treeView_morphologyThree->model ( )->data ( index );
//   unsigned int value = extractedData.toInt ( );
//   unsigned int deph = 0;
//   while (index.isValid ())
//   {
//     ++deph;
//     index = index.parent();
//   }
//   --deph;

//   enhance ( value, ( OBJECT_TYPE )deph );
// }

// //Other
// void ReTracer::testPythonIntegration ( )
// {  
//   viewer->getUtils ( )->reloadInstance ( );
// }

