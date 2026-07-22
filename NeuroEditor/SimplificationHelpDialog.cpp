//
// Created by ivan on 3/11/21.
//

#include "SimplificationHelpDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>

SimplificationHelpDialog::SimplificationHelpDialog( QWidget* parent )
  :QDialog( parent )
{
  auto mainLayout = new QVBoxLayout();
  auto label = new QLabel("<p>This menu provides several simplification and enhancement methods to apply over the neuron morphological tracing."
                                " Each method has its own parameters that are configurable by the user. In addition, there is a particular method named"
                                " “Custom method” that allows to run user-written python code to simplify or enhance the tracing. In case there are"
                                " selected nodes, the simplification will run over the morphological sections that are completely or partially selected;"
                                " in case no node is selected, the simplification will run over all the sections composing the tracing.</p>"
                                "<p>The supported methods are shown below:</p>");
  label->setWordWrap( true );
  mainLayout->addWidget(label);
  QChar bullet (0x2022);
  QString styleSheet = QString (  "QPushButton {"
                                  "   border: none;"
                                  "   color: palette(window-text);"
                                  "   background: transparent;"
                                  "}"
                                  "QPushButton:hover {"
                                  "    color: palette(link);"
                                  "}");

  _buttons.reserve( neuroeditor::TraceModifier::numModifiers( ) -1 );

  auto methodsLayout = new QVBoxLayout();
  methodsLayout->setContentsMargins(20,1,1,1);
  methodsLayout->setAlignment( Qt::AlignLeft );
  for ( int i = 0; i < neuroeditor::TraceModifier::numModifiers( ); i++ )
  {
    neuroeditor::TraceModifier::TModifierMethod modifierMethod =
    static_cast< neuroeditor::TraceModifier::TModifierMethod >( i );
    std::string description =
    neuroeditor::TraceModifier::description( modifierMethod );

    auto name = bullet + QString("\t") + QString::fromStdString(description);
    auto button = new QPushButton( name );
    button->setStyleSheet( styleSheet );
    methodsLayout->addWidget( button, 0, Qt::AlignLeft );
    QObject::connect(button,&QPushButton::clicked,
                     [=](){ showDescription( modifierMethod );});
  }
  mainLayout->addLayout( methodsLayout );
  setLayout(mainLayout);
}

