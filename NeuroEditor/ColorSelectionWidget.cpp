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

#include "ColorSelectionWidget.h"

#include <QPainter>
#include <QRect>
#include <QColor>
#include <QColorDialog>
#include <iostream>

ColorSelectionWidget::ColorSelectionWidget( QWidget* initParent )
  : QWidget( initParent )
  , _color( 0, 0, 0 )
{
  setSizePolicy( QSizePolicy(QSizePolicy::Fixed,
                             QSizePolicy::Fixed));
  setMinimumHeight( 30 );
  setMinimumWidth( 30 );
}

void ColorSelectionWidget::paintEvent( QPaintEvent* /*event*/ )
{
  QPainter painter( this );

  QRect rectSquare( 0, 0, this->width( ), this->height( ));
  painter.fillRect( rectSquare, _color );
}

void ColorSelectionWidget::mousePressEvent( QMouseEvent* /*event*/ )
{

  QColor newColor = QColorDialog::getColor( _color, this,
                                         "Choose new color",
                                         QColorDialog::DontUseNativeDialog);
  if ( newColor.isValid( ))
  {
    _color = newColor;
    _color.setHsv( _color.hsvHue( ), 125, 125 );
    Q_EMIT this->colorChanged( _color );
  }
}

void ColorSelectionWidget::color( const QColor& color_ )
{
  _color = color_;
  Q_EMIT this->colorChanged( _color );
}
