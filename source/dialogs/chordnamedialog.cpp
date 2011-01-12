#include "chordnamedialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QListWidget>

#include <musicfont.h>

ChordNameDialog::ChordNameDialog(ChordName* chord, QWidget *parent) :
        QDialog(parent),
        originalChordName(chord),
        tonicKeyOptions(7),
        bassKeyOptions(7)
{
    chordName = *originalChordName; // make a temporary copy for editing

    setWindowTitle(tr("Chord Name"));
    setModal(true);

    QFont musicFont = MusicFont().getFont();

    preview = new QLineEdit;
    preview->setReadOnly(true);
    preview->setAlignment(Qt::AlignCenter);

    noChord = new QCheckBox(tr("&No Chord"));
    connect(noChord, SIGNAL(clicked()), this, SLOT(updateData()));

    usesBrackets = new QCheckBox(tr("&Brackets"));
    connect(usesBrackets, SIGNAL(clicked()), this, SLOT(updateData()));

    toggleSharps = new QPushButton();
    toggleSharps->setFont(musicFont);
    toggleSharps->setCheckable(true);
    toggleSharps->setText(MusicFont().getSymbol(MusicFont::AccidentalSharp));
    toggleSharps->setToolTip("Toggle Sharps");
    connect(toggleSharps, SIGNAL(toggled(bool)), this, SLOT(updateData()));

    toggleFlats = new QPushButton();
    toggleFlats->setFont(musicFont);
    toggleFlats->setCheckable(true);
    toggleFlats->setText(MusicFont().getSymbol(MusicFont::AccidentalFlat));
    toggleFlats->setToolTip("Toggle Flats");
    connect(toggleFlats, SIGNAL(toggled(bool)), this, SLOT(updateData()));

    tonicKey = new QButtonGroup;
    initKeyOptions(tonicKeyOptions, tonicKey);

    bassKey = new QButtonGroup;
    initKeyOptions(bassKeyOptions, bassKey);

    formulaList = new QListWidget;
    connect(formulaList, SIGNAL(currentRowChanged(int)), this, SLOT(updateData()));

    add2 = new QCheckBox(tr("add2"));
    connect(add2, SIGNAL(clicked()), this, SLOT(updateData()));
    add4 = new QCheckBox(tr("add4"));
    connect(add4, SIGNAL(clicked()), this, SLOT(updateData()));
    add6 = new QCheckBox(tr("add6"));
    connect(add6, SIGNAL(clicked()), this, SLOT(updateData()));
    add9 = new QCheckBox(tr("add9"));
    connect(add9, SIGNAL(clicked()), this, SLOT(updateData()));
    add11 = new QCheckBox(tr("add11"));
    connect(add11, SIGNAL(clicked()), this, SLOT(updateData()));

    QHBoxLayout* additionsGroup = new QHBoxLayout;
    additionsGroup->addWidget(add2);
    additionsGroup->addWidget(add4);
    additionsGroup->addWidget(add6);
    additionsGroup->addWidget(add9);
    additionsGroup->addWidget(add11);

    extended9th = new QCheckBox(tr("9"));
    connect(extended9th, SIGNAL(clicked()), this, SLOT(updateData()));
    extended11th = new QCheckBox(tr("11"));
    connect(extended11th, SIGNAL(clicked()), this, SLOT(updateData()));
    extended13th = new QCheckBox(tr("13"));
    connect(extended13th, SIGNAL(clicked()), this, SLOT(updateData()));

    QHBoxLayout* extensionsGroup = new QHBoxLayout;
    extensionsGroup->addWidget(extended9th);
    extensionsGroup->addWidget(extended11th);
    extensionsGroup->addWidget(extended13th);

    flatted5th = new QCheckBox(tr("b5"));
    connect(flatted5th, SIGNAL(clicked()), this, SLOT(updateData()));
    raised5th = new QCheckBox(tr("+5"));
    connect(raised5th, SIGNAL(clicked()), this, SLOT(updateData()));
    flatted9th = new QCheckBox(tr("b9"));
    connect(flatted9th, SIGNAL(clicked()), this, SLOT(updateData()));
    raised9th = new QCheckBox(tr("+9"));
    connect(raised9th, SIGNAL(clicked()), this, SLOT(updateData()));
    raised11th = new QCheckBox(tr("+11"));
    connect(raised11th, SIGNAL(clicked()), this, SLOT(updateData()));
    flatted13th = new QCheckBox(tr("b13"));
    connect(flatted13th, SIGNAL(clicked()), this, SLOT(updateData()));
    suspended2nd = new QCheckBox(tr("sus2"));
    connect(suspended2nd, SIGNAL(clicked()), this, SLOT(updateData()));
    suspended4th = new QCheckBox(tr("sus4"));
    connect(suspended4th, SIGNAL(clicked()), this, SLOT(updateData()));

    QHBoxLayout* alterationsGroup = new QHBoxLayout;
    alterationsGroup->addWidget(flatted5th);
    alterationsGroup->addWidget(raised5th);
    alterationsGroup->addWidget(flatted9th);
    alterationsGroup->addWidget(raised9th);
    alterationsGroup->addWidget(raised11th);
    alterationsGroup->addWidget(flatted13th);
    alterationsGroup->addWidget(suspended2nd);
    alterationsGroup->addWidget(suspended4th);

    QHBoxLayout* tonicLayout = new QHBoxLayout;
    tonicLayout->addWidget(noChord);
    foreach(QPushButton* button, tonicKeyOptions)
    {
        tonicLayout->addWidget(button);
    }
    tonicLayout->addWidget(toggleFlats);
    tonicLayout->addWidget(toggleSharps);
    tonicLayout->addWidget(usesBrackets);

    QHBoxLayout* bassLayout = new QHBoxLayout;
    foreach(QPushButton* button, bassKeyOptions)
    {
        bassLayout->addWidget(button);
    }

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Preview:"), preview);
    formLayout->addRow(tr("Key:"), tonicLayout);
    formLayout->addRow(tr("Formula:"), formulaList);
    formLayout->addRow(tr("Additions:"), additionsGroup);
    formLayout->addRow(tr("Extensions:"), extensionsGroup);
    formLayout->addRow(tr("Alterations:"), alterationsGroup);
    formLayout->addRow(tr("Bass Note"), bassLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addLayout(formLayout);
    buttonsLayout->addWidget(buttonBox);

    buttonsLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(buttonsLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    init();
}

void ChordNameDialog::init()
{
    preview->setText(QString().fromStdString(chordName.GetText()));

    noChord->setChecked(chordName.IsNoChord());
    usesBrackets->setChecked(chordName.HasBrackets());

    quint8 key=0, variation=0;
    chordName.GetTonic(key, variation);

    // TODO - make sure these can't be simultaneously checked
    toggleSharps->setChecked(variation == ChordName::variationUp);
    toggleFlats->setChecked(variation == ChordName::variationDown);

    tonicKey->button(key)->setChecked(true);

    chordName.GetBassNote(key, variation);
    bassKey->button(key)->setChecked(true);

    ChordName tempChord;
    // Add chord formula text items
    for (int i = ChordName::major; i <= ChordName::minor7thFlatted5th; i++)
    {
        tempChord.SetFormula(i);
        if (i == ChordName::major)
        {
            // C major is written as just 'C', but we need to explicitly show that this option is for major chords
            new QListWidgetItem("maj", formulaList);
        }
        else
        {
            new QListWidgetItem(QString().fromStdString(tempChord.GetFormulaText()), formulaList);
        }

        if (chordName.GetFormula() == i)
        {
            formulaList->setCurrentRow(i);
        }
    }

    // Chord additions
    add2->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::added2nd));
    add4->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::added4th));
    add6->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::added6th));
    add9->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::added9th));
    add11->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::added11th));

    // Chord extensions
    extended9th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::extended9th));
    extended11th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::extended11th));
    extended13th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::extended13th));

    // Chord alterations
    flatted5th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::flatted5th));
    raised5th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::raised5th));
    flatted9th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::flatted9th));
    raised9th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::raised9th));
    raised11th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::raised11th));
    flatted13th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::flatted13th));
    suspended2nd->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::suspended2nd));
    suspended4th->setChecked(chordName.IsFormulaModificationFlagSet(ChordName::suspended4th));
}

