#ifndef GRAPH_H
#define GRAPH_H
#include "vector.h"
#include <vector>
#include <stdexcept>
#include <limits> 
template <typename V>
struct Vertex {
	Vertex() {}
	Vertex(size_t number, V mark) : number(number), mark(mark) {}

	size_t number = std::numeric_limits<size_t>::max();
	V mark = V();
};

template <typename E>
struct Edge {
	Edge() {}
	Edge(E mark) : mark(mark) {}

	E mark = E();
};


template<typename V, typename E>
class Graph {
public:
	enum class GraphType {
		Directed,
		Undirected
	};

	Graph(GraphType type) : type(type) {}
	~Graph() {
		clearEdges();
	}

	
	Graph& operator=(const Graph& other) {
		if (this == &other) {
			return *this;
		}
		clearEdges();

		vertices = other.vertices;
		edgeMatrix.resize(other.edgeMatrix.size());
		for (size_t i = 0; i < other.edgeMatrix.size(); ++i) {
			if (other.edgeMatrix[i] != nullptr) {
				edgeMatrix[i] = new Edge(*other.edgeMatrix[i]);
			}
			else {
				edgeMatrix[i] = nullptr;
			}
		}

		type = other.type;
		return *this;
	}

	void add_Edge(size_t startIv, size_t endIv, E edge_mark) {
		if (startIv >= get_VertexAmount() || endIv >= get_VertexAmount()) {
			throw std::out_of_range("The index is out of range!");
		}

		Edge<E>*& exists_edge = edgeMatrix[startIv * get_VertexAmount() + endIv];

		if (exists_edge != nullptr) {
			delete exists_edge;
		}

		exists_edge = new Edge<E>(edge_mark);

		if (type == GraphType::Undirected) {
			Edge<E>*& exists_ReverseEdge = edgeMatrix[endIv * get_VertexAmount() + startIv];

			if (exists_ReverseEdge != nullptr) {
				delete exists_ReverseEdge;
			}
			exists_ReverseEdge = new Edge<E>(edge_mark);
		}
	}




	void remove_Edge(size_t startIv, size_t endIv) {
		if (startIv >= get_VertexAmount() || endIv >= get_VertexAmount()) {
			throw std::out_of_range("Vertex index out of range.");
		}

		size_t index = startIv * get_VertexAmount() + endIv;
		delete edgeMatrix[index];
		edgeMatrix[index] = nullptr;

		if (type == GraphType::Undirected) {
			size_t reverse_Index = endIv * get_VertexAmount() + startIv;
			delete edgeMatrix[reverse_Index];
			edgeMatrix[reverse_Index] = nullptr;
		}
	}
	Edge<E>* get_Edge(size_t startIv, size_t endIv) const {
		size_t qty_vertex = get_VertexAmount();
		return edgeMatrix[startIv * qty_vertex + endIv];
	}

	bool isEdgeExist(size_t startIv, size_t endIv) const {
		return get_Edge(startIv, endIv) != nullptr;
	}

	Vertex<V>& get_Vertex(size_t index) {
		if (index >= vertices.size()) {
			throw std::out_of_range("The index is out of range!");
		}
		return vertices[index]; 
	}

	size_t get_VertexAmount() const {
		return vertices.size();
	}


	size_t add_Vertex(V vertex_mark) {
		size_t index = vertices.size();
		vertices.push(Vertex<V>(index, vertex_mark));

		size_t qty_vertex = get_VertexAmount();
		Vector<Edge<E>*> newEdgeMatrix;

		for (size_t i = 0; i < qty_vertex; ++i) {
			for (size_t j = 0; j < qty_vertex; ++j) {
				newEdgeMatrix.push(nullptr); 
			}
		}

		edgeMatrix.swap_data(newEdgeMatrix);

		return index;
	}
	void remove_Vertex(size_t index) {
		size_t  qty_vertex = get_VertexAmount();
		if (index >= qty_vertex) {
			return;
		}

		for (size_t i = index; i < qty_vertex - 1; i++) {
			vertices.set(i, vertices.get(i + 1));
		}
		vertices.resize(qty_vertex - 1);

		for (size_t i = 0; i < qty_vertex; i++) {
			Edge<E>* edge = edgeMatrix.get(index * qty_vertex + i);
			if (edge) {
				delete edge;
			}
			edge = edgeMatrix.get(i * qty_vertex + index);
			if (edge) {
				delete edge;
			}
		}

		Vector<Edge<E>*> TimeEdgeMatrix;
		size_t  qty_vertex1 = get_VertexAmount();

		TimeEdgeMatrix.resize(qty_vertex1 * qty_vertex1);
		for (size_t i = 0; i < qty_vertex1; i++) {
			for (size_t j = 0; j < qty_vertex1; j++) {
				Edge<E>* edge = edgeMatrix.get(((i + (i >= index)) * qty_vertex) + (j + (j >= index)));
				TimeEdgeMatrix.set((i * qty_vertex1) + j, edge);
			}
		}

		edgeMatrix.swap_data(TimeEdgeMatrix);

	}

	std::vector<V> get_AllVertexData() const {
		std::vector<V> allData;
		for (size_t i = 0;i < vertices.size();i++) {

			allData.push_back(vertices[i].mark); // ��������� ����� �������
		}
		return allData;
	}

	struct Iterator {
	private:
		const Graph* graph; 
		size_t start;
		size_t end;

		size_t get_index_Vertex_near() {
			for (size_t i = end + 1; i < graph->get_VertexAmount(); i++) {
				if (graph->isEdgeExist(start, i)) {
					return i;
				}
			}
			return static_cast<size_t>(-1);
		}

	public:
		Iterator(const Graph* graph, size_t start) : graph(graph), start(start), end(static_cast<size_t>(-1)) {
			end = get_index_Vertex_near();
		}

		bool hasNext() const {
			return end != static_cast<size_t>(-1);
		}

		size_t next() {
			if (!hasNext()) {
				throw std::out_of_range("No more adjacent vertices");
			}
			size_t current = end;
			end = get_index_Vertex_near();
			return current;
		}

		size_t get_Index() const {
			return end;
		}

		int get_Start() const {
			return start;
		}
	};



	Iterator get_Iterator(size_t start)const {
		return Iterator(this, start);
	}



private:
	void clearEdges() {
		for (size_t i = 0; i < edgeMatrix.size(); i++) {
			delete edgeMatrix.get(i);
			edgeMatrix.set(i, nullptr);
		}
	}

	Vector<Vertex<V>> vertices;
	Vector<Edge<E>*> edgeMatrix;
	GraphType type;
};
#endif