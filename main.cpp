/**************************************
 * Poppy - simple script launcher     *
 * plus splash screen displayer       *
 * triggered by network messages.     *
 * Copyright (c) 2025 by Mike Inman   *
 * aka MangoCats, all rights reserved *
 * Free, Open Source, MIT license     *
 **************************************/
#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QJsonDocument>
#include <QHostInfo>
#include "global.h"
#include "httplistener.h"
#include "requestmapper.h"

int main(int argc, char *argv[])
{ QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("MangoCats");
  QCoreApplication::setOrganizationDomain("mangocats.com");
  QCoreApplication::setApplicationName("Poppy");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QSettings* generalSettings=new QSettings(&app);
  scriptsPath    = generalSettings->value("scriptsPath"   , ":/scripts" ).toString();
  splashPath     = generalSettings->value("splashPath"    , ":/images"  ).toString();
  iconWidth      = generalSettings->value("iconWidth"     , 180         ).toInt();
  defaultRunTime = generalSettings->value("defaultRuntime", 5000        ).toInt();
  hostName       = generalSettings->value("hostName"      , QHostInfo::localHostName() ).toString();
  qDebug( "settings file %s", generalSettings->fileName().toUtf8().data() );

  // Configure session store
  QSettings* sessionSettings=new QSettings(&app);
  sessionSettings->beginGroup("sessions");
  sessionStore=new HttpSessionStore(sessionSettings,&app);

  // Configure static file controller
  QSettings* staticFileSettings=new QSettings(&app);
  staticFileSettings->beginGroup("docroot");
  staticFileController=new StaticFileController(staticFileSettings,&app);

  // Configure and start the TCP listener
  QSettings* listenerSettings=new QSettings(&app);
  listenerSettings->beginGroup("listener");
  new HttpListener(listenerSettings,new RequestMapper(&app),&app);

  return app.exec();
}
