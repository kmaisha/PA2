/*Maisha Nabila
COP3503 Section 19F2*/

#include <iostream>
#include "pa2.h"

using namespace std;


//Displays the choices the user is able to select
void print_choices() {
	cout <<
		"1. Add program\n" <<
		"2. Kill program\n" <<
		"3. Fragmentation\n" <<
		"4. Print memory\n" <<
		"5. Exit\n" << std::endl;
}
//gets the program name
std::string get_program_name() {
	std::string name;
	cout << ("Program name - ");
	cin >> name;
	return name;
}
//Allows user input for choices and stores it
int get_choice() {
	int choice;
	cout << ("Choice - ");
	cin >> choice;
	cout << ("\n");
	return choice;
}
//Gets program's size
info get_program_info() {
	std::string name = get_program_name();
	int size;
	cout << ("Program size (KB) - ");
	cin >> size;
	return info(name, size);
}
//Calls function based of the choice user makes through a loop
int run_loop(std::string algorithm) {
	memoryPlace mem_allocator(algorithm, 32);
	print_choices();
	int action_choice = -1;
	while (action_choice != 5) {
		action_choice = get_choice();
		switch (action_choice) {
		case 1:
			mem_allocator.addPro(get_program_info());
			break;
		case 2:
			mem_allocator.killPro(get_program_name());
			break;
		case 3:
			mem_allocator.printFragment();
			break;
		case 4:
			mem_allocator.printMem();
			break;
		case 5:
			break;
		default:
			cout << ("Unknown option\n");
			return 0;
		}
	}
	return 0;
}
//Template runs differte data types via class m, avoiding the need to creation of numerous classes for same function 
template <class m>
Node<m>::Node(m inputted_value) :
	value(inputted_value),
	next(NULL) {}
// gets the value 
template <class m>
m Node<m>::getValue(){
	return value;
}
//Creates the value
template <class m>
m Node<m>::setValue(m passed){
	value = passed;
	return value;
}
//Get next
template <class m>
Node<m> *Node<m>::getNext(){
	return next;
}
//Creates next
template <class m>
Node<m> *Node<m>::setNext(Node<m> *node){
	next = node;
	return next;
}
template <class m>
linkL<m>::linkL() :
	start(NULL) {}
template <class m>
Node<m> *linkL<m>::getStart(){
	return start;
}
template <class m>
Node<m> *linkL<m>::setStart(Node<m> *start){
	this->start = start;
	return this->start;
}
template <class m>
Node<m> *linkL<m>::appendStart(m value){
	if (start == NULL){
		start = new Node<m>(value);
		return start;
	}
	Node<m> *temp = start;
	while (temp->getNext() != NULL){
		temp = temp->getNext();
	}
	temp->setNext(new Node<m>(value));
	return temp->getNext();
}
//Function for memory allocation 
memoryPlace::memoryPlace(std::string passed_alg, int num_pages){
	algorithm = passed_alg;
	cout << "Using " << algorithm.c_str() << " fit algorithm\n";
	free = linkL<Group>();
	free.appendStart(Group(0, num_pages - 1));
	used = linkL<usedMem>();

}

typedef std::pair<Node<Group>*, int> pairType;
struct CompSec{
	bool operator()(const pairType & left, const pairType & right) const{
		return left.second < right.second;
	}
};

