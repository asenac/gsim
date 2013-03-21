#include "ForbiddenStringWidget.hpp"
#include <cassert>
#include <string>
#include <gsim/core/properties.hpp>

using namespace simple;

QWidget * ForbiddenStringWidget::create(gsim::qt::TypeDescriptor_ptr descriptor,
        QWidget * parent)
{
    return new ForbiddenStringWidget(descriptor, parent);
}

ForbiddenStringWidget::ForbiddenStringWidget(gsim::qt::TypeDescriptor_ptr descriptor,
        QWidget * parent) :
    QLineEdit(parent), gsim::qt::ReflectiveWidget(descriptor)
{
    using namespace gsim::core;

    bool array = descriptor->get_type() == TYPE_ARRAY 
        && descriptor->get_slice()->get_type() == TYPE_CHAR;

    assert(descriptor->get_type() == TYPE_STRING || array);

    if (array)
    {
        ::gsim::qt::Holder dummy;
        unsigned length = descriptor->get_length(dummy);

        setMaxLength((int) length);
    }

    // Inicializa la lista de palabras prohibidas
    const ::gsim::core::property_repository * properties = 
        ::gsim::core::property_repository::instance();

    const std::string * forbiddenWords = 
        properties->get_property< std::string >(descriptor, 
            "value.forbidden");

    if (forbiddenWords)
    {
        const QString qforbiddenWords(forbiddenWords->c_str());
        m_forbiddenWords = qforbiddenWords.split(',');

        connect(this, SIGNAL(textChanged(const QString&)),
                this, SLOT(check(const QString&)));
    }
}

ForbiddenStringWidget::~ForbiddenStringWidget()
{
}

void ForbiddenStringWidget::toHolder(gsim::qt::Holder& holder)
{
    const std::string str(text().toStdString());

    m_reflective->from_string(holder, str);
}

void ForbiddenStringWidget::fromHolder(gsim::qt::Holder& holder)
{
    const std::string str(
            m_reflective->to_string(holder));
    setText(str.c_str());
}

QString ForbiddenStringWidget::value() const 
{
    return text();
}

void ForbiddenStringWidget::setValue(const QString& value) 
{
    setText(value);
}

void ForbiddenStringWidget::save(QVariant& settings)
{
    settings = text();
}

void ForbiddenStringWidget::load(const QVariant& settings)
{
    if (settings.canConvert< QString >())
    {
        setText(settings.toString());
    }
}

void ForbiddenStringWidget::_setReadOnly(bool readOnly)
{
    setReadOnly(readOnly);
}

void ForbiddenStringWidget::check(const QString& text_)
{
    bool valid = !m_forbiddenWords.contains(text_);

    if (valid)
    {
       setStyleSheet("QLineEdit { background-color: white; }"); 
    }
    else
    {
       setStyleSheet("QLineEdit { background-color: red; }"); 
    }
}

