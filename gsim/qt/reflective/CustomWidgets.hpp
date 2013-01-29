// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * CustomWidgets.hpp
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

#ifndef GSIM_QT_CUSTOMWIDGETS_HPP
#define GSIM_QT_CUSTOMWIDGETS_HPP

#include <boost/bind.hpp>
#include <gsim/core/properties.hpp>
#include <gsim/qt/widgets/ConnectionWidget.hpp>
#include <gsim/qt/reflective/QVariantHelper.hpp>
#include <gsim/qt/reflective/LSBWidget.hpp>
#include <gsim/qt/reflective/FixedPointWidget.hpp>

#define GSIM_VARIANT_REGISTER_HELPER(TYPE, HELPER)               \
    { using namespace ::gsim::qt;                                \
      QVariantHelperRepository::instance()->setHelper(           \
            GSIM_STR(TYPE), HELPER); }                          \
    /***/

////////////////////////////////////////////////////////////////
//
// Widgets
//
////////////////////////////////////////////////////////////////

/**
 * @brief Muestra un tipo entero como un valor de coma fija con
 *        LSB.
 *
 * @param fqn El FQN del tipo o miembro.
 * @param LSB El LSB.
 */
#define GSIM_LSB(fqn, LSB)                                     \
    GSIM_PROPERTY(fqn, "widget", std::string("lsb"))           \
    GSIM_PROPERTY(fqn, "value.lsb", LSB)                       \
    GSIM_VARIANT_REGISTER_HELPER(fqn, LSBHelper::create(LSB)); \
    /***/

/**
 * @brief Muestra un tipo entero como un valor de coma fija
 *        especificando el número de bits para la parte real.
 *
 * @param fqn El FQN del tipo o miembro.
 * @param BITS El número de bits para la parte real.
 */
#define GSIM_FIXEDPOINT(fqn, BITS)                             \
    GSIM_PROPERTY(fqn, "widget", std::string("fixed_point"))   \
    GSIM_PROPERTY(fqn, "value.fractional_bits", BITS)          \
    GSIM_VARIANT_REGISTER_HELPER(fqn, FixedPointHelper::create(BITS));\
    /***/

/**
 * @brief Muestra un tipo enumerado como un entero etiquetado.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_INTEGER(fqn)                                       \
    GSIM_PROPERTY(fqn, "widget", std::string("integer"))        \
    /***/

/**
 * @brief Muestra un tipo entero en Binary-Coded Decimal.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_BCD(fqn)                                           \
    GSIM_PROPERTY(fqn, "widget", std::string("bcd"))            \
    /***/

/**
 * @brief Muestra un tipo entero en hexadecimal.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_HEX(fqn)                                           \
    GSIM_PROPERTY(fqn, "widget", std::string("hex"))            \
    /***/

/**
 * @brief Muestra un tipo string como una secuencia de números.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_SEQUENCE(fqn)                                      \
    GSIM_PROPERTY(fqn, "widget", std::string("sequence"))       \
    /***/

/**
 * @brief Muestra cualquier tipo como un árbol.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_TREE(fqn)                                          \
    GSIM_PROPERTY(fqn, "widget", std::string("tree"))           \
    /***/

/**
 * @brief Muestra cualquier tipo como una tabla.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_TABLE(fqn)                                         \
    GSIM_PROPERTY(fqn, "widget", std::string("table"))          \
    /***/

/**
 * @brief Muestra un tipo entero como una máscara de bits.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_BITSET(fqn)                                        \
    GSIM_PROPERTY(fqn, "widget", std::string("bitset"))         \
    /***/

// TODO GSIM_BITSET_WITH_TAGS

/**
 * @brief Muestra un tipo entero o string como una IPv4.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_IPADDRESS(fqn)                                     \
    GSIM_PROPERTY(fqn, "widget", std::string("ipv4"))           \
    /***/

