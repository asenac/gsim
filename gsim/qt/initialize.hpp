// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * initialize.hpp
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

#ifndef GSIM_QT_INITIALIZE_HPP
#define GSIM_QT_INITIALIZE_HPP

namespace gsim 
{
namespace qt 
{

/**
 * @brief Inicializa las estructuras estáticas de la librería GSIM si no
 *        se inicializaron previamente. 
 */
void initialize();

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_INITIALIZE_HPP */

