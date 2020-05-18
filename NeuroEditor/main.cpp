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

#include "NeuroEditor.h"
#include <QApplication>
#include <QErrorMessage>


bool setFormat( int ctxOpenGLMajor, int ctxOpenGLMinor,
                int ctxOpenGLSamples, int ctxOpenGLVSync );

int main ( int argc, char* argv[] )
{
  QApplication a ( argc, argv );


  if ( setFormat( 4, 0, 16, 1 ))
  {
    NeuroEditor w;
    w.show ( );

    if ( argc > 1 )
    {
      QString swcFile( argv[1] );
      w.viewer( )->loadMorphology( swcFile );
    }

    w.init( );

    return a.exec( );
  }
  else
  {
    std::cout << "Error required OpenGL version 4.0 not supported" << std::endl;
    return 0;
  }
}


bool setFormat( int ctxOpenGLMajor,
                int ctxOpenGLMinor,
                int ctxOpenGLSamples,
                int ctxOpenGLVSync )
{
  QSurfaceFormat format;
  format.setVersion( ctxOpenGLMajor, ctxOpenGLMinor);
  format.setProfile( QSurfaceFormat::CoreProfile );

  if ( ctxOpenGLSamples != 0 )
    format.setSamples( ctxOpenGLSamples );

  format.setSwapInterval( ctxOpenGLVSync );


  QSurfaceFormat::setDefaultFormat( format );
  format.setProfile( QSurfaceFormat::CompatibilityProfile );

  return ( format.majorVersion() >= ctxOpenGLMajor ) &&
    ( format.minorVersion( ) >= ctxOpenGLMinor );
}
