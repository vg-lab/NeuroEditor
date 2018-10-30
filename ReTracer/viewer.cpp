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
#include "viewer.h"

#include "Utils.h"

#include <QMouseEvent>
#include <QImage>
#include <math.h>

#include "manipulatedFrameSetConstraint.h"

#include <QGLViewer/manipulatedFrame.h>
#include <limits>
#include <iostream>
#include <algorithm>

using namespace std;

Viewer::Viewer ( QWidget *parent )
  : QGLViewer ( parent )
  , _scene( nullptr )
  , originalMorphology( nullptr )
  , modifiedMorphology( nullptr )
{
  selectionMode_ = NONE;

  _TreeModel = nullptr;

  _morphologyInfoToShow = 0;

  _ShowText = false;
  mSplitScreen = false;

  _ShowTextIniValue
    = _ShowTextFinValue
    = 0;

  _renderOriginalMorphology
    = _renderModifiedMorphology
    = true;

  _autoSaveState = true;

  _firstLayout.renderModifiedStructure = true;
  _firstLayout.renderModifiedMesh = true;
  _firstLayout.renderOriginalStructure = false;
  _firstLayout.renderOriginalMesh = false;

  _secondLayout.renderModifiedStructure = false;
  _secondLayout.renderModifiedMesh = false;
  _secondLayout.renderOriginalStructure = true;
  _secondLayout.renderOriginalMesh = true;
}

Viewer::~Viewer ( )
{
  if ( _scene )
    delete _scene;
}

void Viewer::init ( )
{
  nlrender::Config::init( );
  _scene = new Scene( 0, 600, 600 );

  camera ( )->setZClippingCoefficient ( 500.0 );
  camera ( )->setZNearCoefficient ( 0.0001 );

  setBackgroundColor ( QColor ( 255, 255, 255 ));

  setManipulatedFrame ( new ManipulatedFrame ( ));
  manipulatedFrame ( )->setConstraint ( new ManipulatedFrameSetConstraint ( ));

  glBlendFunc ( GL_ONE, GL_ONE );

}

void Viewer::setupViewer ( )
{
  selectionMode_ = SelectionMode::NONE;
  int argc = 0;
  char **argv = nullptr;
  glutInit ( &argc, argv );
}

void Viewer::initScene( void )
{
}

void Viewer::animate ( )
{
}

