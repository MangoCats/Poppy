/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#include "playController.h"
#include "global.h"
#include <QDateTime>
#include <QJsonDocument>

PlayController::PlayController()
{ connect( this, SIGNAL(startPlayer())         , &sPlayer, SLOT(playTimerTimeout())    , Qt::QueuedConnection );
  connect( this, SIGNAL(enqueueScript(QString)), &sPlayer, SLOT(enqueueScript(QString)), Qt::QueuedConnection );
}

PlayController::~PlayController()
{}

/**
 * @brief PlayController::service - called on its own short lived thread
 * @param request - received from http server
 * @param response - sent back to http server
 */
void PlayController::service(HttpRequest& request, HttpResponse& response)
{ QDir dir( scriptsPath );
  scriptFilesMutex.lock();
  scriptFiles.clear();
  if ( dir.exists() )
    if ( !dir.isEmpty() )
      scriptFiles = dir.entryInfoList( QDir::Files | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase );
  scriptFilesMutex.unlock();
  bool hide = !request.getPath().contains( "/pall/" );

  QByteArray first;
  QByteArray name = request.getPath().mid(6);  // Strip off the /play/ portion of path
  foreach ( QFileInfo fi, scriptFiles )
    if ( name.endsWith( ".sh" ) )
      if ( name == fi.fileName().toUtf8() )
        { first = name;
          emit enqueueScript( fi.fileName() );
        }
        
  // If anything has been enqueued while timer was not running, this will start the queue running
  emit startPlayer();

  // Don't generate a page for a POST request, but otherwise it's processed the same as a GET.
  if ( request.getMethod() == "POST" )
    return;

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
  body.append("   grid-template-columns: repeat(auto-fill, minmax(180px, 1fr));\n");
  body.append("}\n");
  body.append("</style>\n");
  body.append("<title>Poppy: Script Launcher</title>\n");
  body.append("</head>\n");

  body.append("<body style=\"background-color:black; color:white;\">\n");

  body.append(QString("<H1>%1</H1>\n").arg( hostName ).toUtf8() );
  body.append(QString("<H2>%1</H2>\n").arg( QDateTime::currentDateTime().toString() ).toUtf8() );

  qint32 i = 1;
  scriptFilesMutex.lock();
  if ( scriptFiles.size() < 1 )
    body.append( QString( "No files in "+scriptsPath ).toUtf8() );
   else
    { body.append("<div class=\"grid-container grid-container--fill\">\n");
      body.append( curFileFirst( first ) );
      foreach ( QFileInfo fi, scriptFiles )
        if ( fi.fileName().endsWith( ".sh" ) )
          body.append( playButton( fi, i++, hide ) );       
      body.append("<div>\n");
    }
  scriptFilesMutex.unlock();
  body.append("</body>\n");

  body.append("</html>");
  response.write(body,true);
}

/**
 * @brief PlayController::playButton
 * @param fi - file info for play button
 * @param i - index number
 * @param hide - do we hide #hidden scripts?
 * @return html for one file's play button
 */
QByteArray PlayController::playButton( QFileInfo fi, qint32 i, bool hide )
{ QByteArray name  = fi.fileName().toUtf8();
  QByteArray img   = imageFromFileInfo( fi );
  QByteArray label = labelFromFileInfo( fi, hide ).toUtf8();
  if ( hide && ( label.size() == 0 ) )
    return QByteArray();
  QByteArray id    = "btn"+QString::number(i).toUtf8();
  QByteArray ba;
  ba.append( "<div><a href='/play/"+name+"'>\n" );
  ba.append( "  <center>\n" );
  if ( img.size() > 0 )
    ba.append( "    <img src=\"data:image/png;base64,"+img+"\" width=\"180\"/><br/>\n" );
  ba.append( "    "+label+"<br/>\n" );
  ba.append( "  </center>\n" );
  ba.append( "</a></div>\n" );
  return ba;
}

/**
 * @brief PlayController::curFileFirst - if a valid script was enqueued with this
 *   play request, put the launcher button for that script at first position.  It
 *   will also be repeated at its normal position in the list.
 * @param first - filename of button to put first
 * @return button code, or empty array if no matching script file is found.
 */
QByteArray PlayController::curFileFirst( QByteArray first )
{ if ( first.size() < 4 ) 
    return QByteArray();
  // Do NOT lock mutex here, this is called inside the mutex lock above
  QFileInfoList fil = scriptFiles;
  foreach ( QFileInfo fi, fil )
    if ( fi.fileName().endsWith( ".sh" ) )
      if ( first == fi.fileName().toUtf8() )
        return playButton( fi, 0 , false );
  return QByteArray();
}

/**
 * @brief PlayController::labelFromFileInfo
 * @param fi - file info for file to look in
 * @param hide - when true if #hidden is found return emtpy label
 * @return label if found in the file, or the filename if not
 */
QString PlayController::labelFromFileInfo( QFileInfo fi, bool hide )
{ QString name = fi.fileName();
  QFile file( fi.absoluteFilePath() );
  if ( !file.open( QIODevice::ReadOnly ) )
    return name;
  QString contents = QString::fromUtf8( file.readAll() );
  if (  contents.contains( "#hidden"      ) && hide )
    return QString();
  if ( !contents.contains( "#PoppyName=" ) )
    return name;
  QStringList lines = contents.split( QChar('\n'), QString::SkipEmptyParts );
  foreach ( QString line, lines )
    if ( line.startsWith( "#PoppyName=") )
      { return line.mid(12).trimmed();
      }
  return name;
}

QByteArray PlayController::imageFromFileInfo( QFileInfo fi )
{ QString afp = fi.absoluteFilePath();
  if ( !afp.endsWith( ".sh" ) )
    return QByteArray();
  afp.chop(2);
  afp.append( "png" );
  QFile file( afp );
  if ( !file.exists() )
    return QByteArray();
  if ( !file.open( QIODevice::ReadOnly ) )
    return QByteArray();
  QByteArray ba = file.readAll();
  return ba.toBase64();
}
