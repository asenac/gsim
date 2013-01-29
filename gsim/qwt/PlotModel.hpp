#ifndef GSIM_QWT_PLOTMODEL_HPP
#define GSIM_QWT_PLOTMODEL_HPP

#include <gsim/qwt/export.hpp>
#include <gsim/qt/item/MessageModel.hpp>

namespace gsim
{
namespace qwt
{

class GSIM_QWT_DECLSPEC PlotModel : 
    public ::gsim::qt::MessageModel
{
    Q_OBJECT

public:
    PlotModel(QObject *parent = 0);
    virtual ~PlotModel();

protected:
    virtual bool isCheckable(qt::TypeDescriptor_ptr descriptor) const;
};

} // qwt
} // gsim

#endif // GSIM_QWT_PLOTMODEL_HPP

