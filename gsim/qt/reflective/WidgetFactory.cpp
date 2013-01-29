// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * WidgetFactory.cpp
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

#include "WidgetFactory.hpp"
#include "BoolWidget.hpp"
#include "IntegerWidget.hpp"
#include "FloatWidget.hpp"
#include "EnumWidget.hpp"
#include "StringWidget.hpp"
#include "SequenceWidget.hpp"
#include "StructWidget.hpp"

// Factorías conocidas
#include <boost/bind.hpp>
#include <gsim/qt/reflective/LSBWidget.hpp>
#include <gsim/qt/reflective/FixedPointWidget.hpp>
#include <gsim/qt/reflective/BitSetWidget.hpp>
#include <gsim/qt/reflective/IPAddressWidget.hpp>
#include <gsim/qt/reflective/SequenceWidget.hpp>
#include <gsim/qt/reflective/BigTextWidget.hpp>
#include <gsim/qt/reflective/DateTimeWidget.hpp>
#include <gsim/qt/reflective/BCDWidget.hpp>
#include <gsim/qt/reflective/HexWidget.hpp>
#include <gsim/qt/reflective/IntegerWidget.hpp>
#include <gsim/qt/reflective/TableWidget.hpp>
#include <gsim/qt/reflective/ReflectiveTreeWidget.hpp>

#include <QLabel>

#include <memory>
#include <map>
#include <cassert>

#include <boost/bind.hpp>

#include <gsim/core/properties.hpp>
#include <gsim/core/descriptor_utils.hpp>

#include <iostream>

using namespace gsim::qt;

namespace  
{

QWidget * defaultWidgetFactory(
        WidgetFactoryRepository * repository,
        TypeDescriptor_ptr descriptor, 
        QWidget * parent)
{
    using namespace gsim::core;

    if (!descriptor)
        return new QLabel("Null descriptor type!", parent);

    WidgetFactory_t factory = 
        boost::bind(&WidgetFactoryRepository::createWidget,
                repository, _1, _2);

    const descriptor_type type = descriptor->get_type();

    switch(type)
    {
        case TYPE_BOOL:
            return new BoolWidget(descriptor, parent);

            // Integer types
        case TYPE_OCTET:
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_LONG:
        case TYPE_ULONG:
        case TYPE_LONGLONG:
        case TYPE_ULONGLONG:
            return new IntegerWidget(descriptor, parent);
        
        case TYPE_STRING:
        case TYPE_WSTRING:
            return new StringWidget(descriptor, parent);

        case TYPE_ARRAY:
            if (descriptor->get_slice()->get_type() ==
                    TYPE_CHAR)
            {
                return new StringWidget(descriptor, parent);
            }
        case TYPE_SEQUENCE:

            // if (descriptor->get_slice()->is_primitive())
            {
                Holder dummy;
                if (!descriptor->is_variable_length() &&
                        descriptor->get_length(dummy) > 1024)
                {
                    return new QLabel("Maximum size exceeded");
                }

                return new SequenceWidget(descriptor, parent);
            }

            // return new ComplexSequenceWidget(
            //        descriptor, factory, parent);

        case TYPE_DOUBLE:
        case TYPE_FLOAT:
            return new FloatWidget(descriptor, parent);
        case TYPE_STRUCT:
            return new StructWidget(descriptor, factory, parent);
        case TYPE_ENUM:
            return new EnumWidget(descriptor, parent);
        default:
            break;
    }

    return new QLabel("Unsupported item!", parent);
}

struct PropertyToQProperty
{
    const char * property;
    const char * qproperty;
};

const PropertyToQProperty defaultProperties[] = {
    {"widget.tooltip", "toolTip"},
    {"widget.suffix", "suffix"},
    {"widget.prefix", "prefix"},
    {"widget.step", "singleStep"},
    {"widget.decimals", "decimals"},
    {"widget.value", "value"},
    {"widget.enabled", "enabled"},
    {"widget.max_length", "maxLength"},
    {"widget.format", "displayFormat"},
    {"widget.tags", "tags"},
    {"widget.style_sheet", "styleSheet"},
    {"value.min", "validMinimum"},
    {"value.max", "validMaximum"},
    {"value.lsb", "LSB"},
    {"value.fractional_bits", "fractionalBits"},
    {0, 0}
};

QVariant toQVariant(const ::gsim::core::property_t& property)
{
    using namespace ::gsim::core;

    switch(property.which())
    {
        case PROP_STRING:
            return QVariant(boost::get< const std::string >(property).c_str());
        case PROP_BOOL:
	    return QVariant(boost::get< bool >(property));
        case PROP_FLOAT:
	    return QVariant(boost::get< float >(property));
        case PROP_DOUBLE:
	    return QVariant(boost::get< double >(property));
        case PROP_CHAR:
	    return QVariant(boost::get< char >(property));
        case PROP_UCHAR:
	    return QVariant(boost::get< unsigned char >(property));
        case PROP_SHORT:
	    return QVariant(boost::get< short >(property));
        case PROP_USHORT:
	    return QVariant(boost::get< unsigned short >(property));
        case PROP_INT32:
	    return QVariant(boost::get< int32_t >(property));
        case PROP_UINT32:
	    return QVariant(boost::get< uint32_t >(property));
#if 0
        case PROP_INT64:
	    return QVariant(boost::get< int64_t >(property));
        case PROP_UINT64:
	    return QVariant(boost::get< uint64_t >(property));
#endif
        default:
            break;
    }

    return QVariant();
}

} // namespace 