void ChordNameDialog::updateData()
{
    chordName.SetNoChord(noChord->isChecked());
    chordName.SetBrackets(usesBrackets->isChecked());

    quint8 key=0, variation=0;
    chordName.GetTonic(key, variation);

    // tonic key
    foreach(QPushButton* button, tonicKeyOptions)
    {
        if (button->isChecked())
        {
            key = tonicKey->checkedId();
        }
    }

    // set to updated key, with default variation
    chordName.SetTonic(key, ChordName::variationDefault);
    if (toggleSharps->isChecked())
    {
        chordName.SetTonic(key, ChordName::variationUp);
    }
    if (toggleFlats->isChecked())
    {
        chordName.SetTonic(key, ChordName::variationDown);
    }

    // Chord formula
    chordName.SetFormula(formulaList->currentRow());

    // Chord additions
    setIfChecked(add2, ChordName::added2nd);
    setIfChecked(add4, ChordName::added4th);
    setIfChecked(add6, ChordName::added6th);
    setIfChecked(add9, ChordName::added9th);
    setIfChecked(add11, ChordName::added11th);

    // Chord extensions
    setIfChecked(extended9th, ChordName::extended9th);
    setIfChecked(extended11th, ChordName::extended11th);
    setIfChecked(extended13th, ChordName::extended13th);

    // Chord alterations
    setIfChecked(flatted5th, ChordName::flatted5th);
    setIfChecked(raised5th, ChordName::raised5th);
    setIfChecked(flatted9th, ChordName::flatted9th);
    setIfChecked(raised9th, ChordName::raised9th);
    setIfChecked(raised11th, ChordName::raised11th);
    setIfChecked(flatted13th, ChordName::flatted13th);
    setIfChecked(suspended2nd, ChordName::suspended2nd);
    setIfChecked(suspended4th, ChordName::suspended4th);

    // bass key
    chordName.GetBassNote(key, variation);
    foreach(QPushButton* button, bassKeyOptions)
    {
        if (button->isChecked())
        {
            key = bassKey->checkedId();
        }
    }
    chordName.SetBassNote(key, variation);

    // update preview
    preview->setText(QString().fromStdString(chordName.GetText()));
}

