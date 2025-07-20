/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#include "splashController.h"
#include "global.h"
#include <QDateTime>
#include <QJsonDocument>

SplashController::SplashController()
{ connect( this, SIGNAL(startPlayer())                , &splashPlayer, SLOT(playTimerTimeout())           , Qt::QueuedConnection );
  connect( this, SIGNAL(enqueueSplash(QString,qint32)), &splashPlayer, SLOT(enqueueSplash(QString,qint32)), Qt::QueuedConnection );
}

SplashController::~SplashController()
{}

/**
 * @brief SplashController::service - called on its own short lived thread
 * @param request - received from http server
 * @param response - sent back to http server
 */
void SplashController::service(HttpRequest& request, HttpResponse& response)
{ QDir dir( splashPath );
  QFileInfoList pfil;
  if ( dir.exists() )
    if ( !dir.isEmpty() )
      { QFileInfoList fil = dir.entryInfoList( QDir::Files | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase );
        foreach ( QFileInfo fi, fil )
          if ( fi.fileName().endsWith(".png") )
            pfil.append( fi );
      }
  splashFilesMutex.lock();
  splashFiles = pfil;
  splashFilesMutex.unlock();
  QByteArray first;
  QByteArray command = request.getPath().mid(8);  // Strip off the /splash/ portion of path
  QString cmdStr = QString::fromUtf8( command );
  QStringList cmdList = cmdStr.split(QChar('/'));
  if ( cmdList.size() == 2 )
    { QString name = cmdList.at(1);
      name.append(".png");
      foreach ( QFileInfo fi, pfil )
        if ( name == fi.fileName().toUtf8() )
          { first = cmdList.at(1).toUtf8();
            emit enqueueSplash( cmdList.at(1), cmdList.at(0).toInt() );
    }     }
        
  // If anything has been enqueued while timer was not running, this will start the queue running
  emit startPlayer();

  // Don't generate a page for a POST request, but otherwise it's processed the same as a GET.
  if ( request.getMethod() == "POST" )
    return;

  // qDebug( "SplashController::service() building response" );
  response.setHeader("Content-Type", "text/html; charset=UTF-8");

  QByteArray body("<html>\n");
  body.append("<head>\n");
  body.append("<link rel='shortcut icon' href='/images/Poppy.png' />\n");
  body.append("<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
  body.append("<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />\n");
  body.append("<meta http-equiv='Pragma' content='no-cache' />\n");
  body.append("<meta http-equiv='Expires' content='0' />\n");
  body.append("<style>\n");
  body.append("table {\n");
  body.append("    border: 0px solid black;\n");
  body.append("}\n");
  body.append("table, th, td {\n");
  body.append("    padding: 5px;\n");
  body.append("}\n");
  body.append(".grid-container {\n");
  body.append("   display: grid;\n");
  body.append("   align-items: center;\n");
  body.append("}\n");
  body.append(".grid-container--fill {\n");
  body.append( QString("   grid-template-columns: repeat(auto-fill, minmax(%1px, 1fr));\n").arg( iconWidth ).toUtf8() );
  body.append("}\n");
  body.append("</style>\n");
  body.append("<title>Poppy: Splash Launcher</title>\n");
  body.append("</head>\n");

  body.append("<body style=\"background-color:black; color:white;\">\n");

  body.append(QString("<H1>%1</H1>\n").arg( hostName ).toUtf8() );
  body.append(QString("<H2>%1</H2>\n").arg( QDateTime::currentDateTime().toString() ).toUtf8() );

  qint32 i = 1;
  // qDebug( "SplashController::service() locking splashFilesMutex" );
  splashFilesMutex.lock();
  if ( splashFiles.size() < 1 )
    body.append( QString( "No files in "+splashPath ).toUtf8() );
   else
    { body.append("<div class=\"grid-container grid-container--fill\">\n");
      body.append( curFileFirst( first ) );
      foreach ( QFileInfo fi, splashFiles )
        body.append( playButton( fi, i++ ) );       
      body.append("<div>\n");
    }
  splashFilesMutex.unlock();
  body.append("</body>\n");

  body.append("</html>");
  // qDebug( "SplashController::service() response built, writing" );
  response.write(body,true);
}

/**
 * @brief SplashController::playButton
 * @param fi - file info for play button
 * @param i - index number
 * @param hide - do we hide #hidden scripts?
 * @return html for one file's play button
 */
QByteArray SplashController::playButton( QFileInfo fi, qint32 i )
{ QString name  = fi.fileName();
  if ( !name.endsWith( ".png" ) )
    return QByteArray();
  name.chop( 4 );
  QByteArray img   = imageFromFileInfo( fi );
  QByteArray label = labelFromFileInfo( fi ).toUtf8();
  QByteArray id    = "btn"+QString::number(i).toUtf8();
  QByteArray ba;
  ba.append( QString( "<div><a href='/splash/%1/%2'>\n" ).arg( splashTimeDefault ).arg( name ).toUtf8() );
  ba.append( "  <center>\n" );
  if ( img.size() > 0 )
    ba.append( QString( "    <img src=\"data:image/png;base64,%1\" width=\"%2\"/><br/>\n" ).arg( QString::fromUtf8( img ) ).arg( iconWidth ).toUtf8() );
  ba.append( "    "+label+"<br/>\n" );
  ba.append( "  </center>\n" );
  ba.append( "</a></div>\n" );
  return ba;
}

/**
 * @brief SplashController::curFileFirst - if a valid image was enqueued with this
 *   splash request, put the launcher button for that splash at first position.  It
 *   will also be repeated at its normal position in the list.
 * @param first - filename of button to put first
 * @return button code, or empty array if no matching script file is found.
 */
QByteArray SplashController::curFileFirst( QByteArray first )
{ if ( first.size() < 1 ) 
    return QByteArray();
  first.append( ".png" );
  // Do NOT lock mutex here, this is called inside the mutex lock above
  QFileInfoList fil = splashFiles;
  foreach ( QFileInfo fi, fil )
    if ( fi.fileName().endsWith( ".png" ) )
      if ( first == fi.fileName().toUtf8() )
        return playButton( fi, 0 );
  return QByteArray();
}

/**
 * @brief SplashController::labelFromFileInfo
 * @return Just the filename without the .png extension
 */
QString SplashController::labelFromFileInfo( QFileInfo fi )
{ QString name = fi.fileName();
  name.chop(4);
  return name;
}

QByteArray SplashController::imageFromFileInfo( QFileInfo fi )
{ QString afp = fi.absoluteFilePath();
  if ( !afp.endsWith( ".png" ) )
    return QByteArray();
  QFile file( afp );
  if ( !file.exists() )
    return QByteArray();
  if ( !file.open( QIODevice::ReadOnly ) )
    return QByteArray();
  QByteArray ba = file.readAll();
  return ba.toBase64();
}

