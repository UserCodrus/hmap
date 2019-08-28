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
const T* KDTree2D<T>::getNearest(const T& location) const
{
	// The current node we are comparing
	KDNode2D<T>* node = root;
	// The current axis used for comparisons, true = x, false = y
	bool axis = true;

	// The smallest distance found so far
	float nearest_distance = distance2D(*root->point, location);
	// The nearest node found so far
	KDNode2D<T>* nearest_node = root;

	// Get the nearest node
	while (node != nullptr)
	{
		// Determine if the current node is closer than the previous closest node
		float distance = distance2D(*node->point, location);
		if (distance < nearest_distance)
		{
			nearest_node = node;
			nearest_distance = distance;
		}

		if (axis)
		{
			// Compare the x coordinates of the node to the point
			if (location.x < node->point->x)
			{
				// Select the left node
				if (node->l != nullptr)
				{
					node = node->l;
				}
				else
				{
					break;
				}
			}
			else
			{
				// Select the right node
				if (node->r != nullptr)
				{
					node = node->r;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			// Compare the x coordinates of the node to the point
			if (location.y < node->point->y)
			{
				// Select the left node
				if (node->l != nullptr)
				{
					node = node->l;
				}
				else
				{
					break;
				}
			}
			else
			{
				// Select the right node
				if (node->r != nullptr)
				{
					node = node->r;
				}
				else
				{
					break;
				}
			}
		}

		// Switch axis
		axis = !axis;
	}

	return nearest_node->point;
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