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
#include <QSignalMapper>
#include <QIcon>

ChordNameDialog::ChordNameDialog(ChordName* chord, QWidget *parent) :
        QDialog(parent),
        originalChordName(chord),
        tonicKeyOptions(7),
        bassKeyOptions(7)
{
    chordName = *originalChordName; // make a temporary copy for editing

    setWindowTitle(tr("Chord Name"));
    setModal(true);

    const QIcon sharpIcon(":/images/sharp.png");
    const QIcon flatIcon(":/images/flat.png");

    preview = new QLineEdit;
    preview->setReadOnly(true);
    preview->setAlignment(Qt::AlignCenter);

    noChord = new QCheckBox;
    initCheckBox(noChord, tr("&No Chord"));
    usesBrackets = new QCheckBox;
    initCheckBox(usesBrackets, tr("&Brackets"));

    sharpFlatMapper = new QSignalMapper;

    toggleSharps = new QPushButton(sharpIcon, "");
    toggleSharps->setIconSize(QSize(24, 24));
    toggleSharps->setCheckable(true);
    toggleSharps->setToolTip("Toggle Sharps");
    connect(toggleSharps, SIGNAL(released()), sharpFlatMapper, SLOT(map()));
    sharpFlatMapper->setMapping(toggleSharps, toggleSharps);

    toggleFlats = new QPushButton(flatIcon, "");
    toggleFlats->setIconSize(QSize(24, 24));
    toggleFlats->setCheckable(true);
    toggleFlats->setToolTip("Toggle Flats");
    connect(toggleFlats, SIGNAL(released()), sharpFlatMapper, SLOT(map()));
    sharpFlatMapper->setMapping(toggleFlats, toggleFlats);

    connect(sharpFlatMapper, SIGNAL(mapped(QWidget*)), this, SLOT(toggleSharpFlat(QWidget*)));

    sharpFlatBassMapper = new QSignalMapper;

    toggleBassSharps = new QPushButton(sharpIcon, "");
    toggleBassSharps->setIconSize(QSize(24, 24));
    toggleBassSharps->setCheckable(true);
    toggleBassSharps->setToolTip("Toggle Sharps");
    connect(toggleBassSharps, SIGNAL(released()), sharpFlatBassMapper, SLOT(map()));
    sharpFlatBassMapper->setMapping(toggleBassSharps, toggleBassSharps);

    toggleBassFlats = new QPushButton(flatIcon, "");
    toggleBassFlats->setIconSize(QSize(24, 24));
    toggleBassFlats->setCheckable(true);
    toggleBassFlats->setToolTip("Toggle Flats");
    connect(toggleBassFlats, SIGNAL(released()), sharpFlatBassMapper, SLOT(map()));
    sharpFlatBassMapper->setMapping(toggleBassFlats, toggleBassFlats);

    connect(sharpFlatBassMapper, SIGNAL(mapped(QWidget*)), this, SLOT(toggleSharpFlat(QWidget*)));

    tonicKey = new QButtonGroup;
    initKeyOptions(tonicKeyOptions, tonicKey);
    connect(tonicKey, SIGNAL(buttonClicked(int)), this, SLOT(updateTonicNote(int)));

    bassKey = new QButtonGroup;
    initKeyOptions(bassKeyOptions, bassKey);
    connect(bassKey, SIGNAL(buttonClicked(int)), this, SLOT(updateBassNote(int)));

    formulaList = new QListWidget;
    connect(formulaList, SIGNAL(currentRowChanged(int)), this, SLOT(updateData()));

    add2 = new QCheckBox;
    initCheckBox(add2, "add2");
    add4 = new QCheckBox;
    initCheckBox(add4,tr("add4"));
    add6 = new QCheckBox;
    initCheckBox(add6,tr("add6"));
    add9 = new QCheckBox;
    initCheckBox(add9,tr("add9"));
    add11 = new QCheckBox;
    initCheckBox(add11,tr("add11"));

    QHBoxLayout* additionsGroup = new QHBoxLayout;
    additionsGroup->addWidget(add2);
    additionsGroup->addWidget(add4);
    additionsGroup->addWidget(add6);
    additionsGroup->addWidget(add9);
    additionsGroup->addWidget(add11);

    extended9th = new QCheckBox;
    initCheckBox(extended9th,tr("9"));
    extended11th = new QCheckBox;
    initCheckBox(extended11th,tr("11"));
    extended13th = new QCheckBox;
    initCheckBox(extended13th,tr("13"));

    QHBoxLayout* extensionsGroup = new QHBoxLayout;
    extensionsGroup->addWidget(extended9th);
    extensionsGroup->addWidget(extended11th);
    extensionsGroup->addWidget(extended13th);

    flatted5th = new QCheckBox;
    initCheckBox(flatted5th,tr("b5"));
    raised5th = new QCheckBox;
    initCheckBox(raised5th,tr("+5"));
    flatted9th = new QCheckBox;
    initCheckBox(flatted9th,tr("b9"));
    raised9th = new QCheckBox;
    initCheckBox(raised9th,tr("+9"));
    raised11th = new QCheckBox;
    initCheckBox(raised11th,tr("+11"));
    flatted13th = new QCheckBox;
    initCheckBox(flatted13th,tr("b13"));
    suspended2nd = new QCheckBox;
    initCheckBox(suspended2nd,tr("sus2"));
    suspended4th = new QCheckBox;
    initCheckBox(suspended4th,tr("sus4"));

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
    bassLayout->addWidget(toggleBassFlats);
    bassLayout->addWidget(toggleBassSharps);

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

    toggleFlats->setChecked(variation == ChordName::variationDown);
    tonicKey->button(key)->setChecked(true);

    chordName.GetBassNote(key, variation);
    bassKey->button(key)->setChecked(true);
    toggleBassSharps->setChecked(variation == ChordName::variationUp);
    toggleBassFlats->setChecked(variation == ChordName::variationDown);

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

    // update preview
    preview->setText(QString().fromStdString(chordName.GetText()));
}

