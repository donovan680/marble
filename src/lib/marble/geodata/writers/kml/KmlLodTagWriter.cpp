//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlLodTagWriter.h"

#include "GeoDataLod.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLod(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLodType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlLodTagWriter);

bool KmlLodTagWriter::write( const GeoNode *node,
				 GeoWriter& writer ) const
{
    const GeoDataLod *lod = static_cast<const GeoDataLod*>( node );
    writer.writeStartElement(kml::kmlTag_Lod);
    KmlObjectTagWriter::writeIdentifiers( writer, lod );
    writer.writeTextElement( kml::kmlTag_minLodPixels,  QString::number(lod->minLodPixels()) );
    writer.writeTextElement( kml::kmlTag_maxLodPixels,  QString::number(lod->maxLodPixels()) );
    writer.writeTextElement( kml::kmlTag_minFadeExtent, QString::number(lod->minFadeExtent()) );
    writer.writeTextElement( kml::kmlTag_maxFadeExtent, QString::number(lod->maxFadeExtent()) );
    writer.writeEndElement();
    return true;
}

}
