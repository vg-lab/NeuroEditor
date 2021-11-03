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
                                " “Custom method” that allows to run user-written python code to simplify or enhance the tracing.In case there are"
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

  for ( int i = 0; i < neuroeditor::TraceModifier::numModifiers( ); i++ )
  {
    neuroeditor::TraceModifier::TModifierMethod modifierMethod =
    static_cast< neuroeditor::TraceModifier::TModifierMethod >( i );
    std::string description =
    neuroeditor::TraceModifier::description( modifierMethod );

    auto name = bullet + QString("\t") + QString::fromStdString(description);
    auto button = new QPushButton( name );
    button->setStyleSheet( styleSheet );
    mainLayout->addWidget( button );
    QObject::connect(button,&QPushButton::clicked,
                     [=](){ showDescription( modifierMethod );});
  }

  setLayout(mainLayout);
}

void SimplificationHelpDialog::showDescription( neuroeditor::TraceModifier::TModifierMethod modifierMethod )
{
  auto message = QString("PlaceHolder");
  switch( modifierMethod )
  {
    case neuroeditor::TraceModifier::NTHPOINT:
      break;
    case neuroeditor::TraceModifier::RADIAL:
      break;
    case neuroeditor::TraceModifier::PERPDIST:
      break;
    case neuroeditor::TraceModifier::REUMANNWITKAM:
      break;
    case neuroeditor::TraceModifier::OPHEIN:
      break;
    case neuroeditor::TraceModifier::LANG:
      break;
    case neuroeditor::TraceModifier::DOUGLASPEUCKER:
      break;
    case neuroeditor::TraceModifier::LINEAR_ENHANCE:
      break;
    case neuroeditor::TraceModifier::SPLINE_ENHANCE:
      break;
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