void ChordNameDialog::accept()
{
    *originalChordName = chordName;
    done(QDialog::Accepted);
}

// Convenience function for setting formula flags
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

// Initializes the buttons for each note
// needed twice - for tonic and bass notes
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
    }
}

void ChordNameDialog::updateTonicNote(int key)
{
    updateNote(key, false);
}

void ChordNameDialog::updateBassNote(int key)
{
    updateNote(key, true);
}

// Updates the internal note representation, depending on whether flats or sharps are used
// Is used by the tonic and bass note selectors
void ChordNameDialog::updateNote(int key, bool bass)
{
    quint8 displayNote = 0;

    bool sharps = toggleSharps->isChecked();
    bool flats = toggleFlats->isChecked();
    if (bass)
    {
        sharps = toggleBassSharps->isChecked();
        flats = toggleBassFlats->isChecked();
    }

    // rather ugly, but I don't think there's a cleaner way to do this without rewriting the ChordName class
    switch(key)
    {
    case ChordName::C:
        if (sharps) // C#
        {
            setNote(ChordName::CSharp, ChordName::variationDefault, bass);
        }
        else if (flats) // Cb
        {
            setNote(ChordName::B, ChordName::variationUp, bass);
        }
        else // C
        {
            setNote(ChordName::C, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::C;
        break;
    case ChordName::D:
        if (sharps) // D#
        {
            setNote(ChordName::EFlat, ChordName::variationDown, bass);
        }
        else if (flats) // Db
        {
            setNote(ChordName::CSharp, ChordName::variationUp, bass);
        }
        else // D
        {
            setNote(ChordName::D, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::D;
        break;
    case ChordName::E:
        if (sharps) // E#
        {
            setNote(ChordName::F, ChordName::variationDown, bass);
        }
        else if (flats) // Eb
        {
            setNote(ChordName::EFlat, ChordName::variationDefault, bass);
        }
        else // E
        {
            setNote(ChordName::E, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::E;
        break;
    case ChordName::F:
        if (sharps) // F#
        {
            setNote(ChordName::FSharp, ChordName::variationDefault, bass);
        }
        else if (flats) // Fb
        {
            setNote(ChordName::E, ChordName::variationUp, bass);
        }
        else // F
        {
            setNote(ChordName::F, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::F;
        break;
    case ChordName::G:
        if (sharps) // G#
        {
            setNote(ChordName::AFlat, ChordName::variationDown, bass);
        }
        else if (flats) // Gb
        {
            setNote(ChordName::FSharp, ChordName::variationUp, bass);
        }
        else // G
        {
            setNote(ChordName::G, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::G;
        break;
    case ChordName::A:
        if (sharps) // A#
        {
            setNote(ChordName::BFlat, ChordName::variationDown, bass);
        }
        else if (flats) // Ab
        {
            setNote(ChordName::AFlat, ChordName::variationDefault, bass);
        }
        else // A
        {
            setNote(ChordName::A, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::A;
        break;
    case ChordName::B:
        if (sharps) // B#
        {
            setNote(ChordName::C, ChordName::variationDown, bass);
        }
        else if (flats) // Bb
        {
            setNote(ChordName::BFlat, ChordName::variationDefault, bass);
        }
        else // B
        {
            setNote(ChordName::B, ChordName::variationDefault, bass);
        }
        displayNote = ChordName::B;
        break;
    }

    if (!bass)
    {
        tonicKey->button(displayNote)->setChecked(true);
    }
    else
    {
        bassKey->button(displayNote)->setChecked(true);
    }

    if (!bass) // switch the bass note along with the tonic
    {
        quint8 variation = 0, actualKey = 0;
        chordName.GetTonic(actualKey, variation);
        chordName.SetBassNote(actualKey, variation);
        bassKey->button(displayNote)->setChecked(true);
    }

    updateData(); // update the preview
}

// Ensures that only sharps or only flats are selected
// Also allows for neither to be selected, unlike QButtonGroup
void ChordNameDialog::toggleSharpFlat(QWidget* button)
{
    if (toggleFlats == button || toggleSharps == button)
    {
        if (toggleFlats == button)
        {
            toggleSharps->setChecked(false);
        }
        else if (toggleSharps == button)
        {
            toggleFlats->setChecked(false);
        }

        quint8 key = tonicKey->checkedId();
        updateTonicNote(key);
    }

    if (toggleBassFlats == button || toggleBassSharps == button)
    {
        if (toggleBassFlats == button)
        {
            toggleBassSharps->setChecked(false);
        }
        else if (toggleBassSharps == button)
        {
            toggleBassFlats->setChecked(false);
        }

        quint8 key = bassKey->checkedId();
        updateBassNote(key);
    }

}

// Helper function for initializing checkboxes with labels, and connecting them to updateData()
inline void ChordNameDialog::initCheckBox(QCheckBox *checkBox, const QString& text)
{
    checkBox->setText(text);
    connect(checkBox, SIGNAL(clicked()), this, SLOT(updateData()));
}

// Helper function for setting the tonic or bass note
void ChordNameDialog::setNote(quint8 key, quint8 variation, bool bass)
{
    if (bass)
    {
        chordName.SetBassNote(key, variation);
    }
    else
    {
        chordName.SetTonic(key, variation);
    }
}

