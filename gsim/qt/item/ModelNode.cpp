// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ModelNode.cpp
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

#include "ModelNode.hpp"
#include <set>
#include <gsim/qt/model/Connection.hpp>

using namespace gsim::qt;

namespace  
{
    bool is_char_array(TypeDescriptor_ptr d)
    {
        using namespace ::gsim::core;
        return d->get_type() == TYPE_ARRAY &&
            d->get_slice()->get_type() == TYPE_CHAR;
    }
} // namespace

AbstractNode::AbstractNode() : checked(false)
{
}

AbstractNode::~AbstractNode()
{
}

GroupNode::GroupNode(Connection_ptr o, DescriptorGroup_ptr g, InstanceNode * p,
        unsigned int i) :
    instance(o), group(g), parent(p), index(i),
    initialized(false)
{
}

void GroupNode::reset()
{
    initialized = false;
    children.clear();
}

void GroupNode::initialize()
{
    const unsigned int count = group->get_descriptor_count();
    children.reserve(count);

    for (unsigned int i = 0; i < count; i++) 
    {
        DescriptorNode_ptr child(new DescriptorNode(
                    group->get_descriptor_by_index(i), 
                    this, 0, i));

        // Nombre del mensaje
        child->text = group->get_descriptor_name(i), 

        children.push_back(child);
    }

    initialized = true;
}

void GroupNode::check_for_initialized()
{
    if (!initialized)
        initialize();
}

InstanceNode::InstanceNode(Connection_ptr o) :
    instance(o),
    initialized(false)
{
}

void InstanceNode::reset()
{
    initialized = false;
}

void InstanceNode::initialize()
{
    children.clear();

    GroupNode_ptr in(new GroupNode(instance, instance->descriptor()->in(),
                this, 0));

    in->text = "Incoming messages", 

    children.push_back(in);

    GroupNode_ptr out(new GroupNode(instance, instance->descriptor()->out(),
                this, 0));

    out->text = "Outgoing messages", 

    children.push_back(out);

    initialized = true;
}

void InstanceNode::check_for_initialized()
{
    if (!initialized)
        initialize();
}

//
//
// Descriptor Node
//
//
DescriptorNode::DescriptorNode(TypeDescriptor_ptr r,
        GroupNode * i,
        DescriptorNode * p, unsigned int idx) :
    descriptor(r), instance(i),
    parent(p), index(idx), initialized(false)
{
}

void DescriptorNode::reset()
{
    initialized = false;
    children.clear();
}

void DescriptorNode::initialize()
{
    if (descriptor->is_repeated() && !is_char_array(descriptor))
    {
        core::holder dummy;
        const unsigned int count = 
            (descriptor->is_variable_length())? 0:
                descriptor->get_length(dummy);

        // Restricción
        if (count <= 1024)
        {
            children.reserve(count);

            TypeDescriptor_ptr slice =
                descriptor->get_slice();

            for (unsigned int i = 0; i < count; i++) 
            {
                DescriptorNode_ptr child(new DescriptorNode(slice, 
                            instance, this, i));

                children.push_back(child);
            }
        }
    }
    else if(descriptor->get_type() == core::TYPE_STRUCT || 
            descriptor->get_type() == core::TYPE_UNION)
    {
        const unsigned int count = 
            descriptor->get_children_count();
        children.reserve(count);

        for (unsigned int i = 0; i < count; i++) 
        {
            DescriptorNode_ptr child(new DescriptorNode(
                        descriptor->get_child(i), 
                        instance,
                        this, i));

            children.push_back(child);
        }
    }

    initialized = true;
}

void DescriptorNode::check_for_initialized()
{
    if (!initialized)
        initialize();
}

// 
//
// Node
//
//
Node::Node(TypeDescriptor_ptr r,
        gsim::core::holder h, Node * p, unsigned int idx) :
    descriptor(r), holder(h), parent(p), index(idx), initialized(false)
{
}

void Node::reset()
{
    initialized = false;
    children.clear();
}

