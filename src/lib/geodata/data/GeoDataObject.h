//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//


#ifndef GEODATAOBJECT_H
#define GEODATAOBJECT_H

#include "geodata_export.h"

#include "GeoDocument.h" 
#include "Serializable.h"

#include <QtCore/QMetaType>

namespace Marble
{

class GeoDataObjectPrivate;

/**
 * @short A base class for all geodata objects
 *
 * GeoDataObject is the base class for all geodata classes. It is
 * never instantiated by itself, but is always used as part of a
 * derived object.
 *
 * The Geodata objects are all modeled after the Google KML files as
 * defined in
 * http://code.google.com/apis/kml/documentation/kml_tags_21.html.
 *
 * A GeoDataObject contains 2 properties, both corresponding directly
 * to tags in the KML files: the <b>id</b>, which is a unique
 * identifier of the object, and a <b>targetId</b> which is used to
 * reference other objects that have already been loaded.
 *
 * The <b>id</b> property must only be set if the <b>Update</b>
 * mechanism of KML is used, which is currently not supported by
 * Marble.
 */
class GEODATA_EXPORT GeoDataObject : public GeoNode,
                      public Serializable {
public:
    GeoDataObject();
    GeoDataObject( const GeoDataObject & );
    GeoDataObject & operator=( const GeoDataObject & );
    virtual ~GeoDataObject();

    /// Provides type information for downcasting a GeoNode
    virtual QString nodeType() const { return geonode::GeoDataObjectType; }

    /**
     * @brief Get the id of the object.
     */
    int id() const;
    /**
     * @brief Set the id of the object
     * @param value the new id value
     */
    void setId( int value );

    /**
     * @brief Get the targetId of the object to be replaced
     */
    int targetId() const;
    /**
     * @brief set a new targetId of this object
     * @param value the new targetId value
     */
    void setTargetId( int value );

    /**
     * @brief returns the requested child item
     */
    virtual GeoDataObject* child( int );

    /**
     * @brief returns this items position in the parents list
     */
    int row();

    /**
     * @brief returns the position of an item in the list
     */
    virtual int childPosition( GeoDataObject* );

    /**
     * @brief returns the number of items in the list
     */
    virtual int childCount();

    /// Reimplemented from Serializable
    virtual void pack( QDataStream& stream ) const;
    /// Reimplemented from Serializable
    virtual void unpack( QDataStream& steam );

 protected:

    GeoDataObjectPrivate * d;
};

}

Q_DECLARE_METATYPE( Marble::GeoDataObject* )

#endif // GEODATAOBJECT_H
