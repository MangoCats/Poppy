/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/

#ifndef SCRIPTPLAYER_H
#define SCRIPTPLAYER_H

#include <QFileInfo>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QTimer>

class ScriptPlayer : public QObject
{
    Q_OBJECT
public:
        explicit  ScriptPlayer(QObject *parent = nullptr);
            void  play( QFileInfo fi );
       QFileInfo  scriptFileInfo( QString z );
          qint32  runTimeFromFileInfo( QFileInfo fi );

public slots:
            void  enqueueScript( QString );
            void  playTimerTimeout();

signals:

public:
          QTimer  playTimer;
  QQueue<QString> scriptQ;
          QMutex  scriptQMutex;
};

#endif // SCRIPTPLAYER_H
