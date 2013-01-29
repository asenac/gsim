// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * qvariant.cpp
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

#include "qvariant.hpp"
#include <gsim/core/descriptor.hpp>
#include <gsim/core/descriptor_utils.hpp>
#include <gsim/qt/reflective/QVariantHelper.hpp>

namespace gsim 
{
namespace qt 
{

    QVariant toQVariant(
            gsim::core::descriptor_ptr descriptor,
            gsim::core::holder& hold)
    {

        QVariant custom;

        if (QVariantHelperRepository::instance()->toQVariant(
                    descriptor, hold, custom))
            return custom;

        using namespace gsim::core;

        const descriptor_type type = descriptor->get_type();

        switch(type)
        {
            case TYPE_STRUCT:
                {
                    QVariantMap res;

                    const unsigned int count = 
                        descriptor->get_children_count();

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        core::holder child = 
                            descriptor->get_child_value(hold, i);

                        res.insert(descriptor->get_child_name(i),
                                toQVariant(descriptor->get_child(i),
                                    child));
                    }

                    return res;
                }
            case TYPE_ARRAY:
                if (descriptor->get_slice()->get_type() == TYPE_CHAR)
                {
                    const std::string str(descriptor->to_string(hold));
                    return QVariant(str.c_str());
                }
            case TYPE_SEQUENCE:
                {
                    QVariantList res;

                    const unsigned int length = descriptor->get_length(hold);
                    core::descriptor_ptr slice = 
                        descriptor->get_slice();

                    for (unsigned int i = 0; i < length; i++) 
                    {
                        core::holder child = 
                            descriptor->get_child_value(hold, i);

                        res.push_back(toQVariant(slice, child));
                    }

                    return res;
                }
            case TYPE_BOOL:
                return QVariant(hold.to_value< bool >());
            case TYPE_OCTET:
                return QVariant(hold.to_value< unsigned char >());
            case TYPE_CHAR:
                return QVariant(hold.to_value< char >());
            case TYPE_SHORT:
                return QVariant(hold.to_value< short >());
            case TYPE_USHORT:
                return QVariant(hold.to_value< unsigned short >());
            case TYPE_LONG:
                return QVariant(hold.to_value< int32_t >());
            case TYPE_ULONG:
                return QVariant(hold.to_value< uint32_t >());
            case TYPE_LONGLONG:
                return QVariant((qint64) hold.to_value< int64_t >());
            case TYPE_ULONGLONG:
                return QVariant((quint64) hold.to_value< uint64_t >());

            case TYPE_STRING:
            case TYPE_WSTRING:
                {
                    const std::string str(descriptor->to_string(hold));
                    return QVariant(str.c_str());
                }

            case TYPE_ENUM:
                {
                    const char * str = 
                        ::gsim::core::utils::get_enum_literal(hold); 

                    if (str)
                        return QVariant(str);

                    return QVariant(hold.to_value< int32_t >());
                }

            case TYPE_DOUBLE:
                return QVariant(hold.to_value< double >());
            case TYPE_FLOAT:
                return QVariant(hold.to_value< float >());
            default:
                break;
        }
       
        return QVariant();
    }


