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

#ifndef VIEWERINTERFACE_H_
#define VIEWERINTERFACE_H_

#include <stack>
#include <map>
#include <qapplication.h>
#include <qfiledialog.h>

#include <QGLViewer/qglviewer.h>
using namespace qglviewer;

#include <nsol/nsol.h>

#include "Utils.h"
#include "object.h"
#include "nsolRenderer.h"
#include "TreeModel.h"
#include "Scene.h"

typedef struct tLayout{
  bool renderModifiedStructure;
  bool renderModifiedMesh;
  bool renderOriginalStructure;
  bool renderOriginalMesh;
} tLayout;

class Viewer: public QGLViewer
{
  Q_OBJECT        // must include this if you use Qt signals/slots

public:

  Viewer ( QWidget* parent );

  virtual ~Viewer ( );

  void setupViewer ( );

  void initScene( );

  void loadMorphology ( QString pSWCFile );
  void exportMorphology ( QString pFile );

  //Shows
  void setMorphologyInfoToShow ( short int morphologyInfoToShow_ )
  {
    _morphologyInfoToShow = morphologyInfoToShow_;
    updateGL( );
  };
  void setShowText ( bool pShowText ) { _ShowText = pShowText; };
  void setShowTextValues ( int pShowTextIniValue, int pShowTextFinValue )
  {
    _ShowTextIniValue = pShowTextIniValue;
    _ShowTextFinValue = pShowTextFinValue;
  };

  void setModificationInterval ( unsigned int pIniValue,
                                 unsigned int pFinalValue );

  void saveState ( );
  void setAutoSaveState ( bool );

  void undoState ( );

  //Render
  void setRenderOriginalMorphology ( bool renderOriginalMorphology_ );
  void setRenderModifiedMorphology ( bool renderModifiedMorphology_ );


  //Operations
  void simplify ( std::map < std::string, float >& optParams,
                  int objectId = -1, OBJECT_TYPE objectType =
                  OBJECT_TYPE::NEURITE );

  void interpolateRadius ( float pInitDendriteRadius,
                           float pFinalDendriteRadius,
                           float pInitApicalRadius,
                           float pFinalApicalRadius );
  unsigned int getNumNeurites ( )
  {
    return modifiedMorphology->neurites ( ).size ( );
  }
  void attachMorphology ( const nsol::NeuronMorphologyPtr& morphology_ );
  void enhance (  std::map < std::string, float >& optParams,
                  int objectId = -1, OBJECT_TYPE objectType =
                  OBJECT_TYPE::NEURITE );

  void setRadiusToSelectedNode ( float lRadius );
  void brokeLink ( );
  void setLink ( );

  //Others
  void setModifiedAsOriginal ( );
  void middlePosition ( );
  void ExtractNeurite ( QString pFile );

  void changeToDualView ( );
  void selectDendrite ( unsigned int dendriteId_ );
  void selectSection ( unsigned int sectionId_ );
  void selectNode ( unsigned int nodeId_ );

  TreeModel* getTreeModel ( ) { return _TreeModel; }

  retracer::Utils * getUtils ( ) { return util; };

public Q_SLOTS://slots:

  // reset application
  void reset ( );
protected:

  // initialize - update - draw functions
  virtual void init ( );
  virtual void animate ( );
  virtual void draw ( );

  //Manipulation functions
  //----------------------
  // Selection functions
  virtual void drawWithNames ( );
  virtual void endSelection ( const QPoint& );

  // Mouse events functions
  virtual void mousePressEvent ( QMouseEvent* e );
  virtual void mouseMoveEvent ( QMouseEvent* e );
  virtual void mouseReleaseEvent ( QMouseEvent* e );

  virtual void keyPressEvent ( QKeyEvent* e );

  virtual void resizeGL( int width, int height );

  virtual QString helpString ( ) const;

  void startManipulation( );
  void whileManipulation( );
  void endManipulation( );
  void drawSelectionRectangle ( ) const;
  void addIdToSelection( int id );
  void removeIdFromSelection ( int id );

  void renderMorphologyInfo( nsol::NeuronMorphologyPtr morphology_,
                             bool secondView_ = false );

  void createSelectorsAndStructure(
    const nsol::NeuronMorphologyPtr& morphology_ );

  void adjustSelecRegion( int windowWidth_, int windowHeigth_,
                          QPoint& regionCenter_, int& regionWidth_,
                          int& regionHegiht_ );



  Scene* _scene;
  nsol::SwcReader swcReader;
  nsol::SwcWriter swcWriter;

  nsol::NeuronMorphologyPtr originalMorphology;
  nsol::NeuronMorphologyPtr modifiedMorphology;

  bool _autoSaveState;
  std::stack < nsol::NeuronMorphologyPtr > _stack;

  TreeModel* _TreeModel;

  SIMP_METHOD _simpMethod;

  retracer::Utils * util;

  short int _morphologyInfoToShow;
  bool _ShowText;
  int _ShowTextIniValue;
  int _ShowTextFinValue;
  unsigned int _procInitialValue;
  unsigned int _procFinalValue;

  bool _renderOriginalMorphology;
  bool _renderModifiedMorphology;
  bool mSplitScreen;

  // Current rectangular selection
  QRect rectangle_;

  // Different selection modes
  enum SelectionMode { NONE, ADD, REMOVE };
  SelectionMode selectionMode_;

  QList < Object* > objects_;
  std::unordered_set< int > selection_;
  std::unordered_map< int, nsol::NodePtr > idToNode_;
  std::unordered_map< int, Object* > idToObject_;

  tLayout _firstLayout;
  tLayout _secondLayout; 

};

#endif /* VIEWERINTERFACE_H_ */