//  D r a w i n g   f u n c t i o n
void Viewer::draw ( )
{
  glLineWidth( 1.0f );
  std::vector< float > viewVec( 16 );
  camera( )->getModelViewMatrix( viewVec.data( ));
  std::vector< float > projectionVec( 16 );
  camera( )->getProjectionMatrix( projectionVec.data( ));
  Eigen::Matrix4f view( viewVec.data( ));
  _scene->viewMatrix( view );
  Eigen::Matrix4f projection( projectionVec.data( ));
  _scene->projectionMatrix( projection );

  nsol::NeuronMorphologyPtr morphologyInfo;
  switch( _morphologyInfoToShow )
  {
  case 1:
    morphologyInfo = originalMorphology;
    break;
  case 2:
    morphologyInfo = modifiedMorphology;
    break;
  default:
    morphologyInfo = nullptr;
    break;
  }
  if ( !mSplitScreen )
  {
    glViewport ( 0, 0, width ( ), height( ));
    _scene->render( _firstLayout.renderModifiedStructure,
                    _firstLayout.renderModifiedMesh,
                    _firstLayout.renderOriginalStructure,
                    _firstLayout.renderOriginalMesh );
    renderMorphologyInfo ( morphologyInfo );
  }
  else
  {
    glViewport ( 0, 0, width ( )*0.5, height( ));
    _scene->render( _firstLayout.renderModifiedStructure,
                    _firstLayout.renderModifiedMesh,
                    _firstLayout.renderOriginalStructure,
                    _firstLayout.renderOriginalMesh );
    renderMorphologyInfo ( morphologyInfo );

    glViewport ( width( )*0.5, 0, width( )*0.5, height( ));
    _scene->render( _secondLayout.renderModifiedStructure,
                    _secondLayout.renderModifiedMesh,
                    _secondLayout.renderOriginalStructure,
                    _secondLayout.renderOriginalMesh );
    renderMorphologyInfo ( morphologyInfo, true );
  }

  float axisSize = camera( )->distanceToSceneCenter( ) / 10.0f;
  if ( manipulatedFrame ( )->isManipulated ( ))
  {
    if ( mSplitScreen)
    {
      glPushMatrix ( );
      glMultMatrixd ( manipulatedFrame ( )->matrix ( ));
      if ( _firstLayout.renderModifiedStructure )
      {
        glViewport ( 0, 0, width( )*0.5, height( ));
        drawAxis ( axisSize );
      }
      if ( _secondLayout.renderModifiedStructure )
      {
        glViewport ( width( )*0.5, 0, width( )*0.5, height( ));
        drawAxis ( axisSize );
      }
      glPopMatrix ( );
    }
    else if ( _firstLayout.renderModifiedStructure )
    {
      glPushMatrix ( );
      glMultMatrixd ( manipulatedFrame ( )->matrix ( ));
      drawAxis ( axisSize );
      glPopMatrix ( );
    }
  }

  if ( mSplitScreen ) glViewport ( 0, 0, width ( ), height ( ));

  if ( selectionMode_ != NONE )
    drawSelectionRectangle ( );

  glColor3f ( 0, 0, 0 );
  drawText ( 15, 15, "Selected:" + QString::number ( selection_.size ( )));

}
//  S L O T S  F U N C T I O N S
void Viewer::reset ( )
{

}

QString Viewer::helpString ( ) const
{
  QString text ( "<h2>ReTracer: A neuromophological tracings editor</h2>" );
  text += "Description here ...\n";
  return text;
}

void Viewer::renderMorphologyInfo ( nsol::NeuronMorphologyPtr morphology_,
                                    bool secondView_ )
{
  int lIterIni = -1;
  int lIterFin = -1;

  if ( morphology_ != nullptr )
  {
    if (( _ShowTextIniValue != 0 ) || ( _ShowTextFinValue != 0 ))
    {
      lIterIni = _ShowTextIniValue;
      lIterFin = _ShowTextFinValue;
    }
    else lIterFin = std::numeric_limits<int>::max ();

    qglviewer::Vec pos;
    static qglviewer::Vec proj;
    glColor3f ( 0, 0, 0 );

    startScreenCoordinatesSystem ( );
    for ( auto neurite: morphology_->neurites ( ))
    {
      for ( auto section: neurite->sections ( ))
      {
        for ( auto node: section->nodes ( ))
        {
          if (( node->id ( ) > lIterIni ) && ( node->id ( ) <= lIterFin ))
          {
            pos.x = node->point ( ).x ( );
            pos.y = node->point ( ).y ( );
            pos.z = node->point ( ).z ( );
            proj = camera ( )->projectedCoordinatesOf ( pos );
            if ( mSplitScreen )
            {
              if ( proj.x > 0 && proj.x < width( )/2 - 20 )
              {
                if ( secondView_ )
                  proj.x = proj.x + width( )/2;
                drawText ( proj.x, proj.y, QString::number ( node->id ( )));
              }
            }
            else
              drawText ( proj.x, proj.y, QString::number ( node->id ( )));
          }
        }
      }
    }
    stopScreenCoordinatesSystem ( );
  }
}

