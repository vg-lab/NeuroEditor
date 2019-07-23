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

const float Viewer::_colorFactor = 1 / 255.0f;

Viewer::Viewer ( QWidget *parent )
  : QGLViewer ( parent )
  , _scene( nullptr )
  , originalMorphology( nullptr )
  , modifiedMorphology( nullptr )
  , _selectionMode( NONE )
  , _selectionInclusionMode( INCLUSIVE )
  , _selectionType( NODE )
  , _morphoStructure( nullptr )
{
  _morphologyInfoToShow = 0;

  _ShowText = false;
  mSplitScreen = false;

  _ShowTextIniValue
    = _ShowTextFinValue
    = 0;

  _autoSaveState = true;

  _firstLayout.renderModifiedStructure = true;
  _firstLayout.renderModifiedMesh = true;
  _firstLayout.renderOriginalStructure = false;
  _firstLayout.renderOriginalMesh = false;

  _secondLayout.renderModifiedStructure = false;
  _secondLayout.renderModifiedMesh = false;
  _secondLayout.renderOriginalStructure = true;
  _secondLayout.renderOriginalMesh = true;

  _treeModel = new TreeModel( );

  _somaNeurite = new nsol::Neurite( );
  _somaSection = new nsol::NeuronMorphologySection( );
  _somaNeurite->firstSection( _somaSection );
  _somaSection->neurite( _somaNeurite );
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
  _selectionMode = tSelectionMode::NONE;
  int argc = 0;
  char **argv = nullptr;
  glutInit ( &argc, argv );
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

  if ( _selectionMode != NONE )
    drawSelectionRectangle ( );

  glColor3f ( 0, 0, 0 );
  drawText ( 15, 15, "Selected nodes:" +
             QString::number ( _selection.size ( )));

}
//  S L O T S  F U N C T I O N S
void Viewer::reset ( )
{

}

void Viewer::focusOnSelection( )
{

  if ( _selection.empty( ))
    return;

  Eigen::Vector3f center( 0.0f, 0.0f, 0.0f );

  for ( auto id: _selection )
  {
    auto node = _morphoStructure->idToNode[id];
    if ( node )
      center += node->point( );
  }
  center /= _selection.size( );

  float radius = 0.0f;

  if ( _selection.size( ) == 1 )
  {
    radius = _morphoStructure->idToNode[*_selection.begin( )]->radius( );
    radius *= 50.0f;
  }
  else
  {
    for ( auto id: _selection )
    {
      auto node = _morphoStructure->idToNode[id];
      if ( node )
        radius = std::max( radius, ( node->point( ) - center ).norm( ));
    }
    radius *= 5.0f;
  }

  auto pivot = Vec( center.x( ), center.y( ), center.z( ));
  auto axis = camera( )->viewDirection( );

  camera( )->setPivotPoint( pivot );
  camera( )->setPosition( pivot - axis * radius );

  updateGL( );
}

void Viewer::updateMorphology( void )
{
  _morphoStructure->update( );
  _scene->updateModifiedStructure( );
  delete _treeModel;
  _treeModel = new TreeModel( modifiedMorphology );
  Q_EMIT morphologyChanged( );
  _scene->updateModifiedMesh( );
  checkSelection( );
  _changeSelection( );
  Q_EMIT updateSelectionSignal( _selection );
  updateGL( );
}

void Viewer::updateSelection( std::unordered_set< int > selection_ )
{
  _selection.clear( );
  _selection = selection_;
  _changeSelection( );
  updateGL ( );
}

void Viewer::checkSelection( void )
{
  std::vector< int > selection( _selection.begin( ), _selection.end( ));

  for ( auto id: selection )
  {
    if ( _morphoStructure->idToNode.find( id ) ==
         _morphoStructure->idToNode.end( ))
    {
      _selection.erase( id );
    }
  }
}

