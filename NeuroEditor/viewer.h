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

  typedef enum {
    NONE = 0,
    ADD,
    REMOVE
  } tSelectionMode;

  typedef enum {
    INCLUSIVE = 0,
    EXCLUSIVE
  } tSelectionInclusionMode;

  typedef enum {
    NODE = 0,
    SECTION,
    NEURITE
  } tSelectionType;

  Viewer ( QWidget* parent = 0 );

  virtual ~Viewer ( );

  void setupViewer ( );
  void loadMorphology ( QString pSWCFile );
  void exportMorphology ( QString pFile );
  void exportMesh( QString pFile );

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

  unsigned int getNumNeurites ( )
  {
    return modifiedMorphology->neurites ( ).size ( );
  }

  void setModifiedAsOriginal ( );

  void selectDendrite ( unsigned int dendriteId_ );
  void selectSection ( unsigned int sectionId_ );
  void selectNode ( unsigned int nodeId_ );

  TreeModel* getTreeModel ( ) { return _treeModel; }

  // neuroeditor::Utils * getUtils ( ) { return util; };

  neuroeditor::MorphologyStructure* morphologyStructure( void )
  {
    return _morphoStructure;
  }

  std::unordered_set< int >& selection( void )
  {
    return _selection;
  }


public Q_SLOTS://slots:

  void updateSideBySide( bool sideBySideState_ );

  void updateFLROriginalStructure( bool state_ );
  void updateFLROriginalMesh( bool state_ );
  void updateFLRModifiedStructure( bool state_ );
  void updateFLRModifiedMesh( bool state_ );

  void updateSLROriginalStructure( bool state_ );
  void updateSLROriginalMesh( bool state_ );
  void updateSLRModifiedStructure( bool state_ );
  void updateSLRModifiedMesh( bool state_ );

  void changeBackgroundColor( QColor color_ );
  void changeOriginalStructureColor( QColor color_ );
  void changeOriginalMeshColor( QColor color_ );
  void changeModifiedStructureColor( QColor color_ );
  void changeModifiedMeshColor( QColor color_ );

  void setCameraViewFront( void );
  void setCameraViewTop( void );
  void setCameraViewBottom( void );
  void setCameraViewLeft( void );
  void setCameraViewRight( void );

  void undoState( void );

  void reset ( );

  void focusOnSelection( );

  void updateMorphology( void );
  void updateSelection( std::unordered_set< int > selection_ );
  void checkSelection( void );
  void updateSelectionInclusionMode(
    tSelectionInclusionMode selectionInclusionMode_ );
  void updateSelectionType( tSelectionType selectionType_ );

  void changeAveragePos( Eigen::Vector3f& pos_ );
  void changeRotation( Eigen::Quaternionf& q_ );
  void changeAverageRadius( float radius_ );

Q_SIGNALS:

  void morphologyChanged( void );

  void updateSelectionSignal( std::unordered_set< int > selection_ );

  void resetInspectorSignal( void );

  void updateAveragePosSignal( Eigen::Vector3f& pos_ );
  void updateRotationSignal( Eigen::Quaternionf& q_ );
  void updateAverageRadiusSignal( float radius_ );

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

  void _changeSelection( void );

  Scene* _scene;
  nsol::SwcReader swcReader;
  nsol::SwcWriter swcWriter;

  nsol::NeuronMorphologyPtr originalMorphology;
  nsol::NeuronMorphologyPtr modifiedMorphology;

  nsol::NeuritePtr _somaNeurite;
  nsol::NeuronMorphologySectionPtr _somaSection;

  bool _autoSaveState;
  std::stack < nsol::NeuronMorphologyPtr > _stack;

  TreeModel* _treeModel;

  SIMP_METHOD _simpMethod;

  // neuroeditor::Utils * util;

  short int _morphologyInfoToShow;
  bool _ShowText;
  int _ShowTextIniValue;
  int _ShowTextFinValue;
  unsigned int _procInitialValue;
  unsigned int _procFinalValue;

  bool mSplitScreen;

  // Current rectangular selection
  QRect _rectangle;

  // Different selection modes
  tSelectionMode _selectionMode;
  tSelectionInclusionMode _selectionInclusionMode;
  tSelectionType _selectionType;
  std::unordered_set< int > _selection;

  neuroeditor::MorphologyStructure* _morphoStructure;

  tLayout _firstLayout;
  tLayout _secondLayout;

  Eigen::Vector3f _averagePosition;
  float _averageRadius;

  const static float _colorFactor;

};

#endif /* VIEWERINTERFACE_H_ */
