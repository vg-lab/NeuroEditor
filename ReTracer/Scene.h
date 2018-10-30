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

#ifndef __RETRACER_SCENE__
#define __RETRACER_SCENE__

#include <nsol/nsol.h>
#include <nlgeometry/nlgeometry.h>
#include <nlgenerator/nlgenerator.h>
#include <nlrender/nlrender.h>

#include <unordered_set>

class Scene
{
public:

  Scene( unsigned int defaultFbo_, unsigned int width_,
         unsigned int height_ );

  ~Scene( void );

  void render( bool renderModifiedStruture_ = false,
               bool renderModifiedMesh_ = false,
               bool renderOriginalStructure_ = false,
               bool renderOriginalMesh_ = false );

  void size( unsigned int width_, unsigned int height_ );

  void alpha( float alpha_ );

  void viewMatrix( Eigen::Matrix4f& view_ );

  void projectionMatrix( Eigen::Matrix4f& projection_ );

  void originalMorphology( nsol::NeuronMorphologyPtr originalMorphology_ );

  void modifiedMorphology( nsol::NeuronMorphologyPtr modifiedMorphology_ );

  void updateModifiedStructure( void );

  void updateModifiedMesh( void );

  void setSelection( const std::unordered_set< int >& selectedNodes_ );

  nlgeometry::AxisAlignedBoundingBox aabb( void ){ return _aabb; }

protected:

  void updateSelectedNodes( const Eigen::Vector3f& unselectedColor_ =
                            Eigen::Vector3f( 0.0f, 0.2f, 0.2f ),
                            const Eigen::Vector3f& selectedColor_ =
                            Eigen::Vector3f( 0.8f, 0.8f, 0.0f ));

  unsigned int _defaultFbo;

  unsigned int _width;

  unsigned int _height;

  float _alpha;

  nlrender::Renderer* _renderer;

  nsol::NeuronMorphologyPtr _originalMorphology;
  nsol::NeuronMorphologyPtr _modifiedMorphology;

  nlgeometry::MeshPtr _originalStructure;
  nlgeometry::MeshPtr _modifiedStructure;

  nlgeometry::MeshPtr _originalMesh;
  nlgeometry::MeshPtr _modifiedMesh;

  nlgenerator::NodeIdToVertices _nodeIdToVertices;
  nlgenerator::NodeIdToVerticesIds _nodeIdToVerticesIds;

  nlgeometry::AttribsFormat _format;

  nlgeometry::AxisAlignedBoundingBox _aabb;

  std::vector< unsigned int > _selection;
};

#endif