void Viewer::updateSelectionInclusionMode(
    tSelectionInclusionMode selectionInclusionMode_ )
{
  _selectionInclusionMode = selectionInclusionMode_;
}

void Viewer::updateSelectionType( tSelectionType selectionType_ )
{
  _selectionType = selectionType_;
}

void Viewer::changeAveragePos( Eigen::Vector3f& pos_ )
{
  if ( _selection.size( ) > 0 )
  {
    saveState( );
    Eigen::Vector3f averageDiff = _averagePosition - pos_;
    _averagePosition = pos_;
    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      auto node = _morphoStructure->idToNode[select];
      if ( object && node )
      {
        Eigen::Vector3f pos = node->point( ) - averageDiff;
        node->point( pos );
        object->frame.setPosition( Vec( pos ));
      }
    }
    _scene->updateModifiedStructure( );
    _scene->updateModifiedMesh( );
    updateGL( );
  }
}

void Viewer::changeRotation( Eigen::Quaternionf& q_ )
{
  if ( _selection.size( ) > 0 )
  {
    saveState( );
    Quaternion goalRotQ( q_.x( ), q_.y( ), q_.z( ), q_.w( ));
    goalRotQ.normalize( );
    Quaternion currentRotQ = manipulatedFrame( )->rotation( );
    Quaternion aplyRotQ = currentRotQ.inverse( ) * goalRotQ;

    manipulatedFrame( )->rotate( aplyRotQ );
    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      auto node = _morphoStructure->idToNode[select];
      if ( object && node )
      {
        node->point( Eigen::Vector3f( object->frame.position( )));
      }
    }
    _scene->updateModifiedStructure( );
    _scene->updateModifiedMesh( );
    updateGL( );
  }
}

void Viewer::changeAverageRadius( float radius_ )
{
  if ( _selection.size( ) > 0 )
  {
    saveState( );
    float averageDiff = _averageRadius - radius_;
    _averageRadius = radius_;
    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      auto node = _morphoStructure->idToNode[select];
      if ( object && node )
      {
        float radius = node->radius( ) - averageDiff;
        node->radius( radius );
        object->radius = radius;
      }
    }
    _scene->updateModifiedStructure( );
    _scene->updateModifiedMesh( );
    updateGL( );
  }
}

