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
#include <neuroeditor/version.h>
#include <QApplication>
#include <QErrorMessage>
#include <cstring>
#include <iostream>

bool setFormat(int ctxOpenGLMajor, int ctxOpenGLMinor, int ctxOpenGLSamples,
               int ctxOpenGLVSync);

static void printUsage(const char* program)
{
    std::cout
        << "Usage: " << program << " [OPTION] [FILE]\n"
        << "\n"
        << "NeuroEditor: visualization and edition of morphological tracings.\n"
        << "\n"
        << "Arguments:\n"
        << "  FILE           SWC morphology file to load at startup "
           "(optional)\n"
        << "\n"
        << "Options:\n"
        << "  -h, --help     Print this help message and exit\n"
        << "  -v, --version  Print the version and exit\n";
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-h") == 0
            || std::strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        }
        if (std::strcmp(argv[i], "-v") == 0
            || std::strcmp(argv[i], "--version") == 0) {
            std::cout << "NeuroEditor " << NEUROEDITOR_REV_STRING << std::endl;
            return 0;
        }
    }

    QApplication a(argc, argv);

    if (setFormat(4, 0, 16, 1)) {
        NeuroEditor w;
        w.show();

        if (argc > 1) {
            QString swcFile(argv[1]);
            w.viewer()->loadMorphology(swcFile);
        }

        w.init();

        return a.exec();
    } else {
        std::cout << "Error required OpenGL version 4.0 not supported"
                  << std::endl;
        return 0;
    }
}

bool setFormat(int ctxOpenGLMajor, int ctxOpenGLMinor, int ctxOpenGLSamples,
               int ctxOpenGLVSync)
{
    QSurfaceFormat format;
    format.setVersion(ctxOpenGLMajor, ctxOpenGLMinor);
    format.setProfile(QSurfaceFormat::CoreProfile);

    if (ctxOpenGLSamples != 0) {
        format.setSamples(ctxOpenGLSamples);
    }

    format.setSwapInterval(ctxOpenGLVSync);

    QSurfaceFormat::setDefaultFormat(format);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);

    return (format.majorVersion() >= ctxOpenGLMajor)
           && (format.minorVersion() >= ctxOpenGLMinor);
}