void Viewer::createSelectorsAndStructure(
  const nsol::NeuronMorphologyPtr &morphology_ )
{
  selectionMode_ = NONE;
  objects_.clear( );
  idToNode_.clear( );
  idToObject_.clear( );

  for ( auto node: morphology_->soma( )->nodes( ))
  {
    auto object = new Object( );
    object->frame.setPosition( node->point( ).x( ),
                               node->point( ).y( ),
                               node->point( ).z( ));
    object->id = node->id( );
    object->radius = node->radius( );
    objects_.append( object );
    idToNode_[node->id( )] = node;
    idToObject_[node->id( )] = object;
  }

  for ( auto neurite: morphology_->neurites( ))
    for ( auto section: neurite->sections( ))
      for ( auto node: section->nodes( ))
      {
        auto object = new Object ( );
        object->frame.setPosition( node->point( ).x( ),
                                   node->point( ).y( ),
                                   node->point( ).z( ));
        object->id = node->id ( );
        object->radius = node->radius( );
        objects_.append ( object );
        idToNode_[node->id( )] = node;
        idToObject_[node->id( )] = object;
      }
}

void Viewer::loadMorphology ( QString pSWCFile )
{
  originalMorphology = swcReader.readMorphology ( pSWCFile.toStdString ( ));
  modifiedMorphology = originalMorphology->clone ( );

  _scene->originalMorphology( originalMorphology );
  _scene->modifiedMorphology( modifiedMorphology );

  auto aabb = _scene->aabb( );

  Vec center( aabb.center( ).x( ), aabb.center( ).y( ),
              aabb.center( ).z( ));
  setSceneCenter( center );
  setSceneRadius( aabb.radius( ));
  camera( )->fitSphere( center, aabb.radius( ));

  createSelectorsAndStructure( modifiedMorphology );
  while ( !_stack.empty ( ))
    _stack.pop ( );
  saveState ( );

  if ( _TreeModel != nullptr )
    delete _TreeModel;
  _TreeModel = new TreeModel ( modifiedMorphology );

  updateGL ( );
}

void Viewer::exportMorphology ( QString pFile )
{
  swcWriter.writeMorphology( pFile.toStdString( ).c_str( ),
                             modifiedMorphology );
}

void Viewer::setModificationInterval ( unsigned int pIniValue,
                                       unsigned int pFinalValue )
{
  _procInitialValue = pIniValue;
  _procFinalValue = pFinalValue;
}

void Viewer::simplify ( std::map < std::string, float > &optParams,
                        int objectId, OBJECT_TYPE objectType )
{
  util->getInstance( )->Simplify( modifiedMorphology, optParams,
                                  objectId, objectType );
  createSelectorsAndStructure( modifiedMorphology );
  _scene->updateModifiedStructure( );
  _scene->updateModifiedMesh( );
  if ( _autoSaveState )
    saveState( );
  updateGL( );
}

void Viewer::setModifiedAsOriginal ( )
{
  originalMorphology = modifiedMorphology->clone ( );
  _scene->originalMorphology( originalMorphology );
  updateGL( );
}

void Viewer::interpolateRadius ( float pInitDendriteRadius,
                                 float pFinalDendriteRadius,
                                 float pInitApicalRadius,
                                 float pFinalApicalRadius )
{
  if ( modifiedMorphology != nullptr )
  {
    for ( auto neurite: modifiedMorphology->neurites ( ))
    {
      for ( auto section: neurite->sections ( ))
      {
        for ( auto node: section->nodes ( ))
        {
          //### It needs to be done using parametric values. 
          node->radius (
            pInitDendriteRadius + pFinalDendriteRadius + pInitApicalRadius
              + pFinalApicalRadius );
        }
      }
    }
    createSelectorsAndStructure( modifiedMorphology );
    _scene->updateModifiedStructure( );
    _scene->updateModifiedMesh( );
    if ( _autoSaveState ) saveState ( );
  }
  updateGL ( );
}

void Viewer::attachMorphology ( const nsol::NeuronMorphologyPtr &morphology_ )
{
  //mNeuronGraphOptimizer->copyGraph();
  //mNeuronGraphOptimizer->getOptimizedGraph().attachSWCImporter(pSWCImporter);
  //### under development
  ( void ) morphology_;
}