QString Viewer::helpString ( ) const
{
  QString text ( "<h2>NeuroEditor: A neuromophological tracings editor</h2>" );
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

void Viewer::loadMorphology ( QString pSWCFile )
{
  if ( originalMorphology )
    delete originalMorphology;
  originalMorphology = swcReader.readMorphology ( pSWCFile.toStdString ( ));
  if ( modifiedMorphology )
    delete modifiedMorphology;
  modifiedMorphology = originalMorphology->clone ( );

  _scene->originalMorphology( originalMorphology );
  _scene->modifiedMorphology( modifiedMorphology );

  auto aabb = _scene->aabb( );

  Vec center( aabb.center( ).x( ), aabb.center( ).y( ),
              aabb.center( ).z( ));
  setSceneCenter( center );
  setSceneRadius( aabb.radius( ));
  camera( )->fitSphere( center, aabb.radius( ));

  if ( _morphoStructure )
    _morphoStructure->changeMorphology( modifiedMorphology );
  else
    _morphoStructure =
      new neuroeditor::MorphologyStructure( modifiedMorphology );

  while ( !_stack.empty ( ))
  {
    delete _stack.top( );
    _stack.pop ( );
  }

  delete _treeModel;
  _treeModel = new TreeModel( modifiedMorphology );
  Q_EMIT morphologyChanged( );
  updateGL ( );
}

void Viewer::exportMorphology ( QString pFile )
{
  swcWriter.writeMorphology( pFile.toStdString( ).c_str( ),
                             modifiedMorphology );
}

void Viewer::exportMesh( QString pFile )
{
  _scene->exportModifiedMesh( pFile.toStdString( ));
}

void Viewer::setModificationInterval ( unsigned int pIniValue,
                                       unsigned int pFinalValue )
{
  _procInitialValue = pIniValue;
  _procFinalValue = pFinalValue;
}

void Viewer::setModifiedAsOriginal ( )
{
  originalMorphology = modifiedMorphology->clone ( );
  _scene->originalMorphology( originalMorphology );
  updateGL( );
}

//Manipulated methods
//-------------------
//   C u s t o m i z e d   m o u s e   e v e n t s
void Viewer::mousePressEvent ( QMouseEvent *e )
{
  // Start selection. Mode is ADD with Shift key and TOGGLE with Alt key.
  _rectangle = QRect ( e->pos ( ), e->pos ( ));

  if (( e->button ( ) == Qt::LeftButton )
    && ( e->modifiers ( ) == Qt::ShiftModifier ))
    _selectionMode = ADD;
  else if (( e->button ( ) == Qt::RightButton )
    && ( e->modifiers ( ) == Qt::ShiftModifier ))
    _selectionMode = REMOVE;
  else
  {
    if ( e->modifiers ( ) == Qt::ControlModifier )
      startManipulation ( );
    QGLViewer::mousePressEvent ( e );
  }
}

void Viewer::mouseMoveEvent ( QMouseEvent *e )
{
  if ( _selectionMode != NONE )
  {
    _rectangle.setBottomRight ( e->pos ( ) );
    updateGL ( );
  }
  else
    if ( e->modifiers ( ) == Qt::ControlModifier )
      whileManipulation( );

  float radius = (camera( )->position( ) - camera( )->pivotPoint( )).norm( );

  QGLViewer::mouseMoveEvent ( e );

  auto axis = camera( )->viewDirection( );
  axis.normalize( );
  axis *= radius;
  auto pivot  =  camera( )->position( ) + axis;
  camera( )->setPivotPoint( pivot );
}

void Viewer::mouseReleaseEvent ( QMouseEvent *e )
{
  if ( _selectionMode != NONE )
  {
    _rectangle = _rectangle.normalized ( );
    QPoint center = _rectangle.center ( );
    int selectWidth = _rectangle.width( );
    int selectHeight = _rectangle.height( );

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

    _selectionMode = NONE;
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
    _selection.clear ( );
    _changeSelection( );
    Q_EMIT updateSelectionSignal( _selection );
  }
  else
    QGLViewer::keyPressEvent ( e );
  updateGL ( );
}

//   C u s t o m i z e d   s e l e c t i o n   p r o c e s s
void Viewer::drawWithNames ( )
{
  for ( int i = 0; i < _morphoStructure->objects.size( ); i++ )
  {
    glPushName( i );
    _morphoStructure->objects[i]->draw( );
    glPopName( );
  }
}

void Viewer::endSelection ( const QPoint & )
{
  glFlush ( );

  GLint nbHits = glRenderMode ( GL_RENDER );

  if ( nbHits > 0 )
  {
    if ( _selectionMode == ADD && _selectionInclusionMode == EXCLUSIVE )
      _selection.clear( );
    for ( int i = 0; i < nbHits; ++i )
    {
      std::vector< int > ids;
      int objectId = selectBuffer( )[4*i + 3];
      int nodeId = _morphoStructure->objects[ objectId ]->id;
      nsol::NodePtr node = _morphoStructure->idToNode[nodeId];
      switch ( _selectionType )
      {
      case NODE:
        ids.push_back( nodeId );
        break;
      case SECTION:
        for ( auto cNode: _morphoStructure->nodeToSection[node]->nodes( ))
          ids.push_back( cNode->id( ));
        break;
      case NEURITE:
        auto section = dynamic_cast< nsol::NeuronMorphologySectionPtr >(
          _morphoStructure->nodeToSection[node] );
        for ( auto cSection: section->neurite( )->sections( ))
          for ( auto cNode: cSection->nodes( ))
            ids.push_back( cNode->id( ));
        break;
      }
      switch ( _selectionMode )
      {
      case ADD:
        for ( auto id: ids )
          _selection.insert( id );
        break;
      case REMOVE:
        for ( auto id: ids )
          _selection.erase( id );
        break;
      default:
        break;
      }
    }
  }
  _changeSelection( );
  Q_EMIT updateSelectionSignal( _selection );
}

void Viewer::startManipulation( )
{
  if ( _selection.size( ) > 0 )
  {
    saveState( );
    _averagePosition = Eigen::Vector3f( 0.0f, 0.0f, 0.0f );

    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      _averagePosition += Eigen::Vector3f( object->frame.position( ));
    }
    _averagePosition /= _selection.size( );
    manipulatedFrame( )->setPosition( Vec( _averagePosition ));
  }
}

