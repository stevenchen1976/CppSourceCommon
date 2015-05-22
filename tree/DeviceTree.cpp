#include "stdafx.h"
#include "DeviceTree.h"
#include <algorithm>

DeviceNode::DeviceNode()
    :eNodeType(NODE_TYPE_NULL)
    ,eNodeStatus(NODE_STATUS_OFFLINE)
{
}

DeviceNode::DeviceNode(const std::string& nodeid)
    :eNodeType(NODE_TYPE_NULL)
    ,eNodeStatus(NODE_STATUS_OFFLINE)
    ,szNodeID(nodeid)
{
}

DeviceNode::DeviceNode(const std::string& nodeid, const std::string& nodename, NODE_TYPE nodetype,
                       NODE_STATUS nodestatus, const std::string& pri)
    :eNodeStatus(nodestatus),eNodeType(nodetype),sNodeName(nodename)
    ,szNodeID(nodeid),sPrivilege(pri)
{

}

DeviceNode::DeviceNode( const DeviceNode& object )
    :eNodeStatus(object.eNodeStatus)
    ,eNodeType(object.eNodeType)
    ,sNodeName(object.sNodeName)
    ,szNodeID(object.szNodeID)
    ,sPrivilege(object.sPrivilege)
{

}


DeviceNode& DeviceNode::operator=( const DeviceNode& object )
{
    if (this == &object) {
        return *this;
    }
    eNodeStatus=object.eNodeStatus;
    eNodeType=object.eNodeType;
    sNodeName=object.sNodeName;
    szNodeID=object.szNodeID;
    sPrivilege=object.sPrivilege;
    return *this;
}

bool DeviceNode::operator ==(const DeviceNode  &rth) const
{
    return this->szNodeID == rth.szNodeID;
}

bool DeviceNode::operator !=(const DeviceNode &rth) const
{
    return this->szNodeID != rth.szNodeID;
}

DeviceNode::~DeviceNode( void )
{

}
//////////////////////////////////////////////////////////////////////////
tree_iterator DeviceTree::set_head( const DeviceNode& node )
{
    return tree_.set_head(node);
}

tree_iterator DeviceTree::insert( tree_iterator iter, const DeviceNode& node )
{
    return tree_.insert(iter,node);
}

tree_iterator DeviceTree::insert_after( tree_iterator iter, const DeviceNode& node )
{
    return tree_.insert_after(iter,node);
}

void DeviceTree::erase_child( tree_iterator iter )
{
    tree_.erase_children(iter);
}

tree_iterator DeviceTree::append_child( tree_iterator iter, const DeviceNode& node )
{
    return tree_.append_child(iter,node);
}

tree_iterator DeviceTree::next_sibling( tree_iterator iter )
{
    return tree_.next_sibling(iter);
}

int DeviceTree::number_of_child( tree_iterator iter )
{
    return tree_.number_of_children(iter);
}

int DeviceTree::number_of_siblings( tree_iterator iter )
{
    return tree_.number_of_siblings(iter);
}

tree_iterator DeviceTree::replace( tree_iterator it, const DeviceNode& node )
{
    return tree_.replace(it,node);
}

tree_iterator DeviceTree::find_node( tree_iterator it_begin, tree_iterator it_end, const DeviceNode& node )
{
    return std::find(it_begin,it_end,node);
}

tree_iterator DeviceTree::find_node( tree_iterator it_begin, tree_iterator it_end, const char* nodeid )
{
    return std::find(it_begin,it_end,DeviceNode(nodeid));
}

tree_iterator DeviceTree::begin()
{
    return tree_.begin();
}

//tree_iterator DeviceTree::silbing_begin( tree_iterator iter )//¶¼ÊÇchild_begin
//{
//    //return iter.begin();//child begin
//    return tree_.begin(iter);//first child's silbing
//}
//tree_iterator DeviceTree::silbing_end( tree_iterator iter )
//{
//    //return iter.end();
//    return tree_.end(iter);
//}

tree_iterator DeviceTree::child_begin( tree_iterator iter )
{
    //return iter.begin();
    return tree_.begin(iter);
}

tree_iterator DeviceTree::end()
{
    return tree_.end();
}

tree_iterator DeviceTree::child_end( tree_iterator iter )
{
    //return iter.end();
    return tree_.end(iter);
}
int DeviceTree::depth( tree_iterator iter )
{
    return tree_.depth(iter);
}

void DeviceTree::Clear()
{
	return tree_.clear();
}


