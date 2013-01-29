// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * SequenceWidget.hpp
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

#ifndef GSIM_QT_SEQUENCEWIDGET_HPP
#define GSIM_QT_SEQUENCEWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/reflective/WidgetFactory.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC SequenceWidget : 
    public QWidget, public ReflectiveWidget
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
public:
    SequenceWidget(TypeDescriptor_ptr reflective,
            QWidget * parent = 0);
    virtual ~SequenceWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);

    void save(QVariant& settings);
    void load(const QVariant& settings);

    void setValue(const QVariant& var);
    QVariant value() const;
    
    void _setReadOnly(bool readOnly);

    static QWidget * create(TypeDescriptor_ptr, QWidget * parent = NULL);

    int maxLength() const;
    
public slots:

    void setMaxLength(int length);

protected slots:

    void lengthChanged(int);
    void indexChanged(int);
    void saveCurrent();

signals:

    void editingFinished();

protected:

    QSpinBox * m_sbLength;
    QSpinBox * m_sbCurrentIndex;

    ReflectiveWidget * m_slice;
    QWidget * m_slice_widget;

    Holder m_holder;
};

class GSIM_QT_DECLSPEC ComplexSequenceWidget : 
    public QWidget, public ReflectiveWidget
{
    Q_OBJECT
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
public:
    ComplexSequenceWidget(TypeDescriptor_ptr reflective,
            WidgetFactory_t factory,
            QWidget * parent = 0);
    virtual ~ComplexSequenceWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);

    void save(QVariant& settings);
    void load(const QVariant& settings);
    
    void _setReadOnly(bool readOnly);

    int maxLength() const;
    
public slots:

    void setMaxLength(int length);

protected slots:

    void lengthChanged(int);
    void indexChanged(int);

signals:

    void editingFinished();

protected:

    WidgetFactory_t m_factory;
    QStackedWidget * m_stack;
    QSpinBox * m_sbLength;
    QSpinBox * m_sbCurrentIndex;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_SEQUENCEWIDGET_HPP */