class WidgetFactoryRepository::Data
{
public:
    typedef std::map< std::string,
           WidgetFactory_t > factories_t;

    factories_t factories;
    WidgetFactory_t defaultFactory;
};

WidgetFactoryRepository::WidgetFactoryRepository() :
    m_data(new Data)
{
    m_data->defaultFactory = 
        boost::bind(defaultWidgetFactory, this, _1, _2);

    // Factorias conocidas
    setFactory("lsb", 
        boost::bind(&LSBWidget::create, _1, 0.1, _2));
    setFactory("fixed_point", 
        boost::bind(&FixedPointWidget::create, _1, 8, _2));
    setFactory("bcd", &BCDWidget::create);
    setFactory("ip", &IPAddressWidget::create);
    setFactory("ipv4", &IPAddressWidget::create);
    setFactory("bigtext", &BigTextWidget::create);
    setFactory("big_text", &BigTextWidget::create);
    setFactory("bitset", &BitSetWidget::create);
    setFactory("bit_set", &BitSetWidget::create);
    setFactory("datetime", &DateTimeWidget::create);
    setFactory("date_time", &DateTimeWidget::create);
    setFactory("hex", &BCDWidget::create);
    setFactory("integer", &IntegerWidget::create);
    setFactory("sequence", &SequenceWidget::create);
    setFactory("table", &TableWidget::create);
    setFactory("tree", &ReflectiveTreeWidget::create);
}

WidgetFactoryRepository::~WidgetFactoryRepository()
{
    delete m_data;
}

QWidget * WidgetFactoryRepository::createWidget(
        TypeDescriptor_ptr descriptor, 
        QWidget * parent) const
{
    const ::gsim::core::property_repository * properties = 
        ::gsim::core::property_repository::instance();

    QWidget * res = NULL;

    const std::string * factoryName = 
        properties->get_property< std::string >(descriptor, 
            "widget");

    // Busqueda por nombre de la factoría asociada al tipo
    // mediante propiedad
    if (factoryName) 
    {
        Data::factories_t::const_iterator it = 
            m_data->factories.find(*factoryName);

        if (it != m_data->factories.end())
        {
            res = (it->second)(descriptor, parent);
        }
    }

    if (!res) 
    {
        res = m_data->defaultFactory(descriptor, parent);
    }

    if (res)
    {
        // aplicar propiedades por defecto: tooltip...
        const std::string fqn = descriptor->get_type_fqn();

        const std::string memberFqn = 
            ::gsim::core::utils::get_member_fqn(descriptor);

        const PropertyToQProperty * p = defaultProperties;

        // Comprueba que al widget se le puede aplicar cada una
        // de las propiedades por defecto, y que han sido 
        // definidas estas propiedades
        for (; p && p->property; p++)
        {
            const QVariant hasProperty = 
                res->property(p->qproperty);

            if (hasProperty.isValid())
            {
                // Propiedad asignada al miembro o al tipo
                const ::gsim::core::property_t * property =
                    properties->get_property(descriptor, 
                        p->property);

                if (property)
                {
                    std::cout << "Applying property " 
                        << p->property 
                        << " to " 
                        << ((memberFqn.empty())? fqn: memberFqn) 
                        << " with value \""
                        << *property
                        << "\""<< std::endl;

                    res->setProperty(p->qproperty, 
                        toQVariant(*property));
                }
            }
        }
    }

    return res;
}

WidgetFactoryRepository * 
WidgetFactoryRepository::defaultInstance()
{
    static std::auto_ptr< WidgetFactoryRepository > _instance(
            new WidgetFactoryRepository);
    return _instance.get();
}

void WidgetFactoryRepository::setDefaultFactory(
        WidgetFactory_t factory)
{
    assert(factory);

    m_data->defaultFactory = factory;
}

void WidgetFactoryRepository::setFactory(
        const std::string& factoryName, 
        WidgetFactory_t factory)
{
    assert(factory);

    m_data->factories[factoryName] = factory;
}

QWidget * WidgetFactoryRepository::create(
        TypeDescriptor_ptr descriptor, 
        QWidget * parent)
{
    return defaultInstance()->createWidget(descriptor, parent);
}

