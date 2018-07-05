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

using namespace std;

Viewer::Viewer ( QWidget *parent )
  : QGLViewer ( parent )
{
  selectionMode_ = NONE;

  originalMorphologyRenderer = nullptr;
  modifiedMorphologyRenderer = nullptr;

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
}

Viewer::~Viewer ( )
{
  if ( originalMorphologyRenderer != nullptr )
    delete originalMorphologyRenderer;
  if ( modifiedMorphologyRenderer != nullptr )
    delete modifiedMorphologyRenderer;
}

void Viewer::init ( )
{
  //Configure the camera
  camera ( )->setZClippingCoefficient ( 500.0 );
  camera ( )->setZNearCoefficient ( 0.0001 );

  setBackgroundColor ( QColor ( 255, 255, 255 ));

  setManipulatedFrame ( new ManipulatedFrame ( ));
  manipulatedFrame ( )->setConstraint ( new ManipulatedFrameSetConstraint ( ));

  glBlendFunc ( GL_ONE, GL_ONE );
}

void Viewer::setupViewer ( )
{
  camera ( )->setZClippingCoefficient ( 500.0 );
  selectionMode_ = SelectionMode::NONE;
  int argc = 0;
  char **argv = nullptr;
  glutInit ( &argc, argv );
  updateGL ( );
}

void Viewer::animate ( )
{
}

//  D r a w i n g   f u n c t i o n
void Viewer::draw ( )
{
#if QT_VERSION < 0x040000
  for (QValueList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
#endif

  //Place light at camera position
  const qglviewer::Vec cameraPos = camera ( )->position ( );
  const GLfloat pos[4] =
    { ( float ) cameraPos[0], ( float ) cameraPos[1], ( float ) cameraPos[2],
      1.0 };
  glLightfv ( GL_LIGHT1, GL_POSITION, pos );

  // Orientate light along view direction
  glLightfv ( GL_LIGHT1, GL_SPOT_DIRECTION, camera ( )->viewDirection ( ));

  if ( !mSplitScreen )
  {
    if (( _renderOriginalMorphology )
      && ( originalMorphologyRenderer != nullptr ))
    {
      originalMorphologyRenderer->renderMorphology ( );
      if (( _morphologyInfoToShow == 1 ) || ( _morphologyInfoToShow == 3 ))
        renderMorphologyInfo ( originalMorphology );
    }
    if (( _renderModifiedMorphology )
      && ( modifiedMorphologyRenderer != nullptr ))
    {
      modifiedMorphologyRenderer->renderMorphology ( );
      if (( _morphologyInfoToShow == 2 ) || ( _morphologyInfoToShow == 3 ))
        renderMorphologyInfo ( modifiedMorphology );
    }
  }
  else
  {
    if (( _renderOriginalMorphology )
      && ( originalMorphologyRenderer != nullptr ))
    {
      glViewport ( 0.5*width ( ), 0, 0.5*width ( ), height ( ));
      originalMorphologyRenderer->renderMorphology ( );
      if (( _morphologyInfoToShow == 2 ) || ( _morphologyInfoToShow == 3 ))
        renderMorphologyInfo ( originalMorphology );
    }

    if (( _renderModifiedMorphology )
      && ( modifiedMorphologyRenderer != nullptr ))
    {
      glViewport ( 0, 0, 0.5*width ( ), height ( ));
      modifiedMorphologyRenderer->renderMorphology ( );
      if (( _morphologyInfoToShow == 1 ) || ( _morphologyInfoToShow == 3 ))
        renderMorphologyInfo ( modifiedMorphology );
    }
  }

  glEnable ( GL_LIGHTING );

  for ( QList < int >::const_iterator it = selection_.begin ( ),
          end = selection_.end ( ); it != end; ++it )
    objects_.at ( *it )->draw ( );

  if ( manipulatedFrame ( )->isManipulated ( ))
  {
    glPushMatrix ( );
    glMultMatrixd ( manipulatedFrame ( )->matrix ( ));
    drawAxis ( 25 );
    glPopMatrix ( );
  }

  //restore viewport
  if ( mSplitScreen ) glViewport ( 0, 0, width ( ), height ( ));

  if ( selectionMode_ != NONE )
    drawSelectionRectangle ( );

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

void Viewer::renderMorphologyInfo ( const nsol::NeuronMorphologyPtr &morphology_ )
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
            if (mSplitScreen) proj.x = proj.x/2.0;
            drawText ( proj.x, proj.y, QString::number ( node->id ( )));
          }
        }
      }
    }
    stopScreenCoordinatesSystem ( );
  }
}

