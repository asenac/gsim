// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ModelNode.hpp
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

#ifndef GSIM_QT_MODELNODE_HPP
#define GSIM_QT_MODELNODE_HPP

#include <boost/shared_ptr.hpp>
#include <gsim/qt/types.hpp>

namespace gsim 
{
namespace qt 
{

struct DescriptorNode;
typedef boost::shared_ptr< DescriptorNode > DescriptorNode_ptr;

struct Node;
typedef boost::shared_ptr< Node > Node_ptr;

struct MetaNode;
typedef boost::shared_ptr< MetaNode > MetaNode_ptr;

struct GroupNode;
typedef boost::shared_ptr< GroupNode > GroupNode_ptr;

struct InstanceNode;
typedef boost::shared_ptr< InstanceNode > InstanceNode_ptr;

typedef const gsim::core::descriptor_group_ptr DescriptorGroup_ptr;

struct AbstractNode
{
    bool checked;
    AbstractNode();
    virtual ~AbstractNode();
};

struct GroupNode : public AbstractNode
{
    Connection_ptr instance;
    DescriptorGroup_ptr group;
    InstanceNode * parent;
    unsigned int index;

    bool initialized;
    std::vector< DescriptorNode_ptr > children;

    std::string text;

    GroupNode(Connection_ptr o, DescriptorGroup_ptr g, InstanceNode * p = 0,
            unsigned int i = 0);

    void initialize();

    void check_for_initialized();

    void reset();
};

struct InstanceNode : public AbstractNode
{
    Connection_ptr instance;

    bool initialized;
    std::vector< GroupNode_ptr > children;

    InstanceNode(Connection_ptr);

    void initialize();

    void check_for_initialized();

    void reset();
};

struct DescriptorNode : public AbstractNode
{
    TypeDescriptor_ptr descriptor;
    
    GroupNode * instance;

    DescriptorNode * parent;
    unsigned int index;

    std::string text;

    bool initialized;
    std::vector< DescriptorNode_ptr > children;

    DescriptorNode(TypeDescriptor_ptr r,
            GroupNode * i = 0,
            DescriptorNode * p = 0, 
            unsigned int idx = 0);

    void initialize();

    void check_for_initialized();

    void reset();
};

struct Node : public AbstractNode
{
    TypeDescriptor_ptr descriptor;
    core::holder holder;

    Node * parent;
    unsigned int index;

    bool initialized;
    std::vector< Node_ptr > children;

    Node(TypeDescriptor_ptr r,
            core::holder h, Node * p = 0, 
            unsigned int idx = 0);

    void initialize();

    void check_for_initialized();

    void reset();
};

struct MetaNode : public AbstractNode
{
    TypeDescriptor_ptr descriptor;

    MetaNode * parent;
    unsigned int index;

    bool initialized;
    std::vector< MetaNode_ptr > children;
    std::vector< Node_ptr > brothers;

    MetaNode(TypeDescriptor_ptr r,
            MetaNode * p = 0, 
            unsigned int idx = 0);

    void initialize();

    void check_for_initialized();

    void reset();
};


} // namespace qt
} // namespace gsim

#endif /* GSIM_QT_MODELNODE_HPP */

