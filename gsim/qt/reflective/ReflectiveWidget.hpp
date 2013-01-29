// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ReflectiveGUI.hpp
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

#ifndef GSIM_QT_REFLECTIVEWIDGET_HPP
#define GSIM_QT_REFLECTIVEWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>

// ReflectiveQWidget
#include <gsim/core/holder.hpp>
#include <gsim/core/descriptor_utils.hpp>

namespace gsim 
{
namespace qt 
{

/**
 * @brief This is the base class of all widget abstraction 
 *        associated to a type descriptor.
 */
class GSIM_QT_DECLSPEC ReflectiveWidget
{
protected:

    ReflectiveWidget(TypeDescriptor_ptr reflective);

public:

    virtual ~ReflectiveWidget();

    TypeDescriptor_ptr getReflective() const;

    virtual void toHolder(Holder& holder) = 0;
    virtual void fromHolder(Holder& holder) = 0;

    virtual void save(QVariant& settings) = 0;
    virtual void load(const QVariant& settings) = 0;

    virtual void _setReadOnly(bool readOnly);

    virtual const char * editingFinishedSignal() const;

    bool isBigWidget() const;
    void setBigWidget(bool isBig);

    bool supportDrag() const;
    void setSupportDrag(bool supportDrag);

protected:

    TypeDescriptor_ptr m_reflective;

    bool m_isBigWidget;
    bool m_supportDrag;
};

/**
 * @brief Envoltorio para generación rápida de paneles. 
 */
class GSIM_QT_DECLSPEC ReflectiveQWidget : public QWidget
{
    Q_OBJECT
public:

    ReflectiveQWidget(const QString& fqn, QWidget * parent = 0);
    virtual ~ReflectiveQWidget();

    template< typename T >
    void toValue(T& t_);

    template< typename T >
    void fromValue(const T& t_);

    ReflectiveWidget * implementation() const;

signals:

    void editingFinished();

protected:

    ReflectiveWidget * m_impl;
};

template < typename T >
void ReflectiveQWidget::toValue(T& t_)
{
    if (m_impl)
    {
        Holder src(m_impl->getReflective()->create_holder());
        Holder dst( ::gsim::core::create_holder(t_));

        m_impl->toHolder(dst);

        ::gsim::core::utils::reflective_copy(src, dst);
    }
}

template < typename T >
void ReflectiveQWidget::fromValue(const T& t_)
{
    if (m_impl)
    {
        Holder src( ::gsim::core::create_holder(const_cast< T& >(t_)));
        Holder dst(m_impl->getReflective()->create_holder());

        ::gsim::core::utils::reflective_copy(src, dst);

        m_impl->fromHolder(dst);
    }
}

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_REFLECTIVEWIDGET_HPP */

