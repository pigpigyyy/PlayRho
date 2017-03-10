/*
* Original work Copyright (c) 2009 Erin Catto http://www.box2d.org
* Modified work Copyright (c) 2017 Louis Langholtz https://github.com/louis-langholtz/Box2D
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_DYNAMIC_TREE_H
#define B2_DYNAMIC_TREE_H

#include <Box2D/Collision/AABB.hpp>
#include <Box2D/Collision/RayCastInput.hpp>

#include <functional>

namespace box2d {

/// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt.
///
/// @detail A dynamic tree arranges data in a binary tree to accelerate
/// queries such as volume queries and ray casts. Leafs are proxies
/// with an AABB. In the tree we expand the proxy AABB by AabbMultiplier
/// so that the proxy AABB is bigger than the client object. This allows the client
/// object to move by small amounts without triggering a tree update.
///
/// Nodes are pooled and relocatable, so we use node indices rather than pointers.
///
/// @note This data structure is 24-bytes large (on at least one 64-bit platform).
///
class DynamicTree
{
public:

	using size_type = std::remove_const<decltype(MaxContacts)>::type;

	/// AABB Multiplier.
	/// @detail
	/// This is used to fatten AABBs in the dynamic tree. This is used to predict
	/// the future position based on the current displacement.
	/// This is a dimensionless multiplier.
	static constexpr auto AabbMultiplier = 2;

	/// Null node index value.
	static constexpr size_type NullNode = static_cast<size_type>(-1);

	static constexpr size_type GetDefaultInitialNodeCapacity() noexcept;

	/// Constructing the tree initializes the node pool.
	DynamicTree(const size_type nodeCapacity = GetDefaultInitialNodeCapacity());

	/// Destroys the tree, freeing the node pool.
	~DynamicTree() noexcept;

	DynamicTree(const DynamicTree& copy) = delete;
	DynamicTree& operator=(const DynamicTree&) = delete;

	/// Creates a proxy.
	/// @detail Creates a proxy for a tight fitting AABB and a userData pointer.
	/// @return ID of the created proxy.
	size_type CreateProxy(const AABB aabb, void* userData);

	/// Destroys a proxy. This asserts if the id is invalid.
	void DestroyProxy(const size_type proxyId);

	/// Move a proxy with a swepted AABB. If the proxy has moved outside of its fattened AABB,
	/// then the proxy is removed from the tree and re-inserted. Otherwise
	/// the function returns immediately.
	/// @warning Behavior is undefined if given an invalid proxy ID.
	/// @param proxyId Proxy ID. Behavior is undefined if this is not a valid ID.
	/// @param aabb Axis aligned bounding box.
	/// @param displacement Displacement. Behavior is undefined if this is an invalid value.
	/// @return true if the proxy was re-inserted.
	bool MoveProxy(const size_type proxyId, const AABB aabb, const Vec2 displacement);

	/// Gets the user data for the node identified by the given identifier.
	/// @warning Behavior is undefined if the given index is invalid.
	/// @param proxyId Identifier of node to get the user data for.
	/// @return User data for the specified node.
	void* GetUserData(const size_type proxyId) const noexcept;

	/// Gets the fat AABB for a proxy.
	/// @warning Behavior is undefined if the given proxy ID is not a valid ID.
	/// @param proxyId Proxy ID. Must be a valid ID.
	AABB GetFatAABB(const size_type proxyId) const noexcept;

	/// Query an AABB for overlapping proxies. The callback class
	/// is called for each proxy that overlaps the supplied AABB.
	void Query(std::function<bool(size_type)> callback, const AABB aabb) const;

	/// Ray-cast against the proxies in the tree. This relies on the callback
	/// to perform a exact ray-cast in the case were the proxy contains a shape.
	/// The callback also performs the any collision filtering.
	/// @note Performance is roughly k * log(n), where k is the number of collisions and n is the
	/// number of proxies in the tree.
	/// @param input the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
	/// @param callback a callback class that is called for each proxy that is hit by the ray.
	void RayCast(std::function<RealNum(const RayCastInput&, size_type)> callback,
				 const RayCastInput& input) const;

	/// Validate this tree.
	/// @detail Validates this tree. Meant for testing.
	/// @return <code>true</code> if valid, <code>false</code> otherwise.
	bool Validate() const;

	bool ValidateStructure(const size_type index) const noexcept;

	bool ValidateMetrics(size_type index) const noexcept;

	/// Gets the height of the binary tree.
	/// @return Height of the tree (as stored in the root node) or 0 if the root node is not valid.
	size_type GetHeight() const noexcept;

	/// Gets the maximum balance.
	/// @detail This gets the maximum balance of nodes in the tree.
	/// @note The balance is the difference in height of the two children of a node.
	size_type GetMaxBalance() const;

	/// Gets the ratio of the sum of the perimeters of nodes to the root perimeter.
	/// @note Zero is returned if no proxies exist at the time of the call.
	/// @return Value of zero or more.
	RealNum GetAreaRatio() const noexcept;

	/// Build an optimal tree. Very expensive. For testing.
	void RebuildBottomUp();

	/// Shifts the world origin. Useful for large worlds.
	/// The shift formula is: position -= newOrigin
	/// @param newOrigin the new origin with respect to the old origin
	void ShiftOrigin(const Vec2 newOrigin);

	/// Computes the height of the tree from a given node.
	/// @warning Behavior is undefined if the given ID does not reference a valid node.
	/// @param nodeId ID of node to compute height from.
	size_type ComputeHeight(const size_type nodeId) const noexcept;

	/// Computes the height of the tree from its root.
	/// @warning Behavior is undefined if the tree doesn't have a valid root.
	size_type ComputeHeight() const noexcept;

	/// Gets the current node capacity of this tree.
	size_type GetNodeCapacity() const noexcept;
	
	size_type GetNodeCount() const noexcept;

	/// Finds the lowest code node.
	/// @warning Behavior is undefined if the tree doesn't have a valid root.
	size_type FindLowestCostNode(const AABB leafAABB) const noexcept;

private:

	/// A node in the dynamic tree. The client does not interact with this directly.
	struct TreeNode
	{
		/// Whether or not this node is a leaf node.
		/// @note This has constant complexity.
		/// @return <code>true</code> if this is a leaf node, <code>false</code> otherwise.
		bool IsLeaf() const noexcept
		{
			return child1 == NullNode;
		}
		
		/// Enlarged AABB
		AABB aabb;
		
		void* userData;
		
		union
		{
			size_type parent;
			size_type next;
		};
		
		size_type child1; ///< Index of child 1 in DynamicTree::m_nodes or NullNode.
		size_type child2; ///< Index of child 2 in DynamicTree::m_nodes or NullNode.
		
		size_type height; ///< Height - for tree balancing. 0 if leaf node. NullNode if free node.
	};

	size_type AllocateNode();
	void FreeNode(const size_type node) noexcept;

	void InsertLeaf(const size_type node);
	void RemoveLeaf(const size_type node);

	size_type Balance(const size_type index);

	size_type m_root = NullNode; ///< Index of root element in m_nodes or NullNode.

	size_type m_nodeCount = 0;
	size_type m_nodeCapacity;

	size_type m_freeList = 0;

	/// Initialized on construction.
	TreeNode* m_nodes;
};

constexpr DynamicTree::size_type DynamicTree::GetDefaultInitialNodeCapacity() noexcept
{
	return size_type{16};
}

inline DynamicTree::size_type DynamicTree::GetNodeCapacity() const noexcept
{
	return m_nodeCapacity;
}

inline DynamicTree::size_type DynamicTree::GetNodeCount() const noexcept
{
	return m_nodeCount;
}

inline void* DynamicTree::GetUserData(const size_type proxyId) const noexcept
{
	assert(proxyId != NullNode);
	assert(proxyId < m_nodeCapacity);
	return m_nodes[proxyId].userData;
}

inline AABB DynamicTree::GetFatAABB(const size_type proxyId) const noexcept
{
	assert(proxyId != NullNode);
	assert(proxyId < m_nodeCapacity);
	return m_nodes[proxyId].aabb;
}

inline DynamicTree::size_type DynamicTree::GetHeight() const noexcept
{
	return (m_root != NullNode)? m_nodes[m_root].height: 0;
}

inline DynamicTree::size_type DynamicTree::ComputeHeight() const noexcept
{
	return ComputeHeight(m_root);
}

} /* namespace box2d */

#endif
