/**
 * Red Black Tree Hpp
 * by Flower Black
 * 2022.1
 * at Yushan County, Shangrao, Jiangxi
 */

#include <stdexcept>

#include "RedBlackTree.h"

template<typename KeyType, typename DataType>
RedBlackTree<KeyType, DataType>::RedBlackTree()
{
}

template<typename KeyType, typename DataType>
RedBlackTree<KeyType, DataType>::~RedBlackTree()
{
	if (this->root != nullptr) {
		this->cleanup(this->root);
		this->root = nullptr;
	}
}

template<typename KeyType, typename DataType>
void RedBlackTree<KeyType, DataType>::clear()
{
	if (this->root != nullptr) {
		this->cleanup(this->root);
		this->root = nullptr;
	}
}

template<typename KeyType, typename DataType>
bool RedBlackTree<KeyType, DataType>::hasKey(const KeyType& queryKey)
{
	Node* currentNode = this->root;
	
	while (currentNode != nullptr) {
		if (queryKey == currentNode->key) {
			return true; // 找到对应键。
		}
		else if (queryKey < currentNode->key) {
			currentNode = currentNode->leftChild; // 目标键小于当前键，向左查找。
		}
		else { // queryKey > currentNode->key
			currentNode = currentNode->rightChild; // 目标键大于当前键，向右查找。
		}
	}

	return false; // 找不到键。
}

template<typename KeyType, typename DataType>
DataType& RedBlackTree<KeyType, DataType>::getData(const KeyType& key)
{
	Node* currentNode = root;
	while (currentNode != nullptr) {
		if (key == currentNode->key) {
			return currentNode->data; // 找到对应键。
		}
		else if (key < currentNode->key) {
			currentNode = currentNode->leftChild; // 目标键小于当前键，向左查找。
		}
		else { // key > currentNode->key
			currentNode = currentNode->rightChild; // 目标键大于当前键，向右查找。
		}
	}

	throw std::runtime_error("could not find your key in this object."); // 找不到对应键。抛出异常。
}

template<typename KeyType, typename DataType>
RedBlackTree<KeyType, DataType>& RedBlackTree<KeyType, DataType>::setData(
	const KeyType& key, 
	const DataType& data
)
{
	Node* currentNode = root;
	Node* currentFather = nullptr;

	while (currentNode != nullptr) {
		if (key == currentNode->key) { // 找到对应键。
			currentNode->data = data;
			return *this; // 更新完成。结束。
		}
		else {
			currentFather = currentNode;
			currentNode = (key < currentNode->key ? currentNode->leftChild : currentNode->rightChild);
		}
	}

	// 如果是更新现有键值，则在上面的循环里会触发 return.
	// 下面部分负责创建新的节点，完成新数据的存储。

	/*
		此时，currentNode 指向 nullptr, currentFather 指向最后遍历到的节点，也可能是 nullptr.
		插入时，新节点设为红色，根据键值插入到最后一个节点的左或右。
	*/

	// 创建新节点。
	currentNode = new Node;
	currentNode->father = currentFather;
	currentNode->leftChild = nullptr;
	currentNode->rightChild = nullptr;
	currentNode->key = key;
	currentNode->data = data;

	// 如果树是空的，插入节点设为根即可。
	if (currentFather == nullptr) {
		currentNode->color = NodeColor::BLACK;
		this->root = currentNode;
		return *this;
	}

	// 下面处理树是非空时的情况。
	// 先将节点设为红色。
	currentNode->color = NodeColor::RED;
	// 将新节点绑定到父节点。
	if (key < currentFather->key) {
		currentFather->leftChild = currentNode;
	}
	else {
		currentFather->rightChild = currentNode;
	}

	// 对可能出现的“连续红色节点”问题进行修复。
	this->fixContinuousRedNodeProblem(currentNode);
	return *this;
}

