/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#ifndef SPLASHCONTROLLER_H
#define SPLASHCONTROLLER_H

#include <QDir>
#include <QQueue>
#include <QTimer>
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller takes splash commands and generates a controller interface in the response.
*/

class SplashController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(SplashController)
public:
             SplashController();
            ~SplashController();
       void  service(HttpRequest& request, HttpResponse& response);
 QByteArray  playButton( QFileInfo fi, qint32 i );
 QByteArray  curFileFirst( QByteArray first );
    QString  labelFromFileInfo( QFileInfo fi );
 QByteArray  imageFromFileInfo( QFileInfo fi );

signals:
       void  startPlayer();
       void  enqueueSplash(QString,qint32);

public:
};

#endif // SPLASHCONTROLLER_H

