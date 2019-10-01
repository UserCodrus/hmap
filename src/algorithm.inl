template <class T>
KDNode2D<T>::KDNode2D(T* data)
{
	point = data;
}

template <class T>
KDNode2D<T>::~KDNode2D()
{
	// Delete children when the parent is deleted
	if (l != nullptr)
	{
		delete l;
	}
	if (r != nullptr)
	{
		delete r;
	}
}

template <class T>
void KDTree2D<T>::build(std::vector<T>& data)
{
	// Delete the existing tree
	if (root != nullptr)
	{
		delete root;
	}

	// Build a new tree
	root = addNode(data, data.begin(), data.end());
}

template <class T>
KDNode2D<T>* KDTree2D<T>::addNode(std::vector<T>& data, typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end, bool xaxis)
{
	// Check for invalid ranges
	if (begin > end)
	{
		return nullptr;
	}
	else if (begin == end)
	{
		if (begin == data.end())
		{
			return nullptr;
		}
		return new KDNode2D<T>(&(*begin));
	}

	// Get the midpoint between the iterators
	typename std::vector<T>::iterator median = begin;
	std::advance(median, std::distance(begin, end) / 2);

	// Sort the median element into place based on the current dimension of this branch of the kd tree
	if (xaxis)
	{
		std::nth_element(begin, median, end,
			[](const T& a, const T& b) {return a.x < b.x; });
	}
	else
	{
		std::nth_element(begin, median, end,
			[](const T& a, const T& b) {return a.y < b.y; });
	}

	// Create the node and its branches
	KDNode2D<T>* node = new KDNode2D<T>(&(*median));
	node->l = addNode(data, begin, median, !xaxis);
	node->r = addNode(data, ++median, end, !xaxis);

	return node;
}

template <class T>
const T* KDTree2D<T>::nearest(const T& location) const
{
	// The list of nodes to evaluate
	std::stack<KDNode2D<T>*> nodes;
	// The axis to evaluate each node on, set to true for x axis or false for y axis
	std::stack<bool> axes;

	nodes.push(root);
	axes.push(true);

	// The smallest distance found so far
	float nearest_distance = distance2D(*root->point, location);
	// The nearest node found so far
	KDNode2D<T>* nearest_node = root;

	// Get the nearest node
	while (nodes.size() > 0)
	{
		KDNode2D<T>* node = nodes.top();

		if (node != nullptr)
		{
			// Determine if the current node is closer than the closest node found so far
			float distance = distance2D(*node->point, location);
			if (distance < nearest_distance)
			{
				nearest_node = node;
				nearest_distance = distance;
			}

			// Compare the point to the current node to determine the order to evaluate branches
			float location_value, node_value;
			bool axis = !axes.top();
			if (axes.top())
			{
				location_value = location.x;
				node_value = node->point->x;
			}
			else
			{
				location_value = location.y;
				node_value = node->point->y;
			}

			nodes.pop();
			axes.pop();
			if (location_value < node_value)
			{
				// Evaluate the left branch first
				nodes.push(node->r);
				axes.push(axis);
				nodes.push(node->l);
				axes.push(axis);
			}
			else
			{
				// Evaluate right branch first
				nodes.push(node->l);
				axes.push(axis);
				nodes.push(node->r);
				axes.push(axis);
			}
		}
		else
		{
			// Ignore null nodes
			nodes.pop();
			axes.pop();
		}
	}

	return nearest_node->point;
}

template <class T>
KDNode2D<T>* KDTree2D<T>::nearestNode(const T& location, KDNode2D<T>* root, float& nearest_distance, bool xaxis)
{

}

template <class T>
KDTree2D<T>::~KDTree2D()
{
	if (root != nullptr)
	{
		delete root;
	}
}

template <class T>
float distance2D(const T& a, const T& b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	return dx * dx + dy * dy;
}