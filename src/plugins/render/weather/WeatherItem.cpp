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
#include "WeatherItem.h"

// Marble
#include "GeoPainter.h"
#include "WeatherData.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QAction>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>

namespace Marble {
    
const QSize borderSpacing( 2, 2 );
const qint32 lineHeight = 28;
const QSize imageSize( lineHeight, lineHeight );
const qint32 horizontalSpacing = 2;

class WeatherItemPrivate {
 public:
    WeatherItemPrivate( WeatherItem *parent )
        : m_priority( 0 ),
          m_action( new QAction( "Weather", parent ) ),
          m_parent( parent ),
          m_temperatureSize()
    {
    }
    
    void updateSize() {
        QSize size( 0, lineHeight );
        // Border
        size += borderSpacing*2;
        // Image size
        size += QSize( imageSize.width(), 0 );
        // Spacing
        size += QSize( horizontalSpacing, 0 );
        // TextSize
        size += QSize( m_temperatureSize.width(), 0 );
        
        m_parent->setSize( size );
    }
    
    WeatherData m_currentWeather;
    quint8 m_priority;
    QAction *m_action;
    WeatherItem *m_parent;
    QSize m_temperatureSize;
    
    static QFont s_font;
};

// FIXME: Fonts to be defined globally
#ifdef Q_OS_MACX
    QFont WeatherItemPrivate::s_font = QFont( "Sans Serif", 10 );
#else
    QFont WeatherItemPrivate::s_font = QFont( "Sans Serif", 8 );
#endif

WeatherItem::WeatherItem( QObject *parent )
    : AbstractDataPluginItem( parent ),
    d( new WeatherItemPrivate( this ) )
{
    setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );
    d->updateSize();
}

WeatherItem::~WeatherItem() {
    delete d;
}

QAction *WeatherItem::action() {
    return d->m_action;
}

QString WeatherItem::itemType() const {
    return QString( "weatherItem" );
}
 
bool WeatherItem::initialized() {
    // FIXME: This is not the correct behavior
    return true;
}

bool WeatherItem::paint( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );
    
    quint32 horizontalPaintPosition = borderSpacing.width();
    
    painter->save();
    // FIXME: I think background rendering should be done in MarbleGraphicsItem.
    // This would be able to merge background with abstractfloatitem (on branch).
    painter->setBrush( QBrush( QColor( 192, 192, 192, 192 ) ) );
    painter->setPen( Qt::SolidLine );
    
    painter->setFont( d->s_font );
    painter->drawRoundedRect( QRect( QPoint( 0, 0 ), size() ), 4, 4 );
    
    painter->drawPixmap( horizontalPaintPosition,
                         borderSpacing.height(),
                         d->m_currentWeather.icon().pixmap( imageSize ) );
    horizontalPaintPosition += imageSize.width();
    horizontalPaintPosition += horizontalSpacing;
    
    painter->drawText( QRect( horizontalPaintPosition,
                              borderSpacing.height(),
                              d->m_temperatureSize.width(),
                              lineHeight ),
                       Qt::AlignVCenter,
                       d->m_currentWeather.temperatureString() );
    painter->restore();
    return true;
}

bool WeatherItem::operator<( const AbstractDataPluginItem *other ) const {
    if( other->itemType() == "weatherItem" ) {
        return ( priority() > ( (WeatherItem *) other )->priority() );
    }
    else {
        return false;
    }
}

QString WeatherItem::stationName() const {
    return id();
}

void WeatherItem::setStationName( const QString& name ) {
    d->m_action->setText( name );
    setId( name );
    update();
}

WeatherData WeatherItem::currentWeather() const {
    return d->m_currentWeather;
}

void WeatherItem::setCurrentWeather( const WeatherData &weather ) {
    d->m_currentWeather = weather;
    QFontMetrics metrics( d->s_font );
    d->m_temperatureSize
        = metrics.boundingRect( weather.temperatureString() ).size()
          + QSize( 4, 0 );
    d->updateSize();
    update();
}

quint8 WeatherItem::priority() const {
    return d->m_priority;
}

void WeatherItem::setPriority( quint8 priority ) {
    d->m_priority = priority;
}

void WeatherItem::setId( const QString& id ) {
    AbstractDataPluginItem::setId( id );
}

} // namespace Marble

#include "WeatherItem.moc"
