#include "PlotModel.hpp"
#include <iostream>

using namespace gsim::qwt;

PlotModel::PlotModel(QObject *parent) : 
    ::gsim::qt::MessageModel(MessageModel::kIn, parent)
{
}

PlotModel::~PlotModel()
{
}

bool PlotModel::isCheckable(qt::TypeDescriptor_ptr descriptor) const
{
    while (descriptor->is_repeated())
        descriptor = descriptor->get_slice();

   return (descriptor->is_primitive());
}