template<typename KeyType, typename DataType>
RedBlackTree<KeyType, DataType>& RedBlackTree<KeyType, DataType>::removeKey(
	const KeyType& key
)
{
	Node* currentNode = root;

	while (currentNode != nullptr) {
		if (key == currentNode->key) {
			break; // 键匹配，查询成功。
		}
		else {
			currentNode = (key < currentNode->key ? currentNode->leftChild : currentNode->rightChild);
		}
	}

	if (currentNode == nullptr) {
		return *this; // 找不到对应键。
	}

	// 否则，使用替代法，锁定替代的节点。
	// 只要有至少一个孩子，就要继续寻找替代节点。
	while (currentNode->leftChild != nullptr || currentNode->rightChild != nullptr) {
		if (currentNode->rightChild != nullptr) {
			// 当前节点有右孩子。
			// 使用后继节点代替原删除节点。
			// 注意：后继节点的左孩子一定是 nullptr.
			Node* replacementNode = currentNode->rightChild;
			while (replacementNode->leftChild != nullptr) {
				replacementNode = replacementNode->leftChild;
			}

			struct {
				Node* leftChild;
				Node* rightChild;
				NodeColor color;
				Node* father;
			} currentNodeInfo = {
					currentNode->leftChild, currentNode->rightChild,
					currentNode->color, currentNode->father
			}, replacementNodeInfo = {
					replacementNode->leftChild, replacementNode->rightChild,
					replacementNode->color, replacementNode->father
			};

			// 交换颜色。
			currentNode->color = replacementNodeInfo.color;
			replacementNode->color = currentNodeInfo.color;

			// 编辑父节点。
			if (currentNodeInfo.father != nullptr) {
				if (currentNodeInfo.father->leftChild == currentNode) {
					currentNodeInfo.father->leftChild = replacementNode;
				}
				else {
					currentNodeInfo.father->rightChild = replacementNode;
				}
			}
			else {
				this->root = replacementNode;
			}

			// 编辑 currentNode 左孩子的信息。
			if (currentNodeInfo.leftChild != nullptr) {
				currentNodeInfo.leftChild->father = replacementNode;
			}

			// 编辑 replacementNode 右孩子的信息（如果有）。
			if (replacementNodeInfo.rightChild != nullptr) {
				replacementNodeInfo.rightChild->father = currentNode;
			}
			
			currentNode->leftChild = nullptr; // 替换节点的左孩子一定是 nullptr.
			currentNode->rightChild = replacementNodeInfo.rightChild;
			if (replacementNodeInfo.father == currentNode) {
				currentNode->father = replacementNode;
			}
			else {
				currentNode->father = replacementNodeInfo.father;
			}

			replacementNode->leftChild = currentNodeInfo.leftChild;
			replacementNode->father = currentNodeInfo.father;
			if (currentNodeInfo.rightChild == replacementNode) {
				replacementNode->rightChild = currentNode;
			}
			else {
				replacementNode->rightChild = currentNodeInfo.rightChild;
			}

			if (currentNodeInfo.rightChild != replacementNode) {
				currentNodeInfo.rightChild->father = replacementNode;
				replacementNodeInfo.father->leftChild = currentNode;
			}
		} // if (当前节点有右孩子)
		else { // 当前节点有左孩子，但没有右孩子。
			// 使用前驱节点代替原删除节点。
			// 注意：前驱节点的右孩子一定是 nullptr.
			Node* replacementNode = currentNode->leftChild;
			while (replacementNode->rightChild != nullptr) {
				replacementNode = replacementNode->rightChild;
			}

			struct {
				Node* leftChild;
				Node* rightChild;
				NodeColor color;
				Node* father;
			} currentNodeInfo = {
					currentNode->leftChild, currentNode->rightChild,
					currentNode->color, currentNode->father
			}, replacementNodeInfo = {
					replacementNode->leftChild, replacementNode->rightChild,
					replacementNode->color, replacementNode->father
			};

			// 交换颜色。
			currentNode->color = replacementNodeInfo.color;
			replacementNode->color = currentNodeInfo.color;

			// 编辑父节点。
			if (currentNodeInfo.father != nullptr) {
				if (currentNodeInfo.father->leftChild == currentNode) {
					currentNodeInfo.father->leftChild = replacementNode;
				}
				else {
					currentNodeInfo.father->rightChild = replacementNode;
				}
			}
			else {
				this->root = replacementNode;
			}

			// 编辑 currentNode 右孩子的信息。
			if (currentNodeInfo.rightChild != nullptr) {
				currentNodeInfo.rightChild->father = replacementNode;
			}

			// 编辑 replacementNode 左孩子的信息（如果有）。
			if (replacementNodeInfo.leftChild != nullptr) {
				replacementNodeInfo.leftChild->father = currentNode;
			}

			currentNode->rightChild = nullptr; // 替换节点的右孩子一定是 nullptr.
			currentNode->leftChild = replacementNodeInfo.leftChild;
			if (replacementNodeInfo.father == currentNode) {
				currentNode->father = replacementNode;
			}
			else {
				currentNode->father = replacementNodeInfo.father;
			}

			replacementNode->rightChild = currentNodeInfo.rightChild;
			replacementNode->father = currentNodeInfo.father;
			if (currentNodeInfo.leftChild == replacementNode) {
				replacementNode->leftChild = currentNode;
			}
			else {
				replacementNode->leftChild = currentNodeInfo.leftChild;
			}

			if (currentNodeInfo.leftChild != replacementNode) {
				currentNodeInfo.leftChild->father = replacementNode;
				replacementNodeInfo.father->rightChild = currentNode;
			}
		} // 当前节点有左孩子，但没有右孩子。
	} // while (currentNode->leftChild != nullptr || currentNode->rightChild != nullptr)

	// 至此，删除目标没有子树。

	if (currentNode == this->root) { // 删除的是根。
		this->root = nullptr;
		delete currentNode;
	} // 删除的是根。
	else if (currentNode->color == NodeColor::RED) {
		if (currentNode == currentNode->father->leftChild) {
			currentNode->father->leftChild = nullptr;
		}
		else {
			currentNode->father->rightChild = nullptr;
		}
		delete currentNode;
	} // 要删除的是红色的叶节点。
	else { // 要删除的是黑色的叶节点。
		// 目标节点的父节点一定存在。因为目标节点是黑色的，所以兄弟一定存在。
		Node* sibling = (currentNode->father->leftChild != currentNode ?
			currentNode->father->leftChild : currentNode->father->rightChild);

		Node* currentFather = currentNode->father;

		ChildSide siblingSideToFather = 
			(sibling == currentFather->leftChild ? ChildSide::LEFT : ChildSide::RIGHT);

		// 前面已经找完了与删除目标相关的节点。现在可以删掉目标节点了。
		// 取消父节点对它的绑定。
		if (currentFather->leftChild == currentNode) {
			currentFather->leftChild = nullptr;
		}
		else {
			currentFather->rightChild = nullptr;
		}
		// 释放节点。
		delete currentNode;

		// 接下来开始分情况讨论。

		if (currentFather->color == NodeColor::RED) {
			// 父节点是红色时，兄弟节点一定是黑色的。

			if (sibling->leftChild != nullptr && sibling->rightChild != nullptr)
			{
				/*
					X: 被删除; R: Red; B: Black
					  R
					 / \
					X   B
					   / \
					  R   R   或其对称形态。
				*/
				// 兄弟的左右孩子都是红色。
				// 注意：如果这个黑色的叔叔有孩子，那么孩子一定是红色的。
				// 操作：对兄弟做旋转，再对父节点做旋转。父节点设为黑色，兄弟设为红色。
				sibling->color = NodeColor::RED;
				currentFather->color = NodeColor::BLACK;

				if (siblingSideToFather == ChildSide::RIGHT) {
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(currentFather);
				}
				else {
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(currentFather);
				}
			}
			else if (siblingSideToFather == ChildSide::RIGHT && sibling->leftChild != nullptr)
			{
				/*
					X: 被删除; R: Red; B: Black
					  R
					 / \
					X   B
					   /
					  R
				*/

				currentFather->color = NodeColor::BLACK;
				this->rotateRight(sibling);
				this->rotateLeft(currentFather);
			}
			else if (siblingSideToFather == ChildSide::LEFT && sibling->rightChild != nullptr)
			{
				/*
					X: 被删除; R: Red; B: Black
					  R
					 / \
					B   X
					 \
					  R
				*/

				currentFather->color = NodeColor::BLACK;
				this->rotateLeft(sibling);
				this->rotateRight(currentFather);
			}
			else if (siblingSideToFather == ChildSide::RIGHT && sibling->rightChild != nullptr) {
				/*
					X: 被删除; R: Red; B: Black
					  R
					 / \
					X   B
					     \
					      R
				*/
				currentFather->color = NodeColor::BLACK;
				sibling->color = NodeColor::RED;
				sibling->rightChild->color = NodeColor::BLACK;
				this->rotateLeft(currentFather);
			}
			else if (siblingSideToFather == ChildSide::LEFT && sibling->leftChild != nullptr)
			{
				/*
					X: 被删除; R: Red; B: Black
					    R
					   / \
					  B   X
					 /
					R
				*/
				currentFather->color = NodeColor::BLACK;
				sibling->color = NodeColor::RED;
				sibling->leftChild->color = NodeColor::BLACK;
				this->rotateRight(currentFather);
			}
			else { // sibling 没有孩子。
				sibling->color = NodeColor::RED;
				currentFather->color = NodeColor::BLACK;
			}

		} // currentFather->color == NodeColor::RED
		else { // currentFather->color == NodeColor::BLACK
			if (sibling->color == NodeColor::BLACK
				&& sibling->leftChild != nullptr && sibling->rightChild != nullptr)
			{
				// 兄弟是黑色的，且兄弟有两个孩子。那么这两个孩子一定是红色的。
				if (siblingSideToFather == ChildSide::RIGHT) {
					/*
						X: 被删除; R: Red; B: Black
							B
						   / \
						  X   B
						     / \
							R   R
					*/
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(sibling);
					this->rotateLeft(currentFather);
				}
				else {
					/*
						X: 被删除; R: Red; B: Black
							B
						   / \
						  B   X
						 / \
						R   R
					*/
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(sibling);
					this->rotateRight(currentFather);
				}
			} // 兄弟是黑色，且有两个孩子。
			else if (sibling->color == NodeColor::BLACK &&
				(sibling->leftChild != nullptr || sibling->rightChild != nullptr))
			{
				// 兄弟是黑色，且有一个孩子（一定是红色）。
				if (siblingSideToFather == ChildSide::RIGHT && sibling->rightChild != nullptr) {
					/*
						X: 被删除; R: Red; B: Black
							B
						   / \
						  X   B
						       \
						        R
					*/
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(currentFather);
				}
				else if (siblingSideToFather == ChildSide::RIGHT && sibling->leftChild != nullptr)
				{
					/*
						X: 被删除; R: Red; B: Black
							B
						   / \
						  X   B
							 /
							R
					*/
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(sibling);
					this->rotateLeft(currentFather);
				}
				else if (siblingSideToFather == ChildSide::LEFT && sibling->leftChild != nullptr)
				{
					/*
						X: 被删除; R: Red; B: Black
							B
						   / \
						  B   X
						 /
						R
					*/
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(currentFather);
				}
				else {
					/*
						X: 被删除; R: Red; B: Black
							B
						   / \
						  B   X
						   \
							R
					*/
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(sibling);
					this->rotateRight(currentFather);
				}
			} // 兄弟是黑色，且有一个孩子。
			else if (sibling->color == NodeColor::BLACK) {
				// 兄弟是黑色，但是没有孩子。
				/*
					   B          B
					  / \   ->   / \
					 X   B      X   R
				*/
				if (currentFather->leftChild == nullptr) {
					currentFather->rightChild->color = NodeColor::RED;
					this->fixUnbalancedChildrenProblem(currentFather);
				}
				else {
					currentFather->leftChild->color = NodeColor::RED;
					this->fixUnbalancedChildrenProblem(currentFather);
				}
			} // 兄弟是黑色的，且没有孩子。
			else {
				// 兄弟是红色的。那么兄弟一定有两个黑色的孩子。
				sibling->color = NodeColor::BLACK;
				currentFather->color = NodeColor::RED;
				if (siblingSideToFather == ChildSide::RIGHT) {
					/*
						   B
						  / \
						 X   R
						    / \
						   B   B
					*/
					this->rotateLeft(currentFather);
					this->rotateLeft(currentFather);
					if (currentFather->rightChild != nullptr) {
						this->fixContinuousRedNodeProblem(currentFather->rightChild);
					}
				}
				else {
					/*
						   B
						  / \
						 R   X
						/ \
					   B   B
					*/
					this->rotateRight(currentFather);
					this->rotateRight(currentFather);
					if (currentFather->leftChild != nullptr) {
						this->fixContinuousRedNodeProblem(currentFather->leftChild);
					}
				}
			}
		}
	}

	return *this; // 删除成功。
}

