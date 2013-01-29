// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * SequenceWidget.cpp
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

#include "SequenceWidget.hpp"
#include <gsim/qt/reflective/qvariant.hpp>
#include <gsim/qt/widgets/FormWidget.hpp>
#include <gsim/core/descriptor_utils.hpp>

using namespace gsim::qt;

QWidget * SequenceWidget::create(TypeDescriptor_ptr descriptor, 
        QWidget * parent)
{
    return new SequenceWidget(descriptor, parent);
}

SequenceWidget::SequenceWidget(
        TypeDescriptor_ptr reflective,
        QWidget * parent) :
    QWidget(parent), ReflectiveWidget(reflective), 
    m_sbLength(NULL), m_sbCurrentIndex(NULL)
{
    setBigWidget(true);
    setSupportDrag(true);

    QVBoxLayout * layout = new QVBoxLayout;
    FormWidget * form = new FormWidget(this);
    layout->addWidget(form);
 
    m_sbLength = new QSpinBox();
    form->addField("Length", m_sbLength);
    m_sbLength->setObjectName("length");

    m_sbCurrentIndex = new QSpinBox();
    form->addField("Index", m_sbCurrentIndex);
    m_sbCurrentIndex->setObjectName("index");

    m_holder = ::gsim::core::utils::create_initialized_holder(reflective);

    m_slice_widget = 
        WidgetFactoryRepository::defaultInstance()->createWidget(
                reflective->get_slice(), this);

    m_slice = dynamic_cast< ReflectiveWidget * >(m_slice_widget);
    
    bool isBig = (!m_slice)? false: m_slice->isBigWidget();

    if (isBig)
        form->addBigField("Value", m_slice_widget);
    else
        form->addField("Value", m_slice_widget);

    // Spacer
    /*
    QSpacerItem * spacer = new QSpacerItem(40, 20, 
            QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addItem(spacer);
     */

    setLayout(layout);

    if (reflective->is_variable_length())
    {
        connect(m_sbLength, SIGNAL(valueChanged(int)),
                this, SLOT(lengthChanged(int)));

        // TODO maximo razonable
        m_sbLength->setRange(0, 9999999);
        m_sbLength->setValue(0);
        m_sbCurrentIndex->setReadOnly(true);
        m_slice_widget->setEnabled(false);
    }
    else
    {
        unsigned int length = 
            m_reflective->get_children_count();

        // Fixed length
        m_sbLength->setRange(0, length);
        m_sbLength->setValue(length);
        m_sbLength->setReadOnly(true);
        m_sbLength->setButtonSymbols(QAbstractSpinBox::NoButtons);

        // Initializes the array
        for (unsigned int i = 0; i < length; i++) 
        {
            Holder ch (m_reflective->get_child_value(m_holder, i));
            m_slice->toHolder(ch);
        }

        m_sbCurrentIndex->setRange(0, length - 1);
        indexChanged(0);
    }

    connect(m_sbCurrentIndex, SIGNAL(valueChanged(int)),
            this, SLOT(indexChanged(int)));

    connect(m_slice_widget, 
            m_slice->editingFinishedSignal(),
            this, SLOT(saveCurrent()));

    saveCurrent();
}

SequenceWidget::~SequenceWidget()
{
}

int SequenceWidget::maxLength() const
{
    return m_sbLength->maximum();
}

void SequenceWidget::setMaxLength(int length)
{
    if (m_reflective->is_variable_length())
    {
        m_sbLength->setRange(0, length);
    }
}

void SequenceWidget::setValue(const QVariant& var)
{
    Holder h = m_reflective->create_holder();

    if (fromQVariant(m_reflective, h, var))
    {
        fromHolder(h);
    }
}

QVariant SequenceWidget::value() const
{
    Holder h = m_reflective->create_holder();
    m_reflective->copy(m_holder, h);

    return toQVariant(m_reflective, h);
}

void SequenceWidget::toHolder(Holder& holder)
{
    m_reflective->copy(m_holder, holder);
}

void SequenceWidget::fromHolder(Holder& holder)
{
    m_reflective->copy(holder, m_holder);

    if (m_reflective->is_variable_length())
    {
        // show current value
        m_sbLength->setValue(m_reflective->get_length(m_holder));
    }

    // Reload current value
    indexChanged(m_sbCurrentIndex->value());
}

