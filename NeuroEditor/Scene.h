/**
 * Copyright (c) 2020 CCS/GMRV/URJC/UPM.
 *
 * Authors: Juan P. Brito <juanpedro.brito@upm.es>
 *          Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
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

#ifndef __NEUROEDITOR_SCENE__
#define __NEUROEDITOR_SCENE__

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

  void changeBackgroundColor( Eigen::Vector3f color_ );
  void changeOriginalStructureColor( const Eigen::Vector3f& color_ );
  void changeOriginalMeshColor( const Eigen::Vector3f& color_ );
  void changeModifiedStructureColor( const Eigen::Vector3f& color_ );
  void changeModifiedMeshColor( const Eigen::Vector3f& color_ );

  void exportModifiedMesh( const std::string& path_  );

  void originalMeshAlpha( float alpha_ );
  void modifiedMeshAlpha( float alpha_ );

protected:

  void _updateSelectedNodes( void );
  Eigen::Vector3f _convertToSelected( Eigen::Vector3f color_ );

  unsigned int _defaultFbo;

  unsigned int _width;

  unsigned int _height;

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

  Eigen::Vector3f _backgroundColor;
  Eigen::Vector3f _originalStructureColor;
  Eigen::Vector3f _originalMeshColor;
  Eigen::Vector3f _modifiedStructureColor;
  Eigen::Vector3f _modifiedStructureSelectedColor;
  Eigen::Vector3f _modifiedMeshColor;

  float _originalMeshAlpha;
  float _modifiedMeshAlpha;

};

#endif