void Viewer::whileManipulation( )
{
  Vec lAuxVec;

  if ( _selection.size ( ) > 0 )
  {
    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      auto node = _morphoStructure->idToNode[select];
      if ( object && node )
      {
        lAuxVec = object->frame.position( );
        node->point( nsol::Vec3f( lAuxVec ));
      }
    }
    _scene->updateModifiedStructure( );
    Eigen::Vector3f aPos = Eigen::Vector3f( manipulatedFrame( )->position( ));
    auto q = manipulatedFrame( )->rotation( );
    auto v = q.axis( );
    Eigen::Quaternionf quat(
      Eigen::AngleAxisf( q.angle( ), Eigen::Vector3f( v.x, v.y, v.z )));

    Q_EMIT updateAveragePosSignal( aPos );
    Q_EMIT updateRotationSignal( quat );
    updateGL( );
  }
}

void Viewer::endManipulation( )
{
  Vec lAuxVec;
  if ( _selection.size ( ) > 0 )
  {
    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      auto node = _morphoStructure->idToNode[select];
      if ( object && node )
      {
        lAuxVec = object->frame.position( );
        node->point( nsol::Vec3f( lAuxVec ));
      }
    }
    _scene->updateModifiedStructure( );
    _scene->updateModifiedMesh( );

    _averagePosition = Eigen::Vector3f( manipulatedFrame( )->position( ));
    auto q = manipulatedFrame( )->rotation( );
    auto v = q.axis( );
    Eigen::Quaternionf quat(
      Eigen::AngleAxisf( q.angle( ), Eigen::Vector3f( v.x, v.y, v.z )));

    Q_EMIT updateAveragePosSignal( _averagePosition );
    Q_EMIT updateRotationSignal( quat );
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
    glVertex2i ( _rectangle.left ( ), _rectangle.top ( ));
    glVertex2i ( _rectangle.right ( ), _rectangle.top ( ));
    glVertex2i ( _rectangle.right ( ), _rectangle.bottom ( ));
    glVertex2i ( _rectangle.left ( ), _rectangle.bottom ( ));
  glEnd ( );
  glDisable ( GL_BLEND );

  glLineWidth ( 2.0 );
  glColor3f ( 0.2f, 0.2f, 0.2f);
  glBegin ( GL_LINE_LOOP );
    glVertex2i ( _rectangle.left ( ), _rectangle.top ( ));
    glVertex2i ( _rectangle.right ( ), _rectangle.top ( ));
    glVertex2i ( _rectangle.right ( ), _rectangle.bottom ( ));
    glVertex2i ( _rectangle.left ( ), _rectangle.bottom ( ));
  glEnd ( );
  glEnable ( GL_LIGHTING );
  stopScreenCoordinatesSystem ( );
}

void Viewer::updateSideBySide( bool sideBySideState_ )
{
  mSplitScreen = sideBySideState_;

  if ( !mSplitScreen )
    camera( )->setScreenWidthAndHeight( width( ), height( ));
  else
    camera( )->setScreenWidthAndHeight( width( )/2, height( ));

  updateGL( );
}

void Viewer::updateFLROriginalStructure( bool state_ )
{
  _firstLayout.renderOriginalStructure = state_;
  updateGL( );
}