void SequenceWidget::lengthChanged(int len)
{
    if (!m_reflective->is_variable_length())
        return;

    unsigned int oldLength = m_reflective->get_length(m_holder);

    if (len != 0)
    {
        m_sbCurrentIndex->setRange(0, len-1);
    }

    m_reflective->set_length(m_holder, len);

    m_sbCurrentIndex->setReadOnly(len == 0);
    m_slice_widget->setEnabled(len != 0);

    // Reload current value
    if (!oldLength && len)
    {
        indexChanged(m_sbCurrentIndex->value());
    }

    emit editingFinished();
}

void SequenceWidget::indexChanged(int idx)
{
    unsigned int length = m_reflective->get_length(m_holder);

    if (length && idx < (int) length)
    {
        core::holder child_value = 
            m_reflective->get_child_value(m_holder, (unsigned int) idx);

        m_slice->fromHolder(child_value);
    }
}

void SequenceWidget::saveCurrent()
{
    unsigned int length = m_reflective->get_length(m_holder);
    int index = m_sbCurrentIndex->value();

    if (length && index < (int) length)
    {
        core::holder child_value = m_reflective->get_child_value(
                m_holder, index);
        m_slice->toHolder(child_value);

        emit editingFinished();
    }
}

// Complex Sequence Widget

ComplexSequenceWidget::ComplexSequenceWidget(
        TypeDescriptor_ptr reflective,
        WidgetFactory_t factory,
        QWidget * parent) :
    QWidget(parent), ReflectiveWidget(reflective), 
    m_factory(factory),
    m_sbLength(NULL), m_sbCurrentIndex(NULL)
{
    setBigWidget(true);
    setSupportDrag(true);

    QVBoxLayout * layout = new QVBoxLayout;
 
    QHBoxLayout * headerLayout = new QHBoxLayout;

    // Read only for fixed length types
    m_sbLength = new QSpinBox();
    headerLayout->addWidget(new QLabel("Length"));
    headerLayout->addWidget(m_sbLength);
    m_sbLength->setObjectName("length");

    m_sbCurrentIndex = new QSpinBox();
    headerLayout->addWidget(new QLabel("Index"));
    headerLayout->addWidget(m_sbCurrentIndex);
    m_sbCurrentIndex->setObjectName("index");

    layout->addLayout(headerLayout);

    m_stack = new QStackedWidget();

    if (reflective->is_variable_length())
    {
        connect(m_sbLength, SIGNAL(valueChanged(int)),
                this, SLOT(lengthChanged(int)));

        // TODO maximo razonable
        m_sbLength->setRange(0, 99999);
        m_sbLength->setValue(0);

        lengthChanged(0);
    }
    else
    {
        Holder holder = m_reflective->create_holder();
        unsigned int length = m_reflective->get_length(holder);

        // Fixed length
        m_sbLength->setRange(0, length);
        m_sbLength->setValue(length);
        m_sbLength->setReadOnly(true);
        m_sbLength->setButtonSymbols(QAbstractSpinBox::NoButtons);

        for (unsigned int i = 0; i < length; i++) 
        {
            m_stack->addWidget(factory(reflective->get_slice(), NULL));
        }

        m_sbCurrentIndex->setRange(0, length - 1);

        indexChanged(0);
    }

    connect(m_sbCurrentIndex, SIGNAL(valueChanged(int)),
            this, SLOT(indexChanged(int)));

    layout->addWidget(m_stack);
    setLayout(layout);
}

ComplexSequenceWidget::~ComplexSequenceWidget()
{
}

int ComplexSequenceWidget::maxLength() const
{
    return m_sbLength->maximum();
}

void ComplexSequenceWidget::setMaxLength(int length)
{
    if (m_reflective->is_variable_length())
    {
        m_sbLength->setRange(0, length);
    }
}

void ComplexSequenceWidget::toHolder(Holder& holder)
{
    unsigned int len = 0;

    if (m_reflective->is_variable_length())
    {
        len = (unsigned int) m_sbLength->value();

        m_reflective->set_length(holder, len);
    }
    else
    {
        len = m_reflective->get_length(holder);
    }

    for (unsigned int i = 0; i < len; i++) 
    {
        Holder child_value = m_reflective->get_child_value(holder, i);

        ReflectiveWidget * w = 
            dynamic_cast< ReflectiveWidget * >(m_stack->widget(i));

        if (w) w->toHolder(child_value);
    }
}

