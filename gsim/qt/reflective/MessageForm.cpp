// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MessageForm.cpp
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

#include "MessageForm.hpp"
#include <gsim/qt/widgets/private/ScriptEditor.hpp>
#include <gsim/qt/tools/ParametersFromFilesTool.hpp>
#include <gsim/qt/tools/Engine.hpp>
#include <gsim/qt/reflective/qvariant.hpp>
#include <gsim/core/event.hpp>
#include <limits>

#include <gsim/qt/widgets/StartStopButton.hpp>
#include <gsim/qt/widgets/SizeGrip.hpp>

#include <gsim/qt/reflective/WidgetFactory.hpp>

// JSON
#include <sstream>
#include <gsim/json/reflective.hpp>

using namespace gsim::qt;

MessageForm::MessageForm(
        QWidget * parent) :
    QWidget(parent), 
    m_reflective(NULL), 
    m_widget(NULL),
    m_files(NULL),
    m_sizeGrip(NULL)
{
}

MessageForm::~MessageForm()
{
}

void MessageForm::initialize(
        Connection_ptr connection,
        const QString& messageName,
        TypeDescriptor_ptr factory)
{
    m_reflective = factory;

    QVBoxLayout * ly = new QVBoxLayout();
    QTabWidget * tabs = new QTabWidget();

    // Input form
    QScrollArea * scroll = new QScrollArea();
    m_widget = new MessageFormWidget(m_reflective, messageName);
    scroll->setWidgetResizable(true);
    scroll->setLineWidth(0);
    scroll->setFrameStyle(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(m_widget);
    tabs->addTab(scroll, "Form");

    // Script
    m_code = new qt::priv::ScriptEditor();
    tabs->addTab(m_code, "Script");
 
    // Files
    m_files = new ParametersFromFilesTool();
    m_files->initialize(connection, messageName, factory);
    tabs->addTab(m_files, "Files");

    // Main layout
    ly->addWidget(tabs);

    ly->setMargin(0);

    // Size grip
    qt::SizeGrip * sizeGrip = new qt::SizeGrip(this);
    sizeGrip->setAffectedWidget(tabs);
    sizeGrip->setVisible(false);

    //ly->addWidget(sizeGrip, 0, Qt::AlignBottom | Qt::AlignRight);
    ly->addWidget(sizeGrip);
    m_sizeGrip = sizeGrip;
    // End size grip
    
    setLayout(ly);
}

void MessageForm::showSizeGrip(bool show)
{
    m_sizeGrip->setVisible(show);
}

Message_ptr MessageForm::createMessage()
{
    return m_widget->createMessage();
}

MessageFormWidget * MessageForm::getWidget()
{
    return m_widget;
}

ParametersFromFilesTool * MessageForm::getFiles()
{
    return m_files;
}

//
//
// Property code
//
//

void MessageForm::setCode(const QString& code)
{
    m_code->setPlainText(code);
}

QString MessageForm::code() const
{
    return m_code->toPlainText();
}

//
//
// Save and load
//
//

void MessageForm::save(QVariant& settings)
{
    QVariantMap map;

    m_widget->save(map["form"]);
    map["code"] = m_code->toPlainText();
    m_files->save(map["files"]);

    settings = map;
}

void MessageForm::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    m_widget->load(map["form"]);
    m_code->setPlainText(map["code"].toString());
    m_files->load(map["files"]);
}

//
//
// Message form widget
//
//

MessageFormWidget::MessageFormWidget(
        TypeDescriptor_ptr reflective,
        const QString& messageName,
        QWidget * parent) :
    QWidget(parent), 
    m_reflective(reflective), 
    m_messageName(messageName.toStdString()),
    m_widget(NULL)
{
    QVBoxLayout * l = new QVBoxLayout();

    QWidget * widget = WidgetFactoryRepository::create(m_reflective, this);
    l->addWidget(widget);

    m_widget = dynamic_cast< ReflectiveWidget * >(widget);

    m_drag = new Drag(widget, this);
    m_drop = new Drop(widget, this);

    setLayout(l);

    setAcceptDrops(true);
}

MessageFormWidget::~MessageFormWidget()
{
    delete m_drag;
    delete m_drop;
}

void MessageFormWidget::toHolder(Holder h)
{
    if (m_widget) m_widget->toHolder(h);
}

//
//
// Property value
//
//

void MessageFormWidget::setValue(const QVariant& var)
{
    if (m_widget)
    {
        core::holder h = m_reflective->create_holder();

        if (fromQVariant(m_reflective, h, var))
        {
            m_widget->fromHolder(h);
        }
    }
}