//Manipulated methods
//-------------------
//   C u s t o m i z e d   m o u s e   e v e n t s
void Viewer::mousePressEvent ( QMouseEvent *e )
{
  // Start selection. Mode is ADD with Shift key and TOGGLE with Alt key.
  rectangle_ = QRect ( e->pos ( ), e->pos ( ));

  if (( e->button ( ) == Qt::LeftButton )
    && ( e->modifiers ( ) == Qt::ShiftModifier ))
    selectionMode_ = ADD;
  else if (( e->button ( ) == Qt::RightButton )
    && ( e->modifiers ( ) == Qt::ShiftModifier ))
    selectionMode_ = REMOVE;
  else
  {
    if ( e->modifiers ( ) == Qt::ControlModifier )
      startManipulation ( );
    QGLViewer::mousePressEvent ( e );
  }
}

void Viewer::mouseMoveEvent ( QMouseEvent *e )
{
  if ( selectionMode_ != NONE )
  {
    rectangle_.setBottomRight ( e->pos ( ) );
    updateGL ( );
  }
  else
    if ( e->modifiers ( ) == Qt::ControlModifier )
      whileManipulation( );
    QGLViewer::mouseMoveEvent ( e );
}

void Viewer::mouseReleaseEvent ( QMouseEvent *e )
{
  if ( selectionMode_ != NONE )
  {
    rectangle_ = rectangle_.normalized ( );
    QPoint center = rectangle_.center ( );
    int selectWidth = rectangle_.width( );
    int selectHeight = rectangle_.height( );

    if ( !mSplitScreen )
    {
      if ( _firstLayout.renderModifiedStructure )
      {
        adjustSelecRegion( width( ), height( ), center,
                           selectWidth, selectHeight );
        setSelectRegionWidth ( selectWidth );
        setSelectRegionHeight ( selectHeight );
        if ( selectWidth != 0 && selectHeight != 0 )
          select( center );
      }
    }
    else
    {
      int auxSelectWidth = selectWidth;
      int auxSelectHeight = selectHeight;
      QPoint auxCenter = center;
      if ( _firstLayout.renderModifiedStructure )
      {
        adjustSelecRegion( width( )*0.5, height( ), auxCenter,
                           auxSelectWidth, auxSelectHeight );
        setSelectRegionWidth ( auxSelectWidth );
        setSelectRegionHeight ( auxSelectHeight );
        if ( auxSelectWidth != 0 && auxSelectHeight != 0 )
          select( auxCenter );
      }
      if ( _secondLayout.renderModifiedStructure )
      {
        auxSelectWidth = selectWidth;
        auxSelectHeight = selectHeight;
        auxCenter = center;
        auxCenter.setX( center.x( ) - width( ) * 0.5 );
        adjustSelecRegion( width( )*0.5, height( ), auxCenter,
                           auxSelectWidth, auxSelectHeight );
        setSelectRegionWidth ( auxSelectWidth );
        setSelectRegionHeight ( auxSelectHeight );
        if ( auxSelectWidth != 0 && auxSelectHeight != 0 )
          select( auxCenter );
      }
    }

    selectionMode_ = NONE;
    updateGL ( );
  }
  else
  {
    if ( e->modifiers ( ) == Qt::ControlModifier )
      endManipulation( );

    QGLViewer::mouseReleaseEvent ( e );
  }
}

void Viewer::resizeGL( int width, int height )
{
  QGLViewer::resizeGL( width, height );
  _scene->size( width, height );
  if ( mSplitScreen )
    camera( )->setScreenWidthAndHeight( width/2, height );
}

void Viewer::keyPressEvent ( QKeyEvent *e )
{
  if ( e->key ( ) == Qt::Key_Escape )
  {
    selection_.clear ( );
    _scene->setSelection( selection_ );
  }
  else
    QGLViewer::keyPressEvent ( e );
  updateGL ( );
}

//   C u s t o m i z e d   s e l e c t i o n   p r o c e s s
void Viewer::drawWithNames ( )
{
  for ( int i = 0; i < objects_.size( ); i++ )
  {
    glPushName( i );
    objects_[i]->draw( );
    glPopName( );
  }
}

