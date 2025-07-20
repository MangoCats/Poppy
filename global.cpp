/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 *                                    *
 * Webserver code adapted from the    *
 * Stefan Frings project.             *
 **************************************/

#include "global.h"

              QString  scriptsPath;
              QString  splashPath;
              QString  hostName;
               qint32  iconWidth;
               qint32  defaultRunTime;
        QFileInfoList  splashFiles;
               QMutex  splashFilesMutex;
               Splash  splashPlayer;
        QFileInfoList  scriptFiles;
               QMutex  scriptFilesMutex;
         ScriptPlayer  sPlayer;
     HttpSessionStore *sessionStore;
 StaticFileController *staticFileController;
