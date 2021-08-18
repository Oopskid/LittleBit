#pragma once

#include <vector>
#include <map>
#include <forward_list>

namespace LilBit
{
	//Graph structure to represent reliance
	template<typename Key, typename Data> class Dependency
	{
		public:

		class Dependant
		{
			public:

			bool add(Dependant* other) 
			{
				if (this == other) { return true; }
				
				connections.push_front(other);
				return false;
			}

			private:
			std::forward_list<Dependant*> connections;
			Data data;
			bool flagged;
		};

		private:
		std::map<Key, Dependant*> vertices;
	};
}

