#include "prefixer.h"

using namespace std;

namespace boolean_parser {

//Initialize the static operators array 
std::vector<char> abstract_syntax_tree::operators({'|','&','!',' '});

std::vector<char> abstract_syntax_tree::unary_operators({'!',' '});

//Constructor for the nodes in the syntax tree
syntax_node::syntax_node(char v, syntax_node *l, syntax_node *r):value(v),left(l),right(r){};

//Represent in prefix
void syntax_node::repr(ostream& str) {
    //If this is a leaf node don't put parenthesis
    if('a'<=value && 'z'>=value || 'T' == value || 'F' == value) {
       str << value;
       return;
    }

    //If this is a unary node
    if(abstract_syntax_tree::is_unary(value))
    { 
        str << "(" << value << " ";
        left->repr(str);
        str << ")";
        return;
    }
    //If it is a binary operation then 
    str << "(" << value << " ";
    right->repr(str);
    str << " ";
    left->repr(str);
    str << ")";
}

void abstract_syntax_tree::repr(ostream& ss) {
		if(tree_root!=NULL) tree_root->repr(ss);
}
//Returns true if the operator is unary
bool abstract_syntax_tree::is_unary(char op) {
    return (std::find(abstract_syntax_tree::unary_operators.begin(),
			          abstract_syntax_tree::unary_operators.end(),
			          op)!=abstract_syntax_tree::unary_operators.end());
}

//Returns true if p is an operator (is in the operators array)
bool abstract_syntax_tree::is_operator(char p) {
    return (std::find(abstract_syntax_tree::operators.begin(),
				      abstract_syntax_tree::operators.end(),
					  p)!=abstract_syntax_tree::operators.end());
}

//Returns true if p has precedence over q
//undefined if either p or q are not operators
bool abstract_syntax_tree::precedence(char p, char q) {
    auto p_ix = std::find(abstract_syntax_tree::operators.begin(),abstract_syntax_tree::operators.end(),p);
    auto q_ix = std::find(abstract_syntax_tree::operators.begin(),abstract_syntax_tree::operators.end(),q);
    return p_ix > q_ix;
}

//Removes an operator from the operator stack, pops
//its operands from the top of the operand stack, forms a node of the syntax tree 
//and pushes it back to the top of the stack
void abstract_syntax_tree::pop_operator() {

    syntax_node *left, *right, *current; //Pointers for the operands 
    left = NULL;
    right = NULL;
    //Get the operator on the top of the stack
    char op = operator_stack.back();
    operator_stack.pop_back();
    if(!operand_stack.empty()) {
        left = operand_stack.back();
        operand_stack.pop_back();
    }
    else
        throw new parsing_exception();
     
    if(!is_unary(op)) {  
        if(!operand_stack.empty()) {
            right = operand_stack.back();
            operand_stack.pop_back();
        }
        else 
            throw new parsing_exception(); 
    }
   
    //Form the new node of the tree and push to the top of the stack
    current = new syntax_node(op,left,right);
    operand_stack.push_back(current);
	nodes.push_back(current); //Keep a pointer in the nodes vector
}

//Simplification rules for nodes with value &
void abstract_syntax_tree::and_simplify(syntax_node* node) {
	if(node->left->value == 'F') {
			node->value = 'F';
		   	node->left == NULL;
		   	node->right == NULL;
			return;
   	}

	if(node->right->value == 'F') {
			node->value = 'F';
		   	node->left == NULL;
		   	node->right == NULL;
			return;
   	}

	if(node->left->value == 'T') {
			replace_with_right(node);
			return;
   	}
	if(node->right->value == 'T') {
			replace_with_left(node);
			return;
   	}
}

//Simplification rules for nodes with value |
void abstract_syntax_tree::or_simplify(syntax_node* node) {
	
	if(node->left->value == 'T') {
			node->value = 'T';
		   	node->left == NULL;
		   	node->right == NULL;
			return;
   	}

	if(node->right->value == 'T') {
			node->value = 'T';
		   	node->left == NULL;
		   	node->right == NULL;
			return;
   	}

	if(node->left->value == 'F') {
			replace_with_right(node);
			return;
   	}
	if(node->right->value == 'F') {
			replace_with_left(node);
			return;
   	}
}

//Simplification rules for nodes with value !
void abstract_syntax_tree::not_simplify(syntax_node* node) {
	if(node->left->value == 'T'){node->value = 'F'; node->left == NULL; return; }
	if(node->left->value == 'F'){node->value = 'T'; node->left == NULL; return; }	
}

//Replaces this node with its left child
void abstract_syntax_tree::replace_with_left(syntax_node* node) {
		node->value = node->left->value;	
		node->right = node->left->right;
		node->left  = node->left->left;		
}

//Replaces this node with its right child
void abstract_syntax_tree::replace_with_right(syntax_node* node) {
		node->value = node->right->value;	
		node->left  = node->right->left;	
		node->right = node->right->right;
}

//Traverses the tree starting at the leafs, if the node is 
//an operator node and its arguments are such that a simplification is possible, 
//the respective children are pruned. For example the node | with children T and * 
//T|*->T is transformed to T and its children pruned.
void abstract_syntax_tree::simplify() {
	if(tree_root == NULL) return; //Sucess by default

	//We implement the traversal with a queue to prevent stack overflows 
	//of a recursion
	std::vector<syntax_node*> node_queue;
	
	//Push the root node into the queue
	node_queue.push_back(tree_root);	
	
	int q_ix = 0;
	syntax_node* iter;
	//Push all the tree into the queue level by level
	while(q_ix < node_queue.size())
	{
		    	
			iter = node_queue[q_ix];
			if(iter->left!=NULL) node_queue.push_back(iter->left);
			if(iter->right!=NULL) node_queue.push_back(iter->right);
			q_ix ++;
	}
	//At this point the queue contains all the tree level by level starting from the 
	//root.
	//Pop the nodes starting from the leaves
	while(!node_queue.empty()) {
			iter = node_queue.back();
			if(is_operator(iter->value))
			{
				switch(iter->value)
				{
					case '!':
							not_simplify(iter);
							break;
					case '&':
							and_simplify(iter);
							break;
				    case '|':
							or_simplify(iter);
							break;
				}
			}
			node_queue.pop_back();
	}
}

//Private constructor for testing
abstract_syntax_tree::abstract_syntax_tree() {}

//Builds the abstract syntax tree for the boolean expression in query
abstract_syntax_tree::abstract_syntax_tree(string query) {    
    
    //Temporary variables
    char c;
    syntax_node* n;
    
    //Main loop
    for(auto iter = query.begin(); iter != query.end(); iter++) {
        //Get the next character from the string 
        c = *iter;
		//Skip spaces 
		if(c == ' ') 
				continue;
        if( c == '(') {
			operator_stack.push_back('(');
		}	
        else if(c==')') { //If the present token is a right parenthesis, pop all elements until we find the matching

            while(!operator_stack.empty() && operator_stack.back()!='(') {

                pop_operator();
            }

            if(operator_stack.empty()) { 
                //The stack was emptied before finding (, there is an unmatched parenthesis
                throw new parsing_exception();
            }
            //Remove the parenthesis from the stack 
            operator_stack.pop_back();
        }
		else if(is_operator(c)) {
            //Look at the top of the operator stack and remove all the 
            //higher precedence operators except if it is a parenthesis
            while(!operator_stack.empty() 
							&& operator_stack.back() != '('
						   	&& precedence(operator_stack.back(),c)) {

                pop_operator();
            }
            operator_stack.push_back(c);
        }
        else if(c >= 'a' && c <= 'z' || c == 'T' || c == 'F') {   
            //If it is a variable or constant,
            //make a node and put it in the operand stack
            n = new syntax_node(c,NULL,NULL); 
            operand_stack.push_back(n);
			nodes.push_back(n); //Keep a pointer in the nodes 
        }
        else { //Unknown token
            throw new parsing_exception();
        }
    }
    
    //All the tokens have been parsed now empty the rest of the operator stack
    while(!operator_stack.empty()) {

        if(operator_stack.back() == '(') { //There was an unmatched parenthesis
            throw new parsing_exception();
        }
        else {
            pop_operator();
        }
    }

	//If the boolean expresion is not trivial ie: "", "()", "(())",... then 
	//the operand_stack should contain only one element: the root of the tree.
	//If it is trivial it will contain none.
	if(operand_stack.size() == 0) {
		tree_root = NULL;
	}
	else if(operand_stack.size()==1) {
		tree_root = operand_stack.back();
	}
	else {
		throw new parsing_exception();
	}
}

abstract_syntax_tree::~abstract_syntax_tree() {
		for(auto iter = nodes.begin(); iter != nodes.end(); iter++)
		{
				delete(*iter);
		}
}

}