void Viewer::endSelection ( const QPoint & )
{
  glFlush ( );

  GLint nbHits = glRenderMode ( GL_RENDER );

  if ( nbHits > 0 )
  {
    for ( int i = 0; i < nbHits; ++i )
      switch ( selectionMode_ )
      {
        case ADD    :
          selection_.insert( objects_[ selectBuffer( )[4*i + 3]]->id );
          break;
        case REMOVE :
          selection_.erase( objects_[ selectBuffer( )[4*i + 3]]->id );
          break;
        default : break;
      }
  }
  _scene->setSelection( selection_ );
}

void Viewer::startManipulation( )
{
  if ( selection_.size( ) > 0 )
  {
    Vec averagePosition;
    ManipulatedFrameSetConstraint *mfsc =
      ( ManipulatedFrameSetConstraint* )( manipulatedFrame( )->constraint( ));
    mfsc->clearSet( );

    for ( auto select: selection_ )
    {
      auto object = idToObject_[select];
      mfsc->addObjectToSet( object );
      averagePosition += object->frame.position( );
    }
    averagePosition /= selection_.size ( );
    manipulatedFrame( )->setPosition( averagePosition );
  }
}

void Viewer::whileManipulation( )
{
  Vec lAuxVec;

  if ( selection_.size ( ) > 0 )
  {
    for ( auto select: selection_ )
    {
      auto object = idToObject_[select];
      auto node = idToNode_[select];
      if ( object && node )
      {
        lAuxVec = object->frame.position( );
        node->point( nsol::Vec3f( lAuxVec ));
      }
    }
    _scene->updateModifiedStructure( );
    updateGL( );
  }
}

void Viewer::endManipulation( )
{
  ManipulatedFrameSetConstraint *mfsc =
    ( ManipulatedFrameSetConstraint* )( manipulatedFrame( )->constraint( ));
  mfsc->clearSet( );

  Vec lAuxVec;

  if ( selection_.size ( ) > 0 )
  {
    for ( auto select: selection_ )
    {
      auto object = idToObject_[select];
      auto node = idToNode_[select];
      if ( object && node )
      {
        lAuxVec = object->frame.position( );
        node->point( nsol::Vec3f( lAuxVec ));
      }
    }
    _scene->updateModifiedStructure( );
    _scene->updateModifiedMesh( );
    if ( _autoSaveState )
      saveState( );
    updateGL( );
  }
}

//   S e l e c t i o n   t o o l s

void Viewer::drawSelectionRectangle ( ) const
{
  startScreenCoordinatesSystem ( );
  glDisable ( GL_LIGHTING );
  glEnable ( GL_BLEND );
  glBlendFunc ( GL_ONE, GL_ONE );
  glColor4f ( 0.3, 0.3, 0.3f, 1.0f );
  glBegin ( GL_QUADS );
    glVertex2i ( rectangle_.left ( ), rectangle_.top ( ));
    glVertex2i ( rectangle_.right ( ), rectangle_.top ( ));
    glVertex2i ( rectangle_.right ( ), rectangle_.bottom ( ));
    glVertex2i ( rectangle_.left ( ), rectangle_.bottom ( ));
  glEnd ( );
  glDisable ( GL_BLEND );

  glLineWidth ( 2.0 );
  glColor3f ( 0.2f, 0.2f, 0.2f);
  glBegin ( GL_LINE_LOOP );
    glVertex2i ( rectangle_.left ( ), rectangle_.top ( ));
    glVertex2i ( rectangle_.right ( ), rectangle_.top ( ));
    glVertex2i ( rectangle_.right ( ), rectangle_.bottom ( ));
    glVertex2i ( rectangle_.left ( ), rectangle_.bottom ( ));
  glEnd ( );
  glEnable ( GL_LIGHTING );
  stopScreenCoordinatesSystem ( );
}

