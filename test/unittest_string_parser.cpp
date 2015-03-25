#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include "prefixer.h"

using namespace testing;

namespace boolean_parser {

TEST(AST,precedence)
{
    //Test the precedence methods 
    //
    ASSERT_EQ(abstract_syntax_tree::precedence('(','!'),true);
    ASSERT_EQ(abstract_syntax_tree::precedence('(','&'),true);
    ASSERT_EQ(abstract_syntax_tree::precedence('(','|'),true);

    ASSERT_EQ(abstract_syntax_tree::precedence('!','('),false);
    ASSERT_EQ(abstract_syntax_tree::precedence('!','&'),true);
    ASSERT_EQ(abstract_syntax_tree::precedence('!','|'),true);

    ASSERT_EQ(abstract_syntax_tree::precedence('&','!'),false);
    ASSERT_EQ(abstract_syntax_tree::precedence('&','('),false);
    ASSERT_EQ(abstract_syntax_tree::precedence('&','|'),true);

    ASSERT_EQ(abstract_syntax_tree::precedence('|','!'),false);
    ASSERT_EQ(abstract_syntax_tree::precedence('|','&'),false);
    ASSERT_EQ(abstract_syntax_tree::precedence('|','('),false);

}

//Test the vars constructor
TEST(AST,is_unary)
{
    ASSERT_EQ(abstract_syntax_tree::is_unary('!'),true);
    ASSERT_EQ(abstract_syntax_tree::is_unary(')'),false);
    ASSERT_EQ(abstract_syntax_tree::is_unary('('),false);
    ASSERT_EQ(abstract_syntax_tree::is_unary('|'),false);
    ASSERT_EQ(abstract_syntax_tree::is_unary('&'),false);
}

TEST(AST,is_operator)
{
    ASSERT_EQ(abstract_syntax_tree::is_operator('!'),true);
    ASSERT_EQ(abstract_syntax_tree::is_operator('&'),true);
    ASSERT_EQ(abstract_syntax_tree::is_operator('|'),true);
    ASSERT_EQ(abstract_syntax_tree::is_operator(')'),false);
    ASSERT_EQ(abstract_syntax_tree::is_operator('('),false);
    ASSERT_EQ(abstract_syntax_tree::is_operator('a'),false);
    ASSERT_EQ(abstract_syntax_tree::is_operator('b'),false);
    ASSERT_EQ(abstract_syntax_tree::is_operator('T'),false);
    ASSERT_EQ(abstract_syntax_tree::is_operator('F'),false);

}

TEST(AST,pop_valid)
{

    abstract_syntax_tree ast;
 
	//Push two nodes to the operand stack and one operator to the operator 
    ast.operator_stack.push_back('|');
    ast.operand_stack.push_back(new syntax_node('T',NULL,NULL));
    ast.operand_stack.push_back(new syntax_node('F',NULL,NULL));
   
	ast.pop_operator();
    
    ASSERT_EQ((ast.operand_stack.back())->value,'|');
    ASSERT_EQ((ast.operand_stack.back())->left->value,'F');
    ASSERT_EQ((ast.operand_stack.back())->right->value,'T');
}

TEST(AST,pop_valid_unary)
{
    //Stacks
    std::vector<char> operator_stack;
    std::vector<syntax_node*> operand_stack;
    std::vector<syntax_node*> node_stack;

    abstract_syntax_tree ast;
    //Push two nodes to the operand stack and one operator to the operator 
    ast.operator_stack.push_back('!');
    ast.operand_stack.push_back(new syntax_node('T',NULL,NULL)); 

    ast.pop_operator();

    ASSERT_EQ((ast.operand_stack.back())->value,'!');
    ASSERT_EQ((ast.operand_stack.back())->left->value,'T');
}

//Pop a binary operator without enough operands
TEST(AST,pop_invalid)
{
    //Stacks 
    abstract_syntax_tree ast;
	
	//Push two nodes to the operand stack and one operator to the operator 
    ast.operator_stack.push_back('|');
    ast.operand_stack.push_back(new syntax_node('T',NULL,NULL)); 
    ASSERT_ANY_THROW(ast.pop_operator()); 
}

TEST(AST,build_1)
{
    string query = "a|b|c|d|e|F|T";
    abstract_syntax_tree ast(query);
    stringstream ss;
    ast.repr(ss);
    std::cout << query << endl;
    std::cout << ss.str() << endl;
}

TEST(AST,build_2)
{
    string query = "(a|b)&c|d";
    std::cout << query << endl;
	abstract_syntax_tree ast(query);
    stringstream ss;
    ast.repr(ss);
    std::cout << ss.str() << endl;
}

TEST(AST,build_3)
{
    string query = "((a|b)&c|d)";
    std::cout << query << endl;
	abstract_syntax_tree ast(query);
    stringstream ss;
    ast.repr(ss);
    std::cout << ss.str() << endl;
}

TEST(AST,build_trivial)
{
    string query = "";
    abstract_syntax_tree ast(query);
}

TEST(AST,build_trivial_2)
{
    string query = "()";
    abstract_syntax_tree ast(query);
}

TEST(AST,simplify_trivial)
{
    string query = "!T";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}


TEST(AST,specification_1)
{
    string query = "F";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;

}

TEST(AST,specification_2)
{
    string query = "T|F";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}

TEST(AST,specification_3)
{
    string query = "! (T & F)";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}
TEST(AST,specification_4)
{
    string query = "a | ! a";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;

}

TEST(AST,specification_5)
{
    string query = "F & T | F"; 
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}

TEST(AST,specification_6)
{
    string query = "a & b";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}

TEST(AST,specification_7)
{
    string query = "a & b | c";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;

}

TEST(AST,specification_8)
{
    string query = "a & b | c & ! F";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}

TEST(AST,specification_9)
{

    string query = "F | a & (T | b) & c";
    abstract_syntax_tree ast(query);

	ast.simplify();
	stringstream ss;
	ast.repr(ss);
	std::cout << ss.str() << endl;
}

}
