//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "AbstractDataPluginModel.h"

// Qt
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QtAlgorithms>

// Marble
#include "AbstractDataPluginWidget.h"
#include "CacheStoragePolicy.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "HttpDownloadManager.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

namespace Marble {

const QString descriptionPrefix( "description_" );

// Time between to new description file downloads in ms
const int timeBetweenDownloads = 250;
    
class AbstractDataPluginModelPrivate {
 public:
    AbstractDataPluginModelPrivate( QString name, AbstractDataPluginModel * parent )
        : m_parent( parent ),
          m_name( name ),
          m_lastBox( new GeoDataLatLonAltBox() ),
          m_downloadedBox( new GeoDataLatLonAltBox() ),
          m_lastNumber( 0 ),
          m_downloadedNumber( 0 ),
          m_downloadTimer( new QTimer( m_parent ) )
    {
    }
    
    ~AbstractDataPluginModelPrivate() {  
        qDeleteAll( m_widgetSet );
        qDeleteAll( m_downloadingWidgets );
        delete m_storagePolicy;
        delete m_lastBox;
        delete m_downloadedBox;
    }
    
    AbstractDataPluginModel *m_parent;
    QString m_name;
    GeoDataLatLonAltBox *m_lastBox;
    GeoDataLatLonAltBox *m_downloadedBox;
    qint32 m_lastNumber;
    qint32 m_downloadedNumber;
    QList<AbstractDataPluginWidget*> m_widgetSet;
    QHash<QString, AbstractDataPluginWidget*> m_downloadingWidgets;
    QTimer *m_downloadTimer;
    
    CacheStoragePolicy *m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
};

AbstractDataPluginModel::AbstractDataPluginModel( QString name, QObject *parent )
    : QObject(  parent ),
      d( new AbstractDataPluginModelPrivate( name, this ) )
{
    // Initializing file and download System
    d->m_storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath()
                                                 + "/cache/" + d->m_name + '/' );
    d->m_downloadManager = new HttpDownloadManager( QUrl(),
                                                    d->m_storagePolicy );
    connect( d->m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
             this,                 SLOT( processFinishedJob( QString , QString ) ) );
    
    // We want to download a new description file every timeBetweenDownloads ms
    connect( d->m_downloadTimer, SIGNAL( timeout() ),
             this,               SLOT( downloadDescriptionFile() ),
             Qt::QueuedConnection );
    d->m_downloadTimer->start( timeBetweenDownloads );
}

AbstractDataPluginModel::~AbstractDataPluginModel() {
    delete d;
}

QList<AbstractDataPluginWidget*> AbstractDataPluginModel::widgets( ViewportParams *viewport,
                                                                   qint32 number )
{
    GeoDataLatLonAltBox *currentBox = new GeoDataLatLonAltBox( viewport->viewLatLonAltBox() );
    QList<AbstractDataPluginWidget*> list;
    
    QList<AbstractDataPluginWidget*>::iterator i;
    
    for ( i = d->m_widgetSet.begin(); i != d->m_widgetSet.end() && list.size() < number; ++i ) {
        // Don't try to access an object that doesn't exist
        if( *i == 0 ) {
            continue;
        }
        
        // Only show widgets that are initialized
        if( !(*i)->initialized() ) {
            continue;
        }
        
        // If the widget is on the viewport, we want to return it
        if( currentBox->contains( (*i)->coordinates() ) ) {
            list.append( *i );
        }
        // FIXME: We have to do something if the widget that is not on the viewport.
    }
    
    if( (!((*currentBox) == (*d->m_lastBox)) || number != d->m_lastNumber) ) {
        delete d->m_lastBox;
        d->m_lastBox = currentBox;
        d->m_lastNumber = number;
    }
    
    return list;
}

