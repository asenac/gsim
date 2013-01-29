// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * SendMessageDialog.hpp
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

#ifndef GSIM_QT_REQUESTDIALOG_HPP
#define GSIM_QT_REQUESTDIALOG_HPP

#include <QDialog>
#include <QTimer>
#include <gsim/qt/types.hpp>
#include <gsim/qt/export.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>

// fwd
class QSpinBox;
class QCheckBox;
class QPushButton;
class QTabWidget;
class QPlainTextEdit;

namespace gsim 
{
namespace qt 
{

class GSIM_QT_DECLSPEC SendMessageDialog : 
    public QDialog
{
    Q_OBJECT
public:
        
    SendMessageDialog(
            const std::string& name,
            TypeDescriptor_ptr reflective,
            QWidget * parent = 0);
    virtual ~SendMessageDialog();
            
    TypeDescriptor_ptr getReflective() const;

    void save(QVariant& settings);
    void load(const QVariant& settings);

public slots:

	void sendClicked(); 
	void sendStored(); 
	void startStopChecked(bool = true); 
    void storeMessage();
    void stopTimer();

signals:

    void sendMessage(Message_ptr);

protected:

    class Data;
    Data * m_data;

    void hideEvent(QHideEvent * event);

    void resizeEvent(QResizeEvent * event);

    Message_ptr createMessage();
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_REQUESTDIALOG_HPP */