void ChordNameDialog::accept()
{
    *originalChordName = chordName;
    done(QDialog::Accepted);
}

void ChordNameDialog::reject()
{
    done(QDialog::Rejected);
}

void ChordNameDialog::setIfChecked(QCheckBox* checkBox, quint16 flag)
{
    if (checkBox->isChecked())
    {
        chordName.SetFormulaModificationFlag(flag);
    }
    else
    {
        chordName.ClearFormulaModificationFlag(flag);
    }
}

void ChordNameDialog::initKeyOptions(QVector<QPushButton*>& buttons, QButtonGroup* group)
{
    const int numKeys = 7;
    buttons[0] = new QPushButton("C");
    buttons[1] = new QPushButton("D");
    buttons[2] = new QPushButton("E");
    buttons[3] = new QPushButton("F");
    buttons[4] = new QPushButton("G");
    buttons[5] = new QPushButton("A");
    buttons[6] = new QPushButton("B");
    group->addButton(buttons[0], ChordName::C);
    group->addButton(buttons[1], ChordName::D);
    group->addButton(buttons[2], ChordName::E);
    group->addButton(buttons[3], ChordName::F);
    group->addButton(buttons[4], ChordName::G);
    group->addButton(buttons[5], ChordName::A);
    group->addButton(buttons[6], ChordName::B);

    for (int i = 0; i < numKeys; i++)
    {
        buttons.at(i)->setCheckable(true);
        connect(buttons.at(i), SIGNAL(toggled(bool)), this, SLOT(updateData()));
    }
}