//Adds program
void memoryPlace::addPro(info prog_info){
	if (prog_info.size <= 0){
		cout << "Not a valid size\n";
		return;
	}

	Node<usedMem> *used_current = used.getStart();
	while (used_current != NULL){
		if (used_current->getValue().program_name == prog_info.name){
			cout << prog_info.name.c_str() << "is already running\n";
			return;
		}
		else{
			used_current = used_current->getNext();
		}
	}
	int page_nums;
	std::map<Node<Group>*, int> free_slots;
	Node<Group> *current = free.getStart();
	if (prog_info.size < 4)
		page_nums = 1;

	else
		page_nums = ceil((prog_info.size / 4));



	while (current != NULL){
		int free_size = current->getValue().pageE -
			current->getValue().pageS + 1;
		if (free_size >= (page_nums)){
			free_slots[current] = free_size;
		}
		current = current->getNext();
	}

	if (free_slots.size() == 0){
		cout << "Not enough memory\n";
		return;
	}

	Node<usedMem> *new_node = NULL;
	Node<Group> *node_to_use = NULL;

	if (this->algorithm == "best"){
		std::pair<Node<Group>*, int> min =
			*std::min_element(free_slots.begin(), free_slots.end(), CompSec());
		node_to_use = min.first;
	}
	else{
		std::pair<Node<Group>*, int> max =
			*std::max_element(free_slots.begin(), free_slots.end(), CompSec());
		node_to_use = max.first;
	}

	Group group_to_use = node_to_use->getValue();
	if (group_to_use.pageS + page_nums > group_to_use.pageE){
		if (free.getStart() == node_to_use){
			free.setStart(node_to_use->getNext());
		}
		else{
			bool previous_node_found = false;
			current = free.getStart()->getNext();
			while (current != NULL && !previous_node_found){
				if (current->getNext() == node_to_use){
					current->setNext(node_to_use->getNext());
					previous_node_found = true;
				}
				else{
					current = current->getNext();
				}
			}
		}
	}
	else{
		node_to_use->setValue(Group(group_to_use.pageS + page_nums,
			group_to_use.pageE));
	}

	int chunk_start = group_to_use.pageS,
		//
		chunk_end = group_to_use.pageS + page_nums - 1;
	new_node = new Node<usedMem>(usedMem(chunk_start, chunk_end, prog_info.name));

	used_current = used.getStart();
	Node<usedMem>* used_last = used.getStart();

	if (used_current != NULL &&
		used_current->getValue().pageS > new_node->getValue().pageS){
		new_node->setNext(used.getStart());
		used.setStart(new_node);
		cout << "Program " << prog_info.name.c_str() << " added successfully: " << page_nums << " page(s) used\n\n";
		return;
	}

	while (used_current != NULL){
		if (used_current->getValue().pageS >
			new_node->getValue().pageS){
			used_last->setNext(new_node);
			new_node->setNext(used_current);
			cout << "Program " << prog_info.name.c_str() << " added successfully: " << page_nums << " page(s) used\n\n";
			return;
		}
		else{
			used_last = used_current;
			used_current = used_current->getNext();
		}
	}

	used.appendStart(new_node->getValue());
	cout << "Program " << prog_info.name.c_str() << " added successfully: " << page_nums << " page(s) used\n\n";
}
//Memory allocator subset and defragment method 
void memoryPlace::defragment(){
	Node<Group> *current = free.getStart(),
		*last = NULL;
	while (current != NULL){
		if (last == NULL){
			last = current;
			current = current->getNext();
		}
		else{
			int last_end = last->getValue().pageE,
				current_start = current->getValue().pageS;
			if (last_end + 1 == current_start){
				Group expanded(last->getValue().pageS,
					current->getValue().pageE);
				last->setValue(expanded);
				last->setNext(current->getNext());
				return;
			}
			else{
				last = current;
				current = current->getNext();
			}
		}
	}
}
//Kills the program through this method
void memoryPlace::killPro(std::string program_name){
	int freed_pageS, freed_pageE;
	bool program_found = false;
	Node<usedMem> *used_current = used.getStart();
	Node<usedMem> *used_last = NULL;

	while (used_current != NULL && !program_found){
		usedMem current_group = used_current->getValue();
		if (program_name.compare(current_group.program_name) == 0){
			freed_pageS = current_group.pageS;
			freed_pageE = current_group.pageE;
			if (used_last != NULL){
				used_last->setNext(used_current->getNext());
			}
			else{
				used.setStart(used_current->getNext());
			}
			program_found = true;
		}
		else{
			used_last = used_current;
			used_current = used_current->getNext();
		}
	}

	if (!program_found){
		cout << "Could not find program with name " << program_name.c_str() << " \n";
		return;
	}

	Group freed(used_current->getValue().pageS,
		used_current->getValue().pageE);
	Node<Group> *new_node = new Node<Group>(freed);

	if (free.getStart() == NULL){
		free.setStart(new_node);
		cout << "Program " << program_name.c_str() << " successfully killed, " << freed_pageE - freed_pageS + 1 << " page(s) reclaimed.\n\n";
		return;
	}


	Node<Group> *free_current = free.getStart();
	Node<Group> *free_last = NULL;
	while (free_current != NULL){
		int current_pageS = free_current->getValue().pageS;
		if (current_pageS > freed.pageS){
			new_node->setNext(free_current);
			if (free_last == NULL){
				free.setStart(new_node);
			}
			else{
				free_last->setNext(new_node);
			}
			defragment();
			cout << "Program " << program_name.c_str() << " successfully killed, " << freed_pageE - freed_pageS + 1 << " page(s) reclaimed.\n\n";
			return;
		}
		else{
			free_last = free_current;
			free_current = free_current->getNext();
		}
	}
}
//Prints fragment
void memoryPlace::printMem(){
	std::map<int, std::string> used_pages;
	Node<usedMem> *current = used.getStart();
	while (current != NULL){
		usedMem info = current->getValue();
		for (int i = info.pageS; i <= info.pageE; i++){
			used_pages[i] = info.program_name;
		}
		current = current->getNext();
	}
	for (int row = 0; row < 4; row++){
		for (int col = 0; col < 8; col++){
			int curr_page = row * 8 + col;
			if (used_pages.find(curr_page) != used_pages.end()){
				cout << used_pages[curr_page].c_str() << "   ";
			}
			else{
				cout << ("Free ");
			}
		}
		cout << ("\n");
	}
	cout << ("\n");
}
//Prints Freagment
void memoryPlace::printFragment(){
	int num_fragments = 0;
	Node<Group> *current = free.getStart();
	while (current != NULL){
		num_fragments++;
		current = current->getNext();
	}
	cout << "There are " << num_fragments << " fragment(s)\n\n";
}
//Main method
int main(int argc, char** argv) {
	if (argv[1] != NULL) {
		std::string algorithm = argv[1];
		if (algorithm.compare("best") == 0 || algorithm.compare("worst") == 0) {
			return run_loop(algorithm);
		}
	}
	cout << ("First argument must be the algorithm to use (namely, ");
	cout << ("\"best\" or \"worst\")\n");
	return 0;
}

 