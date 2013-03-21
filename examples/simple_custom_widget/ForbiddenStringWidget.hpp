#ifndef SIMPLE_FORBIDDENSTRINGWIDGET_HPP
#define SIMPLE_FORBIDDENSTRINGWIDGET_HPP

#include <QtGui>

#include <gsim/qt/reflective/ReflectiveWidget.hpp>
#include <gsim/qt/reflective/CustomWidgets.hpp>

namespace simple
{

class ForbiddenStringWidget : 
    public QLineEdit, public gsim::qt::ReflectiveWidget
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue)
public:
    ForbiddenStringWidget(gsim::qt::TypeDescriptor_ptr descriptor,
            QWidget * parent = 0);
    virtual ~ForbiddenStringWidget();

    virtual void toHolder(gsim::qt::Holder& holder);
    virtual void fromHolder(gsim::qt::Holder& holder);

    void save(QVariant& settings);
    void load(const QVariant& settings);

    QString value() const;
    void setValue(const QString& value);
    
    void _setReadOnly(bool readOnly);

    static QWidget * create(gsim::qt::TypeDescriptor_ptr descriptor,
            QWidget * parent = 0);

protected slots:

    void check(const QString& text);

protected:
    QStringList m_forbiddenWords;
};

} // namespace simple

#define SIMPLE_FORBIDDENWORDS(fqn, words)                      \
    GSIM_PROPERTY(fqn, "widget", std::string("forbiddenwords"))\
    GSIM_PROPERTY(fqn, "value.forbidden", std::string(words))  \
    /***/

#endif /* SIMPLE_FORBIDDENSTRINGWIDGET_HPP */