template<typename KeyType, typename DataType>
void RedBlackTree<KeyType, DataType>::cleanup(Node* node)
{
	if (node->leftChild != nullptr) {
		cleanup(node->leftChild);
	}
	if (node->rightChild != nullptr) {
		cleanup(node->rightChild);
	}
	delete node;
}

template<typename KeyType, typename DataType>
void RedBlackTree<KeyType, DataType>::rotateLeft(Node* node)
{
	Node* father = node->father;
	Node* targetRoot = node->rightChild;

	// 重新绑定子树的根。
	if (father == nullptr) {
		this->root = targetRoot;
	}
	else {
		if (node == father->leftChild) {
			father->leftChild = targetRoot;
		}
		else {
			father->rightChild = targetRoot;
		}
	}
	targetRoot->father = father;

	node->rightChild = targetRoot->leftChild; // 孩子有可能是 nullptr
	if (node->rightChild != nullptr) { // 只有当孩子不是空的时候，才可尝试重新绑定父节点。
		node->rightChild->father = node;
	}
	targetRoot->leftChild = node;
	node->father = targetRoot;
}

template<typename KeyType, typename DataType>
void RedBlackTree<KeyType, DataType>::rotateRight(Node* node)
{
	Node* father = node->father;
	Node* targetRoot = node->leftChild;

	// 重新绑定子树的根。
	if (father == nullptr) {
		this->root = targetRoot;
	}
	else {
		if (node == father->leftChild) {
			father->leftChild = targetRoot;
		}
		else {
			father->rightChild = targetRoot;
		}
	}
	targetRoot->father = father;

	node->leftChild = targetRoot->rightChild; // 孩子有可能是 nullptr
	if (node->leftChild != nullptr) { // 只有当孩子不是空的时候，才可尝试重新绑定父节点。
		node->leftChild->father = node;
	}
	targetRoot->rightChild = node;
	node->father = targetRoot;
}

