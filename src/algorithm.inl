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
	depth = data.size();
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
	KDNode2D<T>** nodes = new KDNode2D<T>* [depth];
	// The axis to evaluate on, set to true for x or false for y
	bool* axes = new bool[depth];
	// The current node being evaluated
	int current = 0;

	nodes[0] = root;
	axes[0] = true;

	// The smallest distance found so far
	float nearest_distance = distance2D(*root->point, location);
	// The nearest node found so far
	KDNode2D<T>* nearest_node = root;

	// Get the nearest node
	while (current >= 0)
	{
		KDNode2D<T>* node = nodes[current];
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
			bool axis = !axes[current];
			if (axes[current])
			{
				if (location.x < node->point->x)
				{
					// Evaluate the left branch first
					nodes[current] = node->r;
					nodes[current + 1] = node->l;

					axes[current] = axis;
					axes[current + 1] = axis;
				}
				else
				{
					// Evaluate the right branch first
					nodes[current] = node->l;
					nodes[current + 1] = node->r;

					axes[current] = axis;
					axes[current + 1] = axis;
				}
			}
			else
			{
				if (location.y < node->point->y)
				{
					// Evaluate the left branch first
					nodes[current] = node->r;
					nodes[current + 1] = node->l;

					axes[current] = axis;
					axes[current + 1] = axis;
				}
				else
				{
					// Evaluate the right branch first
					nodes[current] = node->l;
					nodes[current + 1] = node->r;

					axes[current] = axis;
					axes[current + 1] = axis;
				}
			}
			++current;
		}
		else
		{
			--current;
		}
	}

	delete[] nodes;
	delete[] axes;

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