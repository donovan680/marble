//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013      Roman Karlstetter <roman.karlstetter@googlemail.com>
//

#ifndef ELEVATIONPROFILEDATASOURCE_H
#define ELEVATIONPROFILEDATASOURCE_H

#include <QObject>

#include <QHash>
#include <QList>
#include <QPointF>
#include <QStringList>

namespace Marble
{

class ElevationModel;
class GeoDataCoordinates;
class GeoDataLineString;
class GeoDataObject;
class GeoDataTrack;
class MarbleModel;
class RoutingModel;

class ElevationProfileDataSource : public QObject
{
    Q_OBJECT

public:
    ElevationProfileDataSource( QObject *parent = 0 );

    /**
     * @brief isDataAvailable
     * @return true if data is available to display
     */
    virtual bool isDataAvailable() const = 0;

public Q_SLOTS:
    virtual void requestUpdate() = 0;

Q_SIGNALS:
    void sourceCountChanged();
    void dataUpdated( const GeoDataLineString &points, const QList<QPointF> &elevationData );

protected:
    QList<QPointF> calculateElevationData(const GeoDataLineString &lineString) const;
    virtual qreal getElevation(const GeoDataCoordinates &coordinates) const = 0;
};

/**
 * @brief The ElevationProfileTrackDataSource provides elevation profile of GeoDataTrack Objects in the marblemodel
 */
class ElevationProfileTrackDataSource : public ElevationProfileDataSource
{
    Q_OBJECT

public:
    ElevationProfileTrackDataSource( const MarbleModel *marbleModel, QObject *parent = 0 );

    virtual bool isDataAvailable() const;

    QStringList sourceDescriptions() const;

    void setSourceIndex(int index);

    int currentSourceIndex() const;

public Q_SLOTS:
    virtual void requestUpdate();

protected:
    virtual qreal getElevation(const GeoDataCoordinates &coordinates) const;

private Q_SLOTS:
    void handleObjectAdded( GeoDataObject *object );
    void handleObjectRemoved( GeoDataObject *object );

private:
    QHash<QString, QList<const GeoDataTrack *> > m_trackHash;
    QStringList m_trackChooserList;
    QList<const GeoDataTrack *> m_trackList;
    int m_currentSourceIndex;
};

/**
 * @brief The ElevationProfileRouteDataSource provides elevation profile of the current route
 */
class ElevationProfileRouteDataSource : public ElevationProfileDataSource
{
    Q_OBJECT

public:
    ElevationProfileRouteDataSource( const MarbleModel *marbleModel, QObject *parent = 0 );

    virtual bool isDataAvailable() const;

public Q_SLOTS:
    virtual void requestUpdate();

protected:
    virtual qreal getElevation(const GeoDataCoordinates &coordinates) const;

private:
    const RoutingModel *const m_routingModel;
    const ElevationModel *const m_elevationModel;
    bool m_routeAvailable; // save state if route is available to notify FloatItem when this changes
};

}

#endif // ELEVATIONPROFILEDATASOURCE_H
