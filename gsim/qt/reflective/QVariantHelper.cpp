// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * QVariantHelper.cpp
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

#include "QVariantHelper.hpp"
#include <map>
#include <string>

using namespace gsim::qt;

QVariantHelper::QVariantHelper()
{
}

QVariantHelper::~QVariantHelper()
{
}

//
// Repository
//

class QVariantHelperRepository::Data
{
public:
    typedef std::map< std::string, QVariantHelper_ptr > helpers_t;
    helpers_t helpers;
};

QVariantHelperRepository * QVariantHelperRepository::instance()
{
    static boost::shared_ptr< QVariantHelperRepository > _instance(
            new QVariantHelperRepository);

    return _instance.get();
}

QVariantHelperRepository::QVariantHelperRepository() :
    m_data(new Data)
{
}

QVariantHelperRepository::~QVariantHelperRepository()
{
    delete m_data;
}

void QVariantHelperRepository::setHelper(const std::string& fqn, 
        QVariantHelper_ptr helper)
{
    assert(helper);

    m_data->helpers[fqn] = helper;
}

bool QVariantHelperRepository::toQVariant(
        TypeDescriptor_ptr descriptor,
        Holder& hold,
        QVariant& var) const
{
    Data::helpers_t::const_iterator it = 
        m_data->helpers.find(descriptor->get_type_fqn());

    if (it != m_data->helpers.end())
    {
        var = it->second->toQVariant(descriptor, hold);
        return true;
    }

    return false;
}

bool QVariantHelperRepository::fromQVariant(
        TypeDescriptor_ptr descriptor,
        Holder& hold,
        const QVariant& var) const
{
    Data::helpers_t::const_iterator it = 
        m_data->helpers.find(descriptor->get_type_fqn());

    if (it != m_data->helpers.end())
    {
        return it->second->fromQVariant(descriptor, hold, var);
    }

    return false;
}