QVariant MessageFormWidget::value()
{
    Message_ptr req (createMessage());
    core::holder holder(req->holder());

    return toQVariant(m_reflective, holder);
}

TypeDescriptor_ptr 
MessageFormWidget::getReflective() const
{
    return m_reflective;
}

Message_ptr MessageFormWidget::createMessage()
{
    core::holder holder(m_reflective->create_holder());

    if (m_widget)
    {
        m_widget->toHolder(holder);
    }
    
    return Message_ptr(new ::gsim::core::message(m_messageName, holder));
}

void MessageFormWidget::setValue(Message_ptr req)
{
    if (m_widget)
    {
        core::holder holder(req->holder());

        m_widget->fromHolder(holder);
    }
}

//
//
// Save and load
//
//

void MessageFormWidget::save(QVariant& settings)
{
    if (m_widget) m_widget->save(settings);
}

void MessageFormWidget::load(const QVariant& settings)
{
    if (m_widget) m_widget->load(settings);
}

// 
//
// Message Sender
//
//

class MessageSender::Data
{
public:
    Connection_ptr connection;
    TypeDescriptor_ptr reflective;

    MessageForm * form;
    QSpinBox * times;
    QSpinBox * period;
    QSpinBox * delay;
    QCheckBox * updateForm;
    QProgressBar * progressBar;
    QPushButton * playButton;

    SenderConfig_ptr config;

    Data(Connection_ptr c) : 
        connection(c), reflective(NULL)
    {
    }
};

MessageSender::MessageSender(
        Connection_ptr connection,
        QWidget * parent) :
    QWidget(parent), m_data(new Data(connection))
{
    QVBoxLayout * mainLayout = new QVBoxLayout();

    // Form
    m_data->form = new MessageForm();
    mainLayout->addWidget(m_data->form);

    // Configuration
    QHBoxLayout * cfgLayout = new QHBoxLayout();
    m_data->times = new QSpinBox();
    m_data->times->setRange(0, std::numeric_limits< int >::max());
    m_data->times->setValue(1);
    m_data->times->setSpecialValueText("Infinite");
    m_data->period = new QSpinBox();
    m_data->period->setRange(0, std::numeric_limits< int >::max());
    m_data->period->setValue(100);
    m_data->delay = new QSpinBox();
    m_data->delay->setRange(0, std::numeric_limits< int >::max());
    m_data->delay->setValue(0);

    m_data->updateForm = new QCheckBox();

    m_data->progressBar = new QProgressBar();
    m_data->progressBar->setFixedSize(150, 15);
    m_data->progressBar->setMaximum(0);
    m_data->progressBar->setMaximum(1);

    m_data->playButton = new qt::StartStopButton();
    m_data->playButton->setObjectName("start-stop");

    cfgLayout->addWidget(new QLabel("Times"));
    cfgLayout->addWidget(m_data->times);
    cfgLayout->addWidget(new QLabel("Period (ms)"));
    cfgLayout->addWidget(m_data->period);
    cfgLayout->addWidget(new QLabel("Delay (ms)"));
    cfgLayout->addWidget(m_data->delay);
    cfgLayout->addWidget(new QLabel("Update form"));
    cfgLayout->addWidget(m_data->updateForm);
    cfgLayout->addWidget(m_data->progressBar);
    cfgLayout->addWidget(m_data->playButton);

    mainLayout->addLayout(cfgLayout);

    setLayout(mainLayout);

    // signals
    connect(m_data->playButton,
            SIGNAL(toggled(bool)),
            this,
            SLOT(playClicked(bool)));

    QObject * senderCtl = Engine::instance()->senderController();

    connect(
            this,
            SIGNAL(addSender(SenderConfig_ptr)),
            senderCtl,
            SLOT(addSender(SenderConfig_ptr)));
    connect(
            this,
            SIGNAL(deleteSender(SenderConfig_ptr)),
            senderCtl,
            SLOT(deleteSender(SenderConfig_ptr)));

    connect(m_data->updateForm,
            SIGNAL(toggled(bool)),
            this,
            SLOT(activeUpdateForm(bool)));
}

MessageSender::~MessageSender()
{
    reset();

    delete m_data;
}

TypeDescriptor_ptr MessageSender::getReflective() const
{
    return m_data->reflective;
}

