// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * MessageForm.hpp
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

#ifndef GSIM_QT_OPERATIONFORM_HPP
#define GSIM_QT_OPERATIONFORM_HPP

#include <QtGui>
#include <gsim/qt/export.hpp>
#include <gsim/qt/types.hpp>
#include <gsim/qt/tools/Sender.hpp>
#include <gsim/qt/widgets/FormWidget.hpp>
#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/reflective/DragAndDrop.hpp>

namespace gsim 
{
namespace qt 
{

class ParametersFromFilesTool;

class GSIM_QT_DECLSPEC MessageFormWidget : 
    public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    MessageFormWidget(
            TypeDescriptor_ptr reflective,
            const QString& messageName,
            QWidget * parent = 0);
    virtual ~MessageFormWidget();

    TypeDescriptor_ptr getReflective() const;

    // Property value
    void setValue(const QVariant& var);
    QVariant value();

    Message_ptr createMessage();

    void toHolder(Holder h);

    void save(QVariant& settings);
    void load(const QVariant& settings);
    
    void _setReadOnly(bool readOnly);

public slots:

    void setValue(Message_ptr req);

protected:

    TypeDescriptor_ptr m_reflective;
    std::string m_messageName;

    ReflectiveWidget * m_widget;

    Drag * m_drag;
    Drop * m_drop;
};


class GSIM_QT_DECLSPEC MessageForm : 
    public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString code READ code WRITE setCode)
public:
    MessageForm(QWidget * parent = 0);
    virtual ~MessageForm();
    
    void initialize(Connection_ptr, 
            const QString& messageName,
            TypeDescriptor_ptr);

    void showSizeGrip(bool show);

    // Property code
    void setCode(const QString& code);
    QString code() const;
    
    Message_ptr createMessage();

    MessageFormWidget * getWidget();
    ParametersFromFilesTool * getFiles();

    void save(QVariant& settings);
    void load(const QVariant& settings);
    
    void _setReadOnly(bool readOnly);

protected:

    TypeDescriptor_ptr m_reflective;

    QPlainTextEdit * m_code;
    MessageFormWidget * m_widget;
    ParametersFromFilesTool * m_files;

    QWidget * m_sizeGrip;
};

class GSIM_QT_DECLSPEC MessageSender : 
    public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Connection_ptr connection READ connection)
public:
    MessageSender(Connection_ptr connection,
            QWidget * parent = 0);
    virtual ~MessageSender();

    void initialize(const QString& messageName,
            TypeDescriptor_ptr);

    Connection_ptr connection() const;

    void save(QVariant& settings);
    void load(const QVariant& settings);

    MessageForm * getForm() const;

    TypeDescriptor_ptr getReflective() const;

    void _setReadOnly(bool readOnly);

public slots:

    void stop();

signals:

    void updateForm(Message_ptr req);
    void addSender(SenderConfig_ptr cfg);
    void deleteSender(SenderConfig_ptr cfg);

protected slots:

    void reset();

    void playClicked(bool play);
    void finished();
    void activeUpdateForm(bool update);
    
    void incrementBar();

protected:

    class Data;
    Data * m_data;
};

} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_OPERATIONFORM_HPP */