    bool fromQVariant(
            gsim::core::descriptor_ptr descriptor,
            gsim::core::holder& hold,
            const QVariant& var)
    {
        if (QVariantHelperRepository::instance()->fromQVariant(
                    descriptor, hold, var))
            return true;

        using namespace gsim::core;

        const descriptor_type type = descriptor->get_type();

        switch(type)
        {

            case TYPE_STRUCT:
                {
                    if (!var.canConvert(QVariant::Map)) return false;

                    QVariantMap map = var.toMap();

                    const unsigned int count = 
                        descriptor->get_children_count();

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        QVariantMap::const_iterator it = 
                            map.find(descriptor->get_child_name(i));

                        if (it != map.end())
                        {
                            core::holder child = 
                                descriptor->get_child_value(hold, i);
                            
                            if (!fromQVariant(descriptor->get_child(i), 
                                        child, it.value()))
                                return false;
                        }
                    }

                    return true;
                }

            case TYPE_ARRAY:
                if (descriptor->get_slice()->get_type() == TYPE_CHAR)
                {
                    const std::string str(var.toString().toStdString());
                    descriptor->from_string(hold, str);
                    return true;;
                }
            case TYPE_SEQUENCE:
                {
                    if (!var.canConvert(QVariant::List)) return false;

                    QVariantList list = var.toList();

                    unsigned int length = (unsigned int) list.size();

                    if (descriptor->is_variable_length())
                    {
                        descriptor->set_length(hold, length);
                    }
                    else if (descriptor->get_length(hold) < length)
                    {
                        // length = descriptor->get_length(hold);
                        // Overflow
                        return false;
                    }

                    core::descriptor_ptr slice = 
                        descriptor->get_slice();

                    for (unsigned int i = 0; i < length; i++) 
                    {
                        core::holder child = 
                            descriptor->get_child_value(hold, i);
                        
                        if (!fromQVariant(slice, child, list.at(i)))
                            return false;
                    }

                    return true;
                }

            case TYPE_BOOL:
                {
                    if (!var.canConvert(QVariant::Bool)) return false;

                    hold.to_value< bool >() = var.toBool();

                    return true;
                }

            case TYPE_OCTET:
                {
                    if (!var.canConvert< unsigned char >()) return false;

                    hold.to_value< unsigned char >() = 
                        var.value< unsigned char >();

                    return true;
                }
            case TYPE_CHAR:
                {
                    if (!var.canConvert< char >()) return false;

                    hold.to_value< char >() = var.value< char >();

                    return true;
                }
            case TYPE_SHORT:
                {
                    if (!var.canConvert< short >()) return false;

                    hold.to_value< short >() = var.value< short >();

                    return true;
                }
            case TYPE_USHORT:
                {
                    if (!var.canConvert< unsigned short >()) return false;

                    hold.to_value< unsigned short >() = 
                        var.value< unsigned short >();

                    return true;
                }
            case TYPE_LONG:
                {
                    if (!var.canConvert< int32_t >()) return false;

                    hold.to_value< int32_t >() = var.value< int32_t >();

                    return true;
                }
            case TYPE_ULONG:
                {
                    if (!var.canConvert< uint32_t >()) return false;

                    hold.to_value< uint32_t >() = var.value< uint32_t >();

                    return true;
                }
            case TYPE_LONGLONG:
                {
                    if (!var.canConvert< int64_t >()) return false;

                    hold.to_value< int64_t >() = var.value< int64_t >();

                    return true;
                }
            case TYPE_ULONGLONG:
                {
                    if (!var.canConvert< uint64_t >()) return false;

                    hold.to_value< uint64_t >() = var.value< uint64_t >();

                    return true;
                }
            case TYPE_STRING:
            case TYPE_WSTRING:
                {
                    bool canConvert = var.canConvert(QVariant::String);

                    if (canConvert)
                        descriptor->from_string(hold, 
                                var.toString().toStdString());

                    return canConvert;
                }

            case TYPE_ENUM:
                {
                    bool canConvert = var.canConvert(QVariant::String);

                    if (canConvert)
                    {
                        const QString val = var.toString();

                        const unsigned int count = 
                            descriptor->get_children_count();

                        unsigned int idx = 
                            descriptor->get_child_index(
                                    val.toStdString().c_str());

                        if (idx < count)
                        {
                            core::holder tmp(
                                    descriptor->get_child_value(hold, idx));
                            descriptor->copy(tmp, hold);

                            // Es un string valido
                            return true;
                        }
                    }

                    return false;
                }

            case TYPE_DOUBLE:
                {
                    bool canConvert = var.canConvert(QVariant::Double);

                    if (canConvert)
                        hold.to_value< double >() = var.toDouble();

                    return canConvert;
                }
            case TYPE_FLOAT:
                {
                    bool canConvert = var.canConvert(QVariant::Double);

                    if (canConvert)
                        hold.to_value< float >() = var.toFloat();

                    return canConvert;
                }

            default:
                break;
        }

        return false;
    }



} // namespace qt
} // namespace gsim

