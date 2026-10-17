/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "toolbox.h"
#include "ui_toolbox.h"

#include <app/command.h>

#include <QAction>
#include <QEvent>
#include <QGridLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QShowEvent>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>

ToolBox::ToolBox(Command *keySignatureCommand,
                 Command *timeSignatureCommand,
                 Command *standardBarlineCommand,
                 Command *repeatEndingCommand,
                 Command *octave8vaCommand,
                 Command *octave15maCommand,
                 Command *octave8vbCommand,
                 Command *octave15mbCommand,
                 Command *wholeNoteCommand,
                 Command *halfNoteCommand,
                 Command *quarterNoteCommand,
                 Command *eighthNoteCommand,
                 Command *sixteenthNoteCommand,
                 Command *thirtySecondNoteCommand,
                 Command *sixtyFourthNoteCommand,
                 Command *addRestCommand,
                 Command *dottedCommand,
                 Command *doubleDottedCommand,
                 Command *tripletCommand,
                 Command *irregularGroupingCommand,
                 Command *tieCommand,
                 Command *fermataCommand,
                 Command *tempoMarkerCommand,
                 Command *multibarRestCommand,
                 Command *dynamicPPPCommand,
                 Command *dynamicPPCommand,
                 Command *dynamicPCommand,
                 Command *dynamicMPCommand,
                 Command *dynamicMFCommand,
                 Command *dynamicFCommand,
                 Command *dynamicFFCommand,
                 Command *dynamicFFFCommand,
                 Command *mutedCommand,
                 Command *ghostNoteCommand,
                 Command *marcatoCommand,
                 Command *sforzandoCommand,
                 Command *staccatoCommand,
                 Command *letRingCommand,
                 Command *palmMuteCommand,
                 Command *tapCommand,
                 Command *naturalHarmonicCommand,
                 Command *artificialHarmonicCommand,
                 Command *tappedHarmonicCommand,
                 Command *bendCommand,
                 Command *tremoloBarCommand,
                 Command *vibratoCommand,
                 Command *wideVibratoCommand,
                 Command *graceNoteCommand,
                 Command *leftHandFingeringCommand,
                 Command *hammerPullCommand,
                 Command *trillCommand,
                 Command *legatoSlideCommand,
                 Command *shiftSlideCommand,
                 Command *slideIntoFromAboveCommand,
                 Command *slideIntoFromBelowCommand,
                 Command *slideOutOfDownwardsCommand,
                 Command *slideOutOfUpwardsCommand,
                 Command *pickStrokeUpCommand,
                 Command *pickStrokeDownCommand,
                 Command *arpeggioUpCommand,
                 Command *arpeggioDownCommand,
                 Command *textCommand,
                 Command *directionCommand,
                 QWidget *parent,
                 int iconsPerRow) :
    QWidget(parent),
    ui(new Ui::ToolBox),
    myIconsPerRow(iconsPerRow > 0 ? iconsPerRow : DEFAULT_ICONS_PER_ROW),
    myScrollArea(nullptr),
    myLastWidth(-1),
    myLastHeight(-1)
{
    ui->setupUi(this);

    setMinimumSize(0, 0);
    ui->gridLayoutWidget->setMinimumSize(0, 0);

    myButtons = {
        // Row 0
        ui->keySignatureButton,
        ui->timeSignatureButton,
        ui->standardBarlineButton,
        ui->repeatEndingButton,
        ui->octave8vaButton,
        ui->octave15maButton,
        ui->octave8vbButton,
        ui->octave15mbButton,

        // Row 1
        ui->wholeNoteButton,
        ui->halfNoteButton,
        ui->quarterNoteButton,
        ui->eighthNoteButton,
        ui->sixteenthNoteButton,
        ui->thirtySecondNoteButton,
        ui->sixtyFourthNoteButton,
        ui->addRestButton,

        // Row 2
        ui->dottedButton,
        ui->doubleDottedButton,
        ui->tripletButton,
        ui->irregularGroupingButton,
        ui->tieButton,
        ui->fermataButton,
        ui->tempoMarkerButton,
        ui->multibarRestButton,

        // Row 3
        ui->dynamicPPPButton,
        ui->dynamicPPButton,
        ui->dynamicPButton,
        ui->dynamicMPButton,
        ui->dynamicMFButton,
        ui->dynamicFButton,
        ui->dynamicFFButton,
        ui->dynamicFFFButton,

        // Row 4
        ui->mutedButton,
        ui->ghostNoteButton,
        ui->marcatoButton,
        ui->sforzandoButton,
        ui->staccatoButton,
        ui->letRingButton,
        ui->palmMuteButton,
        ui->tapButton,

        // Row 5
        ui->naturalHarmonicButton,
        ui->artificialHarmonicButton,
        ui->tappedHarmonicButton,
        ui->bendButton,
        ui->tremoloBarButton,
        ui->vibratoButton,
        ui->wideVibratoButton,
        ui->graceNoteButton,

        // Row 6
        ui->leftHandFingeringButton,
        ui->hammerPullButton,
        ui->trillButton,
        ui->legatoSlideButton,
        ui->shiftSlideButton,
        ui->slideIntoFromAboveButton,
        ui->slideIntoFromBelowButton,
        ui->slideOutOfDownwardsButton,

        // Row 7
        ui->slideOutOfUpwardsButton,
        ui->pickStrokeUpButton,
        ui->pickStrokeDownButton,
        ui->arpeggioUpButton,
        ui->arpeggioDownButton,
        ui->textButton,
        ui->directionButton
    };

    ui->keySignatureButton->setDefaultAction(keySignatureCommand);
    ui->timeSignatureButton->setDefaultAction(timeSignatureCommand);
    ui->standardBarlineButton->setDefaultAction(standardBarlineCommand);
    ui->repeatEndingButton->setDefaultAction(repeatEndingCommand);
    ui->octave8vaButton->setDefaultAction(octave8vaCommand);
    ui->octave15maButton->setDefaultAction(octave15maCommand);
    ui->octave8vbButton->setDefaultAction(octave8vbCommand);
    ui->octave15mbButton->setDefaultAction(octave15mbCommand);

    ui->wholeNoteButton->setDefaultAction(wholeNoteCommand);
    ui->halfNoteButton->setDefaultAction(halfNoteCommand);
    ui->quarterNoteButton->setDefaultAction(quarterNoteCommand);
    ui->eighthNoteButton->setDefaultAction(eighthNoteCommand);
    ui->sixteenthNoteButton->setDefaultAction(sixteenthNoteCommand);
    ui->thirtySecondNoteButton->setDefaultAction(thirtySecondNoteCommand);
    ui->sixtyFourthNoteButton->setDefaultAction(sixtyFourthNoteCommand);
    ui->addRestButton->setDefaultAction(addRestCommand);

    ui->dottedButton->setDefaultAction(dottedCommand);
    ui->doubleDottedButton->setDefaultAction(doubleDottedCommand);
    ui->tripletButton->setDefaultAction(tripletCommand);
    ui->irregularGroupingButton->setDefaultAction(irregularGroupingCommand);
    ui->tieButton->setDefaultAction(tieCommand);
    ui->fermataButton->setDefaultAction(fermataCommand);
    ui->tempoMarkerButton->setDefaultAction(tempoMarkerCommand);
    ui->multibarRestButton->setDefaultAction(multibarRestCommand);

    ui->dynamicPPPButton->setDefaultAction(dynamicPPPCommand);
    ui->dynamicPPButton->setDefaultAction(dynamicPPCommand);
    ui->dynamicPButton->setDefaultAction(dynamicPCommand);
    ui->dynamicMPButton->setDefaultAction(dynamicMPCommand);
    ui->dynamicMFButton->setDefaultAction(dynamicMFCommand);
    ui->dynamicFButton->setDefaultAction(dynamicFCommand);
    ui->dynamicFFButton->setDefaultAction(dynamicFFCommand);
    ui->dynamicFFFButton->setDefaultAction(dynamicFFFCommand);

    ui->mutedButton->setDefaultAction(mutedCommand);
    ui->ghostNoteButton->setDefaultAction(ghostNoteCommand);
    ui->marcatoButton->setDefaultAction(marcatoCommand);
    ui->sforzandoButton->setDefaultAction(sforzandoCommand);
    ui->staccatoButton->setDefaultAction(staccatoCommand);
    ui->letRingButton->setDefaultAction(letRingCommand);
    ui->palmMuteButton->setDefaultAction(palmMuteCommand);
    ui->tapButton->setDefaultAction(tapCommand);

    ui->naturalHarmonicButton->setDefaultAction(naturalHarmonicCommand);
    ui->artificialHarmonicButton->setDefaultAction(artificialHarmonicCommand);
    ui->tappedHarmonicButton->setDefaultAction(tappedHarmonicCommand);
    ui->bendButton->setDefaultAction(bendCommand);
    ui->tremoloBarButton->setDefaultAction(tremoloBarCommand);
    ui->vibratoButton->setDefaultAction(vibratoCommand);
    ui->wideVibratoButton->setDefaultAction(wideVibratoCommand);
    ui->graceNoteButton->setDefaultAction(graceNoteCommand);
    ui->leftHandFingeringButton->setDefaultAction(leftHandFingeringCommand);

    ui->hammerPullButton->setDefaultAction(hammerPullCommand);
    ui->trillButton->setDefaultAction(trillCommand);
    ui->legatoSlideButton->setDefaultAction(legatoSlideCommand);
    ui->shiftSlideButton->setDefaultAction(shiftSlideCommand);
    ui->slideIntoFromAboveButton->setDefaultAction(slideIntoFromAboveCommand);
    ui->slideIntoFromBelowButton->setDefaultAction(slideIntoFromBelowCommand);
    ui->slideOutOfDownwardsButton->setDefaultAction(slideOutOfDownwardsCommand);
    ui->slideOutOfUpwardsButton->setDefaultAction(slideOutOfUpwardsCommand);

    ui->pickStrokeUpButton->setDefaultAction(pickStrokeUpCommand);
    ui->pickStrokeDownButton->setDefaultAction(pickStrokeDownCommand);
    ui->arpeggioUpButton->setDefaultAction(arpeggioUpCommand);
    ui->arpeggioDownButton->setDefaultAction(arpeggioDownCommand);
    ui->textButton->setDefaultAction(textCommand);
    ui->directionButton->setDefaultAction(directionCommand);

    myScrollArea = new QScrollArea(this);
    myScrollArea->setFrameShape(QFrame::NoFrame);
    myScrollArea->setWidgetResizable(true);
    myScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    myScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    myScrollArea->setWidget(ui->gridLayoutWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(myScrollArea);

    myScrollArea->viewport()->installEventFilter(this);

    reorganizeGrid();
    updateButtonSizes(true);
}

ToolBox::~ToolBox()
{
    delete ui;
}

int ToolBox::getIconsPerRow() const
{
    return myIconsPerRow;
}

void ToolBox::setIconsPerRow(int count)
{
    if (count <= 0) {
        count = 1;
    }
    if (myIconsPerRow != count) {
        myIconsPerRow = count;
        reorganizeGrid();
        updateButtonSizes(true);
    }
}

QSize ToolBox::sizeHint() const
{
    int iconWidth = width() > 0 ? std::max(16, width() / myIconsPerRow) : 34;
    int w = iconWidth * myIconsPerRow;
    int numRows = (static_cast<int>(myButtons.size()) + myIconsPerRow - 1) / myIconsPerRow;
    int h = iconWidth * numRows;
    return QSize(w, h);
}

QSize ToolBox::minimumSizeHint() const
{
    int minIconWidth = 16;
    return QSize(minIconWidth * myIconsPerRow, minIconWidth * 2);
}

void ToolBox::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateButtonSizes();
}

