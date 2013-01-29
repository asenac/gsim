// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ScriptEvaluatorWidget.cpp
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

#include "ScriptEvaluator.hpp"

#include <gsim/json/reflective.hpp>
#include <gsim/qt/widgets/private/ScriptEditor.hpp>
#include <gsim/qt/item/ModelNode.hpp>
#include <gsim/qt/json.hpp>
#include <gsim/core/event.hpp>
//#include <gsim/qt/MessageForm.hpp>

// Debug
#include <iostream>
#include <fstream>

using namespace gsim::qt;

MessageEvaluator::MessageEvaluator(
        TypeDescriptor_ptr reflective,
        QObject * parent) :
    QObject(parent), m_reflective(reflective), 
    m_clazz(&m_engine)
{
}

MessageEvaluator::~MessageEvaluator()
{
}

void MessageEvaluator::evaluate(const QString& code)
{
    m_engine.evaluate(code);

    if (m_engine.hasUncaughtException())
    {
        // TODO
    }
    else
    {
        // Look for this functions
        m_initFunc = m_engine.evaluate("init");
        m_preFunc = m_engine.evaluate("pre");
        m_postFunc = m_engine.evaluate("post");

        m_engine.clearExceptions();
    }
}

void MessageEvaluator::init(Message_ptr req)
{   
    call(m_initFunc, req);
}

void MessageEvaluator::pre(Message_ptr req)
{
    call(m_preFunc, req);
}

void MessageEvaluator::post(Message_ptr req)
{
    call(m_postFunc, req);
}

void MessageEvaluator::call(QScriptValue& func, 
        Message_ptr req)
{
    if (func.isFunction())
    {
        Node_ptr node(new Node(m_reflective, req->holder()));

        QScriptValue thisObject = m_engine.newObject(&m_clazz,
                m_engine.newVariant(qVariantFromValue(node)));

        func.call(thisObject);
    }
}

bool MessageEvaluator::hasError() const
{
    return m_engine.hasUncaughtException();
}

QString MessageEvaluator::error() const
{
    return m_engine.uncaughtException().toString();
}