/**
 * @brief Muestra un tipo string como un caja de texto grande.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_BIGTEXT(fqn)                                       \
    GSIM_PROPERTY(fqn, "widget", std::string("big_text"))       \
    /***/

/**
 * @brief Muestra un tipo estructura o array como un DateTimeEdit.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_DATETIME(fqn)                                      \
    GSIM_PROPERTY(fqn, "widget", std::string("date_time"))      \
    /***/

/**
 * @brief Muestra un tipo estructura o array como un DateTimeEdit
 *        especificando el formato de edición.
 *
 * @param fqn El FQN del tipo o miembro.
 */
#define GSIM_DATETIME_WITH_FORMAT(fqn, FORMAT)                  \
    GSIM_PROPERTY(fqn, "widget", std::string("date_time"))      \
    GSIM_PROPERTY(fqn, "widget.format", std::string(FORMAT))    \
    /***/

////////////////////////////////////////////////////////////////
//
// Propiedades
//
////////////////////////////////////////////////////////////////

/**
 * @brief Define el rango válido para un tipo numérico. 
 *
 * @param fqn El FQN del tipo o miembro.
 * @param min El valor mínimo.
 * @param max El valor máximo.
 */
#define GSIM_RANGE(fqn, min, max)			        \
    GSIM_PROPERTY(fqn, "value.min", min)		    \
    GSIM_PROPERTY(fqn, "value.max", max)		    \
    /***/

/**
 * @brief Define el tooltip que se mostrará para cualquier tipo. 
 *
 * @param fqn El FQN del tipo o miembro.
 * @param text El texto mostrado en el tooltip.
 */
#define GSIM_TOOLTIP(fqn, text)				            \
    GSIM_PROPERTY(fqn, "widget.tooltip", text)		    \
    /***/

/**
 * @brief Define el valor por defecto para cualquier tipo 
 *        primitivo. 
 *
 * @param fqn El FQN del tipo o miembro.
 * @param value El valor por defecto.
 */
#define GSIM_DEFAULT_VALUE(fqn, value)		            \
    GSIM_PROPERTY(fqn, "widget.value", value)		    \
    /***/

/**
 * @brief Define la unidad de un tipo primitivo. Esta unidad se
 *        mostrará en el spinbox asociado al tipo.
 *
 * @param fqn El FQN del tipo o miembro.
 * @param value El texto de la unidad.
 */
#define GSIM_UNIT(fqn, value)       		                    \
    GSIM_PROPERTY(fqn, "widget.suffix", std::string(" " value)) \
    /***/

/**
 * @brief Define el incremento del spinbox asociado a un tipo
 *        numérico.
 *
 * @param fqn El FQN del tipo o miembro.
 * @param value El valor de incremento.
 */
#define GSIM_STEP(fqn, value)       		                    \
    GSIM_PROPERTY(fqn, "widget.step", value)                    \
    /***/

/**
 * @brief Define el número de decimales con los que se mostrará un
 *        tipo numérico.
 *
 * @param fqn El FQN del tipo o miembro.
 * @param value El número de decimales.
 */
#define GSIM_DECIMALS(fqn, value)       		                \
    GSIM_PROPERTY(fqn, "widget.decimals", value)                \
    /***/

/**
 * @brief Define la longitud máxima de un tipo secuencia. 
 *
 * @param fqn El FQN del tipo o miembro.
 * @param value La lonfigud máxima.
 */
#define GSIM_MAXLENGTH(fqn, value)       		                \
    GSIM_PROPERTY(fqn, "widget.max_length", value)              \
    /***/

/**
 * @brief Define el estilo para cualquier tipo. 
 *
 * @param fqn El FQN del tipo o miembro.
 * @param text El texto del estilo.
 */
#define GSIM_STYLESHEET(fqn, text)				                \
    GSIM_PROPERTY(fqn, "widget.style_sheet", text)		        \
    /***/

#endif /* GSIM_QT_CUSTOMWIDGETS_HPP */

