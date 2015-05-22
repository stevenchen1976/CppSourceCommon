#ifndef DEVICE_TREE_H
#define DEVICE_TREE_H
#include <string>
#include <vector>
#include "VideoSDK.h"
#include "tree.hh"

class DeviceNode
{
public:
	DeviceNode(void);
	explicit DeviceNode(const std::string& nodeid);
	explicit DeviceNode(const std::string& nodeid, const std::string& nodename, NODE_TYPE nodetype, 
		NODE_STATUS nodestatus, const std::string& pri);
	DeviceNode(const DeviceNode &object);//�������캯��
	DeviceNode& operator = (const DeviceNode& object);//��ֵ����
	bool operator ==(const DeviceNode &rth) const;
	bool operator !=(const DeviceNode &rth) const;
	~DeviceNode(void);

public:
	std::string szNodeID;//�ڵ�id
	std::string sNodeName;//�ڵ�����
	NODE_TYPE eNodeType;//�ڵ����ͣ����豸������ͨ������Ƶͨ�������ɲο�NODE_TYPE�Ķ���
	NODE_STATUS eNodeStatus;//�ڵ�״̬���豸����״̬��ͨ��û�б�ʶ״̬��
	std::string sPrivilege;//�Ըýڵ�Ĳ���Ȩ��(������)
};

typedef tree<DeviceNode>::pre_order_iterator tree_iterator;

class DeviceTree
{
private:
	tree<DeviceNode> tree_;
public:
	tree_iterator set_head(const DeviceNode& node);
	tree_iterator insert(tree_iterator iter, const DeviceNode& node);
	tree_iterator insert_after(tree_iterator iter, const DeviceNode& node);
	tree_iterator append_child(tree_iterator iter, const DeviceNode& node);
	void erase_child(tree_iterator iter);
	tree_iterator next_sibling(tree_iterator iter);
	int number_of_child(tree_iterator iter);
	int number_of_siblings(tree_iterator iter);
	tree_iterator replace(tree_iterator it, const DeviceNode& node);
	tree_iterator find_node(tree_iterator it_begin, tree_iterator it_end, const DeviceNode& node);
	tree_iterator find_node(tree_iterator it_begin, tree_iterator it_end, const char* nodeid);
	tree_iterator begin();
	//tree_iterator silbing_begin(tree_iterator iter);
	tree_iterator child_begin(tree_iterator iter);
	tree_iterator end();
	//tree_iterator silbing_end(tree_iterator iter);
	tree_iterator child_end(tree_iterator iter);
	int depth(tree_iterator iter);
	void Clear();
};

#endif