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

#include "Utils.h"
#include "imppsimpl.h"

namespace retracer //Probably this code must be in nsol in the future
{
  void Utils::Simplify ( nsol::NeuronMorphologyPtr& morphology_, std::map < std::string, float > params_,
                         int objectId_, OBJECT_TYPE objectType)
  {
    float* generatedPoints;
    std::vector < float > simplifiedCoords;
    int actNeurite = 0;
    int actSection = 0;
    unsigned int i = 0;

    for ( auto neurite: morphology_->neurites ( ))
    {
      if ( ( objectId_ == -1 )  //Full Neuron
            || ( objectType == OBJECT_TYPE::SECTION ) //Specific section
            || ((objectType == OBJECT_TYPE::NEURITE)  && ( actNeurite ==
              objectId_ )) //Specific neurite
        )
      {
        //std::cout<<"Simplifying onle the neurite:"<<objectId_<<std::endl;
        for ( auto section: neurite->sections ( ))
        {
          if ( ( objectId_ == -1 )  //Full Neuron
            || ((objectType == OBJECT_TYPE::NEURITE)  && ( actNeurite == objectId_ ))
            || ((objectType == OBJECT_TYPE::SECTION)  && ( actSection == objectId_ )) //Specific section
            )
          {
            i = 0;
            unsigned int numNodes = section->nodes ( ).size ( );
            generatedPoints = new float[numNodes*3];

            //Traverse for simplification
            for ( auto node: section->nodes ( ))
            {
              generatedPoints[i] = node->point ( ).x ( );
              ++i;
              generatedPoints[i] = node->point ( ).y ( );
              ++i;
              generatedPoints[i] = node->point ( ).z ( );
              ++i;
            }
            float *begin = generatedPoints;
            float *end = generatedPoints + ( numNodes*3 );

            simplifiedCoords.clear ( );
            SIMP_METHOD sw = ( SIMP_METHOD ) params_.at ( "Method" );

            switch (( int ) sw )
            {
              case SIMP_METHOD::NTHPOINT:
                imppsimpl::simplify_nth_point < 3 > ( begin,
                                                      end,
                                                      ( int ) params_
                                                        .at ( "Value" ),
                                                      std::back_inserter (
                                                        simplifiedCoords ));
                break;
              case SIMP_METHOD::RADIAL:
                imppsimpl::simplify_radial_distance < 3 > ( begin,
                                                            end,
                                                            params_
                                                              .at ( "Value" ),
                                                            std::back_inserter (
                                                              simplifiedCoords ));
                break;
              case SIMP_METHOD::PERPDIST:
                imppsimpl::simplify_perpendicular_distance < 3 > ( begin,
                                                                   end,
                                                                   params_
                                                                     .at ( "Value" ),
                                                                   std::back_inserter (
                                                                     simplifiedCoords ));
                break;
              case SIMP_METHOD::REUMANNWITKAM:
                imppsimpl::simplify_reumann_witkam < 3 > ( begin,
                                                           end,
                                                           params_.at ( "Value" ),
                                                           std::back_inserter (
                                                             simplifiedCoords ));
                break;
              case SIMP_METHOD::OPHEIN:
                imppsimpl::simplify_opheim < 3 > ( begin,
                                                   end,
                                                   params_.at ( "Value" ),
                                                   params_.at ( "Value2" ),
                                                   std::back_inserter (
                                                     simplifiedCoords ));
                break;
              case SIMP_METHOD::LANG:
                imppsimpl::simplify_lang < 3 > ( begin,
                                                 end,
                                                 params_.at ( "Value" ),
                                                 params_.at ( "Value2" ),
                                                 std::back_inserter (
                                                   simplifiedCoords ));
                break;
              case SIMP_METHOD::DOUGLASPEUCKER:
                imppsimpl::simplify_douglas_peucker < 3 > ( begin,
                                                            end,
                                                            params_
                                                              .at ( "Value" ),
                                                            std::back_inserter (
                                                              simplifiedCoords ));
                break;
              case SIMP_METHOD::DOUGLASPEUCKERMOD:
                imppsimpl::simplify_douglas_peucker_n < 3 > ( begin,
                                                              end,
                                                              params_
                                                                .at ( "Value" ),
                                                              std::back_inserter (
                                                                simplifiedCoords ));
                break;
                //### Custom simplification.

              default:std::cout << "Unhandeled method!" << std::endl;
            }

            //Traverse to apply simplification to the morphology
            unsigned int j = 3;
            //nsol::Nodes* nodes = &section->nodes( );
            auto &nodes = section->nodes ( );
            for ( i = 2; i < nodes.size ( ); ++i )
            {
              nsol::Vec3f tmpPoint ( simplifiedCoords[j],
                                     simplifiedCoords[j + 1],
                                     simplifiedCoords[j + 2] );
              if ( tmpPoint != nodes[i - 1]->point ( ))
              {
                delete nodes[i - 1];
                nodes.erase ( nodes.begin ( ) + i - 1 );
                i--;
              }
              else
                j += 3;
            }
            delete[] generatedPoints;
          }
          ++actSection;
        }
      }
      ++actNeurite;
    }

    //Update ids for connectivity
    Autofix ( morphology_, params_ );
  }

