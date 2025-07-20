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

#ifndef SPLASH_H
#define SPLASH_H

#include <QtGui>
#include <QSplashScreen>

class Splash : public QObject
{
    Q_OBJECT

public:
   explicit  Splash( QObject *p = nullptr );
            ~Splash();
       void  liveDelay( int t );
       
public slots:
       void  enqueueSplash( QString, qint32 );
       void  playTimerTimeout();
       void  showSplash( QString filename = "QuietPlease", qint32 dur = 2000 );
       void  splashDown();
public:
                          bool  up;
  QQueue<QPair<QString,qint32> >splashQ;
                        QMutex  splashQMutex;
};

class ClickEater : public QObject
{ Q_OBJECT
public:
    explicit  ClickEater( Splash *parent ) : QObject( parent )
                { sp = parent; }

      Splash *sp;
      
protected:
        bool  eventFilter( QObject *o, QEvent *e );
};

#endif // SPLASH_H