template<typename KeyType, typename DataType>
void RedBlackTree<KeyType, DataType>::fixContinuousRedNodeProblem(Node* node)
{
	Node* currentNode = node;
	
	// 只有当当前节点是红色时，才可能在父节点为红色时违背红黑树规则，于是要进行修复操作。
	while (currentNode->color == NodeColor::RED) {
		Node* currentFather = currentNode->father;
		if (currentFather == nullptr) {
			// currentNode 是根节点。
			currentNode->color = NodeColor::BLACK;
			break;
		}
		else if (currentFather->color == NodeColor::BLACK) {
			// 父节点是黑色，不存在“连续红色节点”问题。
			break;
		}

		// 如果执行到了这里，说明父节点和目标节点都是红色的。

		// 如果父节点是红色的，那么它（父节点）一定不是根，所以祖父存在。
		Node* currentGrandpa = currentFather->father;

		// 寻找叔节点（可能为空）。
		Node* uncle =
			(currentGrandpa->leftChild != currentFather ?
				currentGrandpa->leftChild : currentGrandpa->rightChild);

		if (uncle != nullptr && uncle->color == NodeColor::RED) {
			// 叔叔存在并且是红色，进行反色操作。

			uncle->color = NodeColor::BLACK;
			currentFather->color = NodeColor::BLACK;
			currentGrandpa->color = NodeColor::RED;

			// 将当前节点设为爷爷节点。
			currentNode = currentGrandpa;
		}
		else {
			// 叔叔为黑色或不存在，进行旋转操作。
			// 旋转的每一种情况都可以保证树满足红黑树的要求。所以，进入本分支后将跳出修复函数。

			// 如果父节点是祖父的左孩子...
			if (currentFather == currentGrandpa->leftChild) {
				if (currentNode == currentFather->leftChild) {
					this->rotateRight(currentGrandpa);
					// 重新着色。
					currentFather->color = NodeColor::BLACK;
					currentGrandpa->color = NodeColor::RED;
				}
				else {
					this->rotateLeft(currentFather);
					this->rotateRight(currentGrandpa);
					// 重新着色。
					currentGrandpa->color = NodeColor::RED;
					currentNode->color = NodeColor::BLACK;
				}
			} // if (currentFather == currentGrandpa->leftChild)
			else {
				// 否则，则父节点是祖父的右孩子...

				if (currentNode == currentFather->rightChild) {
					this->rotateLeft(currentGrandpa);
					// 重新着色。
					currentFather->color = NodeColor::BLACK;
					currentGrandpa->color = NodeColor::RED;
				}
				else {
					this->rotateRight(currentFather);
					this->rotateLeft(currentGrandpa);
					// 重新着色。
					currentGrandpa->color = NodeColor::RED;
					currentNode->color = NodeColor::BLACK;
				}
			} // if (currentFather != currentGrandpa->leftChild)

			return;
		}
	}
}

