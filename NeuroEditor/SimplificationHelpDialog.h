//
// Created by ivan on 3/11/21.
//

#ifndef __SIMPLIFICATIONHELPDIALOG__
#define __SIMPLIFICATIONHELPDIALOG__

#include "TraceModifier.h"
#include <QDialog>


class SimplificationHelpDialog: public QDialog
{
  Q_OBJECT
public:
  explicit SimplificationHelpDialog(QWidget* parent = 0);

public Q_SLOTS:
  void showDescription( neuroeditor::TraceModifier::TModifierMethod modifierMethod );


protected:
  std::vector<QPushButton* > _buttons;
};


#endif //__SIMPLIFICATIONHELPDIALOG__
