#include "musicfont.h"

#include <QGraphicsSimpleTextItem>
#include <QFontDatabase>
#include <QString>

MusicFont::MusicFont()
{
    musicNotationFont = QFont("Emmentaler");
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}

QChar MusicFont::getSymbol(MusicSymbol identifier)
{
    return QChar(identifier);
}

// Sets the text of the QGraphicsSimpleTextItem to the given music symbol
void MusicFont::setSymbol(QGraphicsSimpleTextItem* text, MusicSymbol identifier, int size)
{
    text->setText(getSymbol(identifier));
    musicNotationFont.setPixelSize(size);
    text->setFont(musicNotationFont);
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}

// Sets numeric text using the Emmentaler font (this is useful for time signatures)
void MusicFont::setNumericText(QGraphicsSimpleTextItem* text, QString number, int size)
{
    text->setText(number);
    musicNotationFont.setPixelSize(size);
    text->setFont(musicNotationFont);
    musicNotationFont.setPixelSize(DEFAULT_FONT_SIZE);
}
