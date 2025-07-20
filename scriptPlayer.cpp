/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#include "scriptPlayer.h"
#include "global.h"
#include <QProcess>

ScriptPlayer::ScriptPlayer(QObject *parent) : QObject(parent)
{ playTimer.setSingleShot( true );
  connect( &playTimer, SIGNAL(timeout()), SLOT(playTimerTimeout()) );
}

void ScriptPlayer::play( QFileInfo fi )
{ qDebug( "ScriptPlayer::play() filename %s", qPrintable( fi.fileName() ) );
  QProcess::startDetached( fi.absoluteFilePath(), QStringList(), fi.absolutePath() );
  //QProcess::execute( fi.absoluteFilePath(), QStringList() );
}

void  ScriptPlayer::enqueueScript( QString scriptName )
{ QMutexLocker locker(&scriptQMutex);
  qDebug( "ScriptPlayer::enqueueScript( %s ) existing size %d", qPrintable( scriptName ), scriptQ.size() );
  scriptQ.enqueue( scriptName );
}

void ScriptPlayer::playTimerTimeout()
{ if ( playTimer.isActive() )
    { qDebug( "ScriptPlayer::playTimerTimeout() scriptQ.size() %d playTimer.isActive() aborting.", scriptQ.size() );
      return;
    }
  qDebug( "ScriptPlayer::playTimerTimeout() scriptQ.size() %d", scriptQ.size() );
  QMutexLocker locker(&scriptQMutex);
  while ( scriptQ.size() > 0 )
    { QFileInfo fi = scriptFileInfo( scriptQ.dequeue() );
      qDebug( "ScriptPlayer::playTimerTimeout() dequeued file %s", qPrintable( fi.fileName() ) );
      if ( fi.exists() )
        { qint32 pft = runTimeFromFileInfo( fi );
          play( fi );
          playTimer.start( pft );
          qDebug( "ScriptPlayer::playTimerTimeout() started %dms timer, scriptQ.size() %d", pft, scriptQ.size() );
    }   }
  qDebug( "ScriptPlayer::playTimerTimeout() while fallout exit scriptQ.size() %d.", scriptQ.size() );
}

/**
 * @brief ScriptPlayer::scriptFileInfo
 * @param sName - script name
 * @return file info of first file in the Sounds folder that matches the zone name
 */
QFileInfo ScriptPlayer::scriptFileInfo( QString sName )
{ if ( sName.size() < 2 )
    return QFileInfo();
  QMutexLocker locker(&scriptFilesMutex);
  foreach( QFileInfo fi, scriptFiles )
    { QString fn = fi.fileName();
      if ( fn.startsWith( sName ) )
        return fi;
    }
  return QFileInfo();
}

/**
 * @brief ScriptPlayer::runTimeFromFileInfo
 * @param fi - file to look in
 * @return PoppyRunTime (milliseconds) from the file if present, defaultRunTime otherwise.
 */
qint32 ScriptPlayer::runTimeFromFileInfo( QFileInfo fi )
{ QFile file( fi.absoluteFilePath() );
  if ( !file.open( QIODevice::ReadOnly ) )
    return defaultRunTime;
  QString contents = QString::fromUtf8( file.readAll() );
  if ( !contents.contains( "#PoppyRunTime=" ) )
    return defaultRunTime;
  QStringList lines = contents.split( QChar('\n'), QString::SkipEmptyParts );
  foreach ( QString line, lines )
    if ( line.startsWith( "#PoppyRunTime=") )
      { return line.mid(15).trimmed().toInt();
      }
  return defaultRunTime;
}


