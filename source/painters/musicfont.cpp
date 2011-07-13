#include "musicfont.h"

#include <QGraphicsSimpleTextItem>
#include <QFontDatabase>
#include <QString>

MusicFont::MusicFont()
{
    musicNotationFont = QFont("Emmentaler");
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}

/// Returns the symbol corresponding to the specified MusicSymbol
QChar MusicFont::getSymbol(MusicSymbol identifier)
{
    return QChar(identifier);
}

/// Sets the text of the QGraphicsSimpleTextItem to the given music symbol
void MusicFont::setSymbol(QGraphicsSimpleTextItem* text, MusicSymbol identifier, int size)
{
    text->setText(getSymbol(identifier));
    musicNotationFont.setPixelSize(size);
    text->setFont(musicNotationFont);
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}

/// Returns a reference to the font
const QFont& MusicFont::getFont() const
{
    return musicNotationFont;
}
