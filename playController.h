/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#ifndef PLAYCONTROLLER_H
#define PLAYCONTROLLER_H

#include <QDir>
#include <QQueue>
#include <QTimer>
#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

using namespace stefanfrings;

/**
  This controller takes playout commands and generates a controller interface in the response.
*/

class PlayController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(PlayController)
public:
             PlayController();
            ~PlayController();
       void  service(HttpRequest& request, HttpResponse& response);
 QByteArray  playButton( QFileInfo fi, qint32 i, bool hide );
 QByteArray  curFileFirst( QByteArray first );
    QString  labelFromFileInfo( QFileInfo fi, bool hide );
 QByteArray  imageFromFileInfo( QFileInfo fi );

signals:
       void  startPlayer();
       void  enqueueScript(QString);

public:
};

#endif // PLAYCONTROLLER_H

