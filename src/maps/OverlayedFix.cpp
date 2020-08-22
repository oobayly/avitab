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

#include "OverlayedFix.h"

#include "OverlayedNDB.h"
#include "OverlayedVOR.h"
#include "OverlayedDME.h"
#include "OverlayedILSLocalizer.h"
#include "OverlayedWaypoint.h"

namespace maps {

xdata::Morse OverlayedFix::morse;

OverlayedFix::OverlayedFix(const xdata::Fix *fix):
    fix(fix) {
    auto &loc = fix->getLocation();
    overlayHelper->positionToPixel(loc.latitude, loc.longitude, px, py);
}

bool OverlayedFix::isDMEOnly(const xdata::Fix &fix) {
    // i.e. Is a DME that is not paired with a VOR or ILS/Localizer
    return fix.getDME() && !fix.getVOR() && !fix.getILSLocalizer();
}

std::shared_ptr<OverlayedFix> OverlayedFix::getInstanceIfVisible(const xdata::Fix &fix) {
    if (overlayHelper->getMapWidthNM() > SHOW_NAVAIDS_AT_MAPWIDTHNM) {
        return NULL;
    }

    if (fix.getNDB()) {
        return OverlayedNDB::getInstanceIfVisible(fix);
    } else if (fix.getVOR()) {
        return OverlayedVOR::getInstanceIfVisible(fix);
    } else if (isDMEOnly(fix)) {
        return OverlayedDME::getInstanceIfVisible(fix);
    } else if (fix.getILSLocalizer()) {
        return OverlayedILSLocalizer::getInstanceIfVisible(fix);
    } else {
        return OverlayedWaypoint::getInstanceIfVisible(fix);
    }
}

void OverlayedFix::drawNavTextBox(std::string type, std::string id, std::string freq, int x, int y, uint32_t color) {
    // x, y is top left corner of rectangular border. If type is not required, pass in as ""
    const int TEXT_SIZE = 10;
    const int MORSE_SIZE = 2;
    const int XBORDER = 2;
    // If type is required, id and freq text and bottom of rectangular border drop by half text height
    int yo = ((type == "") ? 0 : (TEXT_SIZE / 2));
    int textWidth = std::max(mapImage->getTextWidth(id, TEXT_SIZE), mapImage->getTextWidth(freq, TEXT_SIZE));
    int morseWidth = 0;
    for (char const &c: id) {
        morseWidth = std::max(morseWidth, morse.getLength(c) * MORSE_SIZE);
    }
    int boxWidth = textWidth + morseWidth + (XBORDER * 4);
    int boxHeight = (TEXT_SIZE * 2) + yo + 2;
    int xTextCentre = x + textWidth / 2 + XBORDER + 1;

    mapImage->fillRectangle(x + 1, y + 1, x + boxWidth, y + boxHeight, img::COLOR_WHITE);
    mapImage->drawText(id, TEXT_SIZE, xTextCentre, y + yo + 1, color, 0, img::Align::CENTRE);
    mapImage->drawText(freq, TEXT_SIZE, xTextCentre, y + TEXT_SIZE + yo + 1, color, 0, img::Align::CENTRE);
    drawMorse(x + textWidth + (XBORDER * 3), y + yo + 4, id, MORSE_SIZE, color);

    mapImage->drawRectangle(x, y, x + boxWidth, y + boxHeight, color);
    if (type != "") {
        mapImage->drawText(type, TEXT_SIZE, x + boxWidth / 2, y - yo + 1, color, img::COLOR_WHITE, img::Align::CENTRE);
    }
}

void OverlayedFix::drawMorse(int x, int y, std::string text, int size, uint32_t color) {
    for (char const &c: text) {
        std::string morseForChar = morse.getCode(c);
        for (int row = 0; row < size; row++) {
            int col = 0;
            for (char const &d: morseForChar) {
                int numPixels = size * ((d == '.') ? 1 : 3);
                for (int p = 0; p < numPixels; p++) {
                    mapImage->blendPixel(x + col++, y + row, color);
                }
                col += size;
            }
        }
        y += (size * 2);
    }
}

} /* namespace maps */