  void Utils::Enhance ( nsol::NeuronMorphologyPtr& morphology_, std::map < std::string, float > params_,
                        int objectId_, OBJECT_TYPE objectType )
  {
    //nsol::NeuronMorphologyPtr morphology = morphology_->clone ( );

    std::vector < glm::vec3 > originalPoints;
    std::vector < float > radiuses;
    std::vector < float > times;
    int actNeurite = 0;
    int actSection = 0;
    unsigned int nodeId = 0;

    for ( auto neurite: morphology_->neurites ( ))
    {
      if ( ( objectId_ == -1 )  //Full Neuron
        || ( objectType == OBJECT_TYPE::SECTION ) //Specific section
        || ((objectType == OBJECT_TYPE::NEURITE)  && ( actNeurite ==
          objectId_ )) //Specific neurite
        )
      {
        for ( auto section: neurite->sections ( ))
        {
          if ( ( objectId_ == -1 )  //Full Neuron
            || ((objectType == OBJECT_TYPE::NEURITE)  && ( actNeurite == objectId_ ))
            || ((objectType == OBJECT_TYPE::SECTION)  && ( actSection == objectId_ )) //Specific section
            )
          {
            originalPoints.clear ( );
            radiuses.clear ( );
            times.clear ( );
            for ( auto node: section->nodes ( ))
            {
              originalPoints.push_back ( glm::vec3 ( node->point ( ).x ( ),
                                                     node->point ( ).y ( ),
                                                     node->point ( ).z ( )));
              radiuses.push_back ( node->radius ( ));
            }
            int numOriNodes = originalPoints.size ( );
            float radioInc = 1.0;
            ENHANCE_METHOD sw = ( ENHANCE_METHOD ) params_.at ( "Method" );
            switch (( int ) sw )
            {
              case ENHANCE_METHOD::LINEAR:
              {
                std::vector < nsol::NodePtr > newNodes;
                for ( int j = 1; j < numOriNodes; ++j )
                {
                  glm::vec3 iniValue ( originalPoints.at ( j - 1 ));
                  glm::vec3 finValue ( originalPoints.at ( j ));
                  glm::vec3 dir ( finValue - iniValue );

                  unsigned int
                    numNewEles = round ( dir.length ( )/params_.at ( "Value" ));
                  float partialAdvance = 1.0/( numNewEles - 2 );
                  radioInc = ( radiuses.at ( j ) - radiuses.at ( j - 1 ))/(
                    (float)numNewEles - 2 );

                  for ( unsigned int i = 1; i < numNewEles-1; ++i )
                  {
                    const float advance = partialAdvance*i;
                    glm::vec3 value ( iniValue + advance*dir );
                    nsol::Vec3f postion ( value.x, value.y, value.z );

                    //Radio y position
                    float radius = radiuses.at ( j - 1 ) + i*radioInc;
                    nsol::NodePtr node = new nsol::Node ( postion, ++nodeId, radius );
                    newNodes.push_back ( node );
                  }
                }

                nsol::NodePtr nodeIni = section->nodes ( )[0];
                nsol::NodePtr nodeFin = section->nodes ( )[section->nodes ( ).size ()-1];
                section->nodes ( ).clear ( );
                section->nodes ( ).push_back ( nodeIni );
                for ( unsigned int i = 0; i < newNodes.size ( ); ++i )
                  section->nodes ( ).push_back ( newNodes[i] );
                section->nodes ( ).push_back ( nodeFin );
              }
                break;
              case ENHANCE_METHOD::SPLINE:
              {
                float totalDist = 0.0f;
                if (numOriNodes<3)
                {
                  originalPoints.emplace (  originalPoints.begin()+1,
                                            (originalPoints[0] + originalPoints[1])/2.0f );
                  radiuses.emplace  (  radiuses.begin()+1,
                                       (radiuses[0] + radiuses[1])/2.0f );
                  ++numOriNodes;
                }
                for ( int j = 1; j < numOriNodes; ++j )
                {
                  glm::vec3 iniValue ( originalPoints.at ( j - 1 ));
                  glm::vec3 finValue ( originalPoints.at ( j ));
                  glm::vec3 dir ( finValue - iniValue );
                  totalDist += dir.length ( );
                }

                times.push_back ( 0.0 );
                unsigned int numNewEles = round ( totalDist/params_.at ( "Value" ));
                float originalAdvance = 1.0/( numOriNodes);
                float partialAdvance = 1.0/( numNewEles-2);
                for ( int j = 1; j < numOriNodes-1; ++j )
                  times.push_back ( j*originalAdvance );
                times.push_back ( 1.0 );

                NSSpline::Spline < float, glm::vec3 > sp ( times, originalPoints );
                NSSpline::Spline < float, float > spRadius ( times, radiuses );

                std::vector < nsol::NodePtr > newNodes;
                for ( unsigned int i = 1; i < numNewEles-1; ++i )
                {
                  const float advance = partialAdvance*i;
                  glm::vec3 value ( sp[advance] );
                  nsol::Vec3f postion ( value.x, value.y, value.z );

                  float radius = spRadius[advance];
                  nsol::NodePtr node = new nsol::Node ( postion, ++nodeId, radius );
                  newNodes.push_back ( node );
                }

                nsol::NodePtr nodeIni = section->nodes ( )[0];
                nsol::NodePtr nodeFin = section->nodes ( )[section->nodes ( ).size ()-1];
                section->nodes ( ).clear ( );
                section->nodes ( ).push_back ( nodeIni );
                for ( unsigned int i = 1; i < newNodes.size ( )-1; ++i )
                  section->nodes ( ).push_back ( newNodes[i] );
                section->nodes ( ).push_back ( nodeFin );
              }
                break;
              case ENHANCE_METHOD::CUSTOM_ENHANCE:std::cout << "Custom pythom method for enhancement!" << std::endl;
                break;
              default:std::cout << "Unhandeled method!" << std::endl;
            }
          }
          ++actSection;
        }
      }
      ++actNeurite;
    }
    Autofix ( morphology_, params_ );
  }

  void Utils::Autofix ( nsol::NeuronMorphologyPtr& morphology_, std::map < std::string, float > params_ )
  {
    ( void ) params_;
    int id = morphology_->soma ()->nodes ().size ();
    bool firstSection = true;
    bool firstNode    = true;

    for ( auto neurite: morphology_->neurites ( ))
    {
      firstSection = true;
      for ( auto section: neurite->sections ( ))
      {
        firstNode = true;
        if (firstSection) firstNode = false;
        for ( auto node: section->nodes ( ) )
        {
          if ( firstNode )  firstNode = false;
          else node->id ( ++id );
        }
        firstSection = false;
      }
    }
  }
}