void Viewer::createSelectors ( const nsol::NeuronMorphologyPtr &morphology_ )
{
  selectionMode_ = NONE;
  objects_.clear ( );
  selection_.clear ( );
  Object *o = nullptr;

  //Soma
  unsigned int  numSomaNodes = morphology_->soma ()->nodes ().size ();
  nsol::Vec3f somaPos = morphology_->soma ( )->center ( );
  float radius = morphology_->soma ( )->minRadius ( );
  for (unsigned int i=0;i<numSomaNodes;++i)
  {
    o = new Object ( );
    o->frame.setPosition ( somaPos.x ( ),
                           somaPos.y ( ),
                           somaPos.z ( )
    );
    o->id = i;
    o->radius = radius + 0.2f;
    objects_.append ( o );
  }

  //Neurites
  int actNodeId=0;
  for ( auto neurite: morphology_->neurites ( ))
  {
    for ( auto section: neurite->sections ( ))
    {
      for ( auto node: section->nodes ( ))
      {
        if (node->id ( )>actNodeId)
        {
          o = new Object ( );
          o->frame.setPosition ( node->point ( ).x ( ),
                                 node->point ( ).y ( ),
                                 node->point ( ).z ( )
          );
          o->id = node->id ( );
          o->radius = node->radius ( ) + 0.2f;
          objects_.append ( o );
          actNodeId = node->id ( );
        }
      }
    }
  }
}

void Viewer::loadMorphology ( QString pSWCFile )
{
  originalMorphology = swcReader.readMorphology ( pSWCFile.toStdString ( ));
  modifiedMorphology = originalMorphology->clone ( );

  originalMorphologyRenderer = new nsolRenderer ( originalMorphology );
  modifiedMorphologyRenderer = new nsolRenderer ( modifiedMorphology );
  modifiedMorphologyRenderer->setNodeColor ( nsol::Vec3f ( 0, 0, 1 ));
  modifiedMorphologyRenderer->setLinkColor ( nsol::Vec3f ( 0, 1, 0 ));

  modifiedMorphologyRenderer->setIncrement ( 0.1 );

  createSelectors ( modifiedMorphology );
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
  swcWriter
    .writeMorphology ( pFile.toStdString ( ).c_str ( ), modifiedMorphology );
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
  retracer::Utils::Simplify ( modifiedMorphology, optParams, objectId, objectType );
  createSelectors ( modifiedMorphology );
  if ( _autoSaveState ) saveState ( );
  updateGL ( );
}

void Viewer::setModifiedAsOriginal ( )
{
  originalMorphology = modifiedMorphology->clone ( );
  originalMorphologyRenderer->setMorphology ( originalMorphology );
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
    if ( _autoSaveState ) saveState ( );
  }
  updateGL ( );
}

void Viewer::attachMorphology ( const nsol::NeuronMorphologyPtr &morphology_ )
{
  //mNeuronGraphOptimizer->copyGraph();
  //mNeuronGraphOptimizer->getOptimizedGraph().attachSWCImporter(pSWCImporter);

  //## development pourposes
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
    // Updates rectangle_ coordinates and redraws rectangle
    rectangle_.setBottomRight ( e->pos ( ) );
    updateGL ( );
  }
  else
    QGLViewer::mouseMoveEvent ( e );
}

void Viewer::mouseReleaseEvent ( QMouseEvent *e )
{
  if ( selectionMode_ != NONE )
  {
    // Actual selection on the rectangular area.
    // Possibly swap left/right and top/bottom to make rectangle_ valid.
    rectangle_ = rectangle_.normalized ( );

    // Define selection window dimensions
    setSelectRegionWidth ( rectangle_.width ( ));
    setSelectRegionHeight ( rectangle_.height ( ));

    // Compute rectangle center and perform selection
    QPoint center = rectangle_.center ( );
    if (mSplitScreen)
    {
      float desp = ( rectangle_.bottomRight ().x () + rectangle_.bottomLeft().x( ) ) /2.0;
      center.setX ( rectangle_.center ( ).x ( ) + desp );
    }

    select ( center );
    // Update display to show new selected objects
    updateGL ( );
  }
  else
  {
    if ( e->modifiers ( ) == Qt::ControlModifier )
      startManipulation ( );

    QGLViewer::mouseReleaseEvent ( e );
  }
}

