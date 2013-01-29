// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * BitSetWidget.hpp
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

#ifndef GSIM_QT_BITSETWIDGET_HPP
#define GSIM_QT_BITSETWIDGET_HPP

#include <QtGui>
#include <vector>

#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/widgets/FormWidget.hpp>

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC BitSetWidget : 
    public FormWidget, public ReflectiveWidget
{
    Q_OBJECT
    // TODO Q_PROPERTY(QString tags READ tags WRITE setTags)
public:
    BitSetWidget(TypeDescriptor_ptr reflective,
            const QString& tags = QString(),
            QWidget * parent = 0);
    virtual ~BitSetWidget();

    virtual void toHolder(Holder& holder);
    virtual void fromHolder(Holder& holder);
    
    void save(QVariant& settings);
    void load(const QVariant& settings);

    void _setReadOnly(bool readOnly);

    static QWidget * create(TypeDescriptor_ptr reflective,
            QWidget * parent = 0);

    static QWidget * createWithTags(TypeDescriptor_ptr reflective,
            const QString& tags,
            QWidget * parent = 0);

/*
    QString tags() const;

public slots:

    void setTags(const QString& tags);
 */

signals:

    void editingFinished();

protected:

    std::vector< QCheckBox * > m_spinBoxes;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_BITSETWIDGET_HPP */

