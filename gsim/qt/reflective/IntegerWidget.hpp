// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * IntegerWidget.hpp
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

#ifndef GSIM_QT_INTEGERWIDGET_HPP
#define GSIM_QT_INTEGERWIDGET_HPP

#include <QtGui>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC IntegerWidget : 
    public QSpinBox, public ReflectiveWidget
{
    Q_OBJECT
    Q_PROPERTY(int validMinimum READ validMinimum WRITE setValidMinimum)
    Q_PROPERTY(int validMaximum READ validMaximum WRITE setValidMaximum)
    Q_PROPERTY(QString tags READ tags WRITE setTags)
public:
    IntegerWidget(TypeDescriptor_ptr reflective,
            QWidget * parent = 0,
            const QString& tags = QString(),
            TypeDescriptor_ptr enumDescriptor = 0);
    virtual ~IntegerWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);
    
    void save(QVariant& settings);
    void load(const QVariant& settings);

    void _setReadOnly(bool readOnly);

    static QWidget * create(TypeDescriptor_ptr reflective,
            QWidget * parent = 0);

    static QWidget * createWithEnum(TypeDescriptor_ptr reflective,
            TypeDescriptor_ptr enumDescriptor,
            QWidget * parent = 0);

    static QWidget * createWithTags(TypeDescriptor_ptr reflective,
            const QString& tags,
            QWidget * parent = 0);

    int validMinimum() const;
    int validMaximum() const;

    QString tags() const;

public slots:

    void setValidMinimum(int m);
    void setValidMaximum(int m);

    void setTags(const QString& tags);

private slots:

    void updateSuffix(int);

    void checkValidValue(int value);

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_INTEGERWIDGET_HPP */