void Viewer::keyPressEvent ( QKeyEvent *e )
{
  if ( e->key ( ) == Qt::Key_Escape )
  {
    //std::cout<<"You pressed ESC"<<std::endl;
    selection_.clear ( );
  }
  else
    QGLViewer::keyPressEvent ( e );
  updateGL ( );
}

//   C u s t o m i z e d   s e l e c t i o n   p r o c e s s
void Viewer::drawWithNames ( )
{
  for ( int i = 0; i < int ( objects_.size ( )); i++ )
  {
    glPushName ( i );
    objects_.at ( i )->draw ( );
    glPopName ( );
  }
}

void Viewer::endSelection ( const QPoint & )
{
  glFlush ( );
  // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
  GLint nbHits = glRenderMode ( GL_RENDER );
  if ( nbHits > 0 )
  {
    // Interpret results : each object created 4 values in the selectBuffer().
    // (selectBuffer())[4*i+3] is the id pushed on the stack.
    for ( int i = 0; i < nbHits; ++i )
      switch ( selectionMode_ )
      {
        case ADD    : addIdToSelection (( selectBuffer ( ))[4*i + 3] );
          break;
        case REMOVE : removeIdFromSelection (( selectBuffer ( ))[4*i + 3] );
          break;
        default : break;
      }
  }
  selectionMode_ = NONE;
}

void Viewer::startManipulation ( )
{
  Vec averagePosition;
  ManipulatedFrameSetConstraint *mfsc =
    ( ManipulatedFrameSetConstraint * ) ( manipulatedFrame ( )->constraint ( ));
  mfsc->clearSet ( );

  for ( QList < int >::const_iterator it = selection_.begin ( ),
          end = selection_.end ( ); it != end; ++it )
  {
    mfsc->addObjectToSet ( objects_[*it] );
    averagePosition += objects_[*it]->frame.position ( );
  }

  Vec lAuxVec;

  if ( selection_.size ( ) > 0 )
  {
    averagePosition = averagePosition/selection_.size ( );
    manipulatedFrame ( )->setPosition ( averagePosition );

    //### Hard traverse!!! -> Change it by direct indexation
    for ( auto neurite: modifiedMorphology->neurites ( ))
    {
      for ( auto section: neurite->sections ( ))
      {
        for ( auto node: section->nodes ( ))
        {
          for ( QList < int >::const_iterator it = selection_.begin ( ),
                  end = selection_.end ( ); it != end; ++it )
          {
            if ( node->id ( ) == objects_[*it]->id )
            {
              lAuxVec = objects_[*it]->frame.position ( );
              node->point ( nsol::Vec3f ( lAuxVec.x, lAuxVec.y, lAuxVec.z ));
            }

          }
        }
      }
    }
    if ( _autoSaveState ) saveState ( );
  }
}

//   S e l e c t i o n   t o o l s
void Viewer::addIdToSelection ( int id )
{
  if ( !selection_.contains ( id ))
    selection_.push_back ( id );
}

void Viewer::removeIdFromSelection ( int id )
{
  selection_.removeAll ( id );
}

