//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//
#include "KmlLatitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "MarbleGlobal.h"

#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"
#include "GeoParser.h"
#include "GeoDataCoordinates.h"

namespace Marble
{
namespace kml
{
    KML_DEFINE_TAG_HANDLER( latitude )
    GeoNode *KmllatitudeTagHandler::parse( GeoParser & parser ) const
    {
        Q_ASSERT( parser.isStartElement()
                  && parser.isValidElement( kmlTag_latitude ) );

        GeoStackItem parentItem = parser.parentElement();

        if ( parentItem.is<GeoDataLookAt>() ) {
            qreal latitude = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataLookAt>()->setLatitude(latitude, GeoDataCoordinates::Degree);
        } else if ( parentItem.is<GeoDataCamera>() ) {
            qreal latitude = parser.readElementText().trimmed().toDouble();
            parentItem.nodeAs<GeoDataCamera>()->setLatitude(latitude, GeoDataCoordinates::Degree);
        }

        return 0;
    }
  }
}