void SimplificationHelpDialog::showDescription( neuroeditor::TraceModifier::TModifierMethod modifierMethod )
{
  auto message = QString("PlaceHolder");
  switch( modifierMethod )
  {
    case neuroeditor::TraceModifier::NTHPOINT:
      message = "<p> This simplification method consists of traversing the different"
                " polylines keeping only those nodes whose identifier is a multiple"
                " of the value of the parameter <em>Num_points</em> and the final"
                " nodes of the polyline. For example, if we have a polyline with 8 vertices  "
                "{v0,v1,...v7) and we select n=3, we will obtain the polyline {v0,v3,v6,v7}.</p> "
                "<p>Parameters:</p> "
                "<ul> "
                "<li><b>Num_points:</b> A number indicating that, in general (except for the ends of the polyline, which are always kept), from each subset of <em>Num_points</em> nodes, only one (the one whose identifier is a multiple of <em>Num_points</em>) will be kept (the one  "
                "whose identifier is a multiple of <em>Num_points</em>).</li> "
                "</ul> "
                "<p><a href=http://psimpl.sourceforge.net/nth-point.html>More Info</a></p>";
      break;
    case neuroeditor::TraceModifier::RADIAL:
      message = "<p>This method checks the distance from a node to its adjacent "
                "nodes and if this distance is less than a threshold (<em>Distance</em>) "
                "the adjacent nodes are eliminated.</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Distance:</b> Determines the threshold distance below which "
                "the adjacent nodes are considered to be too close together and will be eliminated.</li> "
                "</ul> "
                "<p><a href=http://psimpl.sourceforge.net/radial-distance.html>More Info</a></p> ";
      break;
    case neuroeditor::TraceModifier::PERPDIST:
      message = "<p>This method measures the distance from each node to the segment "
                "formed by the nodes before and after it and, if this distance  "
                "is less than a certain <em>Dist_threshold</em>, the node is eliminated. For example for node v1 the segment formed by nodes v0 and v2 (v0_v2) would be used and the distance from node v1 to segment v0_v2 would be measured.</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Dist_threshold:</b> Controls the distance threshold (from a node to the segment formed by its preceding node and its succeeding node) above which it is decided that the node is to be deleted.</li> "
                "</ul>"
                "<p><a href=http://psimpl.sourceforge.net/perpendicular-distance.html>More Info</a></p>";

      break;
    case neuroeditor::TraceModifier::REUMANNWITKAM:
      message = "<h2>Reumann-Witkam simplification</h2> "
                "<p>This method draws a line between two consecutive nodes.  "
                "Starting from this line, two aditional parallel lines are formed at "
                "a given tolerance distance (<em>Distance</em> parameter) forming an area. All nodes contained  "
                "in this area except the last one will be eliminated.</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Distance:</b> Controls the tolerance distance at which the auxiliary "
                "lines will be formed. A greater value implies a larger area and increases simplification.</li> "
                "</ul> "
                "<p><a href=http://psimpl.sourceforge.net/reumann-witkam.html>More Info</a></p>";
      break;
    case neuroeditor::TraceModifier::OPHEIN:
      message = "<p>This method creates two different areas:a minimum tolerance  "
                "area (<em>Min_threshold</em>) around the current node and a maximum tolerance "
                "area (<em>Max_threshold</em>). First, using the minimum tolerance "
                "area, the farthest node still contained in this area is selected,  "
                "then a ray is drawn from the successive current node (the first outside node) to the above mentioned node (the last inside node) with a length (<em>Max_threshold</em>). Starting from this ray, an area of width "
                "(<em>Min_threshold</em>) is created, all the nodes contained in this area except "
                "the last one will be eliminated.</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Min_threshold:</b> This parameter controls the radius of "
                "the minimum area and the width of the maximum tolerance area</li> "
                "<li><b>Max_threshold:</b> This parameter determines the length of "
                "the ray that will be used to generate the area of maximum tolerance.</li> "
                "</ul> "
                "<p><a href=http://psimpl.sourceforge.net/opheim.html>More Info</a></p>";
      break;
    case neuroeditor::TraceModifier::LANG:
      message = "<p>This method generates a segment from the current node to a "
                "node <em>Size</em> positions ahead. Then, the distance of all "
                "intermediate nodes to this segment is measured. If that distances is "
                "greater than a certain <em>Threshold</em>, the segment is recalculated excluding "
                "the last node of the segment. This process is repeated until the distance of "
                "all intermediate nodes to the segment is less than the <em>Threshold.</em> "
                "At this point, all intermediate nodes are eliminated. Then, the last "
                "node of the segment from the previous step is selected as the "
                "current node and the process is repeated.</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Size:</b> Number of nodes skipped to select the node that "
                "will be used to generate the segment. For example, with Size = 3 "
                "if the current node is v0 the segment will be generated from V0 to V3.</li> "
                "<li><b>Threshold:</b> This parameter controls the distance threshold "
                "of the intermediate nodes to the segments to decide the nodes to be deleted.</li> "
                "</ul>  "
                "<p><a href=http://psimpl.sourceforge.net/lang.html>More Info</a></p>";
      break;
    case neuroeditor::TraceModifier::DOUGLASPEUCKER:
      message = "<p>This method starts from a basic simplification formed by a line"
                " from the beginning to the end of the polyline that is being refined."
                " For this refinement the distance of each node to the current simplified"
                " polyline is calculated and the one with the largest distance is "
                "selected. If this distance is greater than a certain threshold, "
                "it is added to the simplified polyline. This process is repeated "
                "until all nodes that are not part of the simplified polyline are "
                "at a distance less than the threshold.</p>"
                "<p>Parameters</p>"
                "<ul>"
                "<li><b>Threshold:</b> This parameter determines how far away the "
                "nodes of the simplified polyline have to be to be added to the "
                "simplified polyline or to terminate the simplification process.</li>"
                "</ul>"
                "<p><a href=http://psimpl.sourceforge.net/douglas-peucker.html>More Info</a></p>";
      break;
    case neuroeditor::TraceModifier::DOUGLASPEUCKERMOD:
      message = "<p>This method starts from a basic simplification formed by a line "
                "from the beginning to the end of the polyline that is being refined. "
                "For this refinement the distance of each node to the current simplified "
                "polyline is calculated and the one with the largest distance is  "
                "added to the simplified polyline. This process continues until the simplified polyline has <em>Num_points</em> points</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Num_points:</b> This parameter determines the total number of points of the simplified polyline.</li> "
                "</ul> "
                "<p><a href=http://psimpl.sourceforge.net/douglas-peucker.html>More Info</a></p>";
      break;
    case neuroeditor::TraceModifier::LINEAR_ENHANCE:
      message = "<p>This method adds new nodes to the sections to achieve a uniform "
                "distribution of nodes in the tracing (this method does not modify the original trajectories).</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Points_per_unit:</b> This parameter controls the node density of the resulting tracing</li> "
                "</ul>";
      break;
    case neuroeditor::TraceModifier::SPLINE_ENHANCE:
      message = "<p>This method smooths the current trajectories of the tracing avoiding "
                "abrupt changes of the direction of neurites (or bifurcations).  "
                "Additionally it adds new nodes to the smoothed trajectories in order  "
                "to achieve a uniform node density in the tracing</p> "
                "<p>Parameters</p> "
                "<ul> "
                "<li><b>Points_per_unit:</b> This parameter controls the node density of the resulting tracing</li> "
                "</ul>";
      break;
    case neuroeditor::TraceModifier::CUSTOM:
      message = "<p> This method can be programmed by the user to implement his own correction methods and must follow the"
                " following structure.</p>"
                " "
                " <div class=\"highlight\"><pre style=\"line-height: 125%;\"><span></span><span style=\"color: #008000; font-weight: bold\">from</span> <span style=\"color: #0000FF; font-weight: bold\">CorrectionFramework</span> <span style=\"color: #008000; font-weight: bold\">import</span> <span style=\"color: #666666\">*</span>\n"
                "\n"
                "<span style=\"color: #008000; font-weight: bold\">class</span> <span style=\"color: #0000FF; font-weight: bold\">CorrectionMethod</span>(  ):\n"
                "\n"
                "  <span style=\"color: #008000; font-weight: bold\">def</span> <span style=\"color: #0000FF\">correct</span>(<span style=\"color: #008000\">self</span>):\n"
                "\n"
                "    <span style=\"color: #008000; font-weight: bold\">global</span> inNodes\n"
                "    <span style=\"color: #008000; font-weight: bold\">global</span> outNodes\n"
                "    \n"
                "    <span style=\"color: #3D7B7B; font-style: italic\">#Correction method</span>\n"
                "</pre></div>"
                ""
                "<p> Where inNodes are the original nodes of the tracing and outNodes are the nodes resulting from the correction."
                " Finally, if you want to create a new empty node you must use the TracingNode() function. You can see examples"
                " on the project <a href= https://vg-lab.es/neuroEditor>page</a></p>";
    default:
      break;
  }

  auto messageBox = new QMessageBox();
  auto title = QString::fromStdString(neuroeditor::TraceModifier::description( modifierMethod ));
  messageBox->setWindowTitle(title);
  messageBox->setIcon(QMessageBox::Information);
  messageBox->setInformativeText(message);
  messageBox->exec();
}


