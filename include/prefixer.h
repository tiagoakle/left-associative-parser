/* 
 * Implements a variation on Djikstra's Shunting-Yard algorithm to parse a well
 * formed boolean expression into an abstract syntax tree.  The resulting tree
 * can then be used to represent the expression in prefix form or execute some
 * basic simplifications of the boolean expression.
 * 
 * The parser can easily be modified to support other grammars with
 * similar structure: binary left-associative infix operators, prefix unary
 * operators, no ties in the precedence of the operators and only ( ).  All
 * that is required is re-defining the ''operators'' and ''unary_operators''
 * static members of abstract_syntax_tree  However, the simplification routines
 * have to be re-implemented
 *
 * Santiago Akle March 2015 
 *
 */

#include <sstream>
#include <iostream>
#include <vector>
#include <gtest/gtest_prod.h>

#ifndef H_PREFIXER
#define H_PREFIXER

using namespace std;

namespace boolean_parser {

//Represents a node in the abstract syntax tree
struct syntax_node {

        char value;
        syntax_node* left;
        syntax_node* right;
        syntax_node(char v, syntax_node *l, syntax_node *r);
        void repr(ostream& str);
};

class parsing_exception : std::exception {
};


//Constructs an abstract syntax tree from a boolean expression
//Implements some basic boolean simplification methods and implements
//methods to represent the boolean clause in prefix form  
class abstract_syntax_tree
{

private:

	//Static members that define the grammar
    //Operators in the grammar in order of precedence low to high
	//Excluding parenthesis
    static std::vector<char> operators;

	//List of operators that are unary operators
    static std::vector<char> unary_operators;

    //Returns true if the operator is unary (in unary_operators)
    static bool is_unary(char op);
    
    //Returns true if p is an operator (is in operators)
    static bool is_operator(char p);
    
    //Returns true if p has precedence over q
    static bool precedence(char p, char q);
   	   
	friend struct syntax_node;

	//Members of a particular instance of an ast	
	//Holds the root of the syntax tree 
	syntax_node* tree_root;
	
	//Holds a reference to each node, populated upon construction
	std::vector<syntax_node*> nodes; 

	//Stacks for the shunting yard algorithm
	std::vector<syntax_node*> operand_stack;
	std::vector<char> operator_stack;

    //Removes an operator from the operator stack, pops
    //its operands from the top of the operand stack, forms a node of the syntax tree 
    //and pushes it back to the top of the stack
    void pop_operator();
   	
    //Rules to simplify and nodes	
	void and_simplify(syntax_node* node);
    //Rules to simplify or nodes	
	void or_simplify(syntax_node* node);
    //Rules to simplify not nodes	
	void not_simplify(syntax_node* node);

	//Utility methods for the simplification
	void replace_with_left(syntax_node* node);
	void replace_with_right(syntax_node* node);

	//Testing classes that require access to private members
	FRIEND_TEST(AST,precedence);
	FRIEND_TEST(AST,is_unary);
	FRIEND_TEST(AST,is_operator);
	FRIEND_TEST(AST,pop_valid);
	FRIEND_TEST(AST,pop_valid_unary);
	FRIEND_TEST(AST,pop_invalid);

	//Constructor for testing
    abstract_syntax_tree();
public:
    //Constructs a tree from a well-formed query, throws 
    //parsing exceptions for some malformed queries 
    abstract_syntax_tree(string query);
	~abstract_syntax_tree();
    
    //Prints the query in prefix notation 
    void repr(ostream& ss);
    
    //Simplifies the syntax tree
    void simplify();
};
}

#endif
