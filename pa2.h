/*Maisha Nabila
COP3503 Section 19F2*/
#ifndef pa2H
#define pa2H
#include <string>
#include <map>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>

template <class m>
class Node
{
private:
	m value;
	Node<m> *next;
public:
	Node(m value);
	m setValue(m value);
	Node<m> *setNext(Node<m> *node);
	m getValue();
	Node<m> *getNext();
};
template <class m>
class linkL
{
private:
	Node<m> *start;
public:
	linkL();
	Node<m> *setStart(Node<m> *start);
	Node<m> *getStart();
	Node<m> *appendStart(m value);
};


struct Group
{
	int pageS, pageE;
	Group(int start, int end)
	{
		pageS = start;
		pageE = end;
	}
};
struct info {
	std::string name;
	int size;

	info(std::string name, int size) {
		this->name = name;
		this->size = size;
	}
};
struct usedMem : Group {
	std::string program_name;

	usedMem(int start, int end, std::string name) :
		Group(pageS, pageE) {
		pageS = start;
		pageE = end;
		program_name = name;
	}
};


class memoryPlace
{
private:
	std::string algorithm;
	linkL<Group> free;
	linkL<usedMem> used;
public:
	memoryPlace(std::string algorithm, int numP);
	void addPro(info prog_info);
	void killPro(std::string program_name);
	void defragment();
	void printFragment();
	void printMem();
};

#endif 