// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * SimplePlot.hpp
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

#ifndef GSIM_QWT_SIMPLEPLOT_HPP
#define GSIM_QWT_SIMPLEPLOT_HPP

#include <QWidget>
#include <QString>
#include <QVector>

#include <gsim/qwt/export.hpp>

namespace gsim 
{
namespace qwt 
{

class GSIM_QWT_DECLSPEC SimplePlot : public QWidget
{
    Q_OBJECT
public:

    SimplePlot(QWidget * parent = 0);
    virtual ~SimplePlot();

public slots:

    void append(const QVector< double >& v);
    void append(double v);

protected slots:

    void updateY();

protected:

    struct Data;

    Data * m_data;
};

} // namespace qwt
} // namespace gsim

#endif /* GSIM_QWT_SIMPLEPLOT_HPP */

