// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * Plot.hpp
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

#ifndef GSIM_QWT_PRIVATE_PLOT_HPP
#define GSIM_QWT_PRIVATE_PLOT_HPP

#include <qwt_plot.h>
#include <QWidget>

namespace gsim 
{
namespace qwt 
{
namespace priv
{

class HistoricData;

class Plot : public QwtPlot
{
    Q_OBJECT
public:

    Plot(QWidget *parent = NULL);

public slots:

    void append(const QVector< double >& v);
    void append(double v);

    void displayedSamples(int samples);
    void clear();

protected slots:

    void update();

protected:

    void populate();

    unsigned long long m_idx;
    QwtInterval m_interval;
    HistoricData * m_data;
};

} // namespace priv
} // namespace qwt
} // namespace gsim

#endif /* GSIM_QWT_PRIVATE_PLOT_HPP */

