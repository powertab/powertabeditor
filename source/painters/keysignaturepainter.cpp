#include "keysignaturepainter.h"

#include "staffdata.h"
#include <powertabdocument/keysignature.h>
#include <painters/musicfont.h>

#include <QMessageBox> // temporary
#include <QFont>
#include <QPainter>
#include <QVector>

QFont KeySignaturePainter::musicFont = MusicFont().getFont();

KeySignaturePainter::KeySignaturePainter(const StaffData& staffInformation, const KeySignature& signature) :
    staffInfo(staffInformation),
    keySignature(signature)
{
    initAccidentalPositions();
    init();
}

void KeySignaturePainter::init()
{
    bounds = QRectF(0, -10, keySignature.GetWidth(), staffInfo.getStdNotationStaffSize());
}

void KeySignaturePainter::mousePressEvent(QGraphicsSceneMouseEvent *)
{
}

void KeySignaturePainter::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    QMessageBox message;
    message.setText("Key Signature");
    message.exec();
}

void KeySignaturePainter::mouseMoveEvent(QGraphicsSceneMouseEvent *)
{
}

void KeySignaturePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setFont(musicFont);

    // draw the appropriate accidentals
    if (keySignature.UsesFlats())
    {
        drawAccidentals(flatPositions, MusicFont::AccidentalFlat, painter);
    }
    else
    {
        drawAccidentals(sharpPositions, MusicFont::AccidentalSharp, painter);
    }
}

void KeySignaturePainter::adjustHeightOffset(QVector<double>& lst)
{
    for (auto i = lst.begin(); i != lst.end(); ++i)
    {
        *i -= staffInfo.getTopStdNotationLine();
    }
}

void KeySignaturePainter::drawAccidentals(QVector<double>& positions, QChar accidental, QPainter* painter)
{
    if (keySignature.IsCancellation()) // display natural if a cancellation occurs
    {
        accidental = MusicFont::Natural; // draw using naturals
    }

    for(int i = 0; i < keySignature.GetKeyAccidentalsIncludingCancel(); i++)
    {
        painter->drawText(i * KeySignature::ACCIDENTAL_WIDTH, positions.at(i), accidental);
    }
}

void KeySignaturePainter::initAccidentalPositions()
{
    flatPositions.resize(7);
    sharpPositions.resize(7);

    // generate the positions for the key signature accidentals
    flatPositions.replace(0, staffInfo.getStdNotationLineHeight(3));
    flatPositions.replace(1, staffInfo.getStdNotationSpaceHeight(1));
    flatPositions.replace(2, staffInfo.getStdNotationSpaceHeight(3));
    flatPositions.replace(3, staffInfo.getStdNotationLineHeight(2));
    flatPositions.replace(4, staffInfo.getStdNotationLineHeight(4));
    flatPositions.replace(5, staffInfo.getStdNotationSpaceHeight(2));
    flatPositions.replace(6, staffInfo.getStdNotationSpaceHeight(4));

    sharpPositions.replace(0, staffInfo.getStdNotationLineHeight(1));
    sharpPositions.replace(1, staffInfo.getStdNotationSpaceHeight(2));
    sharpPositions.replace(2, staffInfo.getStdNotationSpaceHeight(0));
    sharpPositions.replace(3, staffInfo.getStdNotationLineHeight(2));
    sharpPositions.replace(4, staffInfo.getStdNotationSpaceHeight(3));
    sharpPositions.replace(5, staffInfo.getStdNotationSpaceHeight(1));
    sharpPositions.replace(6, staffInfo.getStdNotationLineHeight(3));

    adjustHeightOffset(flatPositions);
    adjustHeightOffset(sharpPositions);
}
