// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ScriptEvaluatorWidget.hpp
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

#ifndef GSIM_QT_SCRIPTEVALUATOR_HPP
#define GSIM_QT_SCRIPTEVALUATOR_HPP

#include <QtGui>
#include <QtScript>
#include <boost/shared_ptr.hpp>
#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/script/ReflectiveScriptClass.hpp>

namespace gsim 
{
namespace qt 
{

class MessageSender;

class MessageEvaluator;

typedef boost::shared_ptr< MessageEvaluator > MessageEvaluator_ptr;

class GSIM_QT_DECLSPEC MessageEvaluator : 
    public QObject
{
    Q_OBJECT
public:
    MessageEvaluator(
            TypeDescriptor_ptr,
            QObject * parent = 0);
    virtual ~MessageEvaluator();

    bool hasError() const;
    QString error() const;

public slots:

    void evaluate(const QString& code);
    void init(Message_ptr);
    void pre(Message_ptr);
    void post(Message_ptr);

protected:

    void call(QScriptValue& func, Message_ptr req);

    TypeDescriptor_ptr m_reflective;

    QScriptEngine m_engine;
    ReflectiveScriptClass m_clazz;
    QScriptValue m_initFunc;
    QScriptValue m_preFunc;
    QScriptValue m_postFunc;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_SCRIPTEVALUATOR_HPP */