void ToolBox::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateButtonSizes();
}

bool ToolBox::eventFilter(QObject *watched, QEvent *event)
{
    if (myScrollArea && watched == myScrollArea->viewport() && event->type() == QEvent::Resize) {
        updateButtonSizes();
    }
    return QWidget::eventFilter(watched, event);
}

void ToolBox::reorganizeGrid()
{
    delete ui->gridLayoutWidget->layout();

    QGridLayout *grid = new QGridLayout(ui->gridLayoutWidget);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    grid->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    for (size_t i = 0; i < myButtons.size(); ++i) {
        int row = static_cast<int>(i) / myIconsPerRow;
        int col = static_cast<int>(i) % myIconsPerRow;
        grid->addWidget(myButtons[i], row, col);
    }

    ui->gridLayout = grid;
}

void ToolBox::updateButtonSizes(bool force)
{
    int w = width();
    int h = height();
    if (w <= 0) {
        w = 272;
    }
    if (h <= 0) {
        h = 500;
    }

    if (!force && w == myLastWidth && h == myLastHeight) {
        return;
    }
    myLastWidth = w;
    myLastHeight = h;

    int scrollBarWidth = 16;
    if (myScrollArea && myScrollArea->style()) {
        int extent = myScrollArea->style()->pixelMetric(
            QStyle::PM_ScrollBarExtent, nullptr, myScrollArea->verticalScrollBar());
        if (extent > 0) {
            scrollBarWidth = extent;
        }
    }

    int numRows = (static_cast<int>(myButtons.size()) + myIconsPerRow - 1) / myIconsPerRow;

    int iconWidth1 = std::max(16, w / myIconsPerRow);
    int totalHeight1 = numRows * iconWidth1;

    int iconWidth2 = std::max(16, (w - scrollBarWidth) / myIconsPerRow);
    int totalHeight2 = numRows * iconWidth2;

    int chosenIconWidth;
    if (totalHeight1 <= h) {
        if (myScrollArea) {
            myScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
        chosenIconWidth = iconWidth1;
    } else if (totalHeight2 <= h) {
        if (myScrollArea) {
            myScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
        chosenIconWidth = iconWidth2;
    } else {
        if (myScrollArea) {
            myScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }
        chosenIconWidth = iconWidth2;
    }

    int padding = std::max(4, chosenIconWidth / 6);
    if (padding % 2 != 0) {
        ++padding;
    }
    int innerIconSize = std::max(12, chosenIconWidth - padding);

    QSize btnSize(chosenIconWidth, chosenIconWidth);
    QSize iconSize(innerIconSize, innerIconSize);

    for (QToolButton *btn : myButtons) {
        if (btn) {
            btn->setFixedSize(btnSize);
            btn->setIconSize(iconSize);
        }
    }
}
