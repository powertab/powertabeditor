#ifndef MUSICFONT_H
#define MUSICFONT_H

#include <QFont>
#include <QHash>
class QGraphicsSimpleTextItem;

/*
 Provides an abstraction over the music notation font, by allowing one to
 access notation symbols without directly specifying the Unicode value
 */
class MusicFont
{
public:
    MusicFont();

    enum MusicSymbol // All of the available music symbols
    {
        // TODO - add the rest of the symbols
        AccidentalSharp = 0xe10e,
        AccidentalFlat = 0xe11a,
        Dot = 0xe130,
        TrebleClef = 0xe1a9,
        BassClef = 0xe1a7,
        TabClef = 0xe1ad,
        CommonTime = 0xe1af,
        CutTime = 0xe1b0,
    };

    // Returns the symbol corresponding to the specified MusicSymbol
    QChar getSymbol(MusicSymbol identifier);

    void setSymbol(QGraphicsSimpleTextItem* text, MusicSymbol identifier, int size = DEFAULT_FONT_SIZE);
    void setNumericText(QGraphicsSimpleTextItem* text, QString number, int size = DEFAULT_FONT_SIZE);

private:
    QFont musicNotationFont;
    QHash<MusicSymbol, QChar> symbolIndex; // maps the MusicSymbol indentifiers to the appropriate characters
    static const int DEFAULT_FONT_SIZE = 16;
};

#endif // MUSICFONT_H