void MessageSender::initialize(
        const QString& messageName,
        TypeDescriptor_ptr op)
{
    m_data->reflective = op;

    m_data->form->initialize(m_data->connection, messageName, op);

    // signals
    connect(this, 
            SIGNAL(updateForm(Message_ptr)),
            m_data->form->getWidget(),
            SLOT(setValue(Message_ptr)));

    // TODO no debería haber formulario si no hay 
    // parámetros. No tiene sentido.
    // Eliminar método initialize.
    if (!m_data->reflective->get_children_count())
    {
        m_data->form->hide();
        m_data->updateForm->setEnabled(false);
    }
}

Connection_ptr MessageSender::connection() const
{
    return m_data->connection;
}

void MessageSender::save(QVariant& settings)
{
    QVariantMap map;

    m_data->form->save(map["form"]);

    map["times"] = m_data->times->value();
    map["period"] = m_data->period->value();
    map["delay"] = m_data->delay->value();
    map["update_form"] = m_data->updateForm->isChecked();

    settings = map;
}

void MessageSender::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    m_data->form->load(map["form"]);

    m_data->times->setValue(map["times"].toInt());
    m_data->period->setValue(map["period"].toInt());
    m_data->delay->setValue(map["delay"].toInt());
    m_data->updateForm->setChecked(map["update_form"].toBool());
}

MessageForm * MessageSender::getForm() const
{
    return m_data->form;
}

void MessageSender::reset()
{
    if (m_data->config)
    {
        // disconnect
        disconnect(m_data->config.get(),
                SIGNAL(messageSent(Message_ptr)),
                this,
                SIGNAL(updateForm(Message_ptr)));

        disconnect(m_data->config.get(),
                SIGNAL(messageSent(Message_ptr)),
                this,
                SLOT(incrementBar()));

        disconnect(m_data->config.get(),
                SIGNAL(finished()), 
                this,
                SLOT(finished()));
        emit deleteSender(m_data->config);

        m_data->config.reset();
    }
}

void MessageSender::incrementBar()
{
    m_data->progressBar->setValue(m_data->progressBar->value() + 1);
}

void MessageSender::playClicked(bool play)
{
    reset();

    if (play)
    {
        m_data->progressBar->setRange(0, m_data->times->value());
        m_data->progressBar->setValue(0);

        // m_data->form->setEnabled(false);
        _setReadOnly(true);

        // Create processors
        QList< MessageProcessor_ptr > processors;
        m_data->form->getFiles()->createProcessors(processors);

        m_data->config.reset(new SenderConfig(
                    connection(),
                    m_data->reflective,
                    m_data->form->createMessage(),
                    m_data->form->code(),
                    processors,
                    m_data->times->value(),
                    m_data->period->value(),
                    m_data->delay->value()));

        // connect signals
        activeUpdateForm(m_data->updateForm->isChecked());

        connect(m_data->config.get(),
                SIGNAL(messageSent(Message_ptr)),
                this,
                SLOT(incrementBar()));

        connect(m_data->config.get(),
                SIGNAL(finished()), 
                this,
                SLOT(finished()));

        emit addSender(m_data->config);
    }
    else
    {
        // m_data->form->setEnabled(true);
        _setReadOnly(false);

        if (!m_data->times->value())
        {
            m_data->progressBar->setMinimum(0);
            m_data->progressBar->setRange(0, 1);
            m_data->progressBar->setValue(1);
        }
    }
}

void MessageSender::finished()
{
    m_data->playButton->setChecked(false);
    // m_data->form->setEnabled(true);
    _setReadOnly(false);
}

void MessageSender::activeUpdateForm(bool update)
{
    if (m_data->config)
    {
        if (update)
        {
            connect(m_data->config.get(),
                    SIGNAL(messageSent(Message_ptr)),
                    this,
                    SIGNAL(updateForm(Message_ptr)));
        }
        else
        {
            disconnect(m_data->config.get(),
                    SIGNAL(messageSent(Message_ptr)),
                    this,
                    SIGNAL(updateForm(Message_ptr)));

        }
    }

}

void MessageSender::stop()
{
    playClicked(false);
    finished();
}

// Read only
void MessageFormWidget::_setReadOnly(bool readOnly)
{
    m_widget->_setReadOnly(readOnly);
}

void MessageForm::_setReadOnly(bool readOnly)
{
    m_widget->_setReadOnly(readOnly);
    m_code->setReadOnly(readOnly);
    m_files->setEnabled(!readOnly);
}

void MessageSender::_setReadOnly(bool readOnly)
{
    m_data->form->_setReadOnly(readOnly);
    m_data->times->setReadOnly(readOnly);
    m_data->period->setReadOnly(readOnly);
    m_data->delay->setReadOnly(readOnly);

    // Can change in read-only mode
    // m_data->updateForm->setEnabled(!readOnly);
}

