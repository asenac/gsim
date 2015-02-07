// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * DetailedLog.cpp
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

#include "DetailedLog.hpp"
#include <gsim/qt/item/LogModel.hpp>
#include <gsim/core/event.hpp>
#include <gsim/qt/reflective/WidgetFactory.hpp>
#include <gsim/qt/reflective/DragAndDrop.hpp>
#include <gsim/qt/widgets/FormWidget.hpp>
#include <map>

#include <fstream>
#include <gsim/core/file_format_helper.hpp>

using namespace gsim::qt;

class DetailedLogItem::Data
{
public:
    Data(DetailedLogController* c, TypeDescriptor_ptr d)
        : controller(c), descriptor(d), reflectiveWidget(NULL), drag(NULL)
    {
    }

    ~Data()
    {
        delete drag;
    }

    DetailedLogController * controller;
    TypeDescriptor_ptr descriptor;

    QDateTimeEdit * dateTimeEdit;
    ReflectiveWidget * reflectiveWidget;
    QLabel * titleLineEdit;

    QObject * drag;
    Holder holder;
};

DetailedLogItem::DetailedLogItem(
        DetailedLogController * controller,
        TypeDescriptor_ptr type, 
        QWidget * parent) :
    QDialog(parent), m_data(new Data(controller, type))
{
    QVBoxLayout * layout = new QVBoxLayout();
    FormWidget * form = new FormWidget(this);

    m_data->titleLineEdit = new QLabel();
    layout->addWidget(m_data->titleLineEdit);

    // Timestamp
    m_data->dateTimeEdit = new QDateTimeEdit();
    m_data->dateTimeEdit->setDisplayFormat("dd/MM/yyyy hh:mm:ss.zzz");
    m_data->dateTimeEdit->setReadOnly(true);
    form->addField("Timestamp", m_data->dateTimeEdit);

    layout->addWidget(form);

    // Widget
    if (type->get_children_count())
    {
        QWidget * widget = WidgetFactoryRepository::create(type, NULL);

        m_data->reflectiveWidget = 
            dynamic_cast< ReflectiveWidget * >(widget);

        bool isBig = false;

        if (m_data->reflectiveWidget)
        {
            isBig = m_data->reflectiveWidget->isBigWidget();

            m_data->reflectiveWidget->_setReadOnly(true);
        }

        if (isBig)
        {
            QScrollArea * scroll = new QScrollArea(this);
            scroll->setWidget(widget);
            scroll->setWidgetResizable(true);
            layout->addWidget(scroll);

            // form->addBigField("Value", widget);
        }
        else
        {
            form->addField("Value", widget);
        }

        m_data->drag = new Drag(widget, this);
    }

    // Button box
    QDialogButtonBox * box = new QDialogButtonBox();
    QPushButton * closeBtn = box->addButton(QDialogButtonBox::Close);
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));

    QPushButton * saveBtn = box->addButton("&Save", QDialogButtonBox::AcceptRole);
    connect(saveBtn, SIGNAL(clicked()), this, SLOT(save()));
    layout->addWidget(box);

    setLayout(layout);
}

DetailedLogItem::~DetailedLogItem()
{
    delete m_data;
}

void DetailedLogItem::setValue(const LogModel::LogEntry * entry)
{
    // Titulo
    const QString& title = entry->text;
    m_data->titleLineEdit->setText(title);
    setWindowTitle(title);

    // Timestamp
    m_data->dateTimeEdit->setDateTime(entry->dateTime);

    // Valores
    if (m_data->reflectiveWidget)
    {
        // Copia el valor del mensaje al panel sin guardar referencia al
        // mensaje
        Holder h(entry->msg->holder());
        m_data->reflectiveWidget->fromHolder(h);

        m_data->holder = h.clone(); 
    }
}

void DetailedLogItem::save()
{
    QString selectedFilter;
    QString file = 
        QFileDialog::getSaveFileName(0, 
                "Select a file", ".",
                "JSON files (*.json);;"
                "Text files (*.txt);;"
                "Binary files (*.bin)",
                &selectedFilter);

    if (!file.isEmpty())
    {
        using namespace gsim::core;

        file_format_helper const * helper = NULL;
        file_format_factory const * factory = 
            file_format_factory::get_instance();

        if (selectedFilter == "JSON files (*.json)")
            helper = factory->get_helper(FILE_FORMAT_JSON);
        else if (selectedFilter == "Text files (*.txt)")
            helper = factory->get_helper(FILE_FORMAT_TEXT);
        else if (selectedFilter == "Binary files (*.bin)")
            helper = factory->get_helper(FILE_FORMAT_BINARY);

        if (helper)
        {
            std::ofstream ofs(file.toStdString().c_str());

            helper->save(ofs, m_data->holder.get_type_descriptor(),
                    m_data->holder);
        }
        else
        {
            QMessageBox::critical(this, "Error", "Unknown file extension.");
        }
    }
}

// 
// Controller
//

class DetailedLogController::Data
{
public:

    QWidget * parent;
    LogModel * logModel;

    Data(QWidget * p) :
        parent(p)
    {
    }

    typedef std::map< TypeDescriptor_ptr, DetailedLogItem * > items_t; 
    items_t items;

};

DetailedLogController::DetailedLogController(QWidget * parent) :
    QObject(parent), m_data(new Data(parent))
{
}

DetailedLogController::~DetailedLogController()
{
    delete m_data;
}

void DetailedLogController::setLogModel(LogModel * logModel)
{
    m_data->logModel = logModel;
}

void DetailedLogController::showDetails(const QModelIndex& idx)
{
    const LogModel::LogEntry * entry = NULL;

    if (m_data->logModel && (entry = m_data->logModel->getEntry(idx)))
    {
        TypeDescriptor_ptr descriptor = entry->msg->holder().get_type_descriptor();

        Data::items_t::iterator it = 
            m_data->items.find(descriptor);

        DetailedLogItem * view = NULL;

        if (it != m_data->items.end())
        {
            view = it->second;
        }
        else
        {
            view = new DetailedLogItem(this, descriptor, m_data->parent);
            m_data->items.insert(std::make_pair(descriptor, view));
        }

        if (view)
        {
            view->setValue(entry);
            view->show();
            view->raise();
            view->activateWindow();
        }
    }
}