void Viewer::setRenderOriginalMorphology ( bool renderOriginalMorphology_ )
{
  _renderOriginalMorphology = renderOriginalMorphology_;
  updateGL ( );
};

void Viewer::setRenderModifiedMorphology ( bool renderModifiedMorphology_ )
{
  _renderModifiedMorphology = renderModifiedMorphology_;
  updateGL ( );
};

void Viewer::middlePosition ( )
{
  // if ( selection_.size ( ) > 0 )
  // {
  //   nsol::Vec3f initialPoint ( 0, 0, 0 );
  //   nsol::Vec3f endPoint ( 0, 0, 0 );

  //   for ( QList < int >::const_iterator it = selection_.begin ( ),
  //           end = selection_.end ( ); it != end; ++it )
  //   {
  //     for ( auto neurite: modifiedMorphology->neurites ( ))
  //     {
  //       for ( auto section: neurite->sections ( ))
  //       {
  //         nsol::Nodes *nodes = &section->nodes ( );
  //         for ( unsigned int i = 0; i < nodes->size ( ); ++i )
  //         {
  //           if ((( *nodes )[i]->id ( ) + 1 ) == objects_[*it]->id )
  //           {
  //             initialPoint = ( *nodes )[i]->point ( );
  //           }
  //           //### Problem with non consecutive nodes.
  //           if ( (i > 0) && ( (( *nodes )[i]->id ( ) - 1 ) == objects_[*it]->id ) )
  //           {
  //             endPoint = ( *nodes )[i]->point ( );
  //             ( *nodes )[i - 1]->point (( initialPoint + endPoint )/2.0 );
  //             Vec objPos (( *nodes )[i - 1]->point ( ).x ( ),
  //                         ( *nodes )[i - 1]->point ( ).y ( ),
  //                         ( *nodes )[i - 1]->point ( ).z ( )
  //             );

  //             objects_[*it]->frame.setPosition ( objPos );
  //           }
  //         }
  //       }
  //     }
  //   }
  //   if ( _autoSaveState ) saveState ( );
  // }
  // updateGL ( );
}

void Viewer::ExtractNeurite ( QString // pFile
  )
{
//   if ( selection_.size ( ) > 0 )
//   {
//     for ( QList < int >::const_iterator it = selection_.begin ( ),
//             end = selection_.end ( ); it != end; ++it )
//     {
// //			if ( (*it>2) && (!mOriginalGraph->getNodeInfo(*it).isTermination) )
// //			{
// //				mOriginalGraph->ExtractNeurite(*it, pFile.toStdString());
// //			}
//       ( void ) pFile;
//     }
//     updateGL ( );
// }
}

void Viewer::enhance ( std::map < std::string, float > &optParams,
                       int objectId, OBJECT_TYPE objectType )
{
  util->getInstance()->Enhance ( modifiedMorphology, optParams, objectId, objectType );
  createSelectorsAndStructure( modifiedMorphology );
  _scene->updateModifiedStructure( );
  _scene->updateModifiedMesh( );
  if ( _autoSaveState ) saveState ( );
  updateGL ( );
}

void Viewer::setRadiusToSelectedNode ( float // lRadius
  )
{
  // if ( selection_.size ( ) > 0 )
  // {
  //   //### Hard traverse
  //   for ( auto neurite: modifiedMorphology->neurites ( ))
  //   {
  //     for ( auto section: neurite->sections ( ))
  //     {
  //       for ( auto node: section->nodes ( ))
  //       {
  //         for ( QList < int >::const_iterator it = selection_.begin ( ),
  //                 end = selection_.end ( ); it != end; ++it )
  //         {
  //           if (( *it > 0 ) && ( node->id ( ) == objects_[*it]->id ))
  //             node->radius ( lRadius );
  //         }
  //       }
  //     }
  //   }
  //   if ( _autoSaveState ) saveState ( );
  //   updateGL ( );
  // }
}

