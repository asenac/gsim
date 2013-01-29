// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * SendMessageDialog.cpp
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

#include "SendMessageDialog.hpp"

#include <gsim/core/event.hpp>
#include <gsim/qt/widgets/StartStopButton.hpp>
#include <gsim/qt/reflective/MessageForm.hpp>

// Qt widgets
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <iostream>

using namespace gsim;
using namespace gsim::qt;

class SendMessageDialog::Data
{
public:
    Data(const std::string& name_) :
        name(name_), 
        reflective(NULL),
        widget(NULL) 
    {
    }

    ~Data()
    {
        delete timer;
    }

    const std::string& name;

    TypeDescriptor_ptr reflective;

    MessageFormWidget * widget;

    QScrollArea * scroll;

    QSpinBox * sbPeriod;
    QSpinBox * sbTimes;
    QCheckBox * cbUseStored;
    QPushButton * pbStartStop;
    QPushButton * pbUpdate;

    QTimer * timer;

    int currentPeriodicMessage;

    Message_ptr storedMessage;
};

SendMessageDialog::SendMessageDialog( 
        const std::string& name,
        TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QDialog(parent), m_data(new Data(name))
{
    m_data->reflective = reflective;

    QVBoxLayout * layout = new QVBoxLayout();

    if (reflective->get_children_count())
    {
        m_data->widget = new MessageFormWidget(reflective, name.c_str(), parent);

        // Input scroll
        m_data->scroll = new QScrollArea();
        m_data->scroll->setObjectName("scroll");
        m_data->scroll->setWidgetResizable(true);
        m_data->widget->setObjectName("form");
        
        QWidget * scrollWidget = new QWidget();
        QVBoxLayout * scrollLayout = new QVBoxLayout();
        scrollLayout->setMargin(0);
        scrollWidget->setLayout(scrollLayout);
        scrollLayout->addWidget(m_data->widget);
        QSpacerItem * spacer = new QSpacerItem(40, 20, 
                QSizePolicy::Expanding, QSizePolicy::Expanding);
        scrollLayout->addItem(spacer);
        scrollWidget->setSizePolicy(QSizePolicy::Preferred, 
                QSizePolicy::Preferred);
        m_data->scroll->setWidget(scrollWidget);
        layout->addWidget(m_data->scroll); 
        m_data->scroll->setLineWidth(0);
        m_data->scroll->setFrameStyle(QFrame::NoFrame);
        m_data->scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        setMinimumWidth(m_data->scroll->widget()->minimumSizeHint().width() + 
                m_data->scroll->verticalScrollBar()->width());
        // End scroll
    }

    QHBoxLayout * periodicLayout = new QHBoxLayout;

    QSpacerItem * spacer = new QSpacerItem(20, 20, 
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    periodicLayout->addItem(spacer);

    periodicLayout->addWidget(new QLabel("Period (ms)"));
    m_data->sbPeriod = new QSpinBox;
    m_data->sbPeriod->setRange(0, 999999);
    m_data->sbPeriod->setValue(100);
    periodicLayout->addWidget(m_data->sbPeriod);

    periodicLayout->addWidget(new QLabel("Times"));
    m_data->sbTimes = new QSpinBox;
    m_data->sbTimes->setRange(-1, 999999);
    m_data->sbTimes->setValue(-1);
    // m_data->sbTimes->setSpecialValueText("Infinite");
    periodicLayout->addWidget(m_data->sbTimes);

    periodicLayout->addWidget(new QLabel("Use stored value"));
    m_data->cbUseStored = new QCheckBox;
    m_data->cbUseStored->setChecked(true);
    periodicLayout->addWidget(m_data->cbUseStored);

    layout->addLayout(periodicLayout);
   
    // Botones 
    QHBoxLayout * btnsLayout = new QHBoxLayout;

    spacer = new QSpacerItem(20, 20, 
            QSizePolicy::Expanding, QSizePolicy::Minimum);
    btnsLayout->addItem(spacer);

    // Start stop
    m_data->pbStartStop = new StartStopButton();
    QObject::connect(m_data->pbStartStop, SIGNAL(toggled(bool)), 
            this, SLOT(startStopChecked(bool)));
    btnsLayout->addWidget(m_data->pbStartStop);

    // Update
    m_data->pbUpdate = new QPushButton("&Update");
    m_data->pbUpdate->setObjectName("updateButton");
    QObject::connect(m_data->pbUpdate, SIGNAL(clicked()), 
            this, SLOT(storeMessage()));
    btnsLayout->addWidget(m_data->pbUpdate);

    // Send button
    QPushButton * btn = new QPushButton("&Send");
    btn->setObjectName("sendButton");
    QObject::connect(btn, SIGNAL(clicked()), 
            this, SLOT(sendClicked())); 
    btnsLayout->addWidget(btn);

    // Close button
    btn = new QPushButton("&Close");
    btn->setObjectName("closeButton");
    QObject::connect(btn, SIGNAL(clicked()), 
            this, SLOT(hide())); 
    btnsLayout->addWidget(btn);

    layout->addLayout(btnsLayout);
   
    // Timer
    m_data->timer = new QTimer(this);
    connect(m_data->timer, SIGNAL(timeout()), 
            this, SLOT(sendStored()));

    setLayout(layout);
}

SendMessageDialog::~SendMessageDialog()
{
    delete m_data;
}

void SendMessageDialog::stopTimer()
{
    m_data->pbStartStop->setChecked(false);
}

void SendMessageDialog::sendClicked()
{
    emit sendMessage(createMessage());
}

void SendMessageDialog::startStopChecked(bool chk)
{
    if (chk)
    {
        storeMessage();
        m_data->currentPeriodicMessage = 0;
        m_data->timer->start(m_data->sbPeriod->value());
    }
    else
    {
        m_data->timer->stop();
    }
}

void SendMessageDialog::storeMessage()
{
    m_data->storedMessage = createMessage();
}

void SendMessageDialog::sendStored()
{
    if (m_data->cbUseStored->isChecked())
        emit sendMessage(m_data->storedMessage);
    else
        sendClicked();

    ++m_data->currentPeriodicMessage;
    if (m_data->sbTimes->value() >= 0 && 
            m_data->currentPeriodicMessage >= m_data->sbTimes->value())
        m_data->pbStartStop->setChecked(false);
}

void SendMessageDialog::hideEvent(QHideEvent * event)
{
    QWidget * parent = parentWidget();

    if (parent && dynamic_cast< QMdiSubWindow* >(parent))
        parent->hide();

    event->accept();
}

void SendMessageDialog::resizeEvent(QResizeEvent * event)
{
    if (m_data->widget)
    {
        setMinimumWidth(m_data->widget->minimumSizeHint().width() + 
                m_data->scroll->verticalScrollBar()->width());
    }

    event->accept();
}

TypeDescriptor_ptr 
SendMessageDialog::getReflective() const
{
    return m_data->reflective;
}

// Settings
void SendMessageDialog::save(QVariant& settings) 
{
    QVariantMap map;

    map["period"] = m_data->sbPeriod->value();
    map["times"] = m_data->sbTimes->value();
    map["use_stored"] = m_data->cbUseStored->isChecked();
    map["start_stop"] = m_data->pbStartStop->isChecked();

    if (m_data->widget)
        m_data->widget->save(map["form"]);

    settings = map;
}

void SendMessageDialog::load(const QVariant& settings) 
{
    const QVariantMap map = settings.toMap();

    m_data->sbPeriod->setValue(map["period"].toInt());
    m_data->sbTimes->setValue(map["times"].toInt());
    m_data->cbUseStored->setChecked(map["use_stored"].toBool());
    m_data->pbStartStop->setChecked(map["start_stop"].toBool());

    if (map.contains("form") && m_data->widget)
        m_data->widget->load(map["form"]);
}

Message_ptr SendMessageDialog::createMessage()
{
    core::holder h (getReflective()->create_holder());

    // Coge los valores del widget
    if (m_data->widget)
        m_data->widget->toHolder(h);

    // Asume la propiedad del holder
    Message_ptr msg(new core::message(m_data->name, h));

    return msg;
}