void Node::initialize()
{
    if (descriptor->is_repeated() && !is_char_array(descriptor))
    {
        const unsigned int count = descriptor->get_length(holder);
        children.reserve(count);

        TypeDescriptor_ptr slice =
            descriptor->get_slice();

        for (unsigned int i = 0; i < count; i++) 
        {
            Node_ptr child(new Node(slice, 
                    descriptor->get_child_value(holder, i), this, i));

            children.push_back(child);
        }
    }
    else if(descriptor->get_type() == core::TYPE_STRUCT)
    {
        const unsigned int count = descriptor->get_children_count();
        children.reserve(count);

        for (unsigned int i = 0; i < count; i++) 
        {
            Node_ptr child(new Node(descriptor->get_child(i), 
                    descriptor->get_child_value(holder, i), this, i));

            children.push_back(child);
        }
    }
    else if(descriptor->get_type() == core::TYPE_UNION)
    {
        children.clear();

        // discriminator
        Node_ptr child(new Node(descriptor->get_child(0), 
                descriptor->get_child_value(holder, 0), this, 0));
        children.push_back(child);

        unsigned int idx = 0;
        if ((idx = descriptor->get_length(holder)) > 0)
        {
            Node_ptr child(new Node(descriptor->get_child(idx), 
                    descriptor->get_child_value(holder, idx), this, idx));

            children.push_back(child);
        }
    }

    initialized = true;
}

void Node::check_for_initialized()
{
    if (!initialized)
        initialize();
}


MetaNode::MetaNode(TypeDescriptor_ptr r,
        MetaNode * p, unsigned int idx) :
    descriptor(r), parent(p), index(idx), initialized(false)
{
}

void MetaNode::reset()
{
    initialized = false;
    
    for (unsigned int j = 0; j < brothers.size(); j++) 
    {
        if (brothers[j]) brothers[j]->reset();
    }
}

void MetaNode::initialize()
{
    unsigned int max_childs = 0;
    for (unsigned int j = 0; j < brothers.size(); j++) 
    {
        if (brothers[j]) 
        {
            brothers[j]->check_for_initialized();

            if (brothers[j]->children.size() > max_childs)
                max_childs = brothers[j]->children.size();
        }
    }

    if (descriptor->is_repeated() && !is_char_array(descriptor))
    {
        children.reserve(max_childs);

        TypeDescriptor_ptr slice =descriptor->get_slice();

        for (unsigned int i = 0; i < max_childs; i++) 
        {
            MetaNode_ptr child(new MetaNode(slice, this, i));

            for (unsigned int j = 0; j < brothers.size(); j++) 
            {
                if (brothers[j] && i < brothers[j]->children.size())
                    child->brothers.push_back(brothers[j]->children[i]);
                else
                    child->brothers.push_back(Node_ptr());
            }

            children.push_back(child);
        }
    }
    else if(descriptor->get_type() == core::TYPE_STRUCT)
    {
        const unsigned int count = descriptor->get_children_count();
        children.reserve(count);

        for (unsigned int i = 0; i < count; i++) 
        {
            MetaNode_ptr child(new MetaNode(
                        descriptor->get_child(i),  this, i));

            for (unsigned int j = 0; j < brothers.size(); j++) 
            {
                if (brothers[j])
                    child->brothers.push_back(brothers[j]->children[i]);
                else
                    child->brothers.push_back(Node_ptr());
            }

            children.push_back(child);
        }
    }
    else if(descriptor->get_type() == core::TYPE_UNION)
    {
        // Isn't the faster approach

        children.clear();

        // Used indexes
        typedef std::set< unsigned int > idxs_t;
        idxs_t idxs;

        for (unsigned int j = 0; j < brothers.size(); j++) 
        {
            if (brothers[j]) 
            {
                for (unsigned int i = 0; i < brothers[j]->children.size(); i++) 
                {
                    idxs.insert(brothers[j]->children[i]->descriptor->get_child_index());
                }
            }
        }

        std::vector< unsigned int > tmp(brothers.size(), 0);

        for (idxs_t::const_iterator it = idxs.begin(); it != idxs.end(); ++it) 
        {
            MetaNode_ptr child(new MetaNode(descriptor->get_child(*it), this, *it));

            for (unsigned int i = 0; i < brothers.size(); i++) 
            {
                if (brothers[i] && tmp[i] < brothers[i]->children.size() 
                        && brothers[i]->children[tmp[i]]->descriptor->get_child_index() == *it)
                {
                    child->brothers.push_back(brothers[i]->children[tmp[i]]);
                    ++tmp[i];
                }
                else
                {
                    child->brothers.push_back(Node_ptr());
                }
            }

            children.push_back(child);
        }
    }

    initialized = true;
}

void MetaNode::check_for_initialized()
{
    if (!initialized)
        initialize();
}

