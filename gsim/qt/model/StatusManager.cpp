// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * StatusManager.cpp
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

#include "StatusManager.hpp"
#include <gsim/qt/initialize.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <fstream>
#include <gsim/core/descriptor_utils.hpp>
#include <gsim/qt/reflective/qvariant.hpp>
#include <gsim/qt/json.hpp>

using namespace gsim::qt;

typedef boost::shared_lock< boost::shared_mutex > shared_lock;
typedef boost::unique_lock< boost::shared_mutex > unique_lock;

class StatusManager::Data
{
public:

    Data()  
    {
        ::gsim::qt::initialize();
    }

    boost::shared_mutex messageMutex;

    typedef std::map< QString, Holder > status_t;
    status_t status;


    typedef std::map< QString, validator_t > validators_t;
    validators_t validators;
};

StatusManager::StatusManager(
        QObject * parent) :
    QObject(parent), m_data(new Data)
{
}

StatusManager::~StatusManager()
{
    delete m_data;
}

Holder StatusManager::status(const QString& name)
{
    Holder res (statusRef(name));

    // Devuelve siempre una copìa
    if (res.is_valid())
    {
        res = Holder(res.clone());
    }

    return res;
}

QList< QString > StatusManager::statusList() const
{
    QList< QString > res;
    shared_lock lock(m_data->messageMutex);

    Data::status_t::const_iterator it = m_data->status.begin();

    for (; it != m_data->status.end(); ++it) 
    {
        res.push_back(it->first);
    }

    return res;
}

Holder StatusManager::statusRef(const QString& name)
{
    unique_lock lock(m_data->messageMutex);
    Holder res;

    Data::status_t::iterator it = 
        m_data->status.find(name);

    if (it != m_data->status.end())
    {
        res = it->second;
    }

    return res;
}

void StatusManager::setStatus(const QString& name, Holder holder)
{
    Holder older(status(name));

    if (holder.is_valid() && older.is_valid())
    {
        using namespace ::gsim::core::utils;

        Holder newer(older.clone());

        // Hace una copia 'reflectiva' ya que holder puede no ser
        // del mismo tipo que el tipo del estado.
        reflective_copy(holder, newer);

        // Region protegida reducida al mínimo
        if (applyStatus(name, older, newer))
        {
            unique_lock lock(m_data->messageMutex);

            m_data->status[name] = newer;

            // Ha de estar dentro de la región protegida para la
            // notificación en orden
            emit statusChanged(name, newer); 
            emit statusChanged(name, older, newer); 
        }
    }
}

bool StatusManager::applyStatus(const QString& name, 
            Holder older, 
            Holder newer)
{
    // TODO buscar en la lista de validadores

    return true;
}

void StatusManager::dump(const QString& filename)
{
    shared_lock lock(m_data->messageMutex);

    Data::status_t::iterator it = 
        m_data->status.begin();

    // Conversión a QVariant
    QVariantMap map;

    for (; it != m_data->status.end(); ++it) 
    {
        map[it->first] = 
            toQVariant(it->second.get_type_descriptor(),
                it->second);
    }

    // Conversión a JSON
    try 
    {
        std::ofstream ofs(filename.toStdString().c_str());
        json::ostream_writer_t ow(ofs, true);

        toJson(ow, map);
    } 
    catch (...)
    {
        emit error(QString("Unable to save file '%1'").arg(filename));
    }
}

void StatusManager::restore(const QString& filename)
{
   QVariant var;

    // Try to Read a JSON file
    bool res = 
        fromJsonFile(filename.toStdString().c_str(), var);

    if (res)
    {
        const QVariantMap map = var.toMap();

        for (QVariantMap::const_iterator it = map.begin(); 
                it != map.end(); ++it) 
        {
            Holder holder(status(it.key())); 

            if (holder.is_valid())
            {
                res = fromQVariant(holder.get_type_descriptor(),
                        holder, it.value());

                if (res)
                {
                    setStatus(it.key(), holder);
                }
            }
        }
    }
    else
    {
        emit error(QString("Unable to load file '%1'").arg(filename));
    }
}

void StatusManager::defineStatus(const QString& name, Holder value)
{
    ::gsim::core::utils::initialize(value);
    m_data->status.insert(std::make_pair(name, value));
}