void Viewer::updateFLROriginalMesh( bool state_ )
{
  _firstLayout.renderOriginalMesh = state_;
  updateGL( );
}

void Viewer::updateFLRModifiedStructure( bool state_ )
{
  _firstLayout.renderModifiedStructure = state_;
  updateGL( );
}

void Viewer::updateFLRModifiedMesh( bool state_ )
{
  _firstLayout.renderModifiedMesh = state_;
  updateGL( );
}


void Viewer::updateSLROriginalStructure( bool state_ )
{
  _secondLayout.renderOriginalStructure = state_;
  updateGL( );
}

void Viewer::updateSLROriginalMesh( bool state_ )
{
  _secondLayout.renderOriginalMesh = state_;
  updateGL( );
}

void Viewer::updateSLRModifiedStructure( bool state_ )
{
  _secondLayout.renderModifiedStructure = state_;
  updateGL( );
}

void Viewer::updateSLRModifiedMesh( bool state_ )
{
  _secondLayout.renderModifiedMesh = state_;
  updateGL( );
}

void Viewer::changeBackgroundColor( QColor color_ )
{
  _scene->changeBackgroundColor(
    Eigen::Vector3f( float( color_.red( )) * _colorFactor,
                     float( color_.green( )) * _colorFactor,
                     float( color_.blue( )) * _colorFactor ));
  updateGL( );
}

void Viewer::changeOriginalStructureColor( QColor color_ )
{
  _scene->changeOriginalStructureColor(
    Eigen::Vector3f( float( color_.red( )) * _colorFactor,
                     float( color_.green( )) * _colorFactor,
                     float( color_.blue( )) * _colorFactor ));
  updateGL( );
}

void Viewer::changeOriginalMeshColor( QColor color_ )
{
  _scene->changeOriginalMeshColor(
    Eigen::Vector3f( float( color_.red( )) * _colorFactor,
                     float( color_.green( )) * _colorFactor,
                     float( color_.blue( )) * _colorFactor ));
  updateGL( );
}

void Viewer::changeModifiedStructureColor( QColor color_ )
{
  _scene->changeModifiedStructureColor(
    Eigen::Vector3f( float( color_.red( )) * _colorFactor,
                     float( color_.green( )) * _colorFactor,
                     float( color_.blue( )) * _colorFactor ));
  updateGL( );
}

void Viewer::changeModifiedMeshColor( QColor color_ )
{
  _scene->changeModifiedMeshColor(
    Eigen::Vector3f( float( color_.red( )) * _colorFactor,
                     float( color_.green( )) * _colorFactor,
                     float( color_.blue( )) * _colorFactor ));
  updateGL( );
}

void Viewer::setCameraViewFront( void )
{
  auto pivot = camera( )->pivotPoint( );
  auto axis = Vec( 0.0f, 0.0f, -1.0f );
  float radius = (camera( )->position( ) - pivot).norm( );

  camera( )->setPosition( pivot - axis * radius );
  camera( )->setOrientation( Quaternion( Vec( 0.0, 1.0, 0.0 ), 0.0 ));
  updateGL( );
}
void Viewer::setCameraViewTop( void )
{
  auto pivot = camera( )->pivotPoint( );
  auto axis = Vec( 0.0f, -1.0f, 0.0f );
  float radius = (camera( )->position( ) - pivot).norm( );

  camera( )->setPosition( pivot - axis * radius );
  camera( )->setOrientation( Quaternion( Vec( -1.0, 0.0, 0.0 ),  3.1416f/2.0f));
  updateGL( );
}
void Viewer::setCameraViewBottom( void )
{
  auto pivot = camera( )->pivotPoint( );
  auto axis = Vec( 0.0f, 1.0f, 0.0f );
  float radius = (camera( )->position( ) - pivot).norm( );

  camera( )->setPosition( pivot - axis * radius );
  camera( )->setOrientation( Quaternion( Vec( 1.0, 0.0, 0.0 ),  3.1416f/2.0f));
  updateGL( );
}
void Viewer::setCameraViewLeft( void )
{
  auto pivot = camera( )->pivotPoint( );
  auto axis = Vec( 1.0f, 0.0f, 0.0f );
  float radius = (camera( )->position( ) - pivot).norm( );

  camera( )->setPosition( pivot - axis * radius );
  camera( )->setOrientation( Quaternion( Vec( 0.0, -1.0, 0.0 ),  3.1416f/2.0f));
  updateGL( );
}
void Viewer::setCameraViewRight( void )
{
  auto pivot = camera( )->pivotPoint( );
  auto axis = Vec( -1.0f, 0.0f, 0.0f );
  float radius = (camera( )->position( ) - pivot).norm( );

  camera( )->setPosition( pivot - axis * radius );
  camera( )->setOrientation( Quaternion( Vec( 0.0, 1.0, 0.0 ),  3.1416f/2.0f));
  updateGL( );
}

