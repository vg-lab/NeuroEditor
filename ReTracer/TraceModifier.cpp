/*
 * Copyright (c) 2014-2019 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of nsol <https://github.com/gmrvvis/nsol>
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

#include "TraceModifier.h"
#include "imppsimpl.h"

#include <PyGEmS/StrategyParams.h>
#include <PyGEmSManager/PyGEmSManager.h>
#include <PyGEmSManager/StrategyFrameworkBPModule.h>

namespace retracer
{

  bool TraceModifier::modify( std::unordered_set< nsol::Section* >& sections_,
                              TModifierMethod modifierMethod_,
                              TModifierParams modifierParams_ )
  {

    TSimplifierFunc simplifierFunc = nullptr;
    TEnhacerFunc enhacerFunc = nullptr;

    switch( modifierMethod_ )
    {
    case NTHPOINT:
      simplifierFunc = TraceModifier::_nthpoint;
      break;
    case RADIAL:
      simplifierFunc = TraceModifier::_radial;
      break;
    case PERPDIST:
      simplifierFunc = TraceModifier::_perpdist;
      break;
    case REUMANNWITKAM:
      simplifierFunc = TraceModifier::_reumannWitkam;
      break;
    case OPHEIN:
      simplifierFunc = TraceModifier::_ophein;
      break;
    case LANG:
      simplifierFunc = TraceModifier::_lang;
      break;
    case DOUGLASPEUCKER:
      simplifierFunc = TraceModifier::_douglasPeucker;
      break;
    case DOUGLASPEUCKERMOD:
      simplifierFunc = TraceModifier::_douglasPeuckerMod;
      break;
    case LINEAR_ENHANCE:
      enhacerFunc = TraceModifier::_linearEnhance;
      break;
    case SPLINE_ENHANCE:
      enhacerFunc = TraceModifier::_splineEnhance;
      break;
    case CUSTOM:
      break;
    }

    bool modified = false;

    if ( simplifierFunc )
      for ( auto section: sections_ )
        modified = modified | simplifierFunc( section, modifierParams_ );
    else if ( enhacerFunc )
    {

      int currentId = _maximumId( sections_ );
      for ( auto section: sections_ )
        modified =
          modified | enhacerFunc( section, modifierParams_, currentId );
    }
    return modified;
  }

  bool TraceModifier::customModify(
    std::unordered_set< nsol::Section* >& sections_,
    const std::string& scriptPath_ )
  {
    NSPGManager::PyGEmSManager *  manager= nullptr;
    bool modified = false;

    try
    {

#ifdef PYGEMS_USE_PYTHON3
      manager = new NSPGManager::PyGEmSManager(
        "StrFramework", &NSPGManager::PyInit_StrFramework, "Strategies",
        scriptPath_.c_str( ));
#else
      std::cerr << "python 3 not supported" << std::endl;
#endif

      if ( manager )
      {
        bp::object strategy = manager->getModuleAttrib( "Strategy" );

        for ( auto section: sections_ )
        {
          Container container;
          StrategyParams params;

          for ( auto node: section->nodes( ))
          {
            params.stringParam =
              "MyTest_" + std::to_string( params.intParam ) + "_";
            params.intParam++;
            params.node.id = node->id ( );
            params.node.position.x = node->point ( ).x ( );
            params.node.position.y = node->point ( ).y ( );
            params.node.position.z = node->point ( ).z ( );
            params.node.radius = node->radius ( );
            container.addElement( params );
          }
          bp::object _strategy = strategy( container );
          std::string inVarName = "inNodes";
          std::string outVarName = "outNodes";

          manager->getModule( ).attr( inVarName.c_str( )) =
            container.getContainer( );
          manager->getModule( ).attr( outVarName.c_str( )) = 0;

          _strategy.attr( "method" )( );
          bp::list result =
            manager->extractListFromModule( outVarName.c_str( ));
          int n = bp::len( result );
          std::vector < float > simplVecNodes;
          for ( int t = 0; t < n; ++t )
          {
            StrategyParams val = bp::extract<StrategyParams>(result[t] );
            simplVecNodes.push_back(val.node.position.x);
            simplVecNodes.push_back(val.node.position.y);
            simplVecNodes.push_back(val.node.position.z);
          }
          modified = modified | _fillSection( section, simplVecNodes );
        }
        delete manager;
      }
    }
    catch( const bp::error_already_set& )
    {
      std::cerr << "Python code error" << std::endl;
      modified = false;
    }
    catch( ... )
    {
      std::cerr << "Error loading script: " <<  scriptPath_.c_str( )
                << std::endl;
      modified = false;
    }
    return modified;
  }

  std::string TraceModifier::description( TModifierMethod modifierMethod_ )
  {
    std::string description;
    switch( modifierMethod_ )
    {
    case NTHPOINT:
      description = std::string( "Nth point simplication" );
      break;
    case RADIAL:
      description = std::string( "Radial distance simplication" );
      break;
    case PERPDIST:
      description = std::string( "Perpendicular distance simplication" );
      break;
    case REUMANNWITKAM:
      description = std::string( "Reumann Witkam simplification" );
      break;
    case OPHEIN:
      description = std::string( "Ophein simplification" );
      break;
    case LANG:
      description = std::string( "Lang simplification" );
      break;
    case DOUGLASPEUCKER:
      description = std::string( "Douglas-Peucker simplification" );
      break;
    case DOUGLASPEUCKERMOD:
      description = std::string( "Douglas-Peucker Mod simplification" );
      break;
    case LINEAR_ENHANCE:
      description = std::string( "Linear enhance" );
      break;
    case SPLINE_ENHANCE:
      description = std::string( "Spline enhance" );
      break;
    case CUSTOM:
      description = std::string( "Custom modifier" );
      break;
    }

    return description;
  }

  TraceModifier::TModifierParams TraceModifier::defaultParams(
    TModifierMethod modifierMethod_ )
  {
    TModifierParams modifierParams;

    switch( modifierMethod_ )
    {
    case NTHPOINT:
      modifierParams[std::string( "num_points" )] = 1.0f;
      break;
    case RADIAL:
      modifierParams[std::string( "distance" )] = 0.1f;
      break;
    case PERPDIST:
      modifierParams[std::string( "distance" )] = 0.1f;
      break;
    case REUMANNWITKAM:
      modifierParams[std::string( "distance" )] = 0.1f;
      break;
    case OPHEIN:
      modifierParams[std::string( "min_threshold" )] = 0.1f;
      modifierParams[std::string( "max_threshold" )] = 0.1f;
      break;
    case LANG:
      modifierParams[std::string( "threshold" )] = 0.1f;
      modifierParams[std::string( "size" )] = 0.1f;
      break;
    case DOUGLASPEUCKER:
      modifierParams[std::string( "threshold" )] = 0.1f;
      break;
    case DOUGLASPEUCKERMOD:
      modifierParams[std::string( "num_points" )] = 1.0f;
      break;
    case LINEAR_ENHANCE:
      modifierParams[std::string( "points_per_unit" )] = 1.0f;
      break;
    case SPLINE_ENHANCE:
      modifierParams[std::string( "points_per_unit" )] = 1.0f;
      break;
    case CUSTOM:
      break;
    }
    return modifierParams;
  }

  bool TraceModifier::_nthpoint( nsol::Section* section_,
                         TModifierParams modifierParams_ )
  {
    float value = modifierParams_["num_points"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_nth_point< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
  }

  bool TraceModifier::_radial( nsol::Section* section_,
                               TModifierParams modifierParams_ )
  {
    float value = modifierParams_["distance"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_radial_distance< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
  }

  bool TraceModifier::_perpdist( nsol::Section* section_,
                                 TModifierParams modifierParams_ )
  {
    float value = modifierParams_["distance"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_perpendicular_distance< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
    return false;
  }

  bool TraceModifier::_reumannWitkam( nsol::Section* section_,
                                      TModifierParams modifierParams_ )
  {
    float value = modifierParams_["distance"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_reumann_witkam< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
    return false;
  }



  bool TraceModifier::_ophein( nsol::Section* section_,
                       TModifierParams modifierParams_ )
  {
    float value = modifierParams_["min_threshold"];
    float value2 = modifierParams_["max_threshold"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_opheim< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, value2, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
    return false;
  }

  bool TraceModifier::_lang( nsol::Section* section_,
                             TModifierParams modifierParams_ )
  {
    float value = modifierParams_["threshold"];
    float value2 = modifierParams_["size"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_lang< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, value2, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
    return false;
  }

  bool TraceModifier::_douglasPeucker( nsol::Section* section_,
                                       TModifierParams modifierParams_ )
  {
    float value = modifierParams_["threshold"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_douglas_peucker< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
    return false;
  }

  bool TraceModifier::_douglasPeuckerMod( nsol::Section* section_,
                                          TModifierParams modifierParams_ )
  {
    float value = modifierParams_["num_points"];
    std::vector< float > vecNodes = _vectorizeSection( section_ );
    std::vector< float > simplVecNodes;

    imppsimpl::simplify_douglas_peucker_n< 3 >(
      vecNodes.begin( ), vecNodes.end( ),
      value, std::back_inserter( simplVecNodes ));
    vecNodes.clear( );
    return _fillSection( section_, simplVecNodes );
    return false;
  }

  bool TraceModifier::_linearEnhance( nsol::Section* section_,
                                      TModifierParams modifierParams_,
                                      int& currentId_ )
  {
    bool modified = false;
    float pointsPerUnit = modifierParams_["points_per_unit"];

    auto& nodes = section_->nodes( );
    nsol::Nodes cNodes( nodes );

    nodes.clear( );

    for ( unsigned int i = 1; i < cNodes.size( ); i++ )
    {
      auto startNode = cNodes[i-1];
      auto endNode = cNodes[i];

      nodes.push_back( startNode );
      Eigen::Vector3f dir = ( endNode->point( ) - startNode->point( ));
      float dist = dir.norm( );
      dir.normalize( );
      unsigned int numNewPoints = round( pointsPerUnit * dist );
      if ( numNewPoints > 2 )
      {
        modified = true;
        numNewPoints -= 2;
        Eigen::Vector3f startPoint = startNode->point( );
        float distIncre = dist / ( numNewPoints + 1 );
        float startRadius = startNode->radius( );
        float radiusIncre = ( endNode->radius( ) - startNode->radius( )) /
          ( numNewPoints + 1 );
        for ( unsigned int j = 0; j < numNewPoints; j++ )
        {
          currentId_++;
          nodes.push_back( new nsol::Node(
                             startPoint + dir * ( j + 1 ) * distIncre ,
                             currentId_, startRadius + j * radiusIncre ));
        }
      }
    }
    nodes.push_back( cNodes[cNodes.size( )-1]);

    return modified;
  }

  bool TraceModifier::_splineEnhance( nsol::Section* section_,
                                      TModifierParams modifierParams_,
                                      int& currentId_ )
  {  bool modified = false;
    float pointsPerUnit = modifierParams_["points_per_unit"];

    auto& nodes = section_->nodes( );
    nsol::Nodes cNodes( nodes );

    nodes.clear( );
    Eigen::Vector3f startTangent =
      ( cNodes[1]->point( ) - cNodes[0]->point( ) );
    float norm = startTangent.norm( );
    startTangent.normalize( );
    startTangent *= norm;
    for ( unsigned int i = 1; i < cNodes.size( ); i++ )
    {
      auto startNode = cNodes[i-1];
      auto endNode = cNodes[i];

      nodes.push_back( startNode );
      Eigen::Vector3f dir = ( endNode->point( ) - startNode->point( ));
      float dist = dir.norm( );
      dir.normalize( );
      unsigned int numNewPoints = round( pointsPerUnit * dist );
      if ( numNewPoints > 2 )
      {
        modified = true;
        numNewPoints -= 2;
        Eigen::Vector3f startPoint = startNode->point( );
        Eigen::Vector3f endPoint = endNode->point( );
        Eigen::Vector3f endTangent;
        if ( i == cNodes.size( ) - 1 )
          endTangent = dir;
        else
          endTangent = ( cNodes[i+1]->point( ) - startPoint ).normalized( );

        startTangent.normalize( );
        startTangent *= dist;
        endTangent *= dist;
        float increment = 1.0 / ( numNewPoints + 1 );
        float startRadius = startNode->radius( );
        float radiusIncre = ( endNode->radius( ) - startNode->radius( )) /
          ( numNewPoints + 1 );
        for ( unsigned int j = 0; j < numNewPoints; j++ )
        {
          currentId_++;
          float t = (j + 1)*increment;
          float t2 = t*t;
          float t3 = t2*t;
          Eigen::Vector3f point =
            startPoint*( 2*t3 - 3*t2 + 1 ) + startTangent*( t3 - 2*t2 + t ) +
            endPoint*( -2*t3 + 3*t2 ) + endTangent*( t3 - t2 );
          nodes.push_back( new nsol::Node(
                             point, currentId_,
                             startRadius + ( j + 1 ) * radiusIncre ));
        }
        startTangent = endTangent;
      }
    }
    nodes.push_back( cNodes[cNodes.size( )-1]);

    return modified;
  }

  std::vector< float > TraceModifier::_vectorizeSection(
    nsol::Section* section_ )
  {
    std::vector< float > vecNodes;
    for ( auto node: section_->nodes( ))
    {
      vecNodes.push_back( node->point( ).x( ));
      vecNodes.push_back( node->point( ).y( ));
      vecNodes.push_back( node->point( ).z( ));
    }
    return vecNodes;
  }

  bool TraceModifier::_fillSection(
    nsol::Section* section_, std::vector< float >& simplVecNodes_  )
  {
    if ( simplVecNodes_.size( ) == 0 )
      return false;

    bool modified = false;

    auto& nodes = section_->nodes( );
    unsigned int j = 1;
    nsol::Vec3f simplPoint( simplVecNodes_[j*3], simplVecNodes_[j*3+1],
                           simplVecNodes_[j*3+2] );
    for ( unsigned int i = 1; i < nodes.size( ) - 1 ; i++ )
    {
      auto cNode = nodes[i];
      if ( cNode->point( ) != simplPoint )
      {
        nodes.erase( nodes.begin( ) + i );
        delete cNode;
        i--;
        modified = true;
      }
      else
      {
        if ( j < simplVecNodes_.size( ) - 1 )
        {
          j++;
          simplPoint = nsol::Vec3f( simplVecNodes_[j*3], simplVecNodes_[j*3+1],
                                    simplVecNodes_[j*3+2] );
        }
      }
    }

    simplVecNodes_.clear( );
    return modified;
  }

  int TraceModifier::_maximumId(
    std::unordered_set< nsol::Section* >& sections_ )
  {
    int maxId = 0;
    for ( auto section: sections_ )
      for ( auto node: section->nodes( ))
        maxId = std::max( maxId, node->id( ));
    return maxId;
  }


}
