/**
 * Copyright (c) 2015-2017 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
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

#include "Scene.h"

#ifdef Darwin
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <nlgenerator/nlgenerator.h>

Scene::Scene( unsigned int defaultFbo_, unsigned int width_,
                unsigned int height_ )
  : _defaultFbo( defaultFbo_ )
  , _width( width_ )
  , _height( height_ )
  , _alpha( 0.5f )
  , _originalMorphology( nullptr )
  , _modifiedMorphology( nullptr )
  , _originalStructure( nullptr )
  , _modifiedStructure( nullptr )
  , _originalMesh( nullptr )
  , _modifiedMesh( nullptr )
{
  _renderer = new nlrender::Renderer( true );
  _renderer->initTransparencySystem( _width, _height );

  _format.resize( 3 );
  _format[0] = nlgeometry::TAttribType::POSITION;
  _format[1] = nlgeometry::TAttribType::COLOR;
  _format[2] = nlgeometry::TAttribType::CENTER;

  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );
}


Scene::~Scene( void )
{
  delete _renderer;
  delete _originalMorphology;
  delete _modifiedMorphology;
  delete _originalStructure;
  delete _modifiedStructure;
  delete _originalMesh;
  delete _modifiedMesh;
}

void Scene::render( bool renderModifiedStructure_, bool renderModifiedMesh_,
                    bool renderOriginalStructure_, bool renderOriginalMesh_ )
{
  auto model = Eigen::Matrix4f::Identity( );

  _renderer->setUpOpaqueTransparencyScene( Eigen::Vector3f( 1.0f, 1.0f, 1.0f ),
                                           _width, _height );
  if ( _originalStructure && renderOriginalStructure_ )
  {
    _renderer->colorFunc( ) = nlrender::Renderer::GLOBAL;
    _renderer->render( _originalStructure, model );
  }
  if ( _modifiedStructure && renderModifiedStructure_ )
  {
    _renderer->colorFunc( ) = nlrender::Renderer::PERVERTEX;
    _renderer->render( _modifiedStructure, model );
  }

  if ( _alpha < 1.0f )
  {
    _renderer->setUpTransparentTransparencyScene( );
  }
  if ( _originalMesh && renderOriginalMesh_ )
  {
    _renderer->colorFunc( ) = nlrender::Renderer::GLOBAL;
    _renderer->render( _originalMesh, model );
  }
  if ( _modifiedMesh && renderModifiedMesh_ )
  {
    _renderer->colorFunc( ) = nlrender::Renderer::GLOBAL;
    _renderer->render( _modifiedMesh, model );
  }


  if ( _alpha >= 1.0f )
  {
    _renderer->setUpTransparentTransparencyScene( );
  }
  _renderer->composeTransparencyScene( _defaultFbo );

  glUseProgram( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );
}

void Scene::size( unsigned int width_, unsigned int height_ )
{
  _width = width_;
  _height = height_;
}

void Scene::alpha( float alpha_ )
{
  _alpha = alpha_;
  _renderer->alpha( ) = _alpha;
}

void Scene::viewMatrix( Eigen::Matrix4f& view_ )
{
  _renderer->viewMatrix( ) = view_;
}

void Scene::projectionMatrix( Eigen::Matrix4f& projection_ )
{
  _renderer->projectionMatrix( ) = projection_;
}

void Scene::originalMorphology( nsol::NeuronMorphologyPtr originalMorphology_ )
{
  _originalMorphology = originalMorphology_;

  if ( _originalStructure )
    delete _originalStructure;
  _originalStructure = nlgenerator::MeshGenerator::generateStructureMesh(
    _originalMorphology,  _nodeIdToVertices,
    Eigen::Vector3f( 0.2f, 0.2f, 0.0f ), true, 0.98f );
  _originalStructure->uploadGPU( _format, nlgeometry::Facet::PATCHES );
  _originalStructure->computeBoundingBox( );
  _aabb = _originalStructure->aaBoundingBox( );

  if ( _originalMesh )
    delete _originalMesh;
  _originalMesh =
    nlgenerator::MeshGenerator::generateMesh( _originalMorphology );
  _originalMesh->uploadGPU( _format, nlgeometry::Facet::PATCHES );
}

void Scene::modifiedMorphology( nsol::NeuronMorphologyPtr modifiedMorphology_ )
{
  _modifiedMorphology = modifiedMorphology_;
  if ( _modifiedStructure )
  {
    delete _modifiedStructure;
      _nodeIdToVertices.clear( );
      _nodeIdToVerticesIds.clear( );
  }
  _modifiedStructure = nlgenerator::MeshGenerator::generateStructureMesh(
    _modifiedMorphology,  _nodeIdToVertices,
    Eigen::Vector3f( 0.0f, 0.2f, 0.2f ), true, 0.98f );
  _modifiedStructure->uploadGPU( _format, nlgeometry::Facet::PATCHES );
  nlgenerator::MeshGenerator::verticesToIndices( _nodeIdToVertices,
                                                 _nodeIdToVerticesIds );
  if ( _modifiedMesh )
    delete _modifiedMesh;
  _modifiedMesh =
    nlgenerator::MeshGenerator::generateMesh( _modifiedMorphology );
  _modifiedMesh->uploadGPU( _format, nlgeometry::Facet::PATCHES );
}

void Scene::updateModifiedStructure( void )
{
  if ( _modifiedMorphology )
  {
    if ( _modifiedStructure )
    {
      delete _modifiedStructure;
      _nodeIdToVertices.clear( );
      _nodeIdToVerticesIds.clear( );
    }
    _modifiedStructure = nlgenerator::MeshGenerator::generateStructureMesh(
      _modifiedMorphology,  _nodeIdToVertices,
      Eigen::Vector3f( 0.0f, 0.2f, 0.2f ), true, 0.98f );
    _modifiedStructure->uploadGPU( _format, nlgeometry::Facet::PATCHES );
    nlgenerator::MeshGenerator::verticesToIndices( _nodeIdToVertices,
                                                   _nodeIdToVerticesIds );
    updateSelectedNodes( );
  }
}

void Scene::updateModifiedMesh( void )
{
  if ( _modifiedMorphology )
  {
    _modifiedMorphology->soma( )->computeParams( );
    if ( _modifiedMesh )
      delete _modifiedMesh;
    _modifiedMesh =
      nlgenerator::MeshGenerator::generateMesh( _modifiedMorphology );
    _modifiedMesh->uploadGPU( _format, nlgeometry::Facet::PATCHES );
  }
}

void Scene::setSelection( const std::unordered_set< int >& selectedNodes_ )
{
  _selection.clear( );
  _selection.insert( _selection.end( ), selectedNodes_.begin( ),
                     selectedNodes_.end( ));
  updateSelectedNodes( );
}

void Scene::updateSelectedNodes( const Eigen::Vector3f& unselectedColor_,
                                 const Eigen::Vector3f& selectedColor_ )
{
  if ( _modifiedStructure )
  {
    unsigned int verticesSize = _modifiedStructure->verticesSize( );
    std::vector< float > buffer( verticesSize * 3 );
    for ( unsigned int i = 0; i < verticesSize; i++ )
    {
      buffer[i*3] = unselectedColor_.x( );
      buffer[i*3+1] = unselectedColor_.y( );
      buffer[i*3+2] = unselectedColor_.z( );
    }
    nlgenerator::MeshGenerator::conformBuffer( _selection, _nodeIdToVerticesIds,
                                               buffer, selectedColor_ );
    _modifiedStructure->uploadBuffer( nlgeometry::COLOR, buffer );
  }
}
