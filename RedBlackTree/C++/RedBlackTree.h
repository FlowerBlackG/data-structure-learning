/**
 * Red Black Tree H
 * by Flower Black
 * 2022.1
 * at Yushan County, Shangrao, Jiangxi
 */

#pragma once

template <typename KeyType, typename DataType>
class RedBlackTree {

public:
	/** 树的生命相关操作。 */
	RedBlackTree();
	~RedBlackTree();

	/**
	 * 清空树中所有元素。
	 */
	void clear();

public:
	/** 树的基本查询操作。 */

	/**
	 * 判断键是否在树里。
	 * 
	 * @param queryKey 待判断的键。
	 * @return 是否在树上找到了对应键。
	 */
	bool hasKey(const KeyType& queryKey);

	/**
	 * 根据键获取数据。
	 * 
	 * @param key 键。
	 * @return 键对应的数据。
	 * @exception runtime_error 如果无法找到键，会抛出异常。
	 */
	DataType& getData(const KeyType& key);

	/**
	 * 设置数据。如果键已经存在，会更新原有数据。
	 * 
	 * @param key 键。
	 * @param data 数据。
	 */
	RedBlackTree<KeyType, DataType>& setData(const KeyType& key, const DataType& data);

	/**
	 * 删除键。
	 * 
	 * @param key 键。
	 * @return 红黑树对象自身。
	 */
	RedBlackTree<KeyType, DataType>& removeKey(const KeyType& key);

private:
	enum class NodeColor {
		RED, BLACK
	};
	enum class ChildSide {
		LEFT, RIGHT
	};
	struct Node {
		KeyType key;
		DataType data;
		NodeColor color = NodeColor::RED;
		Node* father = nullptr;
		Node* leftChild = nullptr;
		Node* rightChild = nullptr;
	};

private:
	/**
	 * 释放该节点及其所有子节点。
	 * 
	 * @param node 递归释放的第一个节点。
	 */
	void cleanup(Node* node);

	/**
	 * 左旋。
	 * 
	 * @param node 支点。
	 * @exception 若支点的右子树不存在，会引发未定义的行为（undefined behaviour).
	 */
	void rotateLeft(Node* node);

	/**
	 * 右旋。
	 * 
	 * @param node 支点。
	 * @exception 若支点的左子树不存在，会引发未定义的行为（undefined behaviour).
	 */
	void rotateRight(Node* node);

	/**
	 * 修复“连续红色节点”问题。当出现当前节点和父节点同为红色时，需要进行修复。
	 * 
	 * @param node “连续红色节点”中的子节点。
	 * @exception 若node是nullptr，会产生未定义的行为。
	 */
	void fixContinuousRedNodeProblem(Node* node);

	/**
	 * 修复“左右孩子不平衡”问题。当某边删除黑色节点后，可能产生此问题。
	 * 
	 * @param node 失衡问题所在的较轻的节点。
	 * @exception 若参数不正确，会产生未定义的行为。
	 */
	void fixUnbalancedChildrenProblem(Node* node);

private:
	/**
	 * 根节点。
	 */
	Node* root = nullptr;

};