void Viewer::brokeLink ( )
{
  // if ( selection_.size ( ) > 0 )
  // {
  //   for ( QList < int >::const_iterator it = selection_.begin ( ),
  //           end = selection_.end ( ); it != end; ++it )
  //   {
  //     if (( *it >= 2 ))
  //     {
  //       //mOriginalGraph->brokeLink(*it);
  //     }
  //   }
  //   updateGL ( );
  // }
}

void Viewer::setLink ( )
{
  // if ( selection_.size ( ) == 2 )
  // {
  //   QList < int >::const_iterator it = selection_.begin ( );
  //   ++it;
  //   if (( *it >= 2 ))
  //   {
  //     //mOriginalGraph->setRadiusToSelectedNode(*it, lRadius);
  //     //###mOriginalGraph->setLink(*it,*it2);
  //   }
  //   updateGL ( );
  // }
}

void Viewer::changeToDualView ( )
{
  mSplitScreen = !mSplitScreen;

  if ( !mSplitScreen )
    camera( )->setScreenWidthAndHeight( width( ), height( ));
  else
    camera( )->setScreenWidthAndHeight( width( )/2, height( ));

  updateGL ( );
}

void Viewer::selectDendrite ( unsigned int dendriteId_ )
{
  unsigned int currentDendId = 0;

  for ( auto neurite: modifiedMorphology->neurites( ))
  {
    if ( currentDendId == dendriteId_ )
      for ( auto section: neurite->sections( ))
        for ( auto node: section->nodes( ))
          selection_.insert( node->id( ));
    ++currentDendId;
  }
  updateGL ( );
}

void Viewer::selectSection ( unsigned int sectionId_ )
{
  unsigned int currentSectionId = 0;
  for ( auto neurite: modifiedMorphology->neurites ( ))
    for ( auto section: neurite->sections ( ))
    {
      if ( currentSectionId  == sectionId_ )
        for ( auto node: section->nodes ( ))
          selection_.insert( node->id( ));
      ++currentSectionId;
    }
  updateGL ( );
}

void Viewer::selectNode ( unsigned int nodeId_ )
{
  selection_.insert( nodeId_ );
  updateGL ( );
}

void Viewer::saveState ( )
{
  _stack.push ( modifiedMorphology->clone ( ));
}

void Viewer::setAutoSaveState ( bool autoSaveState_ )
{
  _autoSaveState = autoSaveState_;
}

void Viewer::undoState ( )
{
  //Always store the original morphology
  if ( _stack.size ( ) > 1 )
  {
    //### Memory analyze after pop
    //delete modifiedMorphology;
    _stack.pop ( );
    modifiedMorphology = _stack.top ( );
    createSelectorsAndStructure( modifiedMorphology );
    _scene->modifiedMorphology( modifiedMorphology );
    _scene->setSelection( selection_ );
    updateGL ( );
  }
}

void Viewer::adjustSelecRegion( int windowWidth_, int windowHeight_,
                                QPoint& regionCenter_, int& regionWidth_,
                                int& regionHeight_ )
{
  int minWidth = regionCenter_.x( ) - regionWidth_ * 0.5;
  int maxWidth = regionCenter_.x( ) + regionWidth_ * 0.5;
  int minHeight = regionCenter_.y( ) - regionHeight_ * 0.5;
  int maxHeight = regionCenter_.y( ) + regionHeight_ * 0.5;

  minWidth = std::max( 0, std::min( minWidth, windowWidth_ ));
  maxWidth = std::max( 0, std::min( maxWidth, windowWidth_ ));
  minHeight = std::max( 0, std::min( minHeight, windowHeight_ ));
  maxHeight = std::max( 0, std::min( maxHeight, windowHeight_ ));

  regionWidth_ = maxWidth - minWidth;
  regionHeight_ = maxHeight - minHeight;

  regionCenter_.setX( regionWidth_ * 0.5f + minWidth );
  regionCenter_.setY( regionHeight_ * 0.5f + minHeight );
}
