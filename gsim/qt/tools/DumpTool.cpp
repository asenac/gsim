// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DumpTool.cpp
 *
 * GSIM is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSIM is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DumpTool.hpp"
#include <gsim/qt/proc/DumpProcessor.hpp>
#include <QHBoxLayout>
#include <QTreeView>

#include <gsim/qt/widgets/StartStopButton.hpp>
#include <gsim/qt/widgets/FormWidget.hpp>
#include <gsim/qt/reflective/path.hpp>
#include <gsim/qt/model/Connection.hpp>

using namespace gsim::qt;

// Dumper

class Dumper::Data
{
public:
    QLineEdit * filePrefix;
    QPushButton * browse;
    QComboBox * format;
    QSpinBox * suffixLength;
    QCheckBox * multipleFiles;
    QPushButton * startStopButton;
};

Dumper::Dumper(Connection_ptr connection_,
        TypeDescriptor_ptr message_,
        const QList< int >& path_, 
        QWidget * parent) :
    AbstractInputItem(connection_, message_, path_, parent), 
    m_data(new Data)
{
    const QString& id = connection_->name();

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setMargin(0);

    qt::FormWidget * form = new qt::FormWidget();
    form->layout()->setMargin(0);
    layout->addWidget(form);

    m_data->filePrefix = new QLineEdit();
    QHBoxLayout * prefixLayout = new QHBoxLayout();
    m_data->browse = new QPushButton("&Browse");
    prefixLayout->addWidget(m_data->filePrefix);
    prefixLayout->addWidget(m_data->browse);
    prefixLayout->setMargin(0);
    form->addMediumField("File prefix", prefixLayout);
    
    m_data->suffixLength = new QSpinBox();
    m_data->suffixLength->setRange(1, 10); 
    m_data->suffixLength->setValue(4);
    form->addField("Suffix length", m_data->suffixLength);

    m_data->format = new QComboBox();
    m_data->format->addItem("Binary (*.bin)");
    m_data->format->addItem("Text (*.txt)");
    m_data->format->addItem("JSON (*.json)");
    form->addField("Format", m_data->format);

    m_data->multipleFiles = new QCheckBox();
    m_data->multipleFiles->setChecked(true);
    form->addField("Multiple files", m_data->multipleFiles);

    // start and stop button
    m_data->startStopButton = new qt::StartStopButton();
    m_data->startStopButton->setObjectName("start-stop");
    QHBoxLayout * startStopLayout = new QHBoxLayout();
    QSpacerItem * spacer = new QSpacerItem(40, 20, 
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    startStopLayout->addItem(spacer);
    startStopLayout->addWidget(m_data->startStopButton);
    layout->addLayout(startStopLayout);

    setLayout(layout);

    connect(m_data->startStopButton,
            SIGNAL(clicked(bool)),
            this, SLOT(doStart(bool)));

    connect(m_data->startStopButton,
            SIGNAL(toggled(bool)),
            this, SLOT(setEnabled(bool)));

    connect(m_data->browse, SIGNAL(clicked()),
            this, SLOT(browse()));

    const std::string messageName (
            connection_->descriptor()->in()->get_descriptor_name(message_));
    QString defaultFile(id);
    defaultFile += ".";
    defaultFile += messageName.c_str();
    defaultFile += getFieldName(message_, path_);
    defaultFile += "-";
    m_data->filePrefix->setText(defaultFile);
}

Dumper::~Dumper()
{
    delete m_data;
}

void Dumper::browse()
{
    const QString file = QFileDialog::getSaveFileName(this, 
            "Select a file...", ".");

    m_data->filePrefix->setText(file);
}

void Dumper::setEnabled(bool enabled)
{
    m_data->filePrefix->setEnabled(!enabled);
    m_data->format->setEnabled(!enabled);
    m_data->suffixLength->setEnabled(!enabled);
    m_data->multipleFiles->setEnabled(!enabled);
    m_data->browse->setEnabled(!enabled);
}

void Dumper::doStart(bool start)
{
    reset();

    if (start)
    {
        const QString& file (m_data->filePrefix->text());

        if (file.isEmpty())
        {
            m_data->startStopButton->setChecked(false);
        }
        else
        {
            const DumpProcessor::Config config = {
                m_data->multipleFiles->isChecked(),
                file.toStdString(),
                static_cast< DumpProcessor::Format >(m_data->format->currentIndex()),
                static_cast< unsigned int >(m_data->suffixLength->value())
            };
            
            DumpProcessor * processor =
                new DumpProcessor(connection(), message(), path(), config);

            m_processor.reset(processor);

            emit addProcessor(m_processor);
        }
    }
}

//
//
// Dump Tool
//
//

DumpTool::DumpTool(QWidget * parent) :
    AbstractInputTool(parent)
{
    // Todas las partes son chekeables
    m_model->setCheckable(true);
}

DumpTool::~DumpTool()
{
}

AbstractInputItem * DumpTool::createItem(
        Connection_ptr connection, 
        TypeDescriptor_ptr message,
        const QList< int >& path)
{
    return new Dumper(connection, message, path);
}

// Save and load

void Dumper::save(QVariant& settings)
{
    QVariantMap map;

    const QString& file (m_data->filePrefix->text());

    map["file_prefix"] = file;
    map["multiple_files"] = m_data->multipleFiles->isChecked();
    map["format"] = m_data->format->currentText();
    map["suffix_length"] = m_data->suffixLength->value();
    map["running"] = m_data->startStopButton->isChecked();

    settings = map;
}

void Dumper::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    m_data->filePrefix->setText(map["file_prefix"].toString());
    m_data->multipleFiles->setChecked(map["multiple_files"].toBool());

    m_data->suffixLength->setValue(map["suffix_length"].toInt());

    int format = m_data->format->findText(map["format"].toString());

    if (format != -1)
    {
        m_data->format->setCurrentIndex(format);
    }

    m_data->startStopButton->setChecked(map["running"].toBool());
}

