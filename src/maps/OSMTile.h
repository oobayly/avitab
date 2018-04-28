/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2018 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef SRC_MAPS_OSMTILE_H_
#define SRC_MAPS_OSMTILE_H_

#include <memory>
#include <vector>
#include <cstdint>
#include "Downloader.h"

namespace maps {

class OSMTile {
public:
    OSMTile(int x, int y, int zoom);
    void load(std::shared_ptr<Downloader> downloader);

    int getX() const;
    int getY() const;

    int getImageWidth() const;
    int getImageHeight() const;
    const uint32_t *getImageData() const;
private:
    int x, y;
    int zoomLevel;
    std::vector<uint32_t> image;
    int imageWidth = 0, imageHeight = 0;
};

double longitudeToX(double lon, int zoom);
double latitudeToY(double lat, int zoom);

} /* namespace maps */

#endif /* SRC_MAPS_OSMTILE_H_ */