void AbstractDataPluginModel::downloadWidgetData( QUrl url,
                                                  QString type,
                                                  AbstractDataPluginWidget *widget )
{
    QString id = generateFilename( widget->id(), type );
    
    d->m_downloadManager->addJob( url, id, id );
    d->m_downloadingWidgets.insert( id, widget );
}

static bool lessThanByPointer( const AbstractDataPluginWidget *widget1,
                               const AbstractDataPluginWidget *widget2 )
{
    if( widget1 != 0 && widget2 != 0 ) {
        return widget1->operator<( widget2 );
    }
    else {
        return false;
    }
}

void AbstractDataPluginModel::addWidgetToList( AbstractDataPluginWidget *widget ) {
    qDebug() << "New widget " << widget->id();
    
    // This find the right position in the sorted to insert the new widget 
    QList<AbstractDataPluginWidget*>::iterator i = qLowerBound( d->m_widgetSet.begin(),
                                                                d->m_widgetSet.end(),
                                                                widget,
                                                                lessThanByPointer );
    // Insert the widget on the right position in the list
    d->m_widgetSet.insert( i, widget );
}

QString AbstractDataPluginModel::name() const {
    return d->m_name;
}

void AbstractDataPluginModel::setName( QString name ) {
    d->m_name = name;
}

QString AbstractDataPluginModel::generateFilename( QString id, QString type ) const {
    QString name;
    name += id;
    name += '_';
    name += type;
    
    return name;
}

QString AbstractDataPluginModel::generateFilepath( QString id, QString type ) const {
    return MarbleDirs::localPath() + "/cache/" + d->m_name + '/' + generateFilename( id, type );
}
    
bool AbstractDataPluginModel::fileExists( QString fileName ) {
    return d->m_storagePolicy->fileExists( fileName );
}

bool AbstractDataPluginModel::fileExists( QString id, QString type ) {
    return fileExists( generateFilename( id, type ) );
}

void AbstractDataPluginModel::downloadDescriptionFile() {
    if( ( !( *d->m_downloadedBox == *d->m_lastBox )
          || d->m_downloadedNumber != d->m_lastNumber )
        && d->m_lastNumber != 0 )
    {
        // Save the box we want to download.
        // We don't want to download too often.
        delete d->m_downloadedBox;
        d->m_downloadedBox = new GeoDataLatLonAltBox( *d->m_lastBox );
        d->m_downloadedNumber = d->m_lastNumber;
        
        QString name( descriptionPrefix );
        name += QString::number( d->m_lastBox->east() );
        name += QString::number( d->m_lastBox->west() );
        name += QString::number( d->m_lastBox->north() );
        name += QString::number( d->m_lastBox->south() );
        d->m_downloadManager->addJob( descriptionFileUrl( d->m_lastBox, d->m_lastNumber ),
                                   name, name );
    }
}

void AbstractDataPluginModel::processFinishedJob( QString relativeUrlString, QString id ) {
    Q_UNUSED( relativeUrlString );
    
    if( id.startsWith( descriptionPrefix ) ) {
        parseFile( d->m_storagePolicy->data( id ) );
    }
    else {
        // The downloaded file contains widget data.
        QStringList fileInformation = id.split( '_' );
        
        QHash<QString, AbstractDataPluginWidget *>::iterator i = d->m_downloadingWidgets.find( id );
        if( i != d->m_downloadingWidgets.end() ) {
            if( fileInformation.size() != 2 ) {
                qDebug() << "Strange file information " << id;
                return;
            }
            
            if( fileInformation.at( 0 ) != (*i)->id() ) {
                qDebug() << "Different id";
                return;
            }
            
            (*i)->addDownloadedFile( generateFilepath( fileInformation.at( 0 ),
                                                       fileInformation.at( 1 ) ), 
                                     fileInformation.at( 1 ) );
            d->m_downloadingWidgets.erase( i );
            
            if( (*i)->initialized() ) {
                addWidgetToList( *i );
            }
        }
    }
}

} // namespace Marble

#include "AbstractDataPluginModel.moc"