void Viewer::drawSelectionRectangle ( ) const
{
  startScreenCoordinatesSystem ( );
  glDisable ( GL_LIGHTING );
  glEnable ( GL_BLEND );
  glColor4f ( 0.8, 0.8, 0.8f, 1.9f );
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

//## It is not necessary
void Viewer::middlePosition ( )
{
  if ( selection_.size ( ) > 0 )
  {
    nsol::Vec3f initialPoint ( 0, 0, 0 );
    nsol::Vec3f endPoint ( 0, 0, 0 );

    for ( QList < int >::const_iterator it = selection_.begin ( ),
            end = selection_.end ( ); it != end; ++it )
    {
      for ( auto neurite: modifiedMorphology->neurites ( ))
      {
        for ( auto section: neurite->sections ( ))
        {
          nsol::Nodes *nodes = &section->nodes ( );
          for ( unsigned int i = 0; i < nodes->size ( ); ++i )
          {
            if ((( *nodes )[i]->id ( ) + 1 ) == objects_[*it]->id )
            {
              initialPoint = ( *nodes )[i]->point ( );
            }

            if ((( *nodes )[i]->id ( ) - 1 ) == objects_[*it]->id )
            {
              endPoint = ( *nodes )[i]->point ( );
              ( *nodes )[i - 1]->point (( initialPoint + endPoint )/2.0 );
              Vec objPos (( *nodes )[i - 1]->point ( ).x ( ),
                          ( *nodes )[i - 1]->point ( ).y ( ),
                          ( *nodes )[i - 1]->point ( ).z ( )
              );

              objects_[*it]->frame.setPosition ( objPos );
            }
          }
        }
      }
    }
    if ( _autoSaveState ) saveState ( );
  }
  updateGL ( );
}

void Viewer::ExtractNeurite ( QString pFile )
{
  if ( selection_.size ( ) > 0 )
  {
    for ( QList < int >::const_iterator it = selection_.begin ( ),
            end = selection_.end ( ); it != end; ++it )
    {
//			if ( (*it>2) && (!mOriginalGraph->getNodeInfo(*it).isTermination) )
//			{
//				mOriginalGraph->ExtractNeurite(*it, pFile.toStdString());
//			}
      //### avoid warning
      ( void ) pFile;
    }
    updateGL ( );
  }
}

void Viewer::enhance ( std::map < std::string, float > &optParams,
                       int objectId, OBJECT_TYPE objectType )
{
  retracer::Utils::Enhance ( modifiedMorphology, optParams, objectId, objectType );
  createSelectors ( modifiedMorphology );
  if ( _autoSaveState ) saveState ( );
  updateGL ( );
}

void Viewer::setRadiusToSelectedNode ( float lRadius )
{
  if ( selection_.size ( ) > 0 )
  {
    //### Hard traverse
    for ( auto neurite: modifiedMorphology->neurites ( ))
    {
      for ( auto section: neurite->sections ( ))
      {
        for ( auto node: section->nodes ( ))
        {
          for ( QList < int >::const_iterator it = selection_.begin ( ),
                  end = selection_.end ( ); it != end; ++it )
          {
            if (( *it > 0 ) && ( node->id ( ) == objects_[*it]->id ))
              node->radius ( lRadius );
          }
        }
      }
    }
    if ( _autoSaveState ) saveState ( );
    updateGL ( );
  }
}

void Viewer::brokeLink ( )
{
  if ( selection_.size ( ) > 0 )
  {
    for ( QList < int >::const_iterator it = selection_.begin ( ),
            end = selection_.end ( ); it != end; ++it )
    {
      if (( *it >= 2 ))
      {
        //mOriginalGraph->brokeLink(*it);
      }
    }
    updateGL ( );
  }
}

void Viewer::setLink ( )
{
  if ( selection_.size ( ) == 2 )
  {
    QList < int >::const_iterator it = selection_.begin ( );
    //QList<int>::const_iterator it2=selection_.begin();
    ++it;
    if (( *it >= 2 ))
    {
      //mOriginalGraph->setRadiusToSelectedNode(*it, lRadius);
      //###mOriginalGraph->setLink(*it,*it2);
    }
    updateGL ( );
  }
}

void Viewer::changeToDualView ( )
{
  mSplitScreen = !mSplitScreen;
  if ( !mSplitScreen )
    glViewport ( 0, 0, width ( ), height ( ));
  updateGL ( );
}

void Viewer::selectDendrite ( unsigned int dendriteId_ )
{
  selection_.clear ( );
  unsigned int actDendId = 0;
  int actNodeId = 0;

  for ( auto neurite: modifiedMorphology->neurites ( ))
  {
    if ( actDendId == dendriteId_ )
    {
      for ( auto section: neurite->sections ( ))
      {
        for ( auto node: section->nodes ( ))
        {
          //Avoid repetitions of nodes
          if (node->id ( )>actNodeId)
          {
            addIdToSelection ( node->id ( ) - 1);
            actNodeId = node->id ( );
          }
        }
      }
    }
    ++actDendId;
  }
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
    //### Memory needs to be released explicity?
    //delete modifiedMorphology;
    _stack.pop ( );
    modifiedMorphology = _stack.top ( );
    modifiedMorphologyRenderer->setMorphology ( modifiedMorphology );
    createSelectors ( modifiedMorphology );
  }
  updateGL ( );
}