template<typename KeyType, typename DataType>
void RedBlackTree<KeyType, DataType>::fixUnbalancedChildrenProblem(Node* node)
{
	Node* currentNode = node;

	while (currentNode != this->root) {
		Node* currentFather = currentNode->father;
		Node* sibling = (currentFather->leftChild != currentNode ?
			currentFather->leftChild : currentFather->rightChild);
		ChildSide siblingSideToFather = 
			(sibling == currentFather->leftChild ? ChildSide::LEFT : ChildSide::RIGHT);

		if (currentFather->color == NodeColor::RED) {
			// 父节点为红色。
			// 那么，兄弟一定是黑色的。
			// 此时，兄弟节点的左右孩子一定都存在。
			if (siblingSideToFather == ChildSide::RIGHT
				&& sibling->leftChild->color == NodeColor::BLACK)
			{
				/*
					   R
					  / \
					 X   B
					    / \
					   B   ?
				*/
				this->rotateLeft(currentFather);
				break; // 修复完成。
			}
			else if (siblingSideToFather == ChildSide::LEFT
				&& sibling->rightChild->color == NodeColor::BLACK)
			{
				/*
					    R
					   / \
					  B   X
					 / \
					?   B
				*/
				this->rotateRight(currentFather);
				break;
			}
			else if (siblingSideToFather == ChildSide::RIGHT
				&& sibling->rightChild->color == NodeColor::BLACK)
			{
				/*
					   R
					  / \
					 X   B
						/ \
					   R   B
				*/
				currentFather->color = NodeColor::BLACK;
				sibling->color = NodeColor::RED;
				this->fixContinuousRedNodeProblem(sibling->leftChild);
				break;
			}
			else if (siblingSideToFather == ChildSide::LEFT
				&& sibling->leftChild->color == NodeColor::BLACK)
			{
				/*
					    R
					   / \
					  B   X
					 / \
					B   R
				*/
				currentFather->color = NodeColor::BLACK;
				sibling->color = NodeColor::RED;
				this->fixContinuousRedNodeProblem(sibling->rightChild);
				break;
			}
			else { // 兄弟的左右孩子都是红色的。
				currentFather->color = NodeColor::BLACK;
				sibling->color = NodeColor::RED;
				if (siblingSideToFather == ChildSide::RIGHT) {
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(currentFather);
				}
				else {
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(currentFather);
				}
				break;
			}
		} // 父节点为红色。
		else { // 父节点为黑色。
			if (sibling->color == NodeColor::RED) { // 兄弟是红色的。
				/*
						B
					   / \
					  X   R
						 / \
						B   B

					及其对称形态。
				*/
				currentFather->color = NodeColor::RED;
				sibling->color = NodeColor::BLACK;
				if (siblingSideToFather == ChildSide::RIGHT) {
					
					this->rotateLeft(currentFather);
				}
				else {
					this->rotateRight(currentFather);
				}
			} // 兄弟是红色的。
			else { // 兄弟是黑色的。
				if (sibling->leftChild->color == NodeColor::BLACK 
					&& sibling->rightChild->color == NodeColor::BLACK)
				{
					/*
						  B
						 / \
						X   B
						   / \
						  B   B

						及其对称形态。
					*/
					sibling->color = NodeColor::RED;
					currentNode = currentFather;
				}
				else if (siblingSideToFather == ChildSide::RIGHT 
					&& sibling->rightChild->color == NodeColor::RED) 
				{
					/*
						  B
						 / \
						X   B
						   / \
						  ?   R

					*/
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(currentFather);
					break;
				}
				else if (siblingSideToFather == ChildSide::LEFT
					&& sibling->leftChild->color == NodeColor::RED)
				{
					/*
						    B
						   / \
						  B   X
						 / \
						R   ?

					*/
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(currentFather);
					break;
				}
				else if (siblingSideToFather == ChildSide::RIGHT) 
				{
					/*
						  B
						 / \
						X   B
						   / \
						  R   B

					*/
					sibling->leftChild->color = NodeColor::BLACK;
					this->rotateRight(sibling);
					this->rotateLeft(currentFather);
					break;
				}
				else 
				{
					/*
						    B
						   / \
						  B   X
						 / \
						B   R

					*/
					sibling->rightChild->color = NodeColor::BLACK;
					this->rotateLeft(sibling);
					this->rotateRight(currentFather);
					break;
				}
			} // 兄弟是黑色的。
		} // 父节点为黑色。
	}
}
