/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#include "splash.h"
#include "global.h"
#include <unistd.h>

Splash::Splash(QObject *p) : QObject( p )
{}

Splash::~Splash()
{}

void Splash::liveDelay( int t )
{ QTimer timer;
  timer.setSingleShot( true );
  timer.start( t );
  QCoreApplication::processEvents( QEventLoop::AllEvents, t );
  while ( timer.isActive() )
    { usleep( 500 ); // Get 500us of "real sleep"
      QCoreApplication::processEvents( QEventLoop::AllEvents, 1 );
    }
}

void Splash::splashDown()
{ up = false; }

/**
 * @brief Splash::showSplash 
 * @param filename - image to show
 * @param dur - duration to show splash, in milliseconds
 */
void Splash::showSplash( QString filename, qint32 dur )
{ // qDebug( "showSplash( %s, %d )", filename.toUtf8().data(), dur );
  up = true;
  if ( !splashPath.endsWith("/") )
    filename.prepend( "/" );
  filename.prepend( splashPath );
  filename.append( ".png" );
  if ( !QFile::exists( filename ) )
    { // qDebug( "Attempted to show splash screen but image file '%s' is missing.", filename.toUtf8().data() );
      up = false;
      playTimerTimeout();
      return;
    }
  QPixmap spm = QPixmap( filename );
  if ( spm.isNull() )
    { // qDebug( "Attempted to raise splash screen but file '%s' does not contain a valid image.", filename.toUtf8().data() );
      up = false;
      playTimerTimeout();
      return;
    }
  QSplashScreen *splash = new QSplashScreen( spm );
  ClickEater *cep = new ClickEater( this );
  splash->installEventFilter( cep );
  splash->setWindowFlags( Qt::SplashScreen | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::MaximizeUsingFullscreenGeometryHint | Qt::FramelessWindowHint );
  // valgrind complains about this, and others do too: https://github.com/LMMS/lmms/issues/3926
  // QApplication::setOverrideCursor(Qt::BlankCursor);
  splash->showFullScreen();

  QTimer *playTimer = new QTimer();
  playTimer->setSingleShot( true );
  playTimer->start( dur );
  while ( playTimer->isActive() && up )
    { if ( !splash->isVisible() )
        splash->showFullScreen();
      liveDelay( 10 );
    }
  // qDebug( "Splash going down %d %d", up, playTimer->remainingTime() );
  splash->close();
  splash->deleteLater();
  playTimer->deleteLater();
  cep->deleteLater();
  splashDown();
  // qDebug( "Splash done." );
}

void Splash::enqueueSplash( QString name, qint32 time )
{ QMutexLocker locker(&splashQMutex);
  // qDebug( "Splash::enqueueSplash( %s, %d ) existing size %d", qPrintable( name ), time, splashQ.size() );
  splashQ.enqueue( QPair<QString,qint32>(name, time) );
}

void Splash::playTimerTimeout()
{ if ( up )
    { // qDebug( "Splash is up, splashQ.size() %d, aborting.", splashQ.size() );
      return;
    }
  // qDebug( "Splash::playTimerTimeout() scriptQ.size() %d", splashQ.size() );
  bool done = false;
  while ( !done && !up )
    { splashQMutex.lock();
      if ( splashQ.size() < 1 )
        { splashQMutex.unlock();
          done = true;
        }
       else
        { QPair<QString,qint32> pr = splashQ.dequeue();
          splashQMutex.unlock();
          // qDebug( "Splash::playTimerTimeout() dequeued %s, %d", pr.first.toUtf8().data(), pr.second );
          showSplash( pr.first, pr.second );
    }   }
  // qDebug( "Splash::playTimerTimeout() while fallout exit splashQ.size() %d.", splashQ.size() );
}


bool ClickEater::eventFilter( QObject *o, QEvent *e )
{ if ( sp != nullptr )
    if ( sp->up )
      if ( e->type() == QEvent::MouseButtonPress )
        return true; // eat all MouseButtonPress events
    
  return QObject::eventFilter(o,e); // standard event processing
}

