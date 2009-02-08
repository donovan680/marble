//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataIconStyle.h"

#include <QtCore/QDebug>

namespace Marble
{

class GeoDataIconStylePrivate
{
  public:
    GeoDataIconStylePrivate()
        : m_scale( 1.0 ),
          m_icon( new QPixmap ),
          m_hotSpot( new GeoDataHotSpot() )
    {
    }

    GeoDataIconStylePrivate( const GeoDataIconStylePrivate& other )
        : m_scale( other.m_scale ),
          m_icon( new QPixmap( *other.m_icon ) ),
          m_hotSpot( new GeoDataHotSpot( *other.m_hotSpot ) )
    {
    }

    GeoDataIconStylePrivate( const QPixmap& icon, const QPointF &hotSpot )
        : m_scale( 1.0 ),
          m_icon( new QPixmap( icon ) ),
          m_hotSpot( new GeoDataHotSpot( hotSpot ) )
    {
    }

    ~GeoDataIconStylePrivate()
    {
        delete m_hotSpot;
        delete m_icon;
    }

    float            m_scale;

    QPixmap         *m_icon;
    QString          m_iconPath;
    GeoDataHotSpot  *m_hotSpot;
    mutable QPointF  m_pixelHotSpot;
};

GeoDataIconStyle::GeoDataIconStyle() :
    d( new GeoDataIconStylePrivate() )
{
}

GeoDataIconStyle::GeoDataIconStyle( const GeoDataIconStyle& other ) :
    GeoDataColorStyle( other ), d( new GeoDataIconStylePrivate( *other.d ) )
{
}

GeoDataIconStyle::GeoDataIconStyle( const QPixmap& icon, const QPointF &hotSpot ) :
    d( new GeoDataIconStylePrivate( icon, hotSpot ) )
{
}

GeoDataIconStyle::~GeoDataIconStyle()
{
    delete d;
}

void GeoDataIconStyle::setIcon( const QPixmap &icon )
{
    delete d->m_icon;
    d->m_icon = new QPixmap( icon );
}

void GeoDataIconStyle::setIconPath( const QString& filename )
{
    d->m_iconPath = filename;
}

QPixmap GeoDataIconStyle::icon() const
{
    if(d->m_icon && !d->m_icon->isNull())
        return *(d->m_icon);
    else if(!d->m_iconPath.isEmpty())
        return QPixmap(d->m_iconPath);
    else
        return QPixmap();
}

void GeoDataIconStyle::setHotSpot( const QPointF& hotSpot, 
                                   GeoDataHotSpot::Units xunits,
                                   GeoDataHotSpot::Units yunits )
{
    d->m_hotSpot->setHotSpot( hotSpot, xunits, yunits );
}

const QPointF& GeoDataIconStyle::hotSpot() const // always in pixels, Origin upper left
{
    GeoDataHotSpot::Units xunits;
    GeoDataHotSpot::Units yunits;

    d->m_pixelHotSpot = d->m_hotSpot->hotSpot( xunits, yunits );
    if(d->m_icon) {
        if ( xunits == GeoDataHotSpot::Fraction )
            d->m_pixelHotSpot.setX( d->m_icon->width()  * d->m_pixelHotSpot.x() );
        else {
            if ( xunits == GeoDataHotSpot::InsetPixels )
                d->m_pixelHotSpot.setX( d->m_icon->width()  - d->m_pixelHotSpot.x() );
        }

        if ( yunits == GeoDataHotSpot::Fraction )
            d->m_pixelHotSpot.setY( d->m_icon->height() * ( 1.0 - d->m_pixelHotSpot.y() ) );
        else {
            if ( yunits == GeoDataHotSpot::Pixels )
                d->m_pixelHotSpot.setY( d->m_icon->height() - d->m_pixelHotSpot.y() );
        }
    }

    return d->m_pixelHotSpot;
}

void GeoDataIconStyle::setScale( const float &scale )
{
    d->m_scale = scale;
}

float GeoDataIconStyle::scale() const
{
    return d->m_scale;
}

void GeoDataIconStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << d->m_scale;
    if(d->m_icon)
        stream << *(d->m_icon);
    else
        stream << QPixmap();
    d->m_hotSpot->pack( stream );
}

void GeoDataIconStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> d->m_scale;
    if(!d->m_icon)
        d->m_icon = new QPixmap;
    stream >> *(d->m_icon);
    d->m_hotSpot->unpack( stream );
}

}
