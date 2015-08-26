//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoPolygonGraphicsItem.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"

#include <QVector2D>
#include <QtCore/qmath.h>

namespace Marble
{

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( const GeoDataFeature *feature, const GeoDataPolygon* polygon )
        : GeoGraphicsItem( feature ),
          m_polygon( polygon ),
          m_ring( 0 )
{
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem( const GeoDataFeature *feature, const GeoDataLinearRing* ring )
        : GeoGraphicsItem( feature ),
          m_polygon( 0 ),
          m_ring( ring )
{
}

const float GeoPolygonGraphicsItem::s_decorationZValue = -0.001;

void GeoPolygonGraphicsItem::createDecorations()
{
    if (isDecoration()) {
        return;
    }

    GeoPolygonGraphicsItem *fake3D;

    switch ( feature()->visualCategory() ) {
        case GeoDataFeature::Building:
        //case GeoDataFeature::AccomodationCamping:
           case GeoDataFeature::AccomodationHostel:
           case GeoDataFeature::AccomodationHotel:
           case GeoDataFeature::AccomodationMotel:
           case GeoDataFeature::AccomodationYouthHostel:
           case GeoDataFeature::AmenityLibrary:
           case GeoDataFeature::EducationCollege:
           case GeoDataFeature::EducationSchool:
           case GeoDataFeature::EducationUniversity:
           case GeoDataFeature::FoodBar:
           case GeoDataFeature::FoodBiergarten:
           case GeoDataFeature::FoodCafe:
           case GeoDataFeature::FoodFastFood:
           case GeoDataFeature::FoodPub:
           case GeoDataFeature::FoodRestaurant:
           case GeoDataFeature::HealthDoctors:
           case GeoDataFeature::HealthHospital:
           case GeoDataFeature::HealthPharmacy:
           case GeoDataFeature::MoneyAtm:
           case GeoDataFeature::MoneyBank:
           case GeoDataFeature::ShoppingBeverages:
           case GeoDataFeature::ShoppingHifi:
           case GeoDataFeature::ShoppingSupermarket:
           //case GeoDataFeature::TouristAttraction:
           //case GeoDataFeature::TouristCastle:
           case GeoDataFeature::TouristCinema:
           //case GeoDataFeature::TouristMonument:
           case GeoDataFeature::TouristMuseum:
           //case GeoDataFeature::TouristRuin:
           case GeoDataFeature::TouristTheatre:
           //case GeoDataFeature::TouristThemePark:
           //case GeoDataFeature::TouristViewPoint:
           //case GeoDataFeature::TouristZoo:
           case GeoDataFeature::ReligionPlaceOfWorship:
           case GeoDataFeature::ReligionBahai:
           case GeoDataFeature::ReligionBuddhist:
           case GeoDataFeature::ReligionChristian:
           case GeoDataFeature::ReligionHindu:
           case GeoDataFeature::ReligionJain:
           case GeoDataFeature::ReligionJewish:
           case GeoDataFeature::ReligionShinto:
           case GeoDataFeature::ReligionSikh:
            fake3D = new GeoPolygonGraphicsItem( feature(), m_polygon );
            fake3D->setZValue(this->zValue() + s_decorationZValue);
            break;

        default:
            fake3D = nullptr;
            break;
    }

    this->addDecoration(fake3D);
}

QPointF GeoPolygonGraphicsItem::buildingOffset(const QPointF &point, const ViewportParams *viewport) const
{
    qreal const cameraFactor = 0.5 * tan(0.5 * 110 * DEG2RAD);
    qreal const buildingHeightMeter = 8.0;
    qreal const buildingFactor = buildingHeightMeter / EARTH_RADIUS;

    qreal const cameraHeightPixel = viewport->width() * cameraFactor;
    qreal const buildingHeightPixel = viewport->radius() * buildingFactor;

    qreal const offsetX = point.x() - viewport->width() / 2.0;
    qreal const alpha1 = atan2(offsetX, cameraHeightPixel);
    qreal const alpha2 = atan2(offsetX, cameraHeightPixel-buildingHeightPixel);
    qreal const shiftX = 2 * (cameraHeightPixel-buildingHeightPixel) * sin(0.5*(alpha2-alpha1));

    qreal const offsetY = point.y() - viewport->height() / 2.0;
    qreal const beta1 = atan2(offsetY, cameraHeightPixel);
    qreal const beta2 = atan2(offsetY, cameraHeightPixel-buildingHeightPixel);
    qreal const shiftY = 2 * (cameraHeightPixel-buildingHeightPixel) * sin(0.5*(beta2-beta1));

    return QPointF(shiftX, shiftY);
}

const GeoDataLatLonAltBox& GeoPolygonGraphicsItem::latLonAltBox() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox();
    } else if ( m_ring ) {
        return m_ring->latLonAltBox();
    } else {
        return GeoGraphicsItem::latLonAltBox();
    }
}

void GeoPolygonGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport )
{
    painter->save();

    bool const isBuildingFrame = isDecoration();
    bool const isBuildingRoof = !isDecoration();

    QPen currentPen = painter->pen();

    if ( !style() ) {
        painter->setPen( QPen() );
    }
    else {
        if ( !style()->polyStyle().outline() || isBuildingFrame ) {
            currentPen.setColor( Qt::transparent );
        }
        else {
            if ( currentPen.color() != style()->lineStyle().paintedColor() ||
                 currentPen.widthF() != style()->lineStyle().width() ) {
                currentPen.setColor( style()->lineStyle().paintedColor() );
                currentPen.setWidthF( style()->lineStyle().width() );
            }

            if ( currentPen.capStyle() != style()->lineStyle().capStyle() )
                currentPen.setCapStyle( style()->lineStyle().capStyle() );

            if ( currentPen.style() != style()->lineStyle().penStyle() )
                currentPen.setStyle( style()->lineStyle().penStyle() );
        }

        if ( painter->pen() != currentPen )
            painter->setPen( currentPen );

        if ( !style()->polyStyle().fill() ) {
            if ( painter->brush().color() != Qt::transparent )
                painter->setBrush( QColor( Qt::transparent ) );
        }
        else {
            if ( isBuildingFrame ) {
                painter->setBrush( style()->polyStyle().paintedColor().darker(150) );
            } else if ( painter->brush().color() != style()->polyStyle().paintedColor() ) {
                painter->setBrush( style()->polyStyle().paintedColor() );
            }
        }
    }

    QPointF offsetAtCorner = buildingOffset(QPointF(0, 0), viewport);
    qreal maxOffset = qMax( qAbs( offsetAtCorner.x() ), qAbs( offsetAtCorner.y() ) );
    bool drawAccurate3D = maxOffset > 5.0;

    if ( isBuildingFrame ) {
        QVector<QPolygonF*> polygons;
        if (m_polygon) {
            viewport->screenCoordinates(m_polygon->outerBoundary(), polygons);
        } else if (m_ring) {
            viewport->screenCoordinates(*m_ring, polygons);
        }
        foreach(QPolygonF* polygon, polygons) {
            if (polygon->isEmpty()) {
                continue;
            }
            if ( drawAccurate3D ) {
                int const size = polygon->size();
                QPointF & a = (*polygon)[0];
                QPointF shiftA = a + buildingOffset(a, viewport);
                for (int i=1; i<size; ++i) {
                    QPointF const & b = (*polygon)[i];
                    QPointF const shiftB = b + buildingOffset(b, viewport);
                    QPolygonF buildingSide = QPolygonF() << a << shiftA << shiftB << b;
                    painter->drawPolygon(buildingSide);
                    a = b;
                    shiftA = shiftB;
                }
            } else {
                painter->drawPolygon(*polygon);
            }
        }
        qDeleteAll(polygons);
    } else if (isBuildingRoof) {
        QVector<QPolygonF*> polygons;
        if (m_polygon) {
            viewport->screenCoordinates(m_polygon->outerBoundary(), polygons);
        } else if (m_ring) {
            viewport->screenCoordinates(*m_ring, polygons);
        }
        foreach(QPolygonF* polygon, polygons) {
            if ( drawAccurate3D) {
                QPolygonF buildingRoof;
                foreach(const QPointF &point, *polygon) {
                    buildingRoof << point + buildingOffset(point, viewport);
                }
                painter->drawPolygon(buildingRoof);
            } else {
                QPointF const offset = buildingOffset(polygon->boundingRect().center(), viewport);
                painter->translate(offset);
                painter->drawPolygon(*polygon);
            }
        }
        qDeleteAll(polygons);
    }
    else {
        if ( m_polygon ) {
            painter->drawPolygon( *m_polygon );
        } else if ( m_ring ) {
            painter->drawPolygon( *m_ring );
        }
    }

    painter->restore();
}

}
