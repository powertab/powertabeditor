#include "musicfont.h"

#include <QGraphicsSimpleTextItem>
#include <QFontDatabase>
#include <QString>

MusicFont::MusicFont()
{
    QFontDatabase fontDatabase;
    musicNotationFont = fontDatabase.font("Emmentaler", "", 16);
}

QChar MusicFont::getSymbol(MusicSymbol identifier)
{
    return QChar(identifier);
}

// Sets the text of the QGraphicsSimpleTextItem to the given music symbol
void MusicFont::setSymbol(QGraphicsSimpleTextItem* text, MusicSymbol identifier, int size)
{
    text->setText(getSymbol(identifier));
    musicNotationFont.setPointSize(size);
    text->setFont(musicNotationFont);
    musicNotationFont.setPointSize(DEFAULT_FONT_SIZE);
}

// Sets numeric text using the Emmentaler font (this is useful for time signatures)
void MusicFont::setNumericText(QGraphicsSimpleTextItem* text, QString number, int size)
{
    text->setText(number);
    musicNotationFont.setPointSize(size);
    text->setFont(musicNotationFont);
    musicNotationFont.setPointSize(DEFAULT_FONT_SIZE);
}