void Viewer::selectDendrite ( unsigned int dendriteId_ )
{
  unsigned int currentDendId = 0;

  for ( auto neurite: modifiedMorphology->neurites( ))
  {
    if ( currentDendId == dendriteId_ )
      for ( auto section: neurite->sections( ))
        for ( auto node: section->nodes( ))
          _selection.insert( node->id( ));
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
          _selection.insert( node->id( ));
      ++currentSectionId;
    }
  updateGL ( );
}

void Viewer::selectNode ( unsigned int nodeId_ )
{
  _selection.insert( nodeId_ );
  updateGL ( );
}

void Viewer::saveState(  )
{
  if ( _autoSaveState )
    _stack.push ( modifiedMorphology->clone(  ));
}

void Viewer::setAutoSaveState ( bool autoSaveState_ )
{
  _autoSaveState = autoSaveState_;
}

void Viewer::undoState ( )
{
  //Always store the original morphology
  if ( _stack.size ( ) > 0 )
  {
    delete modifiedMorphology;
    modifiedMorphology = _stack.top( );
    _stack.pop( );
    _scene->modifiedMorphology( modifiedMorphology );
    _morphoStructure->changeMorphology( modifiedMorphology );
    delete _treeModel;
    _treeModel = new TreeModel( modifiedMorphology );
    Q_EMIT morphologyChanged( );
    checkSelection( );
    _changeSelection( );
    Q_EMIT updateSelectionSignal( _selection );
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

void Viewer::_changeSelection( void )
{
  ManipulatedFrameSetConstraint *mfsc =
    ( ManipulatedFrameSetConstraint* )( manipulatedFrame( )->constraint( ));
  mfsc->clearSet( );
  if ( _selection.size( ) <= 0 )
    Q_EMIT resetInspectorSignal( );
  else
  {
    _averagePosition = Eigen::Vector3f( 0.0f, 0.0f, 0.0f );
    _averageRadius = 0.0f;

    for ( auto select: _selection )
    {
      auto object = _morphoStructure->idToObject[select];
      auto node = _morphoStructure->idToNode[select];
      mfsc->addObjectToSet( object );
      _averagePosition += Eigen::Vector3f( object->frame.position( ));
      _averageRadius += node->radius( );
    }
    _averagePosition /= _selection.size( );
    manipulatedFrame( )->setPosition( Vec( _averagePosition ));
    _averageRadius /= _selection.size( );
    Quaternion q;
    manipulatedFrame( )->setOrientation( q );
    auto axis = q.axis( );
    Eigen::Quaternionf quat( q.angle( ), axis.x, axis.y, axis.z );

    Q_EMIT updateAveragePosSignal( _averagePosition );
    Q_EMIT updateRotationSignal( quat );
    Q_EMIT updateAverageRadiusSignal( _averageRadius );
  }
  _scene->setSelection( _selection );
}