void ComplexSequenceWidget::fromHolder(Holder& holder)
{
    unsigned int len = m_reflective->get_length(holder);

    if (m_reflective->is_variable_length())
    {
        m_sbLength->setValue(len);
    }

    for (unsigned int i = 0; i < len; i++) 
    {
        Holder child_value = m_reflective->get_child_value(holder, i);

        ReflectiveWidget * w = 
            dynamic_cast< ReflectiveWidget * >(m_stack->widget(i));

        if (w) w->fromHolder(child_value);
    }
}

void ComplexSequenceWidget::lengthChanged(int len)
{
    if (!m_reflective->is_variable_length())
        return;

    int diff = len - m_stack->count();

    for (int i = 0; i < diff; i++) 
    {
        m_stack->addWidget(m_factory(m_reflective->get_slice(), this));
    }

    if (len == 0)
    {
        m_stack->hide();
        m_sbCurrentIndex->setRange(0, 0);
    }
    else
    {
        m_stack->show();
        m_sbCurrentIndex->setRange(0, len-1);
    }

    m_sbCurrentIndex->setEnabled(len != 0);

    emit editingFinished();
}

void ComplexSequenceWidget::indexChanged(int idx)
{
    m_stack->setCurrentIndex(idx);
}

void SequenceWidget::save(QVariant& settings)
{
    QVariantMap map;

    if (m_reflective->is_variable_length())
    {
        map["length"] = m_sbLength->value();
    }

    map["index"] = m_sbCurrentIndex->value();

    map["value"] = value();

    settings = map;
}

void SequenceWidget::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    if (m_reflective->is_variable_length())
    {
        m_sbLength->setValue(map["length"].toInt());
    }

    setValue(map["value"]);

    m_sbCurrentIndex->setValue(map["index"].toInt());
}

void ComplexSequenceWidget::save(QVariant& settings)
{
    QVariantMap map;

    int length = 0;

    if (m_reflective->is_variable_length())
    {
        length = m_sbLength->value();
        map["length"] = length;
    }
    else
    {
        Holder h;
        length = (int) m_reflective->get_length(h);
    }

    map["index"] = m_sbCurrentIndex->value();

    QVariantList list;

    for(int i = 0; i < length; i++)
    {
        ReflectiveWidget * w = 
            dynamic_cast< ReflectiveWidget * >(m_stack->widget(i));

        if (w)
        {
            QVariant v;
            w->save(v);
            list << v;
        }
    }

    map["sequence"] = list;

    settings = map;
}

void ComplexSequenceWidget::load(const QVariant& settings)
{
    const QVariantMap map = settings.toMap();

    int length = 0;

    if (m_reflective->is_variable_length())
    {
        length = map["length"].toInt();
        m_sbLength->setValue(length);
    }
    else
    {
        Holder h;
        length = (int) m_reflective->get_length(h);
    }

    m_sbCurrentIndex->setValue(map["index"].toInt());

    const QVariantList list = map["sequence"].toList();

    for(int i = 0; i < list.size() && i < length; i++)
    {
        ReflectiveWidget * w = 
            dynamic_cast< ReflectiveWidget * >(m_stack->widget(i));

        if (w) w->load(list.at(i));
    }
}

void ComplexSequenceWidget::_setReadOnly(bool readOnly)
{
    for (unsigned int i = 0; i < (unsigned int) m_stack->count(); i++) 
    {
        ReflectiveWidget * w = 
            dynamic_cast< ReflectiveWidget * >(m_stack->widget(i));

        if (w) w->_setReadOnly(readOnly);
    }

    if (m_reflective->is_variable_length())
    {
        m_sbLength->setReadOnly(readOnly);
    }
}

void SequenceWidget::_setReadOnly(bool readOnly)
{
    m_slice->_setReadOnly(readOnly);

    if (m_reflective->is_variable_length())
    {
        m_sbLength->setReadOnly(readOnly);
    }
}

